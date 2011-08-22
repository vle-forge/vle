/*
 * @file vle/utils/RemoteManager.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <vle/utils/RemoteManager.hpp>
#include <vle/utils/Algo.hpp>
#include <vle/utils/DownloadManager.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Preferences.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/version.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread/thread.hpp>
#include <boost/unordered_map.hpp>
#include <boost/regex.hpp>
#include <fstream>
#include <ostream>
#include <string>

namespace vle { namespace utils {

static bool isName(const std::string& line)
{
    return boost::algorithm::starts_with(line, "Package:");
}

static bool isVersion(const std::string& line)
{
    return boost::algorithm::starts_with(line, "Version:");
}

static bool isDepends(const std::string& line)
{
    return boost::algorithm::starts_with(line, "Depends:");
}

static bool isMaintainer(const std::string& line)
{
    return boost::algorithm::starts_with(line, "Maintainer:");
}

static bool isDescription(const std::string& line)
{
    return boost::algorithm::starts_with(line, "Description:");
}

static bool isEndDescription(const std::string& line)
{
    return line == " .";
}

static bool isTags(const std::string& line)
{
    return boost::algorithm::starts_with(line, "Tags:");
}

static bool isUrl(const std::string& line)
{
    return boost::algorithm::starts_with(line, "Url:");
}

static bool isSize(const std::string& line)
{
    return boost::algorithm::starts_with(line, "Size:");
}

static bool isMD5sum(const std::string& line)
{
    return boost::algorithm::starts_with(line, "MD5sum:");
}

/**
 * \c RemotePackage stores a distant package data.
 *
 * \c RemotePackage have information about a distant package:
 * - description of the package's content.
 * - version of the package in a 3-uple, major, minor, patch.
 * - authors of the package.
 * - license of the package.
 * - an URL to the package source development.
 */
class RemotePackage
{
public:
    RemotePackage(std::istream& is)
        : mName(), mMaintainer(), mDescription(), mUrl(), mMD5sum(),
        mTags(), mDepends(), mVersionMajor(0), mVersionMinor(0),
        mVersionPatch(0), mSize(0)
    {
        try {
            std::string line;
            std::getline(is, line);

            if (isName(line)) {
                setName(std::string(line, 8));
                std::getline(is, line);
            }
            if (isVersion(line)) {
                setVersion(std::string(line, 8));
                std::getline(is, line);
            }
            if (isDepends(line)) {
                setDepends(std::string(line, 8));
                std::getline(is, line);
            }
            if (isMaintainer(line)) {
                setMaintainer(std::string(line, 11));
                std::getline(is, line);
            }
            if (isDescription(line)) {
                setDescription(std::string(line, 12));

                std::getline(is, line);
                while (is and not isEndDescription(line)) {
                    appendDesciption(line);
                    std::getline(is, line);
                }
            }
            if (isTags(line)) {
                setTags(std::string(line, 5));
                std::getline(is, line);
            }
            if (isUrl(line)) {
                setUrl(std::string(line, 4));
                std::getline(is, line);
            }
            if (isSize(line)) {
                setSize(std::string(line, 5));
                std::getline(is, line);
            }
            if (isMD5sum(line)) {
                setMD5sum(std::string(line, 7));
            }
        } catch (const std::ios_base::failure& /*e*/) {
        }
    }

    RemotePackage(const RemotePackage& other)
        : mName(other.mName), mMaintainer(other.mMaintainer),
        mDescription(other.mDescription), mUrl(other.mUrl),
        mMD5sum(other.mMD5sum), mTags(other.mTags), mDepends(other.mDepends),
        mVersionMajor(other.mVersionMajor), mVersionMinor(other.mVersionMinor),
        mVersionPatch(other.mVersionPatch), mSize(other.mSize)
    {
    }

    ~RemotePackage()
    {
    }

    RemotePackage& operator=(const RemotePackage& other)
    {
        RemotePackage tmp(other);
        tmp.swap(*this);
        return *this;
    }

