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

#include <vle/utils/DownloadManager.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/Preferences.hpp>
#include <boost/thread/thread.hpp>
#include <boost/numeric/conversion/cast.hpp>
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
        : mUrl(), mFilename(), mErrorMessage(),
          mDownloadManageredSize(0), mIsStarted(false),
          mIsFinish(false), mHasError(false)
    {
    }

    ~Pimpl()
    {
        join();
    }

    void start(const std::string& url, const std::string& serverfile)
    {
        if (mIsStarted) {
            throw utils::InternalError(
                fmt(_("Download manager: already start")));
        }

        mUrl.assign(url);
        mServerFile.assign("/");
        mServerFile.append(serverfile);
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
        if (not mIsStarted) {
            throw utils::InternalError(
                fmt(_("Download manager: already started")));
        }

        utils::Preferences prefs;
        std::string proxyip;
        prefs.get("vle.remote.proxy_ip", &proxyip);
        std::string proxyport;
        prefs.get("vle.remote.proxy_port", &proxyport);

        if (proxyip.empty()) {
            proxyip.assign(mUrl);
            proxyport.assign("http");
        }

        boost::asio::io_service io_service;
        bip::tcp::resolver resolver(io_service);

        bip::tcp::resolver::query query(proxyip, proxyport);
        bip::tcp::resolver::iterator endpoint_iterator =
                resolver.resolve(query);

        bip::tcp::socket socket(io_service);
        boost::asio::connect(socket, endpoint_iterator);

        boost::asio::streambuf request;
        std::ostream request_stream(&request);
        request_stream << "GET " << mCompletePath << " HTTP/1.1\r\n";
        request_stream << "Host: " << mUrl << "\r\n";
        request_stream << "Accept: */*\r\n";
        request_stream << "Connection: close\r\n\r\n";

        boost::asio::write(socket, request);

        boost::asio::streambuf response;
        boost::asio::read_until(socket, response, "\r\n");

        std::istream response_stream(&response);
        std::string http_version;

        response_stream >> http_version;
        unsigned int status_code;

        response_stream >> status_code;
        std::string status_message;
        std::getline(response_stream, status_message);
        if (!response_stream || http_version.substr(0, 5) != "HTTP/")
        {
            std::ostringstream errorStream;
            errorStream << vle::fmt("[DownloadManager] Invalid response "
                          " while downloading '%1%'") % mCompletePath;
            mErrorMessage.assign(errorStream.str());
            mHasError = true;
        }
        if (mHasError) {
            return ;
        }
        if (status_code != 200)
        {
            std::ostringstream errorStream;
            errorStream << vle::fmt("[DownloadManager] Response returned with "
                    " status code %1%  while downloading '%2%' ")
                    % status_code % mCompletePath;
            mErrorMessage.assign(errorStream.str());
            mHasError = true;
        }
        if (mHasError) {
            return ;
        }
        boost::asio::read_until(socket, response, "\r\n\r\n");

        // Process the response headers.
        std::string header;
        while (std::getline(response_stream, header) && header != "\r") {
            std::string::size_type found = header.find("Content-Length: ");
            if (found != std::string::npos ) {
                std::string sizeString = header.substr(16);
                std::istringstream sizeBuffer(sizeString);
                sizeBuffer >> mDownloadManageredSize;
            }
        }

        std::ofstream file;
        mFilename.assign(utils::Path::getTempFile("vle-dl-", &file));

        if (response.size() > 0) {
            file << &response ;
         }
        boost::system::error_code error;
        while (boost::asio::read(socket, response,
                boost::asio::transfer_at_least(1), error)) {
            file << &response;
        }
        if (error != boost::asio::error::eof) {
            std::ostringstream errorStream;
            errorStream << vle::fmt("[DownloadManager] Unexpected end of"
                    " file while downloading '%1%'") % mCompletePath;
            mErrorMessage.assign(errorStream.str());
            mHasError = true;
        }
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
