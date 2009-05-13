/**
 * @file vle/devs/NetStreamWriter.cpp
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


#include <vle/devs/NetStreamWriter.hpp>
#include <vle/devs/Simulator.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Path.hpp>
#include <vle/oov/PluginFactory.hpp>
#include <string>


namespace vle { namespace devs {

NetStreamWriter::NetStreamWriter() :
    m_paramFrame(0),
    m_newObsFrame(0),
    m_delObsFrame(0),
    m_valueFrame(0),
    m_closeFrame(0),
    m_refreshFrame(0),
    m_client(0)
{
}

NetStreamWriter::~NetStreamWriter()
{
    delete m_paramFrame;
    delete m_newObsFrame;
    delete m_delObsFrame;
    delete m_valueFrame;
    delete m_closeFrame;
    delete m_refreshFrame;
    delete m_client;
}

void NetStreamWriter::open(const std::string& plugin,
                           const std::string& location,
                           const std::string& file,
                           value::Value* parameters,
                           const devs::Time& time)
{
    std::string host, directory, out;
    int port;

    try {
        utils::net::explodeStringNet(location, host, port, directory);
        m_client = new utils::net::Client(host, port);
        TraceInformation(boost::format("NetStreamWriter init %1%:%2% %3% %4%")
                         % host % port % directory % file);

        buildParameters(plugin, directory, file, parameters, time.getValue());

        value::Set::serializeBinaryBuffer(*m_paramFrame, out);

        m_client->send_int(out.size());
        m_client->send_buffer(out);
    } catch(const std::exception& e) {
        throw utils::InternalError(boost::format(
                "NetStreamWriter open: error opening the client connection "
                "with the OOV plugin, hostname %1% type %2% port %3%. "
                "Error reported: %4%") % host % location % port %
            e.what());
    }
}

void NetStreamWriter::processNewObservable(Simulator* simulator,
                                           const std::string& portname,
                                           const devs::Time& time,
                                           const std::string& view)
{
    buildNewObs(simulator->getName(), simulator->getParent(), portname, view,
                time.getValue());

    std::string out;
    value::Set::serializeBinaryBuffer(*m_newObsFrame, out);

    try {
        m_client->send_int(out.size());
        m_client->send_buffer(out);
    } catch(const std::exception& e) {
        throw utils::InternalError(boost::format(
                "NetStreamWriter new observable: error writing new observable. "
                "Error reported: %1%") % e.what());
    }
}

void NetStreamWriter::processRemoveObservable(Simulator* simulator,
                                              const std::string& portname,
                                              const devs::Time& time,
                                              const std::string& view)
{
    buildDelObs(simulator->getName(), simulator->getParent(), portname, view,
                time.getValue());

    std::string out;
    value::Set::serializeBinaryBuffer(*m_delObsFrame, out);

    try {
        m_client->send_int(out.size());
        m_client->send_buffer(out);
    } catch(const std::exception& e) {
        throw utils::InternalError(boost::format(
                "NetStreamWriter delete observable: error writing new"
               " observable. Error reported: %1%") % e.what());
    }
}

void NetStreamWriter::process(ObservationEvent& event)
{
    value::Value* val = 0;
    if (event.haveAttributes()) {
        value::Map::iterator
            it(event.getAttributes().value().find(event.getPortName()));
        if (it != event.getAttributes().value().end()) {
            val = it->second;
            it->second = 0;
            event.getAttributes().value().erase(it);
        }
    }

    buildValue(event.getModel()->getName(), event.getModel()->getParent(),
               event.getPortName(), event.getViewName(),
               event.getTime().getValue(), val);

    std::string out;
    value::Set::serializeBinaryBuffer(*m_valueFrame, out);

    try {
        m_client->send_int(out.size());
        m_client->send_buffer(out);
    } catch(const std::exception& e) {
        throw utils::InternalError(boost::format(
                "NetStreamWriter process state event: error writing a state"
                " event. Error reported: %1%") % e.what());
    }
}

oov::PluginPtr NetStreamWriter::close(const devs::Time& time)
{
    buildClose(time.getValue());

    std::string out;
    value::Set::serializeBinaryBuffer(*m_closeFrame, out);

    try {
        m_client->send_int(out.size());
        m_client->send_buffer(out);

        oov::PluginPtr plugin = getPlugin();

        m_client->close();
        delete m_client;
        m_client = 0;
        return plugin;
    } catch(const std::exception& e) {
        throw utils::InternalError(boost::format(
                "NetStreamWriter close: OOV does not respond. "
                "Error reported: %1%") % e.what());
    }
}

oov::PluginPtr NetStreamWriter::refreshPlugin()
{
    buildRefresh();

    std::string out;
    value::Set::serializeBinaryBuffer(*m_refreshFrame, out);

    try {
        oov::PluginPtr plugin;
        if (m_client) {
            m_client->send_int(out.size());
            m_client->send_buffer(out);
            plugin = getPlugin();
        }
        return plugin;
    } catch(const std::exception& e) {
        throw utils::InternalError(boost::format(
                "NetStreamWriter refreshPlugin: OOV does not respond."
                "Error reported: %1%") % e.what());
    }
}

oov::PluginPtr NetStreamWriter::getPlugin() const
{
    std::string result = m_client->recv_string();
    if (result == "ok") {
        m_client->send_buffer("ok");

        int sz = m_client->recv_int();
        m_client->send_buffer("ok");

        result = m_client->recv_buffer(sz);
        m_client->send_buffer("ok");

        value::Set vals;
        value::Set::deserializeBinaryBuffer(vals, result);

        const std::string& name(vals.get(0).toString().value());

        utils::Path::PathList lst(utils::Path::path().getStreamDirs());
        utils::Path::PathList::const_iterator it;
        oov::PluginPtr plugin;
        std::string error;

        for (it = lst.begin(); it != lst.end(); ++it) {
            try {
                oov::PluginFactory pf(name, *it);
                plugin = pf.build("");
            } catch(const std::exception& e) {
                error += e.what();
            }
        }

        if (not plugin.get()) {
            throw utils::ArgError(error);
        } else {
            plugin->deserialize(vals.get(1));
            return plugin;
        }
    }
    return oov::PluginPtr();
}

void NetStreamWriter::buildParameters(const std::string& plugin,
                                      const std::string& directory,
                                      const std::string& file,
                                      value::Value* parameters,
                                      const double& time)
{
    if (m_paramFrame == 0) {
        m_paramFrame = new value::Set();
        m_paramFrame->addInt(0);
        m_paramFrame->addString(plugin);
        m_paramFrame->addString(directory);
        m_paramFrame->addString(file);
        m_paramFrame->add(parameters);
        m_paramFrame->addDouble(time);
    } else {
        m_paramFrame->get(1).toString().value() = plugin;
        m_paramFrame->get(2).toString().value() = directory;
        m_paramFrame->get(3).toString().value() = file;
        m_valueFrame->value()[4] = parameters;
        m_paramFrame->get(5).toDouble().value() = time;
    }
}

void NetStreamWriter::buildNewObs(const std::string& simulator,
                                  const std::string& parents,
                                  const std::string& port,
                                  const std::string& view,
                                  const double& time)
{
    if (m_newObsFrame == 0) {
        m_newObsFrame = new value::Set();
        m_newObsFrame->addInt(1);
        m_newObsFrame->addString(simulator);
        m_newObsFrame->addString(parents);
        m_newObsFrame->addString(port);
        m_newObsFrame->addString(view);
        m_newObsFrame->addDouble(time);
    } else {
        m_newObsFrame->get(1).toString().value() = simulator;
        m_newObsFrame->get(2).toString().value() = parents;
        m_newObsFrame->get(3).toString().value() = port;
        m_newObsFrame->get(4).toString().value() = view;
        m_newObsFrame->get(5).toDouble().value() = time;
    }
}

void NetStreamWriter::buildDelObs(const std::string& simulator,
                                  const std::string& parents,
                                  const std::string& port,
                                  const std::string& view,
                                  const double& time)
{
    if (m_delObsFrame == 0) {
        m_delObsFrame = new value::Set();
        m_delObsFrame->addInt(2);
        m_delObsFrame->addString(simulator);
        m_delObsFrame->addString(parents);
        m_delObsFrame->addString(port);
        m_delObsFrame->addString(view);
        m_delObsFrame->addDouble(time);
    } else {
        m_delObsFrame->get(1).toString().value() = simulator;
        m_delObsFrame->get(2).toString().value() = parents;
        m_delObsFrame->get(3).toString().value() = port;
        m_delObsFrame->get(4).toString().value() = view;
        m_delObsFrame->get(5).toDouble().value() = time;
    }
}

void NetStreamWriter::buildValue(const std::string& simulator,
                                 const std::string& parents,
                                 const std::string& port,
                                 const std::string& view,
                                 const double& time,
                                 value::Value* value)
{
    if (m_valueFrame == 0) {
        m_valueFrame = new value::Set();
        m_valueFrame->addInt(3);
        m_valueFrame->addString(simulator);
        m_valueFrame->addString(parents);
        m_valueFrame->addString(port);
        m_valueFrame->addString(view);
        m_valueFrame->addDouble(time);
        m_valueFrame->add(value);
    } else {
        m_valueFrame->get(1).toString().value() = simulator;
        m_valueFrame->get(2).toString().value() = parents;
        m_valueFrame->get(3).toString().value() = port;
        m_valueFrame->get(4).toString().value() = view;
        m_valueFrame->get(5).toDouble().value() = time;
        m_valueFrame->value()[6] = value;
    }
}

void NetStreamWriter::buildClose(const double& time)
{
    if (m_closeFrame == 0) {
        m_closeFrame = new value::Set();
        m_closeFrame->addInt(4);
        m_closeFrame->addDouble(time);
    } else {
        m_closeFrame->get(1).toDouble().value() = time;
    }
}

void NetStreamWriter::buildRefresh()
{
    if (m_refreshFrame == 0) {
        m_refreshFrame = new value::Set();
        m_refreshFrame->addInt(5);
    }
}

}} // namespace vle devs