    void swap(RemotePackage& other)
    {
        std::swap(mName, other.mName);
        std::swap(mVersionMajor, other.mVersionMajor);
        std::swap(mVersionMinor, other.mVersionMinor);
        std::swap(mVersionPatch, other.mVersionPatch);
        std::swap(mDepends, other.mDepends);
        std::swap(mMaintainer, other.mMaintainer);
        std::swap(mDescription, other.mDescription);
        std::swap(mTags, other.mTags);
        std::swap(mUrl, other.mUrl);
        std::swap(mSize, other.mSize);
        std::swap(mMD5sum, other.mMD5sum);
    }

    void setName(const std::string& name)
    {
        std::string tmp = boost::algorithm::trim_copy(name);

        if (tmp.empty()) {
            throw utils::ArgError(fmt(_("RemotePackage: empty name")));
        } else {
            mName = tmp;
        }
    }

    void setVersion(const std::string& line)
    {
        using boost::lexical_cast;
        using boost::numeric_cast;

        try {
            std::string::size_type major = line.find('.');
            if (major != std::string::npos and line.size() < major) {
                std::string::size_type minor = line.find('.', major);
                if (minor != std::string::npos and line.size() < minor) {
                    long ma, mi, pa;

                    ma = lexical_cast < long >(std::string(line, 0, major));
                    mi = lexical_cast < long >(std::string(line, major + 1,
                                                           minor));
                    pa = lexical_cast < long >(std::string(line, minor + 1));

                    uint32_t uma, umi, mpa;

                    uma = numeric_cast < uint32_t >(ma);
                    umi = numeric_cast < uint32_t >(mi);
                    mpa = numeric_cast < uint32_t >(pa);

                    setVersion(uma, umi, mpa);
                }
            }
        } catch (const std::exception& /*e*/) {
            throw utils::InternalError(fmt(
                    _("Can not convert version `%1%'")) % line);
        }
    }

    void setVersion(uint32_t major, uint32_t minor, uint32_t patch)
    {
        mVersionMajor = major;
        mVersionMinor = minor;
        mVersionPatch = patch;
    }

    void depends(std::vector < std::string >* depends) const
    {
        depends->resize(mDepends.size());

        std::copy(mDepends.begin(),
                  mDepends.end(),
                  depends->begin());
    }

    void setDepends(const std::string& line)
    {
        std::vector < std::string > depends;

        boost::algorithm::split(depends, line,
                                boost::algorithm::is_any_of(","),
                                boost::algorithm::token_compress_on);

        setDepends(depends);
    }

    void setDepends(const std::vector < std::string >& depends)
    {
        typedef std::vector < std::string >::const_iterator iterator;

        mDepends.clear();

        for (iterator it = depends.begin(); it != depends.end(); ++it) {
            std::string str = boost::algorithm::trim_copy(*it);
            if (not str.empty()) {
                mDepends.push_back(str);
            }
        }
    }

    void setMaintainer(const std::string& maintainer)
    {
        std::string tmp = boost::algorithm::trim_copy(maintainer);

        if (tmp.empty()) {
            throw utils::ArgError(fmt(_("RemotePackage: empty maintainer")));
        } else {
            mMaintainer = tmp;
        }
    }

    void setDescription(const std::string& description)
    {
        std::string tmp = boost::algorithm::trim_copy(description);

        if (tmp.empty()) {
            throw utils::ArgError(fmt(_("RemotePackage: empty description")));
        } else {
            mDescription = tmp;
        }
    }

    void appendDesciption(const std::string& description)
    {
        std::string tmp = boost::algorithm::trim_copy(description);

        mDescription.append(tmp);
    }

    void tags(std::vector < std::string >* tags) const
    {
        tags->resize(mTags.size());

        std::copy(mTags.begin(),
                  mTags.end(),
                  tags->begin());
    }

    void setTags(const std::string& line)
    {
        std::vector < std::string > tags;

        boost::algorithm::split(tags, line,
                                boost::algorithm::is_any_of(","),
                                boost::algorithm::token_compress_on);

        setTags(tags);
    }

    void setTags(const std::vector < std::string >& tags)
    {
        typedef std::vector < std::string >::const_iterator iterator;

        mTags.clear();

        for (iterator it = tags.begin(); it != tags.end(); ++it) {
            std::string str = boost::algorithm::trim_copy(*it);
            if (not str.empty()) {
                mTags.push_back(str);
            }
        }
    }

    void setUrl(const std::string& url)
    {
        std::string tmp = boost::algorithm::trim_copy(url);

        if (tmp.empty()) {
            throw utils::ArgError(fmt(_("RemotePackage: empty url")));
        } else {
            mUrl = tmp;
        }
    }

