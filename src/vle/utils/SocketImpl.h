/**
 * @file src/vle/utils/SocketImpl.h
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


#ifndef VLE_UTILS_SOCKETIMPL_HPP
#define VLE_UTILS_SOCKETIMPL_HPP

#ifdef __cplusplus
extern "C" {
#endif

    void vleSocketInit(void);

    int vleSocketOpen(void);

    int vleSocketOptLinger(int fd);

    int vleSocketOptReuseAddr(int fd);
    
    int vleSocketConnect(int fd, const char* host, int port);

    int vleSocketListen(int fd, int nb);

    int vleSocketBind(int fd, int port);
    
    int vleSocketAccept(int fd);

    int vleSocketClose(int fd);

    int vleSocketShutdown(int fd);

    int vleSocketSend(int fd, const void* buffer, int size);

    int vleSocketReceive(int fd, void* buffer, int size);

    int vleSocketErrno(void);

#ifdef __cplusplus
}
#endif

#endif
