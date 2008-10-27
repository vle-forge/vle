/**
 * @file vle/utils/Socket.hpp
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


#ifndef UTILS_SOCKET_HPP
#define UTILS_SOCKET_HPP

#include <string>
#include <map>
#include <glibmm/ustring.h>



namespace vle { namespace utils {

    /**
     * @brief This namespace provides two public classes of portable and object
     * oriented unix socket API.
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
    void explodeStringNet(const std::string& input,
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
    void explodeStringNet(const std::string& input,
                          std::string& ip,
                          int& port,
                          std::string& directory);

    /**
     * @brief Define base class of socket Client and Server.
     *
     */
    class Base
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
        bool is_running() const;

        /**
         * Get socket value.
         *
         * @return socket value or -1 if is_running = false.
         */
        inline int socket() const { return mSocket; }

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
         * Send an UTF-8 string message to specified socket.
         *
         * @param dst socket destination.
         * @param buffer string message to send.
         * @throw Internal if socket is close or network error.
         */
        void send_string(int dst, const Glib::ustring& buffer);

        /**
         * Send a buffer to specified socket.
         *
         * @param dst socket destination.
         * @param buffer message to send.
         * @throw Internal if socket is close or network error.
         */
        void send_buffer(int dst, const std::string& buffer);

        /**
         * Send a integer to specified socket.
         *
         * @param dst socket destination.
         * @param buffer integer to send.
         * @throw Internal if socket is close or network error.
         */
        void send_int(int dst, gint32 buffer);

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
         * Receive an UTF-8 string from specified socket.
         *
         * @param src socket source.
         * @return string read from server.
         * @throw Internal if socket is close or network error.
         */
        Glib::ustring recv_string(int src);

        /**
         * Receive data from specified socket and fill a string buffer.
         *
         * @param src socket source.
         * @param size date size to read from server.
         * @return string buffer read.
         * @throw Internal if socket is close or network error.
         */
        std::string recv_buffer(int src, size_t size);

        /**
         * Receive a gint32 from specified socket.
         *
         * @param src socket source.
         * @return an integer.
         * @throw Internal if socket is close, buffer null, size is null or if
         * error during reception of data.
         */
        gint32 recv_int(int src);

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
     * @endcode
     */
    class Client : public Base
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
        inline void send(const void* buffer, int size)
        { Base::send(mSocket, buffer, size); }

        /**
         * Send an UT8 string message to server.
         *
         * @param buffer Buffer to send.
         * @throw Internal if socket is close, buffer is null or size equal 0.
         */
        inline void send_string(const Glib::ustring& buffer)
        { Base::send_string(mSocket, buffer); }

        /**
         * Send a buffer message to server.
         *
         * @param buffer Buffer to send.
         * @throw Internal if socket is close, buffer is null or size equal 0.
         */
        void send_buffer(const std::string& buffer)
        { Base::send_buffer(mSocket, buffer); }

        /**
         * Send an integer message to server.
         *
         * @param buffer integer to send.
         * @throw Internal if socket is close, buffer is null or size equal 0.
         */
        void send_int(gint32 buffer)
        { Base::send_int(mSocket, buffer); }

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
         * Receive an UTF-8 string from server.
         *
         * @return string read from server.
         * @throw Internal if socket is close, buffer null, size is null or if
         * error during reception of data.
         */
        Glib::ustring recv_string()
        { return Base::recv_string(mSocket); }

        /**
         * Receive data from server and fill a string buffer.
         *
         * @param size date size to read from server.
         * @return string buffer read.
         * @throw Internal if socket is close, buffer null, size is null or if
         * error during reception of data.
         */
        std::string recv_buffer(size_t size)
        { return Base::recv_buffer(mSocket, size); }


        /**
         * Receive a gint32 from server.
         *
         * @return an integer.
         * @throw Internal if socket is close, buffer null, size is null or if
         * error during reception of data.
         */
        inline gint32 recv_int()
        { return Base::recv_int(mSocket); }

        /**
         * Close the connection with server.
         *
         * @throw Enternal if socket close error.
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
    class Server : public Base
    {
    public:
        Server(int port);

        /**
         * First, close connection with all client, secondd socket of server.
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
        int accept_client(const Glib::ustring& name);

        /**
         * Retrieve the client socket id from client connection list.
         *
         * @param name of client to find.
         * @return socket id of client.
         * @throw Internal client doesn't exist.
         */
        int get_socket_client(const Glib::ustring& name) const;

        /**
         * Close the client socket id from client connection list.
         *
         * @param name of client to find and close.
         */
        void close_client(const Glib::ustring& name);

        /**
         * Retrieve the first client socket id from client connection list.
         *
         * @return socket id of first client.
         * @throw Internal if no client connected or too many client.
         */
        int get_socket_single_client() const;

        /**
         * Send a message to specified client.
         *
         * @param dest client name.
         * @param buffer Buffer to send.
         * @param size Size of buffer to send.
         * @throw Internal if socket is close, buffer is null or size equal 0,
         * if client destination is unknow.
         */
        inline void send(const Glib::ustring& dest, const void* buffer,
                         int size)
        { Base::send(get_socket_client(dest), buffer, size); }

        /**
         * Send a message to specified client.
         *
         * @param dest client name.
         * @param buffer UTF-8 string to send.
         * @throw Internal if socket is close, buffer is null or size equal 0,
         * if client destination is unknow.
         */
        inline void send_string(const Glib::ustring& dest,
                                const Glib::ustring& buffer)
        { Base::send_string(get_socket_client(dest), buffer); }

        /**
         * Send a buffer to specified client.
         *
         * @param dest client name.
         * @param buffer to send.
         * @throw Internal if socket is close, buffer is null or size equal 0,
         * if client destination is unknow.
         */
        inline void send_buffer(const Glib::ustring& dest,
                                const std::string& buffer)
        { Base::send_buffer(get_socket_client(dest), buffer); }

        /**
         * Send a integer to specified client.
         *
         * @param dest client name.
         * @param buffer integer to send.
         * @throw Internal if socket is close, buffer is null or size equal 0,
         * if client destination is unknow.
         */
        inline void send_int(const Glib::ustring& dest, gint32 buffer)
        { Base::send_int(get_socket_client(dest), buffer); }

        /**
         * Receive a buffer from specified client.
         *
         * @param src client name.
         * @param buffer to receive message.
         * @param size of buffer to receive message.
         * @return size of message received.
         * @throw Internal if socket is close, buffer null, size is null or if
         * error during reception of data or if client destination if unknow.
         */
	inline int recv(const Glib::ustring& src, void* buffer, size_t size)
        { return Base::recv(get_socket_client(src), buffer, size); }

        /**
         * Receive a UTF-8 string from specified client.
         *
         * @return string read from server.
         * @throw Internal if socket is close, buffer null, size is null or if
         * error during reception of data or if client destination if unknow.
         */
        inline Glib::ustring recv_string(const Glib::ustring& src)
        { return Base::recv_string(get_socket_client(src)); }

        /**
         * Receive a buffer from specified client.
         *
         * @return buffer read from server.
         * @throw Internal if socket is close, buffer null, size is null or if
         * error during reception of data or if client destination if unknow.
         */
        inline std::string recv_buffer(const Glib::ustring& src, size_t size)
        { return Base::recv_buffer(get_socket_client(src), size); }

        /**
         * Receive a integer from specified client.
         *
         * @return integer read from server.
         * @throw Internal if socket is close, buffer null, size is null or if
         * error during reception of data or if client destination if unknow.
         */
        inline gint32 recv_int(const Glib::ustring& src)
        { return Base::recv_int(get_socket_client(src)); }

    private:
        typedef std::map < Glib::ustring, int > ClientsSocket;

        ClientsSocket   mClientsSocket;
    };

}}} // namespace vle utils net

#endif
