/*
 * @file vle/utils/SocketImpl.h
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


#ifndef VLE_UTILS_SOCKETIMPL_HPP
#define VLE_UTILS_SOCKETIMPL_HPP

#include <vle/DllDefines.hpp>

#ifdef __cplusplus
extern "C" {
#endif

    void VLE_API vleSocketInit(void);

    int VLE_API vleSocketOpen(void);

    int VLE_API vleSocketOptLinger(int fd);

    int VLE_API vleSocketOptReuseAddr(int fd);

    int VLE_API vleSocketConnect(int fd, const char* host, int port);

    int VLE_API vleSocketListen(int fd, int nb);

    int VLE_API vleSocketBind(int fd, int port);

    int VLE_API vleSocketAccept(int fd);

    int VLE_API vleSocketClose(int fd);

    int VLE_API vleSocketShutdown(int fd);

    int VLE_API vleSocketSend(int fd, const void* buffer, int size);

    int VLE_API vleSocketReceive(int fd, void* buffer, int size);

    int VLE_API vleSocketErrno(void);

#ifdef __cplusplus
}
#endif

#endif
