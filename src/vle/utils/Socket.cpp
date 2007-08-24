/**
 * @file utils/Socket.cpp
 * @author The VLE Development Team.
 * @brief Portable and Object oriented socket API.
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


#include <glib/gtypes.h>

#ifdef G_OS_WIN32
#   include <winsock2.h>
#   include <vle/utils/Debug.hpp>
#   include <vle/utils/Socket.hpp>
#   include <glibmm/stringutils.h>
#   include <iostream>
#else
#   include <vle/utils/Debug.hpp>
#   include <vle/utils/Socket.hpp>
#   include <glibmm/stringutils.h>
#   include <iostream>
#   include <sys/types.h>
#   include <sys/socket.h>
#   include <netdb.h>
#   include <string.h>
#   include <unistd.h>
#   include <netinet/in.h>
#   include <arpa/inet.h>
#   include <netdb.h>
#   include <errno.h>
#endif

#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>


namespace vle { namespace utils { namespace net {

void explodeStringNet(const std::string& input, std::string& ip, int& port)
{
    ip.assign("localhost");
    port = 8000;

    if (not input.empty()) {
        std::string::size_type position = 0;
        position = input.find(':');
        
        if (position == std::string::npos) {
            ip.assign(input);
        } else {
            if (position != 0) {
                ip.assign(input, 0, position);
            }

            if (position + 1 < input.size()) {
                std::string tmp(input, position + 1, input.size() - position + 1);
                if (utils::is_int(tmp)) {
                    port = utils::to_int(tmp);
                }
            }
        }
    }
}

void explodeStringNet(const std::string& input, std::string& ip, int& port,
                      std::string& directory)
{
    ip.assign("localhost");
    port = 8000;

    std::vector < std::string > lst;
    boost::algorithm::split(lst, input, boost::is_any_of(":"));

    Assert(utils::InternalError, lst.size() == 3, boost::format(
            "Cannot find IP:port:directory from [%1%]") % input);

    if (not lst[0].empty()) {
        ip.assign(lst[0]);
    }

    try {
        port = boost::lexical_cast < int >(lst[1]);
    } catch(const boost::bad_lexical_cast& e) {
        Throw(utils::InternalError, boost::format(
                "Cannont convert [%1%] in TCP/IP port") % lst[1]);
    }

    Assert(utils::InternalError, port > 0 and port < 65535, boost::format(
            "Bad TCP/IP port value: %1%") % port);

    directory.assign(lst[2]);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Base
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

Base::Base() :
    mRunning(false),
    mSocket(-1)
{
    int r = ::socket(PF_INET, SOCK_STREAM, 0);
    Assert(InternalError, r != -1,
           boost::format("Error acquiring socket: %1%\n") %
           Glib::strerror(errno));

    mSocket = r;
    struct linger l;
    l.l_onoff = 1;
    l.l_linger = 0;

#ifdef G_OS_WIN32
    r = ::setsockopt(mSocket, SOL_SOCKET, SO_LINGER, (const char*)&l,
                     sizeof(l));
#else
    r = ::setsockopt(mSocket, SOL_SOCKET, SO_LINGER, &l, sizeof(l));
#endif

    if (r == -1) {
#ifdef G_OS_WIN32
	::closesocket(mSocket);
#else
        ::close(mSocket);
#endif
        Throw(InternalError, boost::format(
                "Error setsockopt socket: %1%\n") % Glib::strerror(errno));
    }

    mRunning = true;
}

Base::~Base()
{
    if (mRunning) {
#ifdef G_OS_WIN32
        ::shutdown(mSocket, SD_BOTH);
        int r = ::closesocket(mSocket);
#else
        ::shutdown(mSocket, SHUT_RDWR);
        int r = ::close(mSocket);
#endif
        Assert(InternalError, r == 0, "Cannot close socket properly.");
    }
}

void Base::send(int dst, const void* buffer, ssize_t size)
{
    Assert(InternalError, mRunning,
           "Cannot send message, socket is not initialised.");
    Assert(InternalError, buffer,
           "Cannot send message, buffer is null.");
    Assert(InternalError, size > 0,
           "Cannot send message, size must be superior to 0.");

    ssize_t position = 0;
    while (size > 0) {
        ssize_t szcurrent = ::send(dst, (char*)buffer + position, size, 0);
	if (szcurrent == 0)
	    break;

        Assert(InternalError, szcurrent != -1,
               boost::format("Cannot send message, %1%\n") %
               Glib::strerror(errno));

	position += szcurrent;
	size -= szcurrent;
    }
}

void Base::send_string(int dst, const Glib::ustring& buffer)
{
    send(dst, buffer.c_str(), std::strlen(buffer.c_str()));
}

void Base::send_buffer(int dst, const std::string& buffer)
{
    send(dst, buffer.c_str(), buffer.size());
}

void Base::send_int(int dst, gint32 buffer)
{
    send(dst, &buffer, sizeof(gint32));
}

ssize_t Base::recv(int src, void* buffer, size_t size)
{
    Assert(InternalError, mRunning,
           "Client cannot receive message, socket is not initialised.");
    Assert(InternalError, buffer,
           "Client cannot receive message, buffer is null.");
    Assert(InternalError, size > 0,
           "Client cannot receive message, size equal to 0.");

#ifdef G_OS_WIN32
    ssize_t current = ::recv(src, (char*)buffer, size, 0);
#else
    ssize_t current = ::recv(src, buffer, size, 0);
#endif

    Assert(InternalError, current != -1,
           boost::format("Client cannot receive message, %1%\n") %
           Glib::strerror(errno));

    ((char*)buffer)[current] = '\0';
    return current;
}

Glib::ustring Base::recv_string(int src)
{
    Assert(InternalError, mRunning,
	   "Client cannot receive message client is not connected.");

    char* buffer = new char[1024];
    Glib::ustring finished_buffer;

    ssize_t size = recv(src, buffer, 1023);

    if (size == -1) {
        delete[] buffer;
	Throw(InternalError,
              boost::format("Client cannot receive message, %1%\n") %
              Glib::strerror(errno));
    }

    if (size == 1023)
        buffer[size] = '\0';
    else
        buffer[size + 1] = '\0';

    finished_buffer.append(buffer);

    delete[] buffer;
    return finished_buffer;
}

std::string Base::recv_buffer(int src, size_t size)
{
    std::string finished_buffer;
    char* buffer = new char[1024];

    ssize_t current;

    while (size > 0) {
        try {
            if (size > 1024)
                current = recv(src, buffer, 1024);
            else
                current = recv(src, buffer, size);
	} catch(const InternalError& e) {
	    delete[] buffer;
	    throw e;
	}

	if (current == 0)
	    break;

	finished_buffer.append(buffer, current);
	size -= current;
    }

    delete[] buffer;
    return finished_buffer;
}

gint32 Base::recv_int(int src)
{
    Assert(InternalError, mRunning,
           "Socket cannot receive message, socket not initialised.");

    gint32 buffer;
    ssize_t current = recv(src, &buffer, sizeof(gint32));

    Assert(InternalError, current == sizeof(gint32),
           boost::format("Client cannot receive message, %1%\n") %
                      Glib::strerror(errno));

    return buffer;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Client
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

Client::Client(const std::string& server, int port)
{
    int r;

    Assert(InternalError, mRunning, "Cannot open server, socket failed.");

    struct sockaddr_in host;
    memset(&host, '\0', sizeof(host));
    host.sin_family = AF_INET;
    host.sin_port = htons(port);

    struct hostent* hoste = gethostbyname(server.c_str());
    if (hoste == NULL) {
        host.sin_addr.s_addr = INADDR_ANY;
    } else {
        memcpy(&host.sin_addr, hoste->h_addr_list[0], hoste->h_length);
    }

    r = ::connect(mSocket, (struct sockaddr*)&host, sizeof(host));

    Assert(InternalError, r == 0,
           boost::format("Error connection with server, %1%\n") %
           Glib::strerror(errno));

    mRunning = true;
}

void Client::close()
{
    if (mRunning) {
#ifdef G_OS_WIN32
	::shutdown(mSocket, SD_BOTH);
        int r = ::closesocket(mSocket);
#else
	::shutdown(mSocket, SHUT_RDWR);
        int r = ::close(mSocket);
#endif
        Assert(InternalError, r == 0, boost::format(
                "Cannot close client socket properly, %1%\n") % 
            Glib::strerror(errno));
        mRunning = false;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Base
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


Server::Server(int port)
{
    Assert(InternalError, mRunning, "Cannot open server, socket failed");

    struct sockaddr_in adr;
    int reuse = 1;
    int r;

    memset(&adr, 0, sizeof(sockaddr_in));
    adr.sin_family = AF_INET;
    adr.sin_port = htons(port);
    adr.sin_addr.s_addr = INADDR_ANY;

    r = ::bind(mSocket, (struct sockaddr*)&adr, sizeof(sockaddr_in));

    Assert(InternalError, r == 0,
           boost::format("Bind error %1%\n") % Glib::strerror(errno));

#ifdef G_OS_WIN32
    r = ::setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, (char*) &reuse,
                     sizeof(reuse));
#else
    r = ::setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, (int*) &reuse,
                     sizeof(reuse));
#endif

    Assert(InternalError, r == 0,
           boost::format("Setsockopt error %1%\n") % Glib::strerror(errno));

    r = ::listen(mSocket, 4);

    Assert(InternalError, r == 0,
           boost::format("Listen error %1%\n") % Glib::strerror(errno));
}

Server::~Server()
{
    if (mRunning) {
	int r;

	for (ClientsSocket::iterator it = mClientsSocket.begin();
	     it != mClientsSocket.end(); ++it) {
#ifdef G_OS_WIN32
            ::shutdown(mSocket, SD_BOTH);
            int r = ::closesocket(mSocket);
#else
            ::shutdown(mSocket, SHUT_RDWR);
	    r = ::close((*it).second);
#endif

            Assert(InternalError, r == 0, boost::format(
                    "Cannot close client socket properly, %1%\n") %
                Glib::strerror(errno));
	}
    }
}

int Server::accept_client(const Glib::ustring& name)
{
    Assert(InternalError,
           mClientsSocket.find(name) == mClientsSocket.end(),
           "A client with this name already exist.");

    struct sockaddr_in  adr;
#ifdef G_OS_WIN32
    int size = sizeof(adr);
#else
    socklen_t size = sizeof(adr);
#endif
    int r;

    r = ::accept(mSocket, (struct sockaddr*)&adr, &size);

    Assert(InternalError, r != -1,
           boost::format("Bad accept client '%1%', %2%\n") % name %
            Glib::strerror(errno));

    mClientsSocket[name] = r;

    return r;
}

int Server::get_socket_client(const Glib::ustring& name) const
{
    ClientsSocket::const_iterator it = mClientsSocket.find(name);

    Assert(InternalError, it != mClientsSocket.end(),
           boost::format("Unknow client name '%1%'\n") % name);

    return (*it).second;
}

void Server::close_client(const Glib::ustring& name)
{
    if (mRunning) {
        ClientsSocket::iterator it = mClientsSocket.find(name);

        Assert(InternalError, it != mClientsSocket.end(),
               boost::format("Unknow client name '%1%'\n") % name);

#ifdef G_OS_WIN32
        ::closesocket((*it).second);
#else
        ::close((*it).second);
#endif

        mClientsSocket.erase(it);
    }
}

int Server::get_socket_single_client() const
{
    Assert(InternalError, mClientsSocket.size() != 0,
           "No client are connected.");

    Assert(InternalError, mClientsSocket.size() == 1,
           "Too many client are connected.");

    return (*mClientsSocket.begin()).second;
}

}}} // namespace vle net utils
