/** 
 * @file vpz/EOV.cpp
 * @brief 
 * @author The vle Development Team
 * @date mer, 01 fév 2006 19:10:54 +0100
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

#include <vle/vpz/EOV.hpp>
#include <vle/vpz/Outputs.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/XML.hpp>
#include <stack>



namespace vle { namespace vpz {

using namespace vle::utils;

void EOVempty::init(xmlpp::Element* elt)
{
    AssertI(elt);
    AssertI(elt->get_name() == "WIDGET");
    AssertI(xml::get_attribute(elt, "TYPE") == "empty");
}

void EOVempty::write(xmlpp::Element* elt) const
{
    xmlpp::Element* empty = elt->add_child("WIDGET");
    empty->set_attribute("TYPE", "empty");
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

EOVcontainer::EOVcontainer()
{
    setContainerSize(1);
}

EOVcontainer::EOVcontainer(const EOVcontainer& cont) :
    EOVchild(cont)
{
    const size_t sz = cont.getContainerSize();
    for (size_t i = 0; i < sz; ++i) {
        m_lst.push_back(cont.getChild(i).clone());
    }
}

EOVcontainer::EOVcontainer(size_t s)
{
    if (s == 0) {
        setContainerSize(1);
    } else {
        setContainerSize(s);
    }
}

EOVcontainer& EOVcontainer::operator=(const EOVcontainer& cont)
{
    if (this != &cont) {
        std::vector < EOVchild* >::iterator it = m_lst.begin();
        while (it != m_lst.end()) {
            delete (*it);
            ++it;
        }
        m_lst.clear();

        const size_t sz = cont.getContainerSize();
        for (size_t i = 0; i < sz; ++i) {
            m_lst.push_back(cont.getChild(i).clone());
        }
    }
    return *this;
}

EOVcontainer::~EOVcontainer()
{
    std::vector < EOVchild* >::iterator it = m_lst.begin();
    while (it != m_lst.end()) {
        delete (*it);
        ++it;
    }
}

void EOVcontainer::init(xmlpp::Element* elt)
{
    AssertI(elt);
    AssertI(elt->get_name() == "CHILD");

    m_lst.clear();

    xmlpp::Node::NodeList lst = elt->get_children("CHILD");
    setContainerSize(lst.size());
    size_t i = 0;

    xmlpp::Node::NodeList::iterator it = lst.begin();
    while (it != lst.end()) {
        xmlpp::Element* widget =
            xml::get_children((xmlpp::Element*)(*it), "WIDGET");
        Glib::ustring type(xml::get_attribute(widget, "TYPE"));
        if (type == "empty") {
            setEmptyChild(i);
        } else if (type == "vpaned") {
            setVPanedChild(i);
            m_lst[i]->init(widget);
        } else if (type == "hpaned") {
            setHPanedChild(i);
            m_lst[i]->init(widget);
        } else if (type == "plugin") {
            setPluginChild(i);
            m_lst[i]->init(widget);
        } else {
            Throw(utils::ParseError,
                  boost::format("Unknow type %1% for EOV root node\n") %
                  type);
        }
        ++i;
    }
}

void EOVcontainer::write(xmlpp::Element* elt) const
{
    std::vector < EOVchild* >::const_iterator it = m_lst.begin();
    while (it != m_lst.end()) {
        xmlpp::Element* child = elt->add_child("CHILD");
        (*it)->write(child);
        ++it;
    }
}

void EOVcontainer::setContainerSize(size_t sz)
{
    AssertI(sz > 0);

    std::vector < EOVchild* >::iterator it = m_lst.begin();
    while (it != m_lst.end()) {
        delete (*it);
        ++it;
    }

    m_lst.resize(sz);
    it = m_lst.begin();
    while (it != m_lst.end()) {
        (*it) = new EOVempty;
        ++it;
    }
}

size_t EOVcontainer::getContainerSize() const
{
    return m_lst.size();
}

void EOVcontainer::setEmptyChild(size_t number)
{
    AssertI(number < m_lst.size());

    delete m_lst[number];
    m_lst[number] = new EOVempty;
}

void EOVcontainer::setPluginChild(size_t number)
{
    AssertI(number < m_lst.size());

    delete m_lst[number];
    m_lst[number] = new EOVplugin;
}

void EOVcontainer::setHPanedChild(size_t number)
{
    AssertI(number < m_lst.size());

    delete m_lst[number];
    m_lst[number] = new EOVhpaned;
}

void EOVcontainer::setVPanedChild(size_t number)
{
    AssertI(number < m_lst.size());

    delete m_lst[number];
    m_lst[number] = new EOVvpaned;
}

EOVchild& EOVcontainer::getChild(size_t number)
{
    AssertI(number < m_lst.size());
    AssertI(m_lst[number]);

    return *m_lst[number];
}

const EOVchild& EOVcontainer::getChild(size_t number) const
{
    AssertI(number < m_lst.size());
    AssertI(m_lst[number]);

    return *m_lst[number];
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void EOVvpaned::init(xmlpp::Element* elt)
{
    AssertI(elt);
    AssertI(elt->get_name() == "WIDGET");
    AssertI(xml::get_attribute(elt, "TYPE") == "vpaned");

    EOVcontainer::init(elt);
}

void EOVvpaned::write(xmlpp::Element* elt) const
{
    xmlpp::Element* vpaned = elt->add_child("WIDGET");
    vpaned->set_attribute("TYPE", "vpaned");

    EOVcontainer::write(vpaned);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void EOVhpaned::init(xmlpp::Element* elt)
{
    AssertI(elt);
    AssertI(elt->get_name() == "WIDGET");
    AssertI(xml::get_attribute(elt, "TYPE") == "hpaned");

    EOVcontainer::init(elt);
}

void EOVhpaned::write(xmlpp::Element* elt) const
{
    xmlpp::Element* hpaned = elt->add_child("WIDGET");
    hpaned->set_attribute("TYPE", "hpaned");

    EOVcontainer::write(hpaned);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void EOVplugin::setEOVplugin(const std::string& output,
                             const std::string& plugin)
{
    AssertI(not output.empty());
    AssertI(not plugin.empty());

    m_output.assign(output);
    m_plugin.assign(plugin);
}

void EOVplugin::init(xmlpp::Element* elt)
{
    AssertI(elt);
    AssertI(elt->get_name() == "WIDGET");
    AssertI(xml::get_attribute(elt, "TYPE") == "plugin");

    setEOVplugin(xml::get_attribute(elt, "OUTPUT"),
                 xml::get_attribute(elt, "PLUGIN"));
}

void EOVplugin::write(xmlpp::Element* elt) const
{
    xmlpp::Element* empty = elt->add_child("WIDGET");
    empty->set_attribute("TYPE", "plugin");
    empty->set_attribute("OUTPUT", m_output);
    empty->set_attribute("PLUGIN", m_plugin);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void EOV::init(xmlpp::Element* elt)
{
    AssertI(elt);
    AssertI(elt->get_name() == "EOV");

    m_host.assign(xml::get_attribute(elt, "HOST"));

    xmlpp::Element* display = xml::get_children(elt, "DISPLAY");
    EOVcontainer::init(display);
}

void EOV::write(xmlpp::Element* elt) const
{
    AssertI(elt);

    xmlpp::Element* eov = elt->add_child("EOV");
    eov->set_attribute("HOST", m_host);

    xmlpp::Element* display = eov->add_child("DISPLAY");
    xmlpp::Element* child = display->add_child("CHILD");
    getChild(0).write(child);
}

void EOV::setEOV(const std::string& host)
{
    m_host.assign(host);
}

std::list < const EOVplugin* > EOV::plugins() const
{
    std::list < const EOVplugin* > result;
    std::stack < const EOVcontainer* > stk;
    const EOVcontainer* ctn;
    const EOVplugin* plg;

    stk.push(this);
    while (not stk.empty()) {
        const EOVcontainer* current = stk.top();
        stk.pop();

        size_t sz = current->getContainerSize();
        for (size_t i = 0; i < sz; ++i) {
            ctn = dynamic_cast < const EOVcontainer* >(&current->getChild(i));

            if (ctn) {
                stk.push(ctn);
            } else {
                plg = dynamic_cast < const EOVplugin* >(&current->getChild(i));
                if (plg) {
                    result.push_back(plg);
                }
            }
        }
    }
    return result;
}

std::string EOV::xml() const
{
    xmlpp::Document doc;
    xmlpp::Element* root = doc.create_root_node("EOV");
    xmlpp::Element* display = root->add_child("DISPLAY");
    xmlpp::Element* child = display->add_child("CHILD");
    getChild(0).write(child);
    return doc.write_to_string_formatted();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void EOVs::init(xmlpp::Element* elt)
{
    AssertI(elt);
    AssertI(elt->get_name() == "EOVS");

    m_lst.clear();
    xmlpp::Node::NodeList lst = elt->get_children("EOV");
    xmlpp::Node::NodeList::iterator it;
    for (it = lst.begin(); it != lst.end(); ++it) {
        EOV eov;
        eov.init((xmlpp::Element*)(*it));
        addEOV(xml::get_attribute((xmlpp::Element*)(*it), "NAME"),
               eov);
    }
}

void EOVs::write(xmlpp::Element* elt) const
{
    AssertI(elt);

    if (not m_lst.empty()) {
        xmlpp::Element* eovs = elt->add_child("EOVS");

        std::map < std::string, EOV >::const_iterator it;
        for (it = m_lst.begin(); it != m_lst.end(); ++it) {
            xmlpp::Element* eov = eovs->add_child("EOV");
            eov->set_attribute("NAME", (*it).first);
            (*it).second.write(eov);
        }
    }
}

void EOVs::addEOV(const std::string& name, const EOV& e)
{
    AssertI(not name.empty());
    AssertI(m_lst.find(name) == m_lst.end());
    m_lst.insert(std::make_pair(name, e));
}

void EOVs::clear()
{
    m_lst.clear();
}

}} // namespace vle vpz
