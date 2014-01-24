/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#if defined _WIN32 || defined __CYGWIN__
# define BOOST_THREAD_USE_LIB
# define BOOST_THREAD_DONT_USE_CHRONO
#endif

#include <vle/utils/RemoteManager.hpp>
#include <vle/utils/Algo.hpp>
#include <vle/utils/DownloadManager.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Preferences.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/Spawn.hpp>
#include <vle/utils/details/Package.hpp>
#include <vle/utils/details/PackageParser.hpp>
#include <vle/utils/details/PackageManager.hpp>
#include <vle/version.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/cast.hpp>
#include <boost/thread/thread.hpp>
#include <boost/regex.hpp>
#include <fstream>
#include <ostream>
#include <iostream>
#include <string>

#define PACKAGESID_VECTOR_RESERVED_SIZE 100u

namespace vle { namespace utils {

static std::ostream& operator<<(std::ostream& os, const PackageLinkId& b)
{
    static const char *sym[5] = { "=", "<", "<=", ">", ">=" };

    os << b.name;

    if (b.major >= 0) {
        assert((unsigned)b.op < 5);

        os << " (" << sym[static_cast < unsigned >(b.op)] << " " << b.major;

        if (b.minor >= 0) {
            os << "." << b.minor;

            if (b.patch >= 0) {
                os << "." << b.patch;
            }
        }

        os << ")";
    }

    return os;
}

static std::ostream& operator<<(std::ostream& os, const PackagesLinkId& b)
{
    utils::formatCopy(b.begin(),
                      b.end(),
                      os,
                      ", ",
                      std::string(),
                      std::string());

    return os;
}

std::ostream& operator<<(std::ostream& os, const PackageId& b)
{
    os << "Package: " <<  b.name << "\n"
       << "Version: " << b.major
       << "." << b.minor
       << "." << b.patch << "\n"
       << "Depends: " << b.depends << "\n"
       << "Build-Depends: " << b.builddepends << "\n"
       << "Conflicts: " << b.conflicts << "\n"
       << "Maintainer: " << b.maintainer << "\n"
       << "Description: " << b.description << "\n" << " ." << "\n"
       << "Tags: ";

    utils::formatCopy(b.tags.begin(),
                      b.tags.end(),
                      os,
                      ", ",
                      std::string(),
                      std::string());

    return os << "\n"
              << "Url: " << b.url << "\n"
              << "Size: " << b.size << "\n"
              << "MD5sum: " << b.md5sum << "\n";
}

namespace fs = boost::filesystem;

class RemoteManager::Pimpl
{
public:
    Pimpl()
        : mStream(0), mIsStarted(false), mIsFinish(false), mStop(false),
          mHasError(false)
    {
        {
            Packages tmp;

            LocalPackageManager::rebuild(&tmp);
            local.insert(tmp.begin(), tmp.end());
        }

        {
            Packages tmp;

            RemotePackageManager::extract(&tmp);
            remote.insert(tmp.begin(), tmp.end());
        }
    }

