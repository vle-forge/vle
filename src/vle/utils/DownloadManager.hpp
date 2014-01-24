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

#ifndef VLE_UTILS_DOWNLOAD_MANAGER_HPP
#define VLE_UTILS_DOWNLOAD_MANAGER_HPP

#include <vle/DllDefines.hpp>
#include <vle/utils/Types.hpp>
#include <string>

namespace vle { namespace utils {

/**
 * @brief An http file download process.
 *
 * \c DownloadManager is thread-safe and multi-threaded class which allows to
 * download files via http protocol. \c DownloadManager can not be
 * copied or assigned to another \c DownloadManager.
 *
 * @code
 * vle::utils::DownloadManager dl;
 * down.start("www.vle-project.org","vle-1.0.0.dtd");
 *
 * while (not down.isFinish()) {
 *     std::cout << "Size...... :" << down.getDownloadedSize();
 *     [...]
 * }
 * std::cout << "Filename.. :" << down.getFilename();
 * down.join();
 *
 * {
 *   vle::utils::DownloadManager dl;
 *   dl.start("http://www.vle-project.org"); // will download index.html
 * }                                         // join() is in destructor.
 * @endcode
 */
class VLE_API DownloadManager
{
public:
    /**
     * @brief Build a DownloadManager.
     */
    DownloadManager();

    /**
     * @brief Join the alive thread is if exists.
     */
    ~DownloadManager();

    /**
     * Start the download of the specified \c url in a thread. If a previous
     * thread is already alive, this function does nothing.
     *
     * Note: there are two possibilities for dwoloading a file :
     * 1) set the protocol and the directory on server into the url and set
     * the serverfile to file name
     * eg: url="http://www.vle-project.org/pub/1.1" and serverfile="packages.pkg"
     * 2) set only the url into the url and the directory/filename into server
     * serverfile.
     * eg: url="www.vle-project.org" and serverfile="pub/1.1/packages"
     *
     * @param url The url of the server (e.g. "www.vle-project.org").
     * @param serverfile The resource to download (e.g. "vle-1.0.0.dtd").
     */
    void start(const std::string& url,
            const std::string& serverfile);

    /**
     * A blocking function while the download is not finish.
     */
    void join();

    //
    // Get/Set activities
    //

    /**
     * Return the downloaded size.
     *
     * @return A size.
     */
    uint32_t size() const;

    /**
     * Return the filename where downloaded file is stored.
     *
     * @return A filename.
     */
    std::string filename() const;

    /**
     * Return true if the download is finish, false otherwise.
     *
     * @return true if the download is finish.
     */
    bool isFinish() const;

    /**
     * Return true if the download has error, false otherwise.
     *
     * @return true if the download has error.
     */
    bool hasError() const;

    /**
     * Get a string representation of the error, empty string is valid.
     *
     * @return An error message.
     */
    std::string getErrorMessage() const;

private:
    DownloadManager(const DownloadManager&);
    DownloadManager& operator=(const DownloadManager&);

    class Pimpl;
    Pimpl *mPimpl;
};

}} // namespace vle utils

#endif
