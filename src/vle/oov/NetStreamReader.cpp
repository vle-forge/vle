/*
 * @file vle/oov/NetStreamReader.cpp
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


#include <vle/oov/NetStreamReader.hpp>
#include <vle/oov/Plugin.hpp>
#include <vle/utils/Socket.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/Path.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/version.hpp>

#ifdef VLE_HAVE_CAIRO
#   include <vle/oov/CairoPlugin.hpp>
#endif


namespace vle { namespace oov {

NetStreamReader::NetStreamReader(const utils::ModuleManager& module, int port)
    : StreamReader(module), m_port(port),
    m_server(new utils::net::Server(port)), m_image(0)
{
    setBufferSize(4096);
    TraceAlways(fmt(_("Build a NetStreamReader on port %1%")) % port);
}

NetStreamReader::~NetStreamReader()
{
    delete m_server;
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
    m_server->acceptClient("vle");
    TraceAlways(fmt(_("NetStreamReader connection with client %1%")) % "vle");
}

void NetStreamReader::readConnection()
{
    bool stop = false;

    while (not stop) {
        try {
            int32_t val = m_server->recvInteger("vle");

            if (val <= 0) {
                throw utils::InternalError(
                    _("NetStreamReader: bad size for package"));
            }

            m_buffer = m_server->recvBuffer("vle", val);

            value::Set frame;
            value::Set::deserializeBinaryBuffer(frame, m_buffer);
            stop = dispatch(frame);
        } catch (const std::exception& e) {
            throw(utils::InternalError(fmt(
                    _("NetStreamReader: %1%")) % e.what()));
        }
    }
}

bool NetStreamReader::dispatch(value::Set& frame)
{
    bool end = false;

    switch (frame.getInt(0)) {
    case 0:
        onParameter(frame.getString(1),
                    frame.getString(2),
                    frame.getString(3),
                    frame.getString(4),
                    frame.give(5),
                    frame.getDouble(6));
        break;
    case 1:
        onNewObservable(frame.getString(1),
                        frame.getString(2),
                        frame.getString(3),
                        frame.getString(4),
                        frame.getDouble(5));
        break;
    case 2:
        onDelObservable(frame.getString(1),
                        frame.getString(2),
                        frame.getString(3),
                        frame.getString(4),
                        frame.getDouble(5));
        break;
    case 3:
        onValue(frame.getString(1),
                frame.getString(2),
                frame.getString(3),
                frame.getString(4),
                frame.getDouble(5),
                frame.give(6));
        break;
    case 4:
        serializePlugin();
        onClose(frame.getDouble(1));
        end = true;
        break;
    case 5:
        serializePlugin();
        break;
    default:
        throw(utils::InternalError(fmt(
                _("NetStreamReader: unknow tag '%1%'")) %
            frame.getInt(0)));
    }
    return end;
}

void NetStreamReader::closeConnection()
{
    m_server->closeClient("vle");
}

void NetStreamReader::close()
{
    delete m_server;
    m_server = 0;
}

void NetStreamReader::onValue(const std::string& simulator,
                              const std::string& parent,
                              const std::string& port,
                              const std::string& view,
                              const double& time,
                              value::Value* value)
{
#ifdef VLE_HAVE_CAIRO
    if (plugin()->isCairo()) {
        CairoPluginPtr plg = toCairoPlugin(plugin());
        plg->needCopy();
        plugin()->onValue(simulator, parent, port, view, time, value);
        if (plg->isCopyDone()) {
            std::string file(utils::Path::buildFilename(
                    plg->location(), (fmt("img-%1$08d.png") % m_image).str()));

            try {
                plg->stored()->write_to_png(file);
                m_image++;
            } catch(const std::exception& /*e*/) {
                throw utils::InternalError(fmt(
                        _("oov: cannot write image '%1%'")) % file);
            }
        }
    } else {
#endif
        plugin()->onValue(simulator, parent, port, view, time, value);
#ifdef VLE_HAVE_CAIRO
    }
#endif
}

void NetStreamReader::serializePlugin()
{
    if (plugin()->isSerializable()) {
        m_server->sendBuffer("vle", "ok");
        m_server->recvString("vle");

        value::Set* vals = value::Set::create();
        vals->add(plugin()->serialize());

        std::string out;
        value::Set::serializeBinaryBuffer(*vals, out);
        delete vals;

        m_server->sendInteger("vle", out.size());
        m_server->recvString("vle");
        m_server->sendBuffer("vle", out);
        m_server->recvString("vle");
    } else {
        m_server->sendBuffer("vle", "no");
    }
}

void NetStreamReader::setBufferSize(size_t buffer)
{
    if (buffer <= 0 or buffer >= m_buffer.max_size()) {
        throw utils::InternalError(fmt(
                _("Can not allocate a buffer of size '%1%'")) % buffer);
    }

    m_buffer.reserve(buffer);
}

}} // namespace vle oov
