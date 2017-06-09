/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2017 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2017 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2017 INRA http://www.inra.fr
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

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/cast.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>
#include <iostream>
#include <ostream>
#include <regex>
#include <string>
#include <thread>
#include <vle/utils/Algo.hpp>
#include <vle/utils/ContextPrivate.hpp>
#include <vle/utils/DownloadManager.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Filesystem.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/RemoteManager.hpp>
#include <vle/utils/Spawn.hpp>
#include <vle/utils/details/Package.hpp>
#include <vle/utils/details/PackageManager.hpp>
#include <vle/utils/details/PackageParser.hpp>
#include <vle/utils/i18n.hpp>

#define PACKAGESID_VECTOR_RESERVED_SIZE 100u

namespace vle {
namespace utils {

static std::ostream&
operator<<(std::ostream& os, const PackageLinkId& b)
{
    static const char* sym[5] = { "=", "<", "<=", ">", ">=" };

    os << b.name;

    if (b.major >= 0) {
        assert((unsigned)b.op < 5);

        os << " (" << sym[static_cast<unsigned>(b.op)] << " " << b.major;

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

static std::ostream&
operator<<(std::ostream& os, const PackagesLinkId& b)
{
    utils::formatCopy(
      b.begin(), b.end(), os, ", ", std::string(), std::string());

    return os;
}

std::ostream&
operator<<(std::ostream& os, const PackageId& b)
{
    os << "Package: " << b.name << "\n"
       << "Version: " << b.major << "." << b.minor << "." << b.patch << "\n"
       << "Depends: " << b.depends << "\n"
       << "Build-Depends: " << b.builddepends << "\n"
       << "Conflicts: " << b.conflicts << "\n"
       << "Maintainer: " << b.maintainer << "\n"
       << "Description: " << b.description << "\n"
       << " ."
       << "\n"
       << "Tags: ";

    utils::formatCopy(
      b.tags.begin(), b.tags.end(), os, ", ", std::string(), std::string());

    return os << "\n"
              << "Url: " << b.url << "\n"
              << "Size: " << b.size << "\n"
              << "MD5sum: " << b.md5sum << "\n";
}

class RemoteManager::Pimpl
{
public:
    Pimpl(ContextPtr ctx)
      : mContext(ctx)
      , mStream(nullptr)
      , mIsStarted(false)
      , mIsFinish(false)
      , mStop(false)
      , mHasError(false)
    {
        {
            Packages tmp;

            LocalPackageManager::rebuild(mContext, &tmp);
            local.insert(tmp.begin(), tmp.end());
        }

        {
            Packages tmp;

            RemotePackageManager::extract(mContext, &tmp);
            remote.insert(tmp.begin(), tmp.end());
        }
    }

    ~Pimpl()
    {
        try {
            join();
            save();
        } catch (...) {
        }
    }

    void start(RemoteManagerActions action,
               const std::string& arg,
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
                mThread =
                  std::thread(&RemoteManager::Pimpl::actionUpdate, this);
                break;
            case REMOTE_MANAGER_SOURCE:
                mThread =
                  std::thread(&RemoteManager::Pimpl::actionSource, this);
                break;
            case REMOTE_MANAGER_INSTALL:
                mThread =
                  std::thread(&RemoteManager::Pimpl::actionInstall, this);
                break;
            case REMOTE_MANAGER_LOCAL_SEARCH:
                mThread =
                  std::thread(&RemoteManager::Pimpl::actionLocalSearch, this);
                break;
            case REMOTE_MANAGER_SEARCH:
                mThread =
                  std::thread(&RemoteManager::Pimpl::actionSearch, this);
                break;
            case REMOTE_MANAGER_SHOW:
                mThread = std::thread(&RemoteManager::Pimpl::actionShow, this);
                break;
            case REMOTE_MANAGER_LOCAL_SHOW:
                mThread =
                  std::thread(&RemoteManager::Pimpl::actionLocalShow, this);
                break;
            default:
                break;
            }
        }
    }

    void join() noexcept
    {
        try {
            if (mIsStarted and not mIsFinish and mThread.joinable()) {
                mThread.join();
            }
        } catch (const std::exception& e) {
            vErr(mContext,
                 _("Remote: internal error joining thread:`%s'\n"),
                 e.what());
        }
    }

    void stop()
    {
        if (mIsStarted and not mIsFinish)
            mStop = true;
    }

    /**
     * Send the parameter of the template function \c t to
     *
     * @param t
     */
    template <typename T>
    void out(const T& t)
    {
        if (mStream)
            *mStream << t;
    }

    /**
     * A functor to check if a @c Packages::value_type corresponds to the
     * regular expression provided in constructor.
     */
    struct NotHaveExpression : std::unary_function<PackageId, bool>
    {
        const std::regex& expression;

        NotHaveExpression(const std::regex& expression)
          : expression(expression)
        {
        }

        bool operator()(const PackageId& pkg) const
        {
            std::match_results<std::string::const_iterator> what;

            if (std::regex_search(pkg.name,
                                  what,
                                  expression,
                                  std::regex_constants::match_default)) {
                return false;
            }

            if (std::regex_search(pkg.description,
                                  what,
                                  expression,
                                  std::regex_constants::match_default)) {
                return false;
            }

            return true;
        }
    };

    void save() const noexcept
    {
        try {
            std::ofstream file;
            file.exceptions(std::ios_base::eofbit | std::ios_base::failbit |
                            std::ios_base::badbit);

            file.open(mContext->getLocalPackageFilename().string());
            file << local << std::endl;
        } catch (const std::exception& e) {
            vErr(mContext,
                 _("Remote: failed to write local package `%s': %s\n"),
                 mContext->getLocalPackageFilename().string().c_str(),
                 e.what());
        }

        try {
            std::ofstream file;
            file.exceptions(std::ios_base::eofbit | std::ios_base::failbit |
                            std::ios_base::badbit);

            file.open(mContext->getRemotePackageFilename().string());
            file << remote << std::endl;
        } catch (const std::exception& e) {
            vErr(mContext,
                 _("Remote: failed to write remote package `%s': "
                   "%s\n"),
                 mContext->getRemotePackageFilename().string().c_str(),
                 e.what());
        }
    }

    //
    // threaded slot
    //

    struct Download : public std::unary_function<std::string, void>
    {
        ContextPtr context;
        PackageParser* parser;
        bool* remoteHasError;
        std::string* remoteMessageError;

        Download(ContextPtr ctx,
                 PackageParser* parser,
                 bool* remoteHasError,
                 std::string* remoteMessageError)
          : context(ctx)
          , parser(parser)
          , remoteHasError(remoteHasError)
          , remoteMessageError(remoteMessageError)
        {
            assert(parser);
        }

        void operator()(std::string url) const
        {
            if (url.empty()) {
                return;
            }
            DownloadManager dl(context);

            Path filename(Path::temp_directory_path());
            filename /= "vledl-%%%%%%%%.pkg";
            filename = Path::unique_path(filename.string());

            dl.start(url + "/packages.pkg", filename.string());
            dl.join();

            if (not dl.hasError()) {
                try {
                    parser->extract(filename.string(), url);
                } catch (const utils::FileError& err) {
                    *remoteHasError = true;
                    remoteMessageError->assign(
                      (vle::fmt(
                         "failed while parsing packages.pkg file: %1%\n") %
                       err.what())
                        .str());
                }
            } else {
                if (!(*remoteHasError)) {
                    *remoteHasError = true;
                    remoteMessageError->assign(
                      (vle::fmt("failed: %1%\n") % dl.getErrorMessage())
                        .str());
                }
            }
        }
    };

    /**
     * @brief Update the remote.pkg and local.pkg files,
     * the result of this command is the set of packages for which a new
     * version
     * is available on remotes.
     */
    void actionUpdate() throw()
    {
        mHasError = false;

        std::vector<std::string> urls;
        try {
            std::string tmp;
            mContext->get_setting("vle.remote.url", &tmp);

            boost::algorithm::split(urls,
                                    tmp,
                                    boost::algorithm::is_any_of(","),
                                    boost::algorithm::token_compress_on);
        } catch (const std::exception& /*e*/) {
            std::ostringstream errorStream;
            errorStream << _("Failed to read preferences file");
            mErrorMessage.assign(errorStream.str());
            mHasError = true;
            return;
        }

        PackageParser parser;
        std::for_each(urls.begin(),
                      urls.end(),
                      Download(mContext, &parser, &mHasError, &mErrorMessage));
        remote.clear();
        {
            PackageParser::const_iterator itb = parser.begin();
            PackageParser::const_iterator ite = parser.end();
            for (; itb != ite; itb++) {
                remote.insert(*itb);
            }
        }
        {
            PackagesIdSet::const_iterator itlb = local.begin();
            PackagesIdSet::const_iterator itle = local.end();
            bool found;
            PackageIdUpdate pkgUpdateOp;
            std::pair<PackagesIdSet::const_iterator,
                      PackagesIdSet::const_iterator>
              pkgs_range;

            for (; itlb != itle; itlb++) {
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

        mStream = nullptr;
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
        std::pair<PackagesIdSet::const_iterator, PackagesIdSet::const_iterator>
          pkgs_range;
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
            out(fmt(_("Package `%1%` is already uptodate\n")) % pkgid.name);
            return;
        }

        DownloadManager dl(mContext);
        std::string url = it->url + "/";
        out(fmt(_("Download archive  '%1%' from '%2%': ")) % archname % url);

        Path archfile(Path::temp_directory_path());
        archfile /= archname;
        dl.start(url + archname, archfile.string());
        dl.join();
        if (not dl.hasError()) {
            out(fmt(_("ok\n")));

            Path tempDir(Path::temp_directory_path());
            Path archpath(archfile);
            Path dearchpath(tempDir);

            {
                FScurrent_path_restore restore(tempDir);
                decompress(archpath.string(), dearchpath.string());
                vle::utils::Package pkg(mContext, pkgid.name);
                out(fmt(_("Building package '%1%' into '%2%': ")) %
                    pkgid.name % dearchpath.string());
                pkg.configure();
                pkg.wait(std::cerr, std::cerr);
                pkg.build();
                pkg.wait(std::cerr, std::cerr);
                pkg.install();
                pkg.wait(std::cerr, std::cerr);
                out(fmt(_("ok\n")));
            }
            mResults.push_back(*it);
        } else {
            out(fmt(_("failed\n")));
            std::ostringstream errorStream;
            errorStream << fmt(_("Error while downloading package "
                                 "`%1%'\n")) %
                             mArgs;
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
            DownloadManager dl(mContext);
            std::string url = it->url;
            std::string archname = mArgs;
            archname.append(".tar.bz2");
            url.append("/");
            url.append(archname);
            dl.start(url, archname);
            dl.join();
            if (not dl.hasError()) {
                std::string filename = dl.filename();
                Path::rename(filename, archname);
            } else {
                std::ostringstream errorStream;
                errorStream << fmt(_("Error while downloading "
                                     "source package `%1%'\n")) %
                                 mArgs;
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
        mStream = nullptr;
        mIsFinish = true;
        mIsStarted = false;
        mStop = false;
    }

    /**
     * @brief Search for packages installed on local machine. The result of
     * this command is the set of packages found.
     */
    void actionLocalSearch() throw()
    {
        std::regex expression(mArgs, std::regex::grep);

        std::remove_copy_if(local.begin(),
                            local.end(),
                            std::back_inserter(mResults),
                            NotHaveExpression(expression));

        mStream = nullptr;
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
        std::regex expression(mArgs, std::regex::grep);

        std::remove_copy_if(remote.begin(),
                            remote.end(),
                            std::back_inserter(mResults),
                            NotHaveExpression(expression));

        mStream = nullptr;
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

        std::pair<PackagesIdSet::iterator, PackagesIdSet::iterator> found;

        found = remote.equal_range(pkgid);

        if (found.first == remote.end()) {
            std::ostringstream errorStream;
            errorStream << fmt(_("Unknown remote package `%1%'\n")) % mArgs;
            mErrorMessage.assign(errorStream.str());
            mHasError = true;
            return;
        }

        std::copy(found.first, found.second, std::back_inserter(mResults));

        mStream = nullptr;
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

        std::pair<PackagesIdSet::iterator, PackagesIdSet::iterator> found;

        found = local.equal_range(pkgid);

        if (found.first == remote.end()) {
            std::ostringstream errorStream;
            errorStream << fmt(_("Unknown local package `%1%'\n")) % mArgs;
            mErrorMessage.assign(errorStream.str());
            mHasError = true;
            return;
        }

        std::copy(found.first, found.second, std::back_inserter(mResults));

        mStream = nullptr;
        mIsFinish = true;
        mIsStarted = false;
        mStop = false;
    }

    void compress(const Path& filepath, const Path& compressedfilepath)
    {
        if (not filepath.exists())
            throw utils::InternalError(
              (fmt(_("fail to compress '%1%': file or directory does not "
                     "exist")) %
               filepath.string())
                .str());

        Path pwd = Path::current_path();
        std::string command;

        try {
            mContext->get_setting("vle.command.tar", &command);
            command = (vle::fmt(command) % compressedfilepath.string() %
                       filepath.string())
                        .str();

            utils::Spawn spawn(mContext);
            std::vector<std::string> argv = spawn.splitCommandLine(command);
            std::string exe = std::move(argv.front());
            argv.erase(argv.begin());

            if (not spawn.start(exe, pwd.string(), argv))
                throw utils::InternalError(_("fail to start cmake command"));

            std::string output, error;
            while (not spawn.isfinish()) {
                if (spawn.get(&output, &error)) {
                    std::cout << output;
                    std::cerr << error;

                    output.clear();
                    error.clear();

                    std::this_thread::sleep_for(
                      std::chrono::microseconds(200));
                } else
                    break;
            }

            spawn.wait();

            std::string message;
            bool success;
            spawn.status(&message, &success);

            if (not message.empty())
                vErr(mContext, "Compress: %s\n", message.c_str());
        } catch (const std::exception& e) {
            vErr(mContext,
                 _("Compress: unable to compress '%s' in '%s' with "
                   "the '%s' command\n"),
                 compressedfilepath.string().c_str(),
                 pwd.string().c_str(),
                 command.c_str());
        }
    }

    void decompress(const Path& compressedfilepath, const Path& directorypath)
    {
        if (not directorypath.is_directory())
            throw utils::InternalError(
              (fmt(_("fail to extract '%1%' in directory '%2%': "
                     " directory does not exist")) %
               compressedfilepath.string() % directorypath.string())
                .str());

        if (not compressedfilepath.is_file())
            throw utils::InternalError(
              (fmt(_("fail to extract '%1%' in directory '%2%': "
                     "file does not exist")) %
               compressedfilepath.string() % directorypath.string())
                .str());

        Path pwd = Path::current_path();

        std::string command;
        try {
            mContext->get_setting("vle.command.untar", &command);
            command = (vle::fmt(command) % compressedfilepath.string()).str();

            utils::Spawn spawn(mContext);
            std::vector<std::string> argv = spawn.splitCommandLine(command);
            std::string exe = std::move(argv.front());
            argv.erase(argv.begin());

            if (not spawn.start(exe, directorypath.string(), argv))
                throw utils::InternalError(_("fail to start cmake command"));

            std::string output, error;
            while (not spawn.isfinish()) {
                if (spawn.get(&output, &error)) {
                    std::cout << output;
                    std::cerr << error;

                    output.clear();
                    error.clear();

                    std::this_thread::sleep_for(
                      std::chrono::microseconds(200));
                } else
                    break;
            }

            spawn.wait();

            std::string message;
            bool success;
            spawn.status(&message, &success);

            if (not message.empty())
                vErr(mContext, "Decompress: %s\n", message.c_str());
        } catch (const std::exception& e) {
            vErr(mContext,
                 _("Decompress: unable to decompress '%s' "
                   "in '%s' with the '%s' command\n"),
                 compressedfilepath.string().c_str(),
                 pwd.string().c_str(),
                 command.c_str());
        }
    }

    PackagesIdSet local;
    PackagesIdSet remote;

    ContextPtr mContext;
    Packages mResults;
    std::thread mThread;
    std::string mArgs;
    std::ostream* mStream;
    bool mIsStarted;
    bool mIsFinish;
    bool mStop;
    bool mHasError;
    std::string mErrorMessage;
};

RemoteManager::RemoteManager(ContextPtr context)
  : mPimpl(std::make_unique<RemoteManager::Pimpl>(context))
{
}

RemoteManager::~RemoteManager() noexcept
{
}

void
RemoteManager::start(RemoteManagerActions action,
                     const std::string& arg,
                     std::ostream* os)
{
    mPimpl->start(action, arg, os);
}

void
RemoteManager::join()
{
    mPimpl->join();
}

void
RemoteManager::stop()
{
    mPimpl->stop();
}

void
RemoteManager::getResult(Packages* out)
{
    if (out) {
        out->clear();
        out->reserve(mPimpl->mResults.size());

        std::copy(mPimpl->mResults.begin(),
                  mPimpl->mResults.end(),
                  std::back_inserter(*out));
    }
}

bool
RemoteManager::hasError()
{
    return mPimpl->mHasError;
}

const std::string&
RemoteManager::messageError()
{
    return mPimpl->mErrorMessage;
}

void
RemoteManager::compress(const Path& filepath, const Path& compressedfilepath)
{
    mPimpl->compress(filepath, compressedfilepath);
}

void
RemoteManager::decompress(const Path& compressedfilepath,
                          const Path& directorypath)
{
    mPimpl->decompress(compressedfilepath, directorypath);
}
}
} // namespace vle utils
