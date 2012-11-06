/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2012 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2012 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2012 INRA http://www.inra.fr
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
#include <boost/thread/thread.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <curl/curl.h>
#include <cstring>
#include <fstream>

namespace vle { namespace utils {

struct WrittenData
{
    std::ofstream *output;
    size_t        *size;
};

static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
    WrittenData *wd = reinterpret_cast < WrittenData* >(userp);

    if (buffer) {
        wd->output->write((const char *)buffer, size * nmemb);
        (*wd->size) += (size * nmemb);
    }

    return size * nmemb;
}

class DownloadManager::Pimpl
{
public:
    Pimpl()
        : mHandle(NULL), mUrl(), mFilename(), mErrorMessage(),
          mDownloadManageredSize(0), mIsStarted(false),
          mIsFinish(false), mHasError(false)
    {
    }

    ~Pimpl()
    {
        join();
    }

    void start(const std::string& url)
    {
        if (mHandle or mIsStarted) {
            throw utils::InternalError(
                fmt(_("Download manager: already start")));
        }

        mUrl.assign(url);
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
        if (mHandle or not mIsStarted) {
            throw utils::InternalError(
                fmt(_("Download manager: handle already initialized")));
        }

        if (not (mHandle = curl_easy_init())) {
            throw utils::InternalError(
                fmt(_("Download manager: fail to initialized handle")));
        }

        std::ofstream file;
        std::string filename(utils::Path::getTempFile("vle-remote-", &file));

        WrittenData wd;
        wd.output = &file;
        wd.size = &mDownloadManageredSize;

        if (filename.empty() or not file.is_open()) {
            throw utils::InternalError(
                fmt(_("Download manager: fail to build temporary file")));
        }

        {
            boost::mutex::scoped_lock lock(mMutex);
            mFilename.assign(filename);
        }

        curl_easy_setopt(mHandle, CURLOPT_URL, mUrl.c_str());
        curl_easy_setopt(mHandle, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(mHandle, CURLOPT_WRITEDATA,
                         reinterpret_cast < void* >(&wd));

        if (utils::Trace::isInLevel(utils::TRACE_LEVEL_DEVS)) {
            curl_easy_setopt(mHandle, CURLOPT_VERBOSE, 1);
            curl_easy_setopt(mHandle, CURLOPT_NOPROGRESS, 1);
        }

        CURLcode result = curl_easy_perform(mHandle);

        if (result) {
            mHasError = true;
            mErrorMessage = curl_easy_strerror(result);
        }

        curl_easy_cleanup(mHandle);
    }

    boost::mutex mMutex;
    boost::thread mThread;
    CURL *mHandle;
    std::string mUrl;
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

void DownloadManager::start(const std::string& url)
{
    mPimpl->start(url);
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
