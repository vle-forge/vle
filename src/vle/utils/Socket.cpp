/**
 * @file vle/utils/Socket.cpp
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


#include <vle/utils/Socket.hpp>
#include <vle/utils/SocketImpl.h>
#include <glib/gtypes.h>
#include <vle/utils/Debug.hpp>
#include <glibmm/stringutils.h>
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

    Assert < utils::ArgError >(lst.size() == 3, fmt(_(
            "Cannot find IP:port:directory from [%1%]")) % input);

    if (not lst[0].empty()) {
        ip.assign(lst[0]);
    }

    try {
        port = boost::lexical_cast < int >(lst[1]);
    } catch(const boost::bad_lexical_cast& e) {
        throw InternalError(fmt(_("Cannot convert '%1%' in TCP/IP port")) %
                            lst[1]);
    }

    Assert < utils::ArgError >(port > 0 and port < 65535, fmt(_(
                "Bad TCP/IP port value: %1%")) % port);

    directory.assign(lst[2]);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Base
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void Base::init()
{
    vleSocketInit();
}

Base::Base() :
    mRunning(false),
    mSocket(-1)
{
    int r = vleSocketOpen();
    Assert < utils::ArgError >(r != -1, fmt(_("Error acquiring socket: %1%\n")) %
           Glib::strerror(vleSocketErrno()));

    mSocket = r;

    if (vleSocketOptLinger(mSocket) == -1) {
        vleSocketClose(mSocket);
        throw InternalError(fmt(_(
                    "Error setsockopt socket: %1%\n")) %
              Glib::strerror(vleSocketErrno()));
    }

    mRunning = true;
}

Base::~Base()
{
    if (mRunning) {
        vleSocketShutdown(mSocket);
        vleSocketClose(mSocket);
    }
}

void Base::send(int dst, const void* buffer, int size)
{
    Assert < utils::ArgError >(
        mRunning, _("Cannot send message, socket is not initialised."));
    Assert < utils::ArgError >(
        buffer, _("Cannot send message, buffer is null."));
    Assert < utils::ArgError >(
        size > 0, _("Cannot send message, size must be superior to 0."));

    int position = 0;
    while (size > 0) {
        int szcurrent = vleSocketSend(dst, (char*)buffer + position, size);
	if (szcurrent == 0)
	    break;

        Assert < utils::InternalError >(
            szcurrent != -1, fmt(_("Cannot send message, %1%\n")) %
               Glib::strerror(vleSocketErrno()));

	position += szcurrent;
	size -= szcurrent;
    }
}

void Base::send_string(int dst, const Glib::ustring& buffer)
{
    vleSocketSend(dst, buffer.c_str(), std::strlen(buffer.c_str()));
}

void Base::send_buffer(int dst, const std::string& buffer)
{
    vleSocketSend(dst, buffer.c_str(), buffer.size());
}

void Base::send_int(int dst, gint32 buffer)
{
    vleSocketSend(dst, &buffer, sizeof(gint32));
}

int  Base::recv(int src, void* buffer, size_t size)
{
    Assert(mRunning, _("Client cannot receive message, socket is not initialised."));
    Assert(buffer, _("Client cannot receive message, buffer is null."));
    Assert(size > 0, _("Client cannot receive message, size equal to 0."));

    int current = vleSocketReceive(src, buffer, size);
    Assert(current != -1, fmt(_("Client cannot receive message, %1%\n"))
           % Glib::strerror(vleSocketErrno()));

    ((char*)buffer)[current] = '\0';
    return current;
}

Glib::ustring Base::recv_string(int src)
{
    Assert(mRunning, _("Client cannot receive message client is not connected."));

    char* buffer = new char[1024];
    Glib::ustring finished_buffer;

    int size = vleSocketReceive(src, buffer, 1023);

    if (size == -1) {
        delete[] buffer;
        throw InternalError(fmt(_(
                    "Client cannot receive message, %1%\n")) %
              Glib::strerror(vleSocketErrno()));
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

    int current;

    while (size > 0) {
        try {
            if (size > 1024) {
                current = vleSocketReceive(src, buffer, 1024);
            } else {
                current = vleSocketReceive(src, buffer, size);
            }
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
    Assert(mRunning, _("Socket cannot receive message, socket not initialised."));

    gint32 buffer;
    int current = vleSocketReceive(src, &buffer, sizeof(gint32));

    Assert(current == sizeof(gint32), fmt(_(
                "Client cannot receive message, %1%\n")) %
        Glib::strerror(vleSocketErrno()));

    return buffer;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Client
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

Client::Client(const std::string& server, int port)
{
    Assert(mRunning, _("Cannot open server, socket failed."));

    int r = vleSocketConnect(mSocket, server.c_str(), port);
    Assert(r == 0, fmt(_("Error connection with server, %1%\n")) %
           Glib::strerror(vleSocketErrno()));

    mRunning = true;
}

void Client::close()
{
    if (mRunning) {
        vleSocketShutdown(mSocket);
        int r = vleSocketClose(mSocket);
        Assert(r == 0, fmt(_("Cannot close client socket properly, %1%\n")) %
               Glib::strerror(vleSocketErrno()));
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
    Assert(mRunning, _("Cannot open server, socket failed"));

    int r;
    r = vleSocketBind(mSocket, port);
    Assert(r == 0, fmt(_("Bind error %1%\n")) %
           Glib::strerror(vleSocketErrno()));

    r = vleSocketOptReuseAddr(mSocket);
    Assert(r == 0, fmt(_("Setsockopt error %1%\n")) %
           Glib::strerror(vleSocketErrno()));

    r = vleSocketListen(mSocket, 4);
    Assert(r == 0, fmt(_("Listen error %1%\n")) %
           Glib::strerror(vleSocketErrno()));
}

Server::~Server()
{
    if (mRunning) {
	int r;

	for (ClientsSocket::iterator it = mClientsSocket.begin();
	     it != mClientsSocket.end(); ++it) {
            vleSocketShutdown(mSocket);
            r = vleSocketClose(mSocket);
            Assert(r == 0, fmt(_("Cannot close client socket properly, %1%\n"))
                   % Glib::strerror(vleSocketErrno()));
	}
    }
}

int Server::accept_client(const Glib::ustring& name)
{
    Assert(mClientsSocket.find(name) == mClientsSocket.end(),
           _("A client with this name already exist."));

    int r = vleSocketAccept(mSocket);
    Assert(r != -1, fmt(_("Bad accept client '%1%', %2%\n")) % name %
           Glib::strerror(vleSocketErrno()));

    mClientsSocket[name] = r;

    return r;
}

int Server::get_socket_client(const Glib::ustring& name) const
{
    ClientsSocket::const_iterator it = mClientsSocket.find(name);

    Assert(it != mClientsSocket.end(), fmt(_("Unknow client name '%1%'\n")) %
           name);

    return (*it).second;
}

void Server::close_client(const Glib::ustring& name)
{
    if (mRunning) {
        ClientsSocket::iterator it = mClientsSocket.find(name);

        Assert(it != mClientsSocket.end(),
               fmt(_("Unknow client name '%1%'\n")) % name);

        vleSocketClose((*it).second);
        mClientsSocket.erase(it);
    }
}

int Server::get_socket_single_client() const
{
    Assert(mClientsSocket.size() != 0, _("No client are connected."));
    Assert(mClientsSocket.size() == 1, _("Too many client are connected."));

    return (*mClientsSocket.begin()).second;
}

}}} // namespace vle net utils
