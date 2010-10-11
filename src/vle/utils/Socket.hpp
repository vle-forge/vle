/*
 * @file vle/utils/Socket.hpp
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


#ifndef VLE_UTILS_SOCKET_HPP
#define VLE_UTILS_SOCKET_HPP

#include <string>
#include <map>
#include <boost/cstdint.hpp> // for boost::int32_t
#include <vle/utils/DllDefines.hpp>

namespace vle { namespace utils {

    /**
     * @brief This namespace provides a simple API to use Win32 or POSIX
     * sockets. Two classes are proposed, server, which accept client, and
     * client which dialog with server.
     */
    namespace net {

    /**
     * @brief Parse the input string to find correct TCP/IP address and good
     * port. For instance:
     * @code
     * utils::net::explodeStringNet("localhost:8000"); // localhost and 8000
     * utils::net::explodeStringNet(":8000");          // localhost and 8000
     * utils::net::explodeStringNet("127.0.0.1:");     // 127.0.0.1 and 8000
     * utils::net::explodeStringNet("193.49.192.226:9999");
     *                                                 // 193.49.192.226:9999
     * utils::net::explodeStringNet(":");              // localhost and 9000
     * @endcode
     *
     * @param input the string representation of StringNet.
     * @param ip output parameter to receive IP address or server name. Default
     * is localhost.
     * @param port output parameter to receive TCP/IP port. Default is 8000.
     */
    VLE_UTILS_EXPORT void explodeStringNet(const std::string& input,
                                           std::string& ip,
                                           int& port);

    /**
     * @brief Parse the input string to find correct TCP/IP address, port and
     * directory. For instance:
     * @code
     * utils::net::explodeStringNet("localhost:80:xxx"); // localhost, 80, "xxx"
     * utils::net::explodeStringNet(":8000:");          // localhost, 80, ""
     * utils::net::explodeStringNet("127.0.0.1:");     // 127.0.0.1 and 8000, ""
     * utils::net::explodeStringNet("193.49.192.226:9999:");
     *                             // 193.49.192.226:9999, ""
     * utils::net::explodeStringNet("::");              // localhost and 9000
     * @endcode
     *
     * @param input the string representation of StringNet. The string must
     * containt two ':'.
     * @param ip output parameter to receive IP address or server name. Default
     * is localhost.
     * @param port output parameter to receive TCP/IP port. Default is 8000.
     * @param directory output parameter to receive directory. Default is empty.
     */
    VLE_UTILS_EXPORT void explodeStringNet(const std::string& input,
                                           std::string& ip,
                                           int& port,
                                           std::string& directory);

    /**
     * @brief Define base class of socket Client and Server classes.
     */
    class VLE_UTILS_EXPORT Base
    {
    public:
        Base();

        virtual ~Base();

	static void init();

        /**
         * Get state of socket.
         *
         * @return true if connection successful, false otherwise.
         */
        bool isRunning() const;

        /**
         * Get socket value.
         *
         * @return socket value or -1 if isRunning = false.
         */
        int socket() const { return mSocket; }

        /**
         * Send a buffer message to specified socket.
         *
         * @param dst socket destination.
         * @param buffer memory to send.
         * @param size buffer size to send.
         * @throw Internal if socket is close, buffer is null or size equal 0
         * or network error.
         */
        void send(int dst, const void* buffer, int size);

        /**
         * Send an string message to specified socket.
         *
         * @param dst socket destination.
         * @param buffer string message to send.
         * @throw Internal if socket is close or network error.
         */
        void sendString(int dst, const std::string& buffer);

        /**
         * Send a buffer to specified socket.
         *
         * @param dst socket destination.
         * @param buffer message to send.
         * @throw Internal if socket is close or network error.
         */
        void sendBuffer(int dst, const std::string& buffer);

        /**
         * Send an integer to specified socket.
         *
         * @param dst socket destination.
         * @param buffer integer to send.
         * @throw Internal if socket is close or network error.
         */
        void sendInteger(int dst, boost::int32_t buffer);

        /**
         * Receive a buffer from specified socket.
         *
         * @param src socket source.
         * @param buffer to receive message.
         * @param size of buffer to receive message.
         * @return size of message received.
         * @throw Internal if socket is close, buffer null, size is null or
         * network error.
         */
        int recv(int src, void* buffer, size_t size);

        /**
         * Receive an string from specified socket.
         *
         * @param src socket source.
         * @return string read from server.
         * @throw Internal if socket is close or network error.
         */
        std::string recvString(int src);

        /**
         * Receive data from specified socket and fill a string buffer.
         *
         * @param src socket source.
         * @param size date size to read from server.
         * @return string buffer read.
         * @throw Internal if socket is close or network error.
         */
        std::string recvBuffer(int src, size_t size);

        /**
         * Receive a boost::uint32_t from specified socket.
         *
         * @param src socket source.
         * @return an integer.
         * @throw Internal if socket is close, buffer null, size is null or if
         * error during reception of data.
         */
        boost::uint32_t recvInteger(int src);

    protected:
        bool    mRunning;
        int     mSocket;
    };

    /**
     * @brief Define the socket client. All type of errors throw an
     * exception. An example of using client:
     *
     * @code
     * utils::net::Client c("vle.univ-littoral.fr", 8000);
     * utils::net::Client c("vle.toulouse.inra.fr", 8000);
     * utils::net::Client c("www.vle-project.org", 8000);
     * @endcode
     */
    class VLE_UTILS_EXPORT Client : public Base
    {
    public:
        /**
         * Try etablishing connection with specified server address and port.
         *
         * @param server server name like "vle.univ-littoral.fr" or IPv4
         * address like 193.49.192.226.
         * @param port to try connection.
         */
        Client(const std::string& server, int port);

        /**
         * Close connection with server if socket is connected.
         *
         */
        virtual ~Client() { }

        /**
         * Send a message to server.
         *
         * @param buffer Buffer to send.
         * @param size Size of buffer to send.
         * @throw Internal if socket is close, buffer is null or size equal 0.
         */
        void send(const void* buffer, int size)
        { Base::send(mSocket, buffer, size); }

        /**
         * Send an UT8 string message to server.
         *
         * @param buffer Buffer to send.
         * @throw Internal if socket is close, buffer is null or size equal 0.
         */
        void sendString(const std::string& buffer)
        { Base::sendString(mSocket, buffer); }

        /**
         * Send a buffer message to server.
         *
         * @param buffer Buffer to send.
         * @throw Internal if socket is close, buffer is null or size equal 0.
         */
        void sendBuffer(const std::string& buffer)
        { Base::sendBuffer(mSocket, buffer); }

        /**
         * Send an integer message to server.
         *
         * @param buffer integer to send.
         * @throw Internal if socket is close, buffer is null or size equal 0.
         */
        void sendInteger(boost::uint32_t buffer)
        { Base::sendInteger(mSocket, buffer); }

        /**
         * Receive a buffer from server.
         *
         * @param buffer to receive message.
         * @param size of buffer to receive message.
         * @return size of message received.
         * @throw Internal if socket is close, buffer null, size is null or if
         * error during reception of data.
         */
        int recv(void* buffer, size_t size)
        { return Base::recv(mSocket, buffer, size); }

        /**
         * Receive an string from server.
         *
         * @return string read from server.
         * @throw Internal if socket is close, buffer null, size is null or if
         * error during reception of data.
         */
        std::string recvString()
        { return Base::recvString(mSocket); }

        /**
         * Receive data from server and fill a string buffer.
         *
         * @param size date size to read from server.
         * @return string buffer read.
         * @throw Internal if socket is close, buffer null, size is null or if
         * error during reception of data.
         */
        std::string recvBuffer(size_t size)
        { return Base::recvBuffer(mSocket, size); }


        /**
         * Receive a boost::uint32_t from server.
         *
         * @return an integer.
         * @throw Internal if socket is close, buffer null, size is null or if
         * error during reception of data.
         */
        boost::uint32_t recvInteger()
        { return Base::recvInteger(mSocket); }

        /**
         * Close the connection with server.
         *
         * @throw InternalError if socket close error.
         */
        void close();
    };

    /**
     * @brief Define the socket server. All type of errors throw an exception
     * and close connection. An example of using server:
     *
     * @code
     * utils::net::Server s(8000);
     * @endcode
     */
    class VLE_UTILS_EXPORT Server : public Base
    {
    public:
        Server(int port);

        /**
         * First, close connection with all client, second socket of server.
         *
         */
        virtual ~Server();

        /**
         * Accept a new client connection. Client is defined by is name and
         * socket id.
         *
         * @param name of client to connect.
         * @return socket id of client.
         * @throw Internal if error connection, if name already exist.
         */
        int acceptClient(const std::string& name);

        /**
         * Retrieve the client socket id from client connection list.
         *
         * @param name of client to find.
         * @return socket id of client.
         * @throw Internal client doesn't exist.
         */
        int getSocketClient(const std::string& name) const;

        /**
         * Close the client socket id from client connection list.
         *
         * @param name of client to find and close.
         */
        void closeClient(const std::string& name);

        /**
         * Retrieve the first client socket id from client connection list.
         *
         * @return socket id of first client.
         * @throw Internal if no client connected or too many client.
         */
        int getSocketSingleClient() const;

        /**
         * Send a message to specified client.
         *
         * @param dest client name.
         * @param buffer Buffer to send.
         * @param size Size of buffer to send.
         * @throw Internal if socket is close, buffer is null or size equal 0,
         * if client destination is unknown.
         */
        void send(const std::string& dest, const void* buffer, int size)
        { Base::send(getSocketClient(dest), buffer, size); }

        /**
         * Send a message to specified client.
         *
         * @param dest client name.
         * @param buffer string to send.
         * @throw Internal if socket is close, buffer is null or size equal 0,
         * if client destination is unknown.
         */
        void sendString(const std::string& dest, const std::string& buffer)
        { Base::sendString(getSocketClient(dest), buffer); }

        /**
         * Send a buffer to specified client.
         *
         * @param dest client name.
         * @param buffer to send.
         * @throw Internal if socket is close, buffer is null or size equal 0,
         * if client destination is unknown.
         */
        void sendBuffer(const std::string& dest, const std::string& buffer)
        { Base::sendBuffer(getSocketClient(dest), buffer); }

        /**
         * Send an integer to specified client.
         *
         * @param dest client name.
         * @param buffer integer to send.
         * @throw Internal if socket is close, buffer is null or size equal 0,
         * if client destination is unknown.
         */
        void sendInteger(const std::string& dest, boost::uint32_t buffer)
        { Base::sendInteger(getSocketClient(dest), buffer); }

        /**
         * Receive a buffer from specified client.
         *
         * @param src client name.
         * @param buffer to receive message.
         * @param size of buffer to receive message.
         * @return size of message received.
         * @throw Internal if socket is close, buffer null, size is null or if
         * error during reception of data or if client destination if unknown.
         */
	int recv(const std::string& src, void* buffer, size_t size)
        { return Base::recv(getSocketClient(src), buffer, size); }

        /**
         * Receive a string from specified client.
         *
         * @return string read from server.
         * @throw Internal if socket is close, buffer null, size is null or if
         * error during reception of data or if client destination if unknown.
         */
        std::string recvString(const std::string& src)
        { return Base::recvString(getSocketClient(src)); }

        /**
         * Receive a buffer from specified client.
         *
         * @return buffer read from server.
         * @throw Internal if socket is close, buffer null, size is null or if
         * error during reception of data or if client destination if unknown.
         */
        std::string recvBuffer(const std::string& src, size_t size)
        { return Base::recvBuffer(getSocketClient(src), size); }

        /**
         * Receive an integer from specified client.
         *
         * @return integer read from server.
         * @throw Internal if socket is close, buffer null, size is null or if
         * error during reception of data or if client destination if unknown.
         */
        boost::uint32_t recvInteger(const std::string& src)
        { return Base::recvInteger(getSocketClient(src)); }

    private:
        /**
         * @brief Define an associative table between a client' name and is
         * socket.
         */
        typedef std::map < std::string, int > ClientsSocket;

        ClientsSocket   mClientsSocket; /*!< the client objects. */
    };

}}} // namespace vle utils net

#endif