    ~Pimpl()
    {
        join();

        save();
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
            mResults.clear();

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
            case REMOTE_MANAGER_LOCAL_SEARCH:
                mThread = boost::thread(
                    &RemoteManager::Pimpl::actionLocalSearch, this);
                break;
            case REMOTE_MANAGER_SEARCH:
                mThread = boost::thread(
                    &RemoteManager::Pimpl::actionSearch, this);
                break;
            case REMOTE_MANAGER_SHOW:
                mThread = boost::thread(
                    &RemoteManager::Pimpl::actionShow, this);
                break;
            case REMOTE_MANAGER_LOCAL_SHOW:
                mThread = boost::thread(
                    &RemoteManager::Pimpl::actionLocalShow, this);
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
    struct NotHaveExpression
        : std::unary_function < PackageId, bool >
    {
        const boost::regex& expression;

        NotHaveExpression(const boost::regex& expression)
            : expression(expression)
        {
        }

        bool operator()(const PackageId& pkg) const
        {
            boost::match_results < std::string::const_iterator > what;

            if (boost::regex_match(pkg.name,
                                   what,
                                   expression,
                                   boost::match_default |
                                   boost::match_partial)) {
                return false;
            }

            if (boost::regex_match(pkg.description,
                                   what,
                                   expression,
                                   boost::match_default |
                                   boost::match_partial)) {
                return false;
            }

            return true;
        }
    };

    void save() const throw()
    {
        try {
            std::ofstream file;
            file.exceptions(std::ios_base::eofbit |
                            std::ios_base::failbit |
                            std::ios_base::badbit);

            file.open(RemoteManager::getLocalPackageFilename().c_str());
            file << local << std::endl;
        } catch (const std::exception& e) {
            TraceAlways(
                fmt(_("Remote manager: failed to write local package "
                      "`%1%': %2%")) % RemoteManager::getLocalPackageFilename()
                % e.what());
        }

        try  {
            std::ofstream file;
            file.exceptions(std::ios_base::eofbit |
                            std::ios_base::failbit |
                            std::ios_base::badbit);

            file.open(RemoteManager::getRemotePackageFilename().c_str());
            file << remote << std::endl;
        } catch (const std::exception& e) {
            TraceAlways(
                fmt(_("Remote manager: failed to write remote package "
                      "`%1%': %2%")) % RemoteManager::getRemotePackageFilename()
                % e.what());
        }
    }

    //
    // threaded slot
    //

    struct Download
        : public std::unary_function < std::string, void >
    {
        PackageParser* parser;
        bool* remoteHasError;
        std::string* remoteMessageError;


        Download(PackageParser *parser, bool *remoteHasError,
                std::string* remoteMessageError)
            : parser(parser), remoteHasError(remoteHasError),
              remoteMessageError(remoteMessageError)
        {
            assert(parser);
        }

        void operator()(const std::string& url) const
        {
            DownloadManager dl;

            dl.start(url, "packages.pkg");
            dl.join();

            if (not dl.hasError()) {
                parser->extract(dl.filename(), url);
            } else {
                if (!(*remoteHasError)) {
                    *remoteHasError = true;
                    remoteMessageError->assign((vle::fmt("failed: %1%\n")
                    % dl.getErrorMessage()).str());
                }
            }
        }
    };

    /**
     * @brief Update the remote.pkg and local.pkg files,
     * the result of this command is the set of packages for which a new version
     * is available on remotes.
     */
    void actionUpdate() throw()
    {
        mHasError = false;

        std::vector < std::string > urls;
        try {
            utils::Preferences prefs;
            std::string tmp;
            prefs.get("vle.remote.url", &tmp);

            boost::algorithm::split(urls, tmp,
                                    boost::algorithm::is_any_of(","),
                                    boost::algorithm::token_compress_on);
        } catch(const std::exception& /*e*/) {
            std::ostringstream errorStream;
            errorStream << _("Failed to read preferences file");
            mErrorMessage.assign(errorStream.str());
            mHasError = true;
            return;
        }

        PackageParser parser;
        std::for_each(urls.begin(), urls.end(),
                      Download(&parser, &mHasError, &mErrorMessage));
        remote.clear();
        {
            PackageParser::const_iterator itb = parser.begin();
            PackageParser::const_iterator ite = parser.end();
            for (; itb!=ite; itb++) {
                remote.insert(*itb);
            }
        }
        {
            PackagesIdSet::const_iterator itlb = local.begin();
            PackagesIdSet::const_iterator itle = local.end();
            bool found;
            PackageIdUpdate pkgUpdateOp;
            std::pair<PackagesIdSet::const_iterator,
                      PackagesIdSet::const_iterator> pkgs_range;

            for (; itlb!=itle; itlb++) {
                const PackageId& ploc = *itlb;
                pkgs_range = remote.equal_range(ploc);
                found = false;
                while ((!found) && (pkgs_range.first != pkgs_range.second)) {
                    const PackageId& premote = *(pkgs_range.first);
                    if (pkgUpdateOp(ploc, premote)) {
                        found = true;
                        mResults.push_back(premote);
                    }
                    pkgs_range.first++;
                }
            }
        }

        mStream = 0;
        mIsFinish = true;
        mIsStarted = false;
        mStop = false;
    }

    /**
     * @brief Install one or more packages. The result of this command
     * is the set of packages that have been installed
     */
    void actionInstall() throw()
    {
        mHasError = false;

        PackageId pkgid;
        pkgid.name = mArgs;
        std::string archname = pkgid.name;
        archname.append(".tar.bz2");
        std::pair<PackagesIdSet::const_iterator,
                  PackagesIdSet::const_iterator> pkgs_range;
        pkgs_range = remote.equal_range(pkgid);
        if (pkgs_range.first == remote.end()) {
            std::ostringstream errorStream;
            errorStream << fmt(_("Unknown package `%1%'\n")) % mArgs;
            mErrorMessage.assign(errorStream.str());
            mHasError = true;
            return;
        }


        PackagesIdSet::const_iterator it = pkgs_range.first;
        PackageIdUpdate pkgUpdateOp;
        while (pkgs_range.first != pkgs_range.second) {
            if (pkgUpdateOp(*it, *pkgs_range.first)) {
                it = pkgs_range.first;
            }
            pkgs_range.first++;
        }

        PackagesIdSet::const_iterator locPkg = local.find(*it);
        if ((locPkg != local.end()) && (!pkgUpdateOp(*locPkg, *it))) {
            out(fmt(_("Package `%1%` is already uptodate\n"))% pkgid.name);
            return;
        }


        DownloadManager dl;
        std::string url = it->url;
        out(fmt(_("Download archive  '%1%' from '%2%': ")) % archname % url);
        dl.start(url, archname);
        dl.join();
        if (not dl.hasError()) {
            out(fmt(_("ok\n")));
            std::string archfile = vle::utils::Path::buildTemp(archname);
            boost::filesystem::rename(dl.filename(), archfile);
            std::string tempDir = vle::utils::Path::path().getParentPath(
                    archfile);
            std::string oldDir = vle::utils::Path::path().getCurrentDir();
            boost::filesystem::path archpath(archfile);
            boost::filesystem::path dearchpath(tempDir);
            boost::filesystem::current_path(boost::filesystem::path(tempDir));
            vle::utils::Path::path().decompress(archpath.string(),
                    dearchpath.string());
            vle::utils::Package pkg(pkgid.name);
            out(fmt(_("Building package '%1%' into '%2%': ")) % pkgid.name %
                    dearchpath.string());
            pkg.configure();
            pkg.wait(std::cerr, std::cerr);
            pkg.build();
            pkg.wait(std::cerr, std::cerr);
            pkg.install();
            pkg.wait(std::cerr, std::cerr);
            out(fmt(_("ok\n")));
            boost::filesystem::current_path(boost::filesystem::path(oldDir));
            mResults.push_back(*it);
        } else {
            out(fmt(_("failed\n")));
            std::ostringstream errorStream;
            errorStream << fmt(_("Error while downloading package "
                    "`%1%'\n")) % mArgs;
            mErrorMessage.assign(errorStream.str());
            mHasError = true;
        }
    }

    /**
     * @brief Dowload a package form a remote. The zip file is stored into the
     * current directory. The result of the command is the set of one element :
     * the package that has been dowloaded
     */
    void actionSource()
    {
        mHasError = false;

        PackageId pkgid;
        pkgid.name = mArgs;
        PackagesIdSet::const_iterator it = remote.find(pkgid);
        if (it != remote.end()) {
            DownloadManager dl;
            std::string url = it->url;
            std::string archname = mArgs;
            archname.append(".tar.bz2");
            dl.start(url, archname);
            dl.join();
            if (not dl.hasError()) {
                std::string filename = dl.filename();
                boost::filesystem::rename(filename, archname);
            } else {
                std::ostringstream errorStream;
                errorStream << fmt(_("Error while downloading "
                        "source package `%1%'\n")) % mArgs;
                mErrorMessage.assign(errorStream.str());
                mHasError = true;
            }
         } else {
             std::ostringstream errorStream;
             errorStream << fmt(_("Unknown package `%1%'\n")) % mArgs;
             mErrorMessage.assign(errorStream.str());
             mHasError = true;
         }
        mResults.push_back(pkgid);
        mStream = 0;
        mIsFinish = true;
        mIsStarted = false;
        mStop = false;
    }


    /**
     * @brief Search for packages installed on local machine. The result of
     * this command is the set of packages found.
     */
    void actionLocalSearch() throw ()
    {
        boost::regex expression(mArgs, boost::regex::grep);

        std::remove_copy_if(local.begin(),
                            local.end(),
                            std::back_inserter(mResults),
                            NotHaveExpression(expression));

        mStream = 0;
        mIsFinish = true;
        mIsStarted = false;
        mStop = false;
        mHasError = false;
    }

    /**
     * @brief Search for packages available on remotes. The result of
     * this command is the set of packages found.
     */
    void actionSearch() throw()
    {
        boost::regex expression(mArgs, boost::regex::grep);

        std::remove_copy_if(remote.begin(),
                            remote.end(),
                            std::back_inserter(mResults),
                            NotHaveExpression(expression));

        mStream = 0;
        mIsFinish = true;
        mIsStarted = false;
        mStop = false;
        mHasError = false;
    }


    /**
     * @brief Show multiple versions of a package available on remotes.
     * The result of this command is the set of remote packages found.
     */
    void actionShow() throw()
    {

        mHasError = false;

        PackageId pkgid;
        pkgid.name = mArgs;

        std::pair < PackagesIdSet::iterator,
                    PackagesIdSet::iterator > found;

        found = remote.equal_range(pkgid);

        if (found.first == remote.end()) {
            std::ostringstream errorStream;
            errorStream << fmt(_("Unknown remote package `%1%'\n")) % mArgs;
            mErrorMessage.assign(errorStream.str());
            mHasError = true;
            return;
        }

        std::copy(found.first,
                  found.second,
                  std::back_inserter(mResults));

        mStream = 0;
        mIsFinish = true;
        mIsStarted = false;
        mStop = false;
    }

    /**
     * @brief Show multiple versions of a package available on remotes.
     * The result of this command is the set of remote packages found.
     */
    void actionLocalShow() throw()
    {

        mHasError = false;

        PackageId pkgid;
        pkgid.name = mArgs;

        std::pair < PackagesIdSet::iterator,
                    PackagesIdSet::iterator > found;

        found = local.equal_range(pkgid);

        if (found.first == remote.end()) {
            std::ostringstream errorStream;
            errorStream << fmt(_("Unknown local package `%1%'\n")) % mArgs;
            mErrorMessage.assign(errorStream.str());
            mHasError = true;
            return;
        }

        std::copy(found.first,
                  found.second,
                  std::back_inserter(mResults));

        mStream = 0;
        mIsFinish = true;
        mIsStarted = false;
        mStop = false;
    }

    PackagesIdSet local;
    PackagesIdSet remote;
    /**
     * @brief The set of packages resulting form the last command
     */
    Packages mResults;
    boost::mutex mMutex;
    boost::thread mThread;
    std::string mArgs;
    std::ostream* mStream;
    bool mIsStarted;
    bool mIsFinish;
    bool mStop;
    bool mHasError;
    std::string mErrorMessage;

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

void RemoteManager::getResult(Packages *out)
{
    if (out) {
        out->clear();
        out->reserve(mPimpl->mResults.size());

        std::copy(mPimpl->mResults.begin(),
                  mPimpl->mResults.end(),
                  std::back_inserter(*out));
    }
}

bool RemoteManager::hasError()
{
    return mPimpl->mHasError;
}

const std::string& RemoteManager::messageError()
{
    return mPimpl->mErrorMessage;
}

std::string RemoteManager::getLocalPackageFilename()
{
    return utils::Path::path().buildFilename(
            utils::Path::path().getBinaryPackagesDir(),
            "local.pkg");
}

std::string RemoteManager::getRemotePackageFilename()
{
    return utils::Path::path().buildFilename(
            utils::Path::path().getBinaryPackagesDir(),
            "remote.pkg");
}

}} // namespace vle utils
