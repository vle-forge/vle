/**
 * @file src/vle/devs/NetStreamWriter.cpp
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



namespace vle { namespace devs {

NetStreamWriter::NetStreamWriter() :
    m_client(0)
{
}

NetStreamWriter::~NetStreamWriter()
{
    delete m_client;
}

void NetStreamWriter::open(const std::string& plugin,
                           const std::string& location,
                           const std::string& file,
                           const std::string& parameters,
                           const devs::Time& time)
{
    std::string host, directory;
    int port;

    try {
        utils::net::explodeStringNet(location, host, port, directory);
        m_client = new utils::net::Client(host, port);
        TraceInformation(boost::format("NetStreamWriter init %1%:%2% %3% %4%")
                         % host % port % directory % file);
        std::ostringstream out;
        out << "<vle_trame>"
            << "<trame type=\"parameter\""
            << " date=\"" << time << "\""
            << " plugin=\"" << plugin << "\""
            << " location=\"" << Glib::build_filename(directory, file) << "\"";

        if (not parameters.empty()) {
            out << " ><![CDATA[" << parameters << "]]></trame>";
        } else {
            out << " />";
        }
        m_client->send_buffer(out.str());
    } catch(const std::exception& e) {
        Throw(utils::InternalError, boost::format(
                "NetStreamWriter open: error opening the client connection "
                "with the OOV plugin, hostname %1% type %2% port %3%. "
                "Error reported: %4%") % host % location % file %
            e.what());
    }
}

void NetStreamWriter::processNewObservable(Simulator* simulator,
                                           const std::string& portname,
                                           const devs::Time& time,
                                           const std::string& view)
{
    std::ostringstream out;
    out << "<trame type=\"new\""
        << " date=\"" << time << "\""
        << " name=\"" << simulator->getStructure()->getName() << "\""
        << " parent=\"" << simulator->getStructure()->getParentName() << "\""
        << " port=\"" << portname << "\""
        << " view=\"" << view << "\" />";

    try {
        m_client->send_buffer(out.str());
    } catch(const std::exception& e) {
        Throw(utils::InternalError, boost::format(
                "NetStreamWriter new observable: error writing new observable. "
                "Error reported: %1%") % e.what());
    }
}

void NetStreamWriter::processRemoveObservable(Simulator* simulator,
                                              const std::string& portname,
                                              const devs::Time& time,
                                              const std::string& view)
{
    std::ostringstream out;
    out << "<trame type=\"del\""
        << " date=\"" << time << "\""
        << " name=\"" << simulator->getStructure()->getName() << "\""
        << " parent=\"" << simulator->getStructure()->getParentName() << "\""
        << " port=\"" << portname << "\""
        << " view=\"" << view << "\" />";

    try {
        m_client->send_buffer(out.str());
    } catch(const std::exception& e) {
        Throw(utils::InternalError, boost::format(
                "NetStreamWriter delete observable: error writing new"
               " observable. Error reported: %1%") % e.what());
    }
}

void NetStreamWriter::process(const ObservationEvent& event)
{
    std::ostringstream out;
    out << "<trame type=\"value\" date=\"" << event.getTime() << "\" >"
        << "<modeltrame name=\"" << event.getSourceModelName()
        << "\" parent=\"" << event.getModel()->getStructure()->getParentName()
        << "\" port=\"" << event.getPortName()
        << "\" view=\"" << event.getViewName()
        << "\" >"
        << event.getAttributeValue(event.getPortName())->toXML()
        << "</modeltrame></trame>";

    try {
        m_client->send_buffer(out.str());
    } catch(const std::exception& e) {
        Throw(utils::InternalError, boost::format(
                "NetStreamWriter process state event: error writing a state"
                " event. Error reported: %1%") % e.what());
    }
}

oov::PluginPtr NetStreamWriter::close(const devs::Time& time)
{
    std::ostringstream out;
    out << "<trame type=\"end\" date=\"" << time << "\" /></vle_trame>";
    try {
        m_client->send_buffer(out.str());

        oov::PluginPtr plugin = getPlugin();

        m_client->close();
        delete m_client;
        m_client = 0;
        return plugin;
    } catch(const std::exception& e) {
        Throw(utils::InternalError, boost::format(
                "NetStreamWriter close: OOV does not respond. "
                "Error reported: %1%") % e.what());
    }
}

oov::PluginPtr NetStreamWriter::refreshPlugin()
{
    std::string out("<trame type=\"refresh\" />");
    try {
        oov::PluginPtr plugin;
        if (m_client) {
            m_client->send_buffer(out);
            plugin = getPlugin();
        }
        return plugin;
    } catch(const std::exception& e) {
        Throw(utils::InternalError, boost::format(
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

        value::Set vals = value::toSetValue(vpz::Vpz::parseValue(result));
        std::string name = value::toString(vals->getValue(0));
                
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
        
        Assert(utils::ArgError, plugin.get(), error);
        plugin->deserialize(vals->getValue(1));

        return plugin;
    }
    return oov::PluginPtr();
}

}} // namespace vle devs
