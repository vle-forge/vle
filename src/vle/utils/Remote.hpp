/**
 * @file vle/utils/Remote.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
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


#ifndef VLE_UTILS_REMOTE_HPP
#define VLE_UTILS_REMOTE_HPP 1

#include <vle/utils/DllDefines.hpp>
#include <glibmm/spawn.h>
#include <glibmm/thread.h>
#include <string>
#include <map>
#include <vector>

namespace vle { namespace utils {

/**
 * @brief Define a remote list, for instance: key = id of a the remote,
 * value = url of the remote.
 */
typedef std::map < std::string, std::string > RemoteList;

/**
 * @brief Define a package list as a list of string.
 */
typedef std::vector < std::string > PackageList;

/**
 * @brief A class to access to the remote repository. This class uses the
 * libxml2 nanohttp to download resources from the http protocol.
 */
class VLE_UTILS_EXPORT Remote
{
public:
    /**
     * @brief Build a new Remote object and read the "vle.conf" file.
     */
    Remote();

    //
    // Section to the list of commands
    //

    /**
     * @brief List all remote file.
     */
    const RemoteList& list() const;

    /**
     * @brief Get the list of available packages on remote host.
     * @param remote The host.
     * @return A list of packages.
     * @throw utils::ArgError or utils::InternalError if remote is bad or if
     * the remote host is not accessible.
     */
    PackageList show(const std::string& remote);

    /**
     * @brief Get the description of a specified package on remote host.
     * @param remote The host.
     * @param package The package to get the description file.
     * @return The description of the packages.
     * @throw utils::ArgError or utils::InternalError if remote is bad or if
     * the remote host is not accessible.
     */
    std::string show(const std::string& remote, const std::string& package);

    /**
     * @brief Download the specified package on remote host.
     * @param remote The host.
     * @param package The package to get.
     * @return the filename.
     * @throw utils::ArgError or utils::InternalError if remote is bad or if
     * the remote host is not accessible.
     */
    std::string get(const std::string& remote, const std::string& package);

    //
    // Section to "vle.conf" file.
    //

    /**
     * @brief Get an url from the "vle.conf" file.
     * @param remote The remote host.
     * @return The URL attached to the host.
     * @throw utils::ArgError if remote does not exists.
     */
    const std::string& getUrl(const std::string& remote) const;

    /**
     * @brief Reload the vle.conf file.
     */
    void refresh();

    /**
     * @brief Add a new remote on the "vle.conf" file.
     * @param remote The remote host.
     * @param url The URL of the host.
     */
    void config(const std::string& remote, const std::string& url);

private:
    mutable RemoteList mRemotes; ///< the list of remotes.

    /**
     * @brief Get a ressources from the http protocol.
     * @param remote The remote host.
     * @param url The url of the resources.
     * @return The resources.
     * @throw utils::ArgError or utils::Internal error.
     */
    std::string httpGet(const std::string& remote, const std::string& url);
};

}} // namespace vle utils

#endif
