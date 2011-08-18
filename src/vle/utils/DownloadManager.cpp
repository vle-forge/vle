/*
 * @file vle/utils/DownloadManager.cpp
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

#include <vle/utils/DownloadManager.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Path.hpp>
#include <boost/thread/thread.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <libxml/nanohttp.h>
#include <libxml/tree.h>
#include <cstring>
#include <fstream>

namespace vle { namespace utils {

class DownloadManager::Pimpl
{
public:
    Pimpl()
        : mUrl(), mFilename(), mErrorMessage(), mDownloadManageredSize(0),
        mIsStarted(false), mIsFinish(false), mHasError(false)
    {
    }

    ~Pimpl()
    {
        join();
    }

    void start(const std::string& url)
    {
        mUrl.assign(url);

        if (not mIsStarted) {
            mIsStarted = true;
            mThread = boost::thread(&DownloadManager::Pimpl::run, this);
        }
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
        xmlNanoHTTPInit();

        void* ctxt = xmlNanoHTTPMethod(mUrl.c_str(), "GET", NULL, NULL, NULL,
                                       0);

        if (not ctxt) {
            boost::mutex::scoped_lock lock(mMutex);
            mHasError = true;
            mErrorMessage = (fmt(
                    _("Remote access: no response from server `%1%'")) %
                mUrl).str();
            return;
        }

        int code = xmlNanoHTTPReturnCode(ctxt);
        if (code != 200) {
            boost::mutex::scoped_lock lock(mMutex);
            mHasError = true;
            mErrorMessage = (fmt(
                    _("Remote access: server `%1%' return bad access `%2%'")) %
                mUrl % code).str();
            return;
        }

        xmlBuffer* output = xmlBufferCreate();
        xmlChar buf[1024];
        int len;

        while ((len = xmlNanoHTTPRead(ctxt, buf, sizeof(buf))) > 0) {
            if(xmlBufferAdd(output, buf, len) != 0) {
                xmlNanoHTTPClose(ctxt);
                xmlBufferFree(output);

                boost::mutex::scoped_lock lock(mMutex);
                mHasError = true;
                mErrorMessage = (fmt(
                        _("Remote access: error during download at bytes %1%"))
                    % (xmlBufferLength(output))).str();
                return;
            }
        }

        xmlNanoHTTPClose(ctxt);
        xmlBufferFree(output);
        xmlNanoHTTPCleanup();

        std::ofstream file;
        std::string filename(utils::Path::getTempFile("vle-remote-", &file));

        if (not filename.empty() and file.is_open()) {
            {
                boost::mutex::scoped_lock lock(mMutex);
                mFilename.assign(filename);
            }

            file.write(
                reinterpret_cast < const char* >(xmlBufferContent(output)),
                xmlBufferLength(output));
            xmlNanoHTTPClose(ctxt);
            xmlBufferFree(output);
            mHasError = false;
        } else {
            xmlNanoHTTPClose(ctxt);
            xmlBufferFree(output);
            boost::mutex::scoped_lock lock(mMutex);
            mHasError = true;
            mErrorMessage = (fmt(
                    _("Remote access: failed to store in file `%1%'")) %
                filename).str();
        }
    }

    boost::mutex mMutex;
    boost::thread mThread;
    std::string mUrl;
    std::string mFilename;
    std::string mErrorMessage;
    uint32_t mDownloadManageredSize;
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
