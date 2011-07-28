/*
 * @file vle/utils/Socket.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
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


#include <vle/utils/Socket.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/SocketImpl.h>
#include <vle/utils/i18n.hpp>
#include <glibmm/stringutils.h>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>

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
                std::string tmp(input, position + 1, input.size() - position +
                                1);
                if (utils::is < uint32_t >(tmp)) {
                    port = utils::to < uint32_t >(tmp);
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

    if (lst.size() != 3) {
        throw utils::ArgError(fmt(
                _("Cannot find IP:port:directory from [%1%]")) % input);
    }

    if (not lst[0].empty()) {
        ip.assign(lst[0]);
    }

    try {
        port = boost::lexical_cast < int >(lst[1]);
    } catch(const boost::bad_lexical_cast& e) {
        throw InternalError(fmt(
                _("Cannot convert '%1%' in TCP/IP port")) % lst[1]);
    }

    if (port <= 0 or port > 65535) {
        throw utils::ArgError(fmt(_("Bad TCP/IP port value: %1%")) % port);
    }

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

    if (r == -1) {
        throw ArgError(fmt(_("Error acquiring socket: %1%\n")) %
                       Glib::strerror(vleSocketErrno()));
    }

    mSocket = r;

    if (vleSocketOptLinger(mSocket) == -1) {
        vleSocketClose(mSocket);
        throw InternalError(fmt(_("Error setsockopt socket: %1%\n")) %
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
    if (not mRunning) {
        throw InternalError(_("Cannot send message, socket is not "
                              "initialised."));
    }

    if (buffer == 0) {
        throw InternalError(_("Cannot send message, buffer is null."));
    }

    if (size <= 0) {
        throw InternalError(_("Cannot send message, size must be superior "
                              " to 0."));
    }

    int position = 0;
    while (size > 0) {
        int szcurrent = vleSocketSend(dst, (char*)buffer + position, size);
	if (szcurrent == 0)
	    break;

        if (szcurrent == -1) {
            throw InternalError(fmt(
                    _("Cannot send message, %1%\n")) %
                Glib::strerror(vleSocketErrno()));
        }

	position += szcurrent;
	size -= szcurrent;
    }
}

void Base::sendString(int dst, const std::string& buffer)
{
    vleSocketSend(dst, buffer.c_str(), std::strlen(buffer.c_str()));
}

void Base::sendBuffer(int dst, const std::string& buffer)
{
    vleSocketSend(dst, buffer.c_str(), buffer.size());
}

void Base::sendInteger(int dst, boost::int32_t buffer)
{
    vleSocketSend(dst, &buffer, sizeof(boost::int32_t));
}

int  Base::recv(int src, void* buffer, size_t size)
{
    if (not mRunning) {
        throw InternalError(
            _("Client cannot receive message, socket is not initialised."));
    }

    if (not buffer) {
        throw InternalError(
            _("Client cannot receive message, buffer is null."));
    }

    if (size <= 0) {
        throw InternalError(
            _("Client cannot receive message, size equal to 0."));
    }

    int current = vleSocketReceive(src, buffer, size);
    if (current == -1) {
        throw InternalError(fmt(
                _("Client cannot receive message, %1%\n")) %
            Glib::strerror(vleSocketErrno()));
    }

    ((char*)buffer)[current] = '\0';
    return current;
}

std::string Base::recvString(int src)
{
    if (not mRunning) {
        throw InternalError(
            _("Client cannot receive message client is not " "connected."));
    }

    char* buffer = new char[1024];
    std::string finished_buffer;

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

std::string Base::recvBuffer(int src, size_t size)
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

boost::uint32_t Base::recvInteger(boost::int32_t src)
{
    if (not mRunning) {
        throw InternalError(
            _("Socket cannot receive message, socket not initialised."));
    }

    boost::uint32_t buffer;
    int current = vleSocketReceive(src, &buffer, sizeof(boost::uint32_t));

    if (current != sizeof(boost::uint32_t)) {
        throw utils::InternalError(fmt(
                _("Client cannot receive message, %1%\n")) %
            Glib::strerror(vleSocketErrno()));
    }

    return buffer;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * Client
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

Client::Client(const std::string& server, int port)
{
    if (not mRunning) {
        throw InternalError(_("Cannot open server, socket failed."));
    }

    int r = vleSocketConnect(mSocket, server.c_str(), port);

    if (r != 0) {
        throw InternalError(fmt(
                _("Error connection with server, %1%\n")) %
            Glib::strerror(vleSocketErrno()));
    }

    mRunning = true;
}

void Client::close()
{
    if (mRunning) {
        vleSocketShutdown(mSocket);
        int r = vleSocketClose(mSocket);

        if (r != 0) {
            throw InternalError(fmt(
                    _("Cannot close client socket properly, %1%\n")) %
                Glib::strerror(vleSocketErrno()));
        }

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
    if (not mRunning) {
        throw InternalError(_("Cannot open server, socket failed"));
    }

    int r;
    r = vleSocketBind(mSocket, port);

    if (r != 0) {
        throw InternalError(fmt(_("Bind error %1%\n")) %
                            Glib::strerror(vleSocketErrno()));
    }

    r = vleSocketOptReuseAddr(mSocket);

    if (r != 0) {
        throw InternalError(fmt(_("Setsockopt error %1%\n")) %
            Glib::strerror(vleSocketErrno()));
    }

    r = vleSocketListen(mSocket, 4);

    if (r != 0) {
        throw InternalError(fmt(_("Listen error %1%\n")) %
                            Glib::strerror(vleSocketErrno()));
    }
}

Server::~Server()
{
    if (mRunning) {
	int r;

	for (ClientsSocket::iterator it = mClientsSocket.begin();
	     it != mClientsSocket.end(); ++it) {
            vleSocketShutdown(mSocket);
            r = vleSocketClose(mSocket);

            if (r != 0) {
                throw InternalError(fmt(
                        _("Cannot close client socket properly, %1%\n")) %
                    Glib::strerror(vleSocketErrno()));
            }
	}
    }
}

int Server::acceptClient(const std::string& name)
{
    if (mClientsSocket.find(name) != mClientsSocket.end()) {
        throw InternalError(_("A client with this name already exist."));
    }

    int r = vleSocketAccept(mSocket);

    if (r == -1) {
        throw InternalError(fmt(_("Bad accept client '%1%', %2%\n")) % name %
                            Glib::strerror(vleSocketErrno()));
    }

    mClientsSocket[name] = r;

    return r;
}

int Server::getSocketClient(const std::string& name) const
{
    ClientsSocket::const_iterator it = mClientsSocket.find(name);

    if (it == mClientsSocket.end()) {
        throw utils::ArgError(fmt(_("Unknow client name '%1%'\n")) % name);
    }

    return (*it).second;
}

void Server::closeClient(const std::string& name)
{
    if (mRunning) {
        ClientsSocket::iterator it = mClientsSocket.find(name);

        if (it == mClientsSocket.end()) {
            throw utils::ArgError(fmt(_("Unknow client name '%1%'\n")) % name);
        }

        vleSocketClose((*it).second);
        mClientsSocket.erase(it);
    }
}

int Server::getSocketSingleClient() const
{
    if (mClientsSocket.size() == 0) {
        throw InternalError(_("No client connected."));
    }

    if (mClientsSocket.size() == 1) {
        throw InternalError(_("Too many client connected."));
    }

    return (*mClientsSocket.begin()).second;
}

}}} // namespace vle net utils
