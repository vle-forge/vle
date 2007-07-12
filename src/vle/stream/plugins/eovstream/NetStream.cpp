/**
 * @file NetStream.cpp
 * @author The VLE Development Team.
 * @brief Base class of net visualisation plugin.
 */

/*
 * Copyright (C) 2006 - The vle Development Team
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

#include "NetStream.hpp"
#include <vle/utils/Tools.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/XML.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Trace.hpp>
#include <glibmm/spawn.h>
#include <glibmm/timer.h>



namespace vle { namespace stream {

NetStream::NetStream() :
    m_root(0),
    m_client(0)
{ }

NetStream::~NetStream()
{
}

void NetStream::init(const std::string& /*outputPlugin*/, /* nom du plugin */
                     const std::string& outputType, /* local/net */
                     const std::string& outputLocation, /* host/ip */
                     const std::string& parameters) /* xml init */
{
    std::string host;
    int port;

    try {
        m_parser.parse_memory(parameters);
        m_parameterRoot = utils::xml::get_root_node(m_parser, "OUTPUT");
        utils::net::explodeStringNet(outputLocation, host, port);
        TRACE1(boost::format("Netstream init %1%:%2%\n") % host % port);
        m_client = new utils::net::Client(host, port);
    } catch(const std::exception& e) {
        Throw(utils::InternalError,
              boost::format("NetStream error opening client connection"
                            " with host name '%1%' type '%2%' location '%3%'"
                            ".\nError reported: %4%.") % host % outputType %
              outputLocation % e.what());
    }
}

void NetStream::close()
{
    TRACE1("NetStream::close()");
    try {
        char buf[3];
        m_client->send_buffer("<TRAME TYPE=\"end\"></TRAME>");
        m_client->recv( buf, 2);
        m_client->close();
    } catch (const utils::InternalError& e) {
        Throw(utils::InternalError,
              boost::format("A plugin does not respond:\n%1%\n") % e.what());
    }
    delete m_client;
    m_client = 0;
}

void NetStream::writeData()
{
}

void NetStream::writeHead(
                const std::vector < devs::Observable >& observableList)
{
    std::vector < devs::Observable >::const_iterator it;
    it = observableList.begin();

    m_root = m_document.create_root_node("TRAME");
    m_root->set_attribute("TYPE", "parameters");
    if (utils::xml::has_children(m_parameterRoot)) {
        utils::xml::import_children_nodes(m_root, m_parameterRoot);
    } else {
        m_root->add_child_text("");
    }
    std::string msg(utils::xml::write_to_string(m_root));

    try {
        m_client->send_buffer(msg);
    } catch (const std::exception& e) {
        std::cerr << "\n/!\\ Netstream error reported: " <<
            utils::demangle(typeid(e)) << "\n" << e.what();
    }
}

void NetStream::writeTail()
{
}

void NetStream::writeValue(const devs::Time&, value::Value )
{
}

void NetStream::writeValues(const devs::Time& time,
			    const devs::StreamModelPortValue& valuelst,
			    const devs::Stream::ObservableVector& obslst)
{
    std::string msg("<TRAME DATE=\"");
    msg += utils::to_string(time.getValue());
    msg += "\" TYPE=\"data\">";

    if (m_groupList.empty()) {
	std::vector < devs::Observable >::const_iterator it =
	    obslst.begin();

        vle::devs::StreamModelPort index;
        while (it != obslst.end()) {
	    index.first = it->simulator();
	    index.second = it->portname();
	    vle::devs::StreamModelPortValue::const_iterator jt;
	    jt = valuelst.find(index);
	    if (jt != valuelst.end()) {
		msg += "  ";
		msg += (*jt).second->toXML();
		msg += "\n";
	    }
	    ++it;
	}
    } else {
	std::vector < std::string >::const_iterator it2 = m_groupList.begin();
        while (it2 != m_groupList.end()) {
	    std::vector < devs::Observable >::const_iterator it =
		obslst.begin();
	    msg += "/>";
	    ++it2;
	}
    }
    msg += "</TRAME>";

    try {
        m_client->send_buffer(msg);
    } catch (const std::exception& e) {
        std::cerr << "\n/!\\ Netstream error reported: " <<
            utils::demangle(typeid(e)) << "\n" << e.what();
    }
}

}} // namespace vle stream

DECLARE_STREAM(vle::stream::NetStream);

/*
extern "C" __declspec(dllexport) vle::devs::Stream* makeNewStream(void) 
{
	return new vle::stream::NetStream();
}
*/
