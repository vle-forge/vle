/**
 * @file vle/oov/NetStreamReader.hpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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


#ifndef VLE_OOV_NETSTREAMREADER_HPP
#define VLE_OOV_NETSTREAMREADER_HPP

#include <vle/oov/StreamReader.hpp>
#include <vle/utils/Socket.hpp>



namespace vle { namespace oov {

    class NetStreamReader : public StreamReader
    {
    public:
        /** 
         * @brief Build a NetStreamReader based on a new socket server.
         * @param port the port number [0 and 65535].
         * @throw utils::InternalError if the building of the socket server
         * fail.
         */
        NetStreamReader(int port);

        virtual ~NetStreamReader();

        /** 
         * @brief Build a socket server and wait connection from a
         * devs::NetStreamWriter client.
         */
        void process();

        /** 
         * @brief Return the port used by the oov::NetStreamReader socket
         * server.
         * @return the port number [0 and 65535].
         */
        inline const int port() const;

        /** 
         * @brief Affect a new size to the buffer.
         * @param buffer the new size of the buffer.
         * @throw utils::InternalError if buffer equal 0 or greater than memory
         * possibility.
         */
        void setBufferSize(size_t buffer);

        /** 
         * @brief Return the size of the buffer.
         * @return The size of the buffer.
         */
        inline size_t bufferSize() const;

        /** 
         * @brief Return a reference to the buffer.
         * @return A reference to the buffer.
         */
        inline const std::string& buffer() const;

    private:
        int                 m_port;
        utils::net::Server  m_server;
        std::string         m_buffer;

        void waitConnection();
        void readConnection();
        bool dispatch(const vpz::Trame* trame);
        void closeConnection();
        void serializePlugin();
    };

    inline const int NetStreamReader::port() const
    { return m_port; }
    
    inline size_t NetStreamReader::bufferSize() const
    { return m_buffer.size(); }

    inline const std::string& NetStreamReader::buffer() const
    { return m_buffer; }


}} // namespace vle oov

#endif
