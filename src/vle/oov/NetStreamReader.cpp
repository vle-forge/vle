/** 
 * @file NetStreamReader.cpp
 * @brief 
 * @author The vle Development Team
 * @date 2007-08-22
 */

/*
 * Copyright (C) 2007 - The vle Development Team
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <vle/oov/NetStreamReader.hpp>
#include <vle/oov/Plugin.hpp>
#include <vle/vpz/SaxParser.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/vpz/ParameterTrame.hpp>
#include <vle/vpz/ValueTrame.hpp>
#include <vle/vpz/DelObservableTrame.hpp>
#include <vle/vpz/NewObservableTrame.hpp>
#include <vle/vpz/EndTrame.hpp>
#include <vle/utils/Socket.hpp>
#include <vle/utils/Trace.hpp>



namespace vle { namespace oov {

NetStreamReader::NetStreamReader(int port) :
    m_port(port),
    m_server(port)
{
    setBufferSize(4096);
    TraceAlways(boost::format("Build a NetStreamReader on port %1%") % port);
}

NetStreamReader::~NetStreamReader()
{
}

void NetStreamReader::process()
{
    for (; ;) {
        waitConnection();

        readConnection();

        closeConnection();
    }
}

void NetStreamReader::waitConnection()
{
    m_server.accept_client("vle");
    TraceAlways(boost::format(
            "NetStreamReader connection with client %1%") % "vle");
}

void NetStreamReader::readConnection()
{
    vpz::Vpz vpz;
    vpz::SaxParser sax(vpz);
    bool trameend = false;

    while (not sax.isEndTrame()) {
        m_buffer = m_server.recv_string("vle");
        sax.parse_chunk(m_buffer);
        while (not sax.tramelist().empty()) {
            trameend = dispatch(sax.tramelist().front());
            sax.tramelist().pop_front();
        }
    }
}

bool NetStreamReader::dispatch(const vpz::Trame* trame)
{
    {
        const vpz::ValueTrame* tr;
        if ((tr = dynamic_cast < const vpz::ValueTrame* >(trame))) {
            plugin()->onValue(*tr);
            return false;
        }
    }
    {
        const vpz::ParameterTrame* tr;
        if ((tr = dynamic_cast < const vpz::ParameterTrame* >(trame))) {
            TraceAlways(boost::format(
                    "NetStreamReader plugin: [%1%] and location [%2%]") %
                tr->plugin() % tr->location());
            init_plugin(tr->plugin(), tr->location());
            plugin()->onParameter(*tr);
            return false;
        }
    }
    {
        const vpz::NewObservableTrame* tr;
        if ((tr = dynamic_cast < const vpz::NewObservableTrame* >(trame))) {
            plugin()->onNewObservable(*tr);
            return false;
        }
    }
    {
        const vpz::DelObservableTrame* tr;
        if ((tr = dynamic_cast < const vpz::DelObservableTrame* >(trame))) {
            plugin()->onDelObservable(*tr);
            return false;
        }
    }
    {
        const vpz::EndTrame* tr;
        if ((tr = dynamic_cast < const vpz::EndTrame* >(trame))) {
            TraceAlways("NetStreamReader close");
            plugin()->close(*tr);
            return true;
        }
    }

    Throw(utils::InternalError, "Distant stream reader failed: unknow trame");
}

void NetStreamReader::closeConnection()
{
    m_server.close_client("vle");
}

void NetStreamReader::setBufferSize(size_t buffer)
{
    Assert(utils::InternalError, buffer != 0,
           "Cannot build a buffer with an empty size");

    Assert(utils::InternalError, buffer < m_buffer.max_size(), boost::format(
            "Cannot allocate a buffer of size %1%") % buffer);

    m_buffer.reserve(buffer);
}

}} // namespace vle oov
