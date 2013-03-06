/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2013 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2013 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2013 INRA http://www.inra.fr
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
#include <vle/utils/i18n.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Preferences.hpp>
#include <vle/utils/Trace.hpp>
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

        TraceAlways(fmt(_("Remote manager: %1% local / %2% remote"))
                    % local.size() % remote.size());
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
            mPackages.clear();

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

        Download(PackageParser *parser)
            : parser(parser)
        {
            assert(parser);
        }

        void operator()(const std::string& url) const
        {
            DownloadManager dl;

            dl.start(url, "packages");
            dl.join();

            std::cout << vle::fmt("%1% %2%\n") % url % dl.hasError();

            if (not dl.hasError()) {
                std::cout << vle::fmt("%1% %2%\n") % dl.filename() % url;
                parser->extract(dl.filename(), url);
            } else {
                std::cout << vle::fmt("failed: %1%\n") % dl.getErrorMessage();
            }
        }
    };

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

        PackageParser parser;
        std::for_each(urls.begin(), urls.end(),
                      Download(&parser));

        if (not parser.empty()) {
            std::set_intersection(local.begin(),
                                  local.end(),
                                  parser.begin(),
                                  parser.end(),
                                  std::back_inserter(mPackages),
                                  PackageIdUpdate());
        }

        mStream = 0;
        mIsFinish = true;
        mIsStarted = false;
        mStop = false;
        mHasError = false;
    }

    void actionInstall() throw()
    {
        // const_iterator it = mPackages.find(mArgs);

        // if (it != mPackages.end()) {
        //     std::string url = it->second.getBinaryPackageUrl();

        //     DownloadManager dl;

        //     out(fmt(_("Download binary package `%1%' at %2%")) % mArgs % url);
        //     dl.start(url);
        //     dl.join();

        //     if (not dl.hasError()) {
        //         out(_("install"));
        //         std::string filename = dl.filename();
        //         std::string zipfilename = dl.filename();
        //         zipfilename += ".zip";

        //         boost::filesystem::rename(filename, zipfilename);

        //         utils::Package::package().unzip(mArgs, zipfilename);
        //         utils::Package::package().wait(*mStream, *mStream);
        //         out(_(": ok\n"));
        //     } else {
        //         out(_(": failed\n"));
        //     }
        // } else {
        //     out(fmt(_("Unknown package `%1%'")) % mArgs);
        // }

        // mStream = 0;
        // mIsFinish = true;
        // mIsStarted = false;
        // mStop = false;
        // mHasError = false;
    }

    void actionSource() throw()
    {
        // const_iterator it = mPackages.find(mArgs);

        // if (it != mPackages.end()) {
        //     std::string url = it->second.getSourcePackageUrl();

        //     DownloadManager dl;

        //     out(fmt(_("Download source package `%1%' at %2%")) % mArgs % url);
        //     dl.start(url);
        //     dl.join();

        //     if (not dl.hasError()) {
        //         out(_("install"));
        //         std::string filename = dl.filename();
        //         std::string zipfilename = dl.filename();
        //         zipfilename += ".zip";

        //         boost::filesystem::rename(filename, zipfilename);

        //         utils::Package::package().unzip(mArgs, zipfilename);
        //         utils::Package::package().wait(*mStream, *mStream);
        //         out(_(": ok\n"));
        //     } else {
        //         out(_(": failed\n"));
        //     }
        // } else {
        //     out(fmt(_("Unknown package `%1%'")) % mArgs);
        // }

        // mStream = 0;
        // mIsFinish = true;
        // mIsStarted = false;
        // mStop = false;
        // mHasError = false;
    }

    void actionLocalSearch() throw ()
    {
        boost::regex expression(mArgs, boost::regex::grep);

        std::remove_copy_if(local.begin(),
                            local.end(),
                            std::back_inserter(mPackages),
                            NotHaveExpression(expression));

        mStream = 0;
        mIsFinish = true;
        mIsStarted = false;
        mStop = false;
        mHasError = false;
    }

    void actionSearch() throw()
    {
        boost::regex expression(mArgs, boost::regex::grep);

        std::remove_copy_if(remote.begin(),
                            remote.end(),
                            std::back_inserter(mPackages),
                            NotHaveExpression(expression));

        mStream = 0;
        mIsFinish = true;
        mIsStarted = false;
        mStop = false;
        mHasError = false;
    }

    void actionShow() throw()
    {
        std::vector < std::string > args;

        boost::algorithm::split(args, mArgs,
                                boost::algorithm::is_any_of(" "),
                                boost::algorithm::token_compress_on);

        std::vector < std::string >::iterator it, end;

        for (it = args.begin(), end = args.end(); it != end; ++it) {
            PackageId tmp;

            tmp.name = *it;

            std::pair < PackagesIdSet::iterator,
                PackagesIdSet::iterator > found;

            found = local.equal_range(tmp);

            std::copy(found.first,
                      found.second,
                      std::back_inserter(mPackages));

            found = remote.equal_range(tmp);

            std::copy(found.first,
                      found.second,
                      std::back_inserter(mPackages));
        }

        mStream = 0;
        mIsFinish = true;
        mIsStarted = false;
        mStop = false;
        mHasError = false;
    }

    PackagesIdSet local;
    PackagesIdSet remote;
    Packages mPackages;
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

void RemoteManager::getResult(Packages *out)
{
    if (out) {
        out->clear();
        out->reserve(mPimpl->mPackages.size());

        std::copy(mPimpl->mPackages.begin(),
                  mPimpl->mPackages.end(),
                  std::back_inserter(*out));
    }
}

std::string RemoteManager::getLocalPackageFilename()
{
    return utils::Path::path().getHomeFile("local.pkg");
}

std::string RemoteManager::getRemotePackageFilename()
{
    return utils::Path::path().getHomeFile("remote.pkg");
}

}} // namespace vle utils