    void setSize(const std::string& size)
    {
        using boost::lexical_cast;
        using boost::numeric_cast;

        try {
            long lsize = boost::lexical_cast < long >(size);
            setSize(numeric_cast < uint32_t >(lsize));
        } catch (const std::exception& /*e*/) {
            throw utils::InternalError(fmt(
                    _("Can not convert size `%1%'")) % size);
        }
    }

    void setSize(uint32_t size)
    {
        mSize = size;
    }

    void setMD5sum(const std::string& md5sum)
    {
        std::string tmp = boost::algorithm::trim_copy(md5sum);

        if (tmp.empty()) {
            throw utils::ArgError(fmt(_("RemotePackage: empty md5sum")));
        } else {
            mMD5sum = tmp;
        }
    }

    std::string getSourcePackageUrl() const
    {
        return (fmt("%1%/%2%-%3%.%4%.%5%.zip") %
                mUrl % mName % mVersionMajor % mVersionMinor %
                mVersionPatch).str();
    }

    std::string getBinaryPackageUrl() const
    {
        return (fmt("%1%/%2%-%3%.%4%.%5%-%6%-%7%.zip") %
                mUrl % mName % mVersionMajor % mVersionMinor %
                mVersionPatch % VLE_SYSTEM_NAME % VLE_SYSTEM_PROCESSOR).str();
    }

    std::string mName;
    std::string mMaintainer;
    std::string mDescription;
    std::string mUrl;
    std::string mMD5sum;
    std::vector < std::string > mTags;
    std::vector < std::string > mDepends;
    uint32_t mVersionMajor;
    uint32_t mVersionMinor;
    uint32_t mVersionPatch;
    uint32_t mSize;
};

bool operator==(const RemotePackage& a, const RemotePackage& b)
{
    return a.mName == b.mName
        and a.mVersionMajor == b.mVersionMajor
        and a.mVersionMinor == b.mVersionMinor
        and a.mVersionPatch == b.mVersionPatch;
}

bool operator<(const RemotePackage& a, const RemotePackage& b)
{
    if (a.mName == b.mName) {
        if (a.mVersionMajor == b.mVersionMajor) {
            if (a.mVersionMinor == b.mVersionMinor) {
                if (a.mVersionPatch == b.mVersionPatch) {
                    return false;
                } else {
                    return a.mVersionPatch < b.mVersionPatch;
                }
            } else {
                return a.mVersionMinor < b.mVersionMinor;
            }
        } else {
            return a.mVersionMajor < b.mVersionMajor;
        }
    } else {
        return a.mName < b.mName;
    }
}

std::ostream& operator<<(std::ostream& os, const RemotePackage& b)
{
    using boost::algorithm::join;

    return os
        << "Package: " <<  b.mName << "\n"
        << "Version: " << b.mVersionMajor
        << "." << b.mVersionMinor
        << "." << b.mVersionPatch << "\n"
        << "Depends: " << join(b.mDepends, ",") << "\n"
        << "Maintainer: " << b.mMaintainer << "\n"
        << "Description: " << b.mDescription << "\n" << " ." << "\n"
        << "Tags: " << join(b.mTags, ", ") << "\n"
        << "Filename: " << b.mName << "\n"
        << "Size: " << b.mSize << "\n"
        << "MD5sum: " << b.mMD5sum << "\n";
}

class RemoteManager::Pimpl
{
public:
    typedef boost::unordered_map < std::string, RemotePackage > Pkgs;
    typedef Pkgs::const_iterator const_iterator;
    typedef Pkgs::iterator iterator;

    Pimpl()
        : mStream(0), mIsStarted(false), mIsFinish(false), mStop(false),
        mHasError(false)
    {
        try {
            read(buildPackageFilename());
        } catch (...) {
        }
    }

    ~Pimpl()
    {
        join();
    }

