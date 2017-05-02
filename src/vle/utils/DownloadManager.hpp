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

#ifndef VLE_UTILS_DOWNLOAD_MANAGER_HPP
#define VLE_UTILS_DOWNLOAD_MANAGER_HPP

#include <memory>
#include <string>
#include <vle/DllDefines.hpp>
#include <vle/utils/Context.hpp>
#include <vle/utils/Types.hpp>

namespace vle {
namespace utils {

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
    DownloadManager(ContextPtr ctx);

    /**
     * @brief Join the alive thread is if exists.
     */
    ~DownloadManager() noexcept;

    DownloadManager(const DownloadManager&) = delete;
    DownloadManager& operator=(const DownloadManager&) = delete;
    DownloadManager(DownloadManager&&) = delete;
    DownloadManager& operator=(DownloadManager&&) = delete;

    /**
     * Start the download of the specified \c url in a thread. If a
     * previous thread is already alive, this function does nothing.
     *
     * @param url The url that pointe to the object.
     * @param filepath The file where the resources will be store.
     */
    void start(const std::string& url, const std::string& filepath);

    /**
     * A blocking function while the download is not finish.
     */
    void join();

    //
    // Get/Set activities
    //

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
    struct Pimpl;
    std::unique_ptr<Pimpl> mPimpl;
};
}
} // namespace vle utils

#endif
