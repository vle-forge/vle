/**
 * @file vle/utils/Host.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
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


#ifndef VLE_UTILS_HOSTS_HPP
#define VLE_UTILS_HOSTS_HPP

#include <set>
#include <string>
#include <vle/utils/DllDefines.hpp>

namespace vle { namespace utils {

    /**
     * @brief This class define a simulator host. This class represent an entry
     * into the hosts file. The syntax of this entry is:
     *
     * @code
     * <HOST HOSTNAME="127.0.0.1" PORT="1234" PROCESS="1" />
     * @endcode
     *
     * The hostname value is a string and it accepts IP address or complete
     * hostname.
     */
    class VLE_UTILS_EXPORT Host
    {
    public:
        /**
         * Build a simulator host.
         *
         * @param hostname
         * @param port
         * @param process
         */
        Host(const std::string& hostname, int port, int process)
            : mHostname(hostname), mPort(port), mProcess(process)
        {}

        Host(const std::string& hostname, const std::string& port,
             const std::string& process);

        Host()
        {}

        /**
         * @return hostname to contact.
         */
        inline const std::string& hostname() const
        { return mHostname; }

        /**
         * @brief Set the current hostname.
         * @param hostname the new hostname.
         */
        inline void hostname(const std::string& hostname)
        { mHostname.assign(hostname); }

        /**
         * @return host port to listen.
         */
        inline int port() const
        { return mPort; }

        /**
         * @brief Set the current port.
         * @param port the new port.
         */
        inline void port(int port)
        { mPort = port; }

        /**
         * @return number of authorized process on host.
         */
        inline int process() const
        { return mProcess; }

        /**
         * @brief Set the current process.
         * @param process the new process.
         */
        inline void process(int process)
        { mProcess = process; }

        /**
         * Compare two host, all member must be the same using only the
         * hostname.
         *
         * @param host the host to compare.
         * @return true if host equal *this.
         */
        inline bool operator==(const Host& host) const
        { return mHostname == host.hostname(); }

        /**
         * Compare two host and return the mimum using only the hostname.
         *
         * @param host the host to compare.
         * @return true if *this < host.
         */
        inline bool operator<(const Host& host) const
        { return mHostname < host.hostname(); }

    private:
        std::string mHostname; /// An ascii hostname or ip address.
        int         mPort;
        int         mProcess;
    };

    /**
     * This class manage a set of host objet read from XML hosts file. Hosts
     * class can write XML hosts file and add/remove host.
     *
     */
    class VLE_UTILS_EXPORT Hosts
    {
    public:
        typedef std::set < Host > SetHosts;
        typedef SetHosts::const_iterator const_iterator;
        typedef SetHosts::iterator iterator;
        typedef SetHosts::size_type size_type;

        /**
         * @brief Construct a new hosts with empty hosts set.
         */
        Hosts()
            : mIsModified(false)
        {}

        /**
         * @brief If use modify the hosts set, then it is deleted in descructor
         * function.
         */
        ~Hosts();

        /**
         * Parse XML hosts file.
         *
         */
        void read();

        /**
         * Write XML hosts file.
         */
        void write();

        /**
         * @brief Push a new host into the set.
         * @param host a new host to push into list.
         */
        void add(const Host& host);

        /**
         * @brief Remove a host with hostname equivalent.
         * @param hostname the name of host to delete.
         */
        void del(const std::string& hostname);

        /**
         * @brief Get a host with specified hostname.
         * @param hostname the name of host to find.
         * @return Host reasearch.
         * @throw utils::InternalError if host not found.
         */
        const Host& get(const std::string& hostname) const;

        /**
         *  @return hosts bases.
         */
        inline const SetHosts& hosts() const
        { return mHosts; }

        const_iterator begin() const { return mHosts.begin(); }
        iterator end() const { return mHosts.end(); }
        const_iterator begin() { return mHosts.begin(); }
        iterator end() { return mHosts.end(); }
        size_type size() const { return mHosts.size(); }

    private:
        SetHosts        mHosts;
        bool            mIsModified;
    };

}} // namespace vle utils

#endif
