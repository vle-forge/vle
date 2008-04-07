/**
 * @file src/vle/oov/NetStreamReader.cpp
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




#include <vle/oov/NetStreamReader.hpp>
#include <vle/oov/Plugin.hpp>
#include <vle/vpz/SaxParser.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/vpz/ParameterTrame.hpp>
#include <vle/vpz/ValueTrame.hpp>
#include <vle/vpz/DelObservableTrame.hpp>
#include <vle/vpz/NewObservableTrame.hpp>
#include <vle/vpz/EndTrame.hpp>
#include <vle/vpz/RefreshTrame.hpp>
#include <vle/utils/Socket.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Set.hpp>



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
            onValue(*tr);
            delete tr;
            return false;
        }
    }
    {
        const vpz::ParameterTrame* tr;
        if ((tr = dynamic_cast < const vpz::ParameterTrame* >(trame))) {
            TraceAlways(boost::format(
                    "NetStreamReader plugin: [%1%] and location [%2%]") %
                tr->plugin() % tr->location());
            onParameter(*tr);
            delete tr;
            return false;
        }
    }
    {
        const vpz::NewObservableTrame* tr;
        if ((tr = dynamic_cast < const vpz::NewObservableTrame* >(trame))) {
            onNewObservable(*tr);
            delete tr;
            return false;
        }
    }
    {
        const vpz::DelObservableTrame* tr;
        if ((tr = dynamic_cast < const vpz::DelObservableTrame* >(trame))) {
            onDelObservable(*tr);
            delete tr;
            return false;
        }
    }
    {
        const vpz::EndTrame* tr;
        if ((tr = dynamic_cast < const vpz::EndTrame* >(trame))) {
            TraceAlways("NetStreamReader close");
            serializePlugin();
            onClose(*tr);
            delete tr;
            return true;
        }
    }
    {
        const vpz::RefreshTrame* tr;
        if ((tr = dynamic_cast < const vpz::RefreshTrame* >(trame))) {
            serializePlugin();
            return false;
        }
    }

    Throw(utils::InternalError, "Distant stream reader failed: unknow trame");
}

void NetStreamReader::closeConnection()
{
    m_server.close_client("vle");
}

void NetStreamReader::serializePlugin()
{
    if (plugin()->isSerializable()) {
        m_server.send_buffer("vle", "ok");
        m_server.recv_string("vle");

        value::Set vals = value::SetFactory::create();
        vals->addValue(value::StringFactory::create(plugin()->name()));
        vals->addValue(plugin()->serialize());

        std::string result = vals->toXML();
        m_server.send_int("vle", result.size());
        m_server.recv_string("vle");
        m_server.send_buffer("vle", result);
        m_server.recv_string("vle");
    } else {
        m_server.send_buffer("vle", "no");
    }
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
