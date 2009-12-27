/**
 * @file vle/utils/SocketImpl.c
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
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


#include <vle/utils/SocketImpl.h>

#ifdef __WIN32__
#include <winsock2.h>
#include <stdio.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#endif

void vleSocketInit(void)
{
#ifdef __WIN32__
        WSADATA wsa;
        if(WSAStartup(MAKEWORD(1, 1), &wsa)) {
                printf("WSAStartup failed.");
                exit(0);
        }
#endif
}

int vleSocketOpen(void)
{
        return socket(AF_INET, SOCK_STREAM, 0);
}

int vleSocketOptLinger(int fd)
{
        struct linger l;
        int r;
        l.l_onoff = 1;
        l.l_linger = 0;

#ifdef __WIN32__
        r = setsockopt(fd, SOL_SOCKET, SO_LINGER, (const char*)&l, sizeof(l));
#else
        r = setsockopt(fd, SOL_SOCKET, SO_LINGER, &l, sizeof(l));
#endif

        return r;
}

int vleSocketOptReuseAddr(int fd)
{
        int reuse = 1;

#ifdef __WIN32__
        return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*) &reuse,
                        sizeof(reuse));
#else
        return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (int*) &reuse,
                        sizeof(reuse));
#endif
}

int vleSocketListen(int fd, int nb)
{
        return listen(fd, nb);
}

int vleSocketConnect(int fd, const char* str, int port)
{
        struct sockaddr_in host;
        struct hostent* hoste;

        memset(&host, '\0', sizeof(host));
        host.sin_family = AF_INET;
        host.sin_port = htons(port);

        hoste = gethostbyname(str);
        if (hoste == NULL) {
                host.sin_addr.s_addr = INADDR_ANY;
        } else {
                memcpy(&host.sin_addr, hoste->h_addr_list[0], hoste->h_length);
        }

        return connect(fd, (struct sockaddr*)&host, sizeof(host));
}

int vleSocketBind(int fd, int port)
{
        struct sockaddr_in adr;

        memset(&adr, 0, sizeof(adr));
        adr.sin_family = AF_INET;
        adr.sin_port = htons(port);
        adr.sin_addr.s_addr = INADDR_ANY;

        return bind(fd, (struct sockaddr*)&adr, sizeof(adr));
}

int vleSocketAccept(int fd)
{
        struct sockaddr_in  adr;

#ifdef __WIN32__
        int size = sizeof(adr);
#else
        socklen_t size = sizeof(adr);
#endif
        return accept(fd, (struct sockaddr*)&adr, &size);
}

int vleSocketClose(int fd)
{
#ifdef __WIN32__
        return closesocket(fd);
#else
        return close(fd);
#endif
}

int vleSocketShutdown(int fd)
{
#ifdef __WIN32__
        return shutdown(fd, SD_BOTH);
#else
        return shutdown(fd, SHUT_RDWR);
#endif
}

int vleSocketSend(int fd, const void* buffer, int size)
{
        return send(fd, (char*)buffer, size, 0);
}

int vleSocketReceive(int fd, void* buffer, int size)
{
        int r;
#ifdef __WIN32__
        r = recv(fd, (char*)buffer, size, 0);
#else
        r = recv(fd, buffer, size, 0);
#endif
        if (r == -1) {
                ((char*)buffer)[0] = '\0';
        } else if (r >= 0 && r < size) {
                ((char*)buffer)[r] = '\0';
        }

        return r;
}

int vleSocketErrno(void)
{
        return errno;
}