    void start(RemoteManagerActions action, const std::string& arg,
               std::ostream* out)
    {
        if (not mIsStarted) {
            mIsStarted = true;
            mIsFinish = false;
            mStop = false;
            mHasError = false;
            mArgs = arg;
            mStream = out;

            switch (action) {
            case REMOTE_MANAGER_UPDATE:
                mThread = boost::thread(
                    &RemoteManager::Pimpl::actionUpdate, this);
                break;
            case REMOTE_MANAGER_SOURCE:
                mThread = boost::thread(
                    &RemoteManager::Pimpl::actionSource, this);
                break;
            case REMOTE_MANAGER_INSTALL:
                mThread = boost::thread(
                    &RemoteManager::Pimpl::actionInstall, this);
                break;
            case REMOTE_MANAGER_SEARCH:
                mThread = boost::thread(
                    &RemoteManager::Pimpl::actionSearch, this);
                break;
            case REMOTE_MANAGER_SHOW:
                mThread = boost::thread(
                    &RemoteManager::Pimpl::actionShow, this);
                break;
            default:
                break;
            }
        }
    }

    void join()
    {
        if (mIsStarted) {
            if (not mIsFinish) {
                mThread.join();
            }
        }
    }

    void stop()
    {
        if (mIsStarted and not mIsFinish) {
            mStop = true;
        }
    }

private:
    /**
     * Build the filename of the user's packages in \c VLE_HOME/packages.
     *
     * @return A filename.
     */
    static std::string buildPackageFilename()
    {
        return utils::Path::path().getHomeFile("packages");
    }

    /**
     * Send the parameter of the template function \c t to
     *
     * @param t
     */
    template < typename T > void out(const T& t)
    {
        if (mStream) {
            *mStream << t;
        }
    }

    /**
     * A functor to check if a @c Packages::value_type corresponds to the
     * regular expression provided in constructor.
     */
    struct HaveExpression
    {
        HaveExpression(const boost::regex& expression,
                       std::ostream& stream)
            : expression(expression), stream(stream)
        {
        }

        void operator()(const Pkgs::value_type& value) const
        {
            boost::sregex_iterator it(value.first.begin(),
                                      value.first.end(),
                                      expression);
            boost::sregex_iterator end;

            if (it != end) {
                stream << value.first << "\n";
            } else {
                boost::sregex_iterator jt(value.second.mDescription.begin(),
                                          value.second.mDescription.end(),
                                          expression);

                if (jt != end) {
                    stream << value.first << "\n";
                }
            }
        }

        const boost::regex& expression;
        std::ostream& stream;
    };

    /**
     * Read the package file \c filename.
     *
     * @param[in] The filename to read.
     */
    void read(const std::string& filename)
    {
        std::ifstream file(filename.c_str());

        if (file) {
            file.exceptions(std::ios_base::eofbit | std::ios_base::failbit |
                            std::ios_base::badbit);

            while (not file) {
                RemotePackage pkg(file);

                mPackages.insert(
                    std::make_pair < std::string, RemotePackage >(
                        pkg.mName, pkg));

            }
        } else {
            TraceAlways(fmt(_("Failed to open package file `%1%'")) % filename);
        }
    }

    /**
     * Write the default package \c VLE_HOME/packages.
     */
    void save() const throw()
    {
        std::ofstream file(buildPackageFilename().c_str());

        if (file) {
            file.exceptions(std::ios_base::eofbit | std::ios_base::failbit |
                            std::ios_base::badbit);

            try {
                std::transform(
                    mPackages.begin(), mPackages.end(),
                    std::ostream_iterator < RemotePackage >( file, "\n"),
                    select2nd < Pkgs::value_type >());
            } catch (const std::exception& /*e*/) {
                TraceAlways(fmt(_("Failed to write package file `%1%'")) %
                            buildPackageFilename());
            }
        } else {
            TraceAlways(fmt(_("Failed to open package file `%1%'")) %
                        buildPackageFilename());
        }
    }

    //
    // threaded slot
    //

