/** 
 * @file NetStreamWriter.cpp
 * @brief 
 * @author The vle Development Team
 * @date 2007-07-16
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

#include <vle/devs/NetStreamWriter.hpp>
#include <vle/devs/Simulator.hpp>
#include <vle/utils/Trace.hpp>



namespace vle { namespace devs {

NetStreamWriter::NetStreamWriter() :
    m_client(0)
{
}

NetStreamWriter::~NetStreamWriter()
{
    delete m_client;
}

void NetStreamWriter::open(const std::string& /* plugin */,
                           const std::string& location,
                           const std::string& file,
                           const std::string& /* parameters */)
{
    std::string host;
    int port;

    try {
        utils::net::explodeStringNet(location, host, port);
        m_client = new utils::net::Client(host, port);
        TRACE1(boost::format("NetStreamWriter init %1%:%2% %3%") % host % port %
               location);
    } catch(const std::exception& e) {
        Throw(utils::InternalError, boost::format(
                "NetStreamWriter open: error opening the client connection "
                "with the OOV plugin, hostname %1% type %2% port %3%. "
                "Error reported: %4%") % host % location % file %
            e.what());
    }
}

void NetStreamWriter::processNewObservable(const Time& time,
                                           Simulator* simulator,
                                           const std::string& portname,
                                           const std::string& view)
{
    std::ostringstream out;
    out << "<trame type=\"new\" "
        << " data=\"" << time << "\""
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

void NetStreamWriter::processRemoveObservable(const Time& time,
                                              Simulator* simulator,
                                              const std::string& portname,
                                              const std::string& view)
{
    std::ostringstream out;
    out << "<trame type=\"del\" "
        << " data=\"" << time << "\""
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

void NetStreamWriter::process(const StateEvent& event)
{
    std::ostringstream out;
    out << "<trame type=\"value\" data=\"" << event.getTime() << "\" >"
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

void NetStreamWriter::close()
{
    try {
        m_client->send_buffer("<trame type=\"end\"/>");
        m_client->close();
        delete m_client;
        m_client = 0;
    } catch(const std::exception& e) {
        Throw(utils::InternalError, boost::format(
                "NetStreamWriter close: OOV does not respond. "
                "Error reported: %1%") % e.what());
    }
}

}} // namespace vle devs
