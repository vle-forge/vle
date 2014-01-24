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

#include <vle/utils/DownloadManager.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/Preferences.hpp>
#include <vle/utils/i18n.hpp>
#include <boost/thread/thread.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/asio.hpp>
#include <cstring>
#include <fstream>
#include <sstream>


namespace bip = boost::asio::ip;

namespace vle { namespace utils {

class DownloadManager::Pimpl
{
public:
    Pimpl()
        : mMutex(), mThread(), mUrl(), mServerFile(), mCompletePath(),
          mFilename(), mErrorMessage(), mDownloadManageredSize(0),
          mIsStarted(false), mIsFinish(false), mHasError(false)
    {
    }

    ~Pimpl()
    {
        join();
    }

    void start(const std::string& url, const std::string& serverfile)
    {
        if (mIsStarted) {
            mHasError = true;
            mErrorMessage.assign("Download manager: already started");
            return;
        }
        if ( url.substr(0,7) != "http://") {
            mUrl.assign(url);
            mServerFile.assign("/");
            mServerFile.append(serverfile);
        } else {
            mUrl.assign(url.substr(7));
            std::vector < std::string > subUrl;
            boost::algorithm::split(subUrl, mUrl,
                    boost::algorithm::is_any_of("/"),
                    boost::algorithm::token_compress_on);
            mUrl.assign(subUrl[0]);
            mServerFile.assign("");
            for (unsigned int i=1; i<subUrl.size(); i++) {
                mServerFile.append("/");
                mServerFile.append(subUrl[i]);
            }
            mServerFile.append("/");
            mServerFile.append(serverfile);

        }

        mHasError = false;
        mErrorMessage.assign("");
        mCompletePath.assign("http://");
        mCompletePath.append(mUrl);
        mCompletePath.append(mServerFile);
        mIsStarted = true;
        mThread = boost::thread(&DownloadManager::Pimpl::run, this);
    }

    void join()
    {
        if (mIsStarted) {
            if (not mIsFinish) {
                mThread.join();
                mIsFinish = true;
            }
        }
    }

    void run()
    {
        std::string proxyip;
        std::string proxyport;
        boost::asio::io_service io_service;
        bip::tcp::resolver resolver(io_service);
        bip::tcp::resolver::iterator endpoint_iterator;
        bip::tcp::resolver::iterator end;
        bip::tcp::socket socket(io_service);
        boost::asio::streambuf request;
        std::ostream request_stream(&request);
        boost::asio::streambuf response;
        std::istream response_stream(&response);
        std::string http_version;
        std::string header;
        std::ofstream file;
        boost::system::error_code error = boost::asio::error::host_not_found;
        unsigned int status_code;
        std::string status_message;
        std::ostringstream errorStream;

        if (not mIsStarted) {
            mHasError = true;
            mErrorMessage.assign("[DownloadManager] Download manager is not"
                    " initialized");
            goto goto_flag_error;
        }

        try {
            utils::Preferences prefs;
            prefs.get("vle.remote.proxy_ip", &proxyip);
            prefs.get("vle.remote.proxy_port", &proxyport);
            if (proxyip.empty()) {
                proxyip.assign(mUrl);
                proxyport.assign("http");
            }
        } catch (const std::exception& e) {
            errorStream << "[DownloadManager] Error while reading the"
                    " preferences file";
            goto goto_flag_error;
        }

        try {

            bip::tcp::resolver::query query(proxyip, proxyport);
            endpoint_iterator = resolver.resolve(query);
            while (error and endpoint_iterator != end) {
                socket.close();
                socket.connect(*endpoint_iterator++, error);
            }
        } catch (const std::exception& e) {
            errorStream << vle::fmt("[DownloadManager]  Error while resolving"
                    " the host %1%, %2% ") % proxyip % proxyport ;
            goto goto_flag_error;
        }

        try {
            request_stream << "GET " << mCompletePath << " HTTP/1.1\r\n";
            request_stream << "Host: " << mUrl << "\r\n";
            request_stream << "Accept: */*\r\n";
            request_stream << "Connection: close\r\n\r\n";

            boost::asio::write(socket, request);
            boost::asio::read_until(socket, response, "\r\n");

            response_stream >> http_version;
            response_stream >> status_code;

            std::getline(response_stream, status_message);
        } catch (const std::exception& e) {
            errorStream << vle::fmt("[DownloadManager] Error while getting"
                            "the status_message");
            goto goto_flag_error;
        }

        if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
            errorStream << vle::fmt("[DownloadManager] Invalid response while"
                    " downloading '%1%' (status message = '%2%'")
                    % mCompletePath % status_message;
            goto goto_flag_error;
        }
        if (status_code != 200) {
            errorStream << vle::fmt("[DownloadManager] Response returned with "
                    " status code '%1%', (status message = '%2%') ")
                     % status_code % status_message;
            goto goto_flag_error;
        }

