/**
 * @file vle/utils/Host.cpp
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


#include <vle/utils/Host.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/XML.hpp>
#include <boost/format.hpp>



namespace vle { namespace utils {

void Host::write(xmlpp::Element* root) const
{
    if (mHostname.size()) {
        xmlpp::Element* elt = root->add_child("HOST");
        elt->set_attribute("HOSTNAME", mHostname);
        elt->set_attribute("PORT", utils::to_string(mPort));
        elt->set_attribute("PROCESS", utils::to_string(mProcess));
    }
}

void Host::read(xmlpp::Element* root)
{
    mHostname = utils::xml::get_attribute(root, "HOSTNAME");
    mPort = utils::to_int(utils::xml::get_attribute(root, "PORT"));
    mProcess = utils::to_int(utils::xml::get_attribute(root, "PROCESS"));
}

Hosts::~Hosts()
{
    if (mIsModified)
        write_file();
}

void Hosts::read_file()
{
    Host tmp;

    Glib::ustring filename(get_hosts_filename());
    if (utils::exist_file(filename) == false) {
        xmlpp::Document doc;
        xmlpp::Element* root = doc.create_root_node("HOSTS");
        xmlpp::Element* host = root->add_child("HOST");
        host->set_attribute("HOSTNAME", "localhost");
        host->set_attribute("PORT", "8000");
        host->set_attribute("PROCESS", "1");
        doc.write_to_file_formatted(filename);
    } else {
        xmlpp::DomParser dom(get_hosts_filename());
        xmlpp::Element* elt = utils::xml::get_root_node(dom, "HOSTS");
        xmlpp::Node::NodeList lst = elt->get_children("HOST");
        xmlpp::Node::NodeList::iterator it = lst.begin();
        while (it != lst.end()) {
            tmp.read(utils::xml::cast_node_element(*it));
            mHosts.insert(tmp);
            ++it;
        }
    }

    mIsModified = false;
}

void Hosts::write_file()
{
    xmlpp::Document doc;
    xmlpp::Element* root = doc.create_root_node("HOSTS");

    SetHosts::const_iterator it = mHosts.begin();
    while (it != mHosts.end()) {
        (*it).write(root);
         ++it;
    }
    doc.write_to_file_formatted(get_hosts_filename());

    mIsModified = false;
}

void Hosts::push_host(const Host& host)
{
    mIsModified = true;
    mHosts.insert(host);
}

void Hosts::remove_host(const std::string& hostname)
{
    SetHosts::iterator it = mHosts.find(Host(hostname, 0, 0));
    if (it != mHosts.end()) {
        mIsModified = true;
        mHosts.erase(it);
    }
}

const Host& Hosts::get_host(const std::string& hostname) const
{
    SetHosts::iterator it = mHosts.find(Host(hostname, 0, 0));

    Assert < utils::ArgError >(it != mHosts.end(), boost::format(
                "Cannot delete hostname '%1%") % hostname);

    return (*it);
}

Glib::ustring Hosts::get_hosts_filename()
{
    return Glib::build_filename(
        utils::Path::path().Path::getHomeDir(), "hosts.xml");
}

}} // namespace vle utils
