/**
 * @file utils/Host.hpp
 * @author The VLE Development Team.
 * @brief These classes define a simulator host and set of simulators. It
 * works on XML file hosts.
 */

/*
 * Copyright (c) 2004, 2005 The vle Development Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifndef UTILS_HOSTS_HPP
#define UTILS_HOSTS_HPP

#include <glibmm/ustring.h>
#include <libxml++/libxml++.h>
#include <set>
#include <vle/utils/Path.hpp>



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
    class Host
    {
    public:
        /**
         * Build a simulator host.
         *
         * @param hostname
         * @param port
         * @param process
         */
        Host(const std::string& hostname, int port, int process) :
            mHostname(hostname),
            mPort(port),
            mProcess(process) { }

        /**
         * Build an empty entry of simulator host.
         *
         */
        Host() { }

        /**
         * Write XML syntax under the root node.
         *
         * @param root the node above new node for instance HOSTS.
         */
        void write(xmlpp::Element* root) const;

        /**
         * Read XML syntax node.
         *
         * @param root the host node to read.
         */
        void read(xmlpp::Element* root);

        /** @return hostname to contact. */
        inline const std::string& hostname() const
	    { return mHostname; }

        /**
         * Set the current hostname.
         *
         * @param hostname the new hostname.
         */
        inline void hostname(const std::string& hostname)
	    { mHostname.assign(hostname); }

        /** @return host port to listen. */
        inline int port() const { return mPort; }

        /**
         * Set the current port.
         *
         * @param port the new port.
         */
        inline void port(int port) { mPort = port; }

        /** @return number of authorized process on host. */
        inline int process() const { return mProcess; }

        /**
         * Set the current process.
         *
         * @param process the new process.
         */
        inline void process(int process) { mProcess = process; }

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
    class Hosts
    {
    public:
        typedef std::set < Host > SetHosts;

        /**
         * Construct a new hosts with empty hosts set.
         *
         */
        Hosts() : mIsModified(false) { }

        /**
         * If use modify the hosts set, then it is deleted in descructor
         * function.
         *
         */
        ~Hosts();

        /**
         * Parse XML hosts file.
         *
         */
        void read_file();

        /**
         * Write XML hosts file.
         *
         */
        void write_file();

        /**
         * Push a new host into the set.
         *
         * @param host a new host to push into list.
         */
        void push_host(const Host& host);

        /**
         * Remove a host with hostname equivalent.
         *
         * @param hostname the name of host to delete.
         */
        void remove_host(const std::string& hostname);

        /**
         * Get a host with specified hostname.
         *
         * @param hostname the name of host to find.
         * @return Host reasearch.
         * @throw Exception::Interal if host not found.
         */
        const Host& get_host(const std::string& hostname) const;

        /** @return user hosts filename. */
        static Glib::ustring get_hosts_filename();

        /** @return hosts bases. */
        inline const SetHosts& hosts() const
	    { return mHosts; }

    private:
        SetHosts        mHosts;
        bool            mIsModified;
    };

}} // namespace vle utils

#endif