        try {
            boost::asio::read_until(socket, response, "\r\n\r\n");
            while (std::getline(response_stream, header) && header != "\r") {
                std::string::size_type found = header.find("Content-Length: ");
                if (found != std::string::npos ) {
                    std::string sizeString = header.substr(16);
                    std::istringstream sizeBuffer(sizeString);
                    sizeBuffer >> mDownloadManageredSize;
                }
            }
        } catch (const std::exception& e) {
            errorStream << vle::fmt("[DownloadManager] Error while processing"
                    " the response header (header = '%1%')") % header;
            goto goto_flag_error;
        }

        try {
            mFilename.assign(utils::Path::getTempFile("vle-dl-", &file));
            if (response.size() > 0) {
                file << &response ;
            }
            while (boost::asio::read(socket, response,
                    boost::asio::transfer_at_least(1), error)) {
                file << &response;
            }
            if (error != boost::asio::error::eof) {
                errorStream << vle::fmt("[DownloadManager] Unexpected end of"
                        " file while downloading '%1%'") % mCompletePath;
                goto goto_flag_error;
            }
        } catch (const std::exception& e) {
            errorStream << "[DownloadManager] Error while filling the file";
            goto goto_flag_error;
        }
        return;

        goto_flag_error:
            mHasError = true;
            mErrorMessage.assign(errorStream.str());
            file.close();
    }

    boost::mutex mMutex;
    boost::thread mThread;
    std::string mUrl;
    std::string mServerFile;
    std::string mCompletePath;
    std::string mFilename;
    std::string mErrorMessage;
    size_t mDownloadManageredSize;
    bool mIsStarted;
    bool mIsFinish;
    bool mHasError;

private:
    Pimpl(const Pimpl&);
    Pimpl& operator=(const Pimpl&);
};

DownloadManager::DownloadManager()
    : mPimpl(new DownloadManager::Pimpl())
{
}

DownloadManager::~DownloadManager()
{
    delete mPimpl;
}

void DownloadManager::start(const std::string& url, const std::string&
        serverfile)
{
    mPimpl->start(url, serverfile);
}

void DownloadManager::join()
{
    mPimpl->join();
}

uint32_t DownloadManager::size() const
{
    if (mPimpl->mIsStarted and not mPimpl->mIsFinish) {
        boost::mutex::scoped_lock lock(mPimpl->mMutex);
        return mPimpl->mDownloadManageredSize;
    }
    return mPimpl->mDownloadManageredSize;
}

std::string DownloadManager::filename() const
{
    if (mPimpl->mIsStarted and not mPimpl->mIsFinish) {
        boost::mutex::scoped_lock lock(mPimpl->mMutex);
        return mPimpl->mFilename;
    }
    return mPimpl->mFilename;
}

bool DownloadManager::isFinish() const
{
    if (mPimpl->mIsStarted and not mPimpl->mIsFinish) {
        boost::mutex::scoped_lock lock(mPimpl->mMutex);
        return mPimpl->mIsFinish;
    }
    return mPimpl->mIsFinish;
}

bool DownloadManager::hasError() const
{
    if (mPimpl->mIsStarted and not mPimpl->mIsFinish) {
        boost::mutex::scoped_lock lock(mPimpl->mMutex);
        return mPimpl->mHasError;
    }
    return mPimpl->mHasError;
}

std::string DownloadManager::getErrorMessage() const
{
    if (mPimpl->mIsStarted and not mPimpl->mIsFinish) {
        boost::mutex::scoped_lock lock(mPimpl->mMutex);
        return mPimpl->mErrorMessage;
    }
    return mPimpl->mErrorMessage;
}

}} // namespace vle utils