    void actionUpdate() throw()
    {
        std::vector < std::string > urls;

        try {
            utils::Preferences prefs;
            std::string tmp;
            prefs.get("vle.remote.url", &tmp);

            boost::algorithm::split(urls, tmp,
                                    boost::algorithm::is_any_of(","),
                                    boost::algorithm::token_compress_on);
        } catch(const std::exception& /*e*/) {
            TraceAlways(_("Failed to read preferences file"));
        }

        out(_("Update database\n"));

        std::vector < std::string >::const_iterator it, end = urls.end();
        for (it = urls.begin(); it != end; ++it) {
            try {
                DownloadManager dl;

                std::string url = *it;
                url += '/';
                url += "packages";

                out(fmt(_("Download %1% ...")) % url);

                dl.start(url);
                dl.join();

                if (not dl.hasError()) {
                    out(_("ok"));
                    std::string filename(dl.filename());

                    try {
                        out(_("(merged: "));
                        read(filename);
                        out(_("ok)"));
                    } catch (...) {
                        out(_("failed)"));
                    }
                } else {
                    out(_("failed"));
                }
                out("\n");
            } catch (const std::exception& e) {
                out(fmt(_("failed `%1%'")) % e.what());
            }
        }

        out(_("Database updated\n"));

        save();

        mStream = 0;
        mIsFinish = true;
        mIsStarted = false;
        mStop = false;
        mHasError = false;
    }

    void actionInstall() throw()
    {
        const_iterator it = mPackages.find(mArgs);

        if (it != mPackages.end()) {
            std::string url = it->second.getBinaryPackageUrl();

            DownloadManager dl;

            out(fmt(_("Download binary package `%1%' at %2%")) % mArgs % url);
            dl.start(url);
            dl.join();

            if (not dl.hasError()) {
                out(_("install"));
                std::string filename = dl.filename();
                std::string zipfilename = dl.filename();
                zipfilename += ".zip";

                boost::filesystem::rename(filename, zipfilename);

                utils::Package::package().unzip(mArgs, zipfilename);
                utils::Package::package().wait(*mStream, *mStream);
                out(_(": ok\n"));
            } else {
                out(_(": failed\n"));
            }
        } else {
            out(fmt(_("Unknown package `%1%'")) % mArgs);
        }

        mStream = 0;
        mIsFinish = true;
        mIsStarted = false;
        mStop = false;
        mHasError = false;
    }

    void actionSource() throw()
    {
        const_iterator it = mPackages.find(mArgs);

        if (it != mPackages.end()) {
            std::string url = it->second.getSourcePackageUrl();

            DownloadManager dl;

            out(fmt(_("Download source package `%1%' at %2%")) % mArgs % url);
            dl.start(url);
            dl.join();

            if (not dl.hasError()) {
                out(_("install"));
                std::string filename = dl.filename();
                std::string zipfilename = dl.filename();
                zipfilename += ".zip";

                boost::filesystem::rename(filename, zipfilename);

                utils::Package::package().unzip(mArgs, zipfilename);
                utils::Package::package().wait(*mStream, *mStream);
                out(_(": ok\n"));
            } else {
                out(_(": failed\n"));
            }
        } else {
            out(fmt(_("Unknown package `%1%'")) % mArgs);
        }

        mStream = 0;
        mIsFinish = true;
        mIsStarted = false;
        mStop = false;
        mHasError = false;
    }

    void actionSearch() throw()
    {
        if (mStream) {
            boost::regex expression(mArgs, boost::regex::grep);

            std::for_each(mPackages.begin(),
                          mPackages.end(),
                          HaveExpression(expression, *mStream));
        }

        mStream = 0;
        mIsFinish = true;
        mIsStarted = false;
        mStop = false;
        mHasError = false;
    }

    void actionShow() throw()
    {
        const_iterator it = mPackages.find(mArgs);

        if (it != mPackages.end()) {
            out(it->second);
        } else {
            out(fmt(_("Unknown package `%1%'")) % mArgs);
        }

        mStream = 0;
        mIsFinish = true;
        mIsStarted = false;
        mStop = false;
        mHasError = false;
    }

    Pkgs mPackages;
    boost::mutex mMutex;
    boost::thread mThread;
    std::string mArgs;
    std::ostream* mStream;
    bool mIsStarted;
    bool mIsFinish;
    bool mStop;
    bool mHasError;
};

RemoteManager::RemoteManager()
    : mPimpl(new RemoteManager::Pimpl())
{
}

RemoteManager::~RemoteManager()
{
    delete mPimpl;
}

void RemoteManager::start(RemoteManagerActions action,
                          const std::string& arg,
                          std::ostream* os)
{
    mPimpl->start(action, arg, os);
}

void RemoteManager::join()
{
    mPimpl->join();
}

void RemoteManager::stop()
{
    mPimpl->stop();
}

}} // namespace vle utils
