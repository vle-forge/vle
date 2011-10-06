/*
 * @file vle/devs/NetStreamWriter.cpp
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


#include <vle/devs/NetStreamWriter.hpp>
#include <vle/devs/Simulator.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Path.hpp>
#include <vle/oov/PluginFactory.hpp>
#include <string>


namespace vle { namespace devs {

NetStreamWriter::NetStreamWriter()
    : m_paramFrame(0), m_newObsFrame(0), m_delObsFrame(0), m_valueFrame(0),
    m_closeFrame(0), m_refreshFrame(0), m_client(0)
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
                           const std::string& package,
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

        buildParameters(plugin, package, directory, file, parameters,
                        time.getValue());

        value::Set::serializeBinaryBuffer(*m_paramFrame, out);

        m_client->sendInteger(out.size());
        m_client->sendBuffer(out);
    } catch(const std::exception& e) {
        throw utils::InternalError(fmt(_(
                "NetStreamWriter open: error opening the client connection "
                "with the OOV plugin, hostname %1% type %2% port %3%. "
                "Error reported: %4%")) % host % location % port %
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
        m_client->sendInteger(out.size());
        m_client->sendBuffer(out);
    } catch(const std::exception& e) {
        throw utils::InternalError(fmt(_(
                "NetStreamWriter new observable: error writing new observable. "
                "Error reported: %1%")) % e.what());
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
        m_client->sendInteger(out.size());
        m_client->sendBuffer(out);
    } catch(const std::exception& e) {
        throw utils::InternalError(fmt(_(
                "NetStreamWriter delete observable: error writing new"
               " observable. Error reported: %1%")) % e.what());
    }
}

void NetStreamWriter::process(Simulator* simulator,
                              const std::string& portname,
                              const devs::Time& time,
                              const std::string& view,
                              value::Value* val)
{
    if (simulator) {
        buildValue(simulator->getName(),
                   simulator->getParent(),
                   portname,
                   view,
                   time,
                   val);
    } else {
        buildValue(std::string(),
                   std::string(),
                   portname,
                   view,
                   time,
                   val);
    }

    std::string out;
    value::Set::serializeBinaryBuffer(*m_valueFrame, out);

    try {
        m_client->sendInteger(out.size());
        m_client->sendBuffer(out);
    } catch(const std::exception& e) {
        throw utils::InternalError(fmt(_(
                "NetStreamWriter process state event: error writing a state"
                " event. Error reported: %1%")) % e.what());
    }
}

oov::PluginPtr NetStreamWriter::close(const devs::Time& time)
{
    buildClose(time.getValue());

    std::string out;
    value::Set::serializeBinaryBuffer(*m_closeFrame, out);

    try {
        m_client->sendInteger(out.size());
        m_client->sendBuffer(out);

        oov::PluginPtr plugin = getPlugin();

        m_client->close();
        delete m_client;
        m_client = 0;
        return plugin;
    } catch(const std::exception& e) {
        throw utils::InternalError(fmt(_(
                "NetStreamWriter close: OOV does not respond. "
                "Error reported: %1%")) % e.what());
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
            m_client->sendInteger(out.size());
            m_client->sendBuffer(out);
            plugin = getPlugin();
        }
        return plugin;
    } catch(const std::exception& e) {
        throw utils::InternalError(fmt(_(
                "NetStreamWriter refreshPlugin: OOV does not respond."
                "Error reported: %1%")) % e.what());
    }
}

oov::PluginPtr NetStreamWriter::getPlugin() const
{
    std::string result = m_client->recvString();
    if (result == "ok") {
        m_client->sendBuffer("ok");

        int sz = m_client->recvInteger();
        m_client->sendBuffer("ok");

        result = m_client->recvBuffer(sz);
        m_client->sendBuffer("ok");

        value::Set vals;
        value::Set::deserializeBinaryBuffer(vals, result);

        const std::string& name(vals.getString(0));

        utils::PathList lst(utils::Path::path().getStreamDirs());
        utils::PathList::const_iterator it;
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
            plugin->deserialize(*vals.get(1));
            return plugin;
        }
    }
    return oov::PluginPtr();
}

void NetStreamWriter::buildParameters(const std::string& plugin,
                                      const std::string& package,
                                      const std::string& directory,
                                      const std::string& file,
                                      value::Value* parameters,
                                      const double& time)
{
    if (m_paramFrame == 0) {
        m_paramFrame = new value::Set();
        m_paramFrame->addInt(0);
        m_paramFrame->addString(plugin);
        m_paramFrame->addString(package);
        m_paramFrame->addString(directory);
        m_paramFrame->addString(file);
        m_paramFrame->add(parameters);
        m_paramFrame->addDouble(time);
    } else {
        m_paramFrame->getString(1) = plugin;
        m_paramFrame->getString(2) = package;
        m_paramFrame->getString(3) = directory;
        m_paramFrame->getString(4) = file;
        m_valueFrame->value()[5] = parameters;
        m_paramFrame->getDouble(6) = time;
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
        m_newObsFrame->getString(1) = simulator;
        m_newObsFrame->getString(2) = parents;
        m_newObsFrame->getString(3) = port;
        m_newObsFrame->getString(4) = view;
        m_newObsFrame->getDouble(5) = time;
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
        m_delObsFrame->getString(1) = simulator;
        m_delObsFrame->getString(2) = parents;
        m_delObsFrame->getString(3) = port;
        m_delObsFrame->getString(4) = view;
        m_delObsFrame->getDouble(5) = time;
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
        m_valueFrame->getString(1) = simulator;
        m_valueFrame->getString(2) = parents;
        m_valueFrame->getString(3) = port;
        m_valueFrame->getString(4) = view;
        m_valueFrame->getDouble(5) = time;
        m_valueFrame->set(6, value);
    }
}

void NetStreamWriter::buildClose(const double& time)
{
    if (m_closeFrame == 0) {
        m_closeFrame = new value::Set();
        m_closeFrame->addInt(4);
        m_closeFrame->addDouble(time);
    } else {
        m_closeFrame->getDouble(1) = time;
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
