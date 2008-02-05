/**
 * @file src/vle/utils/XML.cpp
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




#include <vle/utils/XML.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Exception.hpp>


namespace vle { namespace utils { namespace xml {

Glib::ustring get_attribute(xmlpp::Element* elt,
			    const Glib::ustring& name)
{
    Assert(ParseError, elt != NULL, "xml node not found");

    xmlpp::Attribute* att = elt->get_attribute(name);

    Assert(ParseError, att != NULL, boost::format(
            "[%1%] tag not found line %2% for node [%3%]\n") %
        name % elt->get_line() % elt->get_name());

    return att->get_value();
}

Glib::ustring get_attribute(xmlpp::Node* node,
			    const Glib::ustring& name)
{
    Assert(ParseError, node != NULL, "xml node not found");

    xmlpp::Element* elt = cast_node_element(node);

    return get_attribute(elt, name);
}

Glib::ustring get_attribute_from_children(xmlpp::Element* elt,
        const Glib::ustring& children, const Glib::ustring& attribute)
{
    Assert(ParseError, elt != NULL, "xml node not found");

    xmlpp::Element* child = get_children(elt, children);

    return get_attribute(child, attribute);
}

int get_int_attribute(xmlpp::Element* elt,
				  const Glib::ustring& name)
{
    return utils::to_int(get_attribute(elt, name));
}

double get_double_attribute(xmlpp::Element* elt,
					const Glib::ustring& name)
{
    return utils::to_double(get_attribute(elt, name));
}

size_t get_size_t_attribute(xmlpp::Element* elt,
                                        const Glib::ustring& name)
{
    return utils::to_size_t(get_attribute(elt, name));
}

bool exist_attribute(xmlpp::Element* elt, const Glib::ustring& name)
{
    Assert(ParseError, elt, "xml node not found");

    return elt->get_attribute(name) != NULL;
}

xmlpp::Element* get_children(xmlpp::Node* node, const
        Glib::ustring& name)
{
    Assert(ParseError, node != NULL, "xml node not found");

    xmlpp::Node::NodeList lst = node->get_children(name);

    Assert(ParseError, lst.empty() == false, boost::format(
            "[%1%] node not found line %2% for node\n") %
        name % node->get_line());

    return cast_node_element(*lst.begin());
}

bool exist_children(xmlpp::Node* node, const Glib::ustring& name)
{
    Assert(ParseError, node != NULL, "xml node not found");

    return ! node->get_children(name).empty();
}

bool has_children(xmlpp::Node* node)
{
    Assert(ParseError, node != NULL, "xml node not found");

    return ! node->get_children().empty();
}

bool is_element(xmlpp::Node* node)
{
    if (node != NULL) {
        return dynamic_cast<xmlpp::Element*>(node) != NULL;
    }
    return false;
}

xmlpp::Element* cast_node_element(xmlpp::Node* node)
{
    Assert(ParseError, node != NULL, "xml node not found");

    xmlpp::Element* elt = dynamic_cast<xmlpp::Element*>(node);

    Assert(ParseError, elt != NULL, boost::format(
           "cast NODE to ELEMENT problem line %1% node\n") % node->get_line());

    return elt;
}

xmlpp::Element* get_root_node(xmlpp::DomParser& dom, const
        Glib::ustring& name)
{
    xmlpp::Document* doc = dom.get_document();
    Assert(ParseError, doc, "no document in xml");

    xmlpp::Element* root = doc->get_root_node();
    Assert(ParseError, root, "No root element in document");

    Assert(ParseError, root->get_name() == name, boost::format(
            "[%1%] not corresponding to root element line %2% node [%3%]\n") %
        name % root->get_line() % root->get_name());

    return root;
}

void add_child_file(xmlpp::Element* current,
				const Glib::ustring& filename)
{
    Assert(ParseError, current, "bad argument");

    xmlpp::DomParser dom(filename);
    xmlpp::Element* root = dom.get_document()->get_root_node();

    Assert(ParseError, root, "Bad root node");

    xmlpp::Node* newly = current->import_node(root, true);

    Assert(ParseError, newly, "Bad import node");
}

void add_child_string(xmlpp::Element* current, const Glib::ustring &
        xml)
{
    Assert(ParseError, current, "Bad argument");

    xmlpp::DomParser dom;
    dom.parse_memory(xml);
    xmlpp::Element* root = dom.get_document()->get_root_node();

    Assert(ParseError, root, "Bad root node");

    xmlpp::Node* newly = current->import_node(root, true);

    Assert(ParseError, newly, "Bad import node");
}

void write_to_file(xmlpp::Element* current, const Glib::ustring &
        filename)
{
    Assert(ParseError, current, "Bad argument");

    xmlpp::Document doc;
    xmlpp::Element* root = doc.create_root_node(current->get_name());

    {
        const xmlpp::Element::AttributeList lst = current->get_attributes();
        for (xmlpp::Element::AttributeList::const_iterator it = lst.begin();
             it != lst.end(); ++it)
            root->set_attribute((*it)->get_name(), (*it)->get_value());
    }

    {
        const xmlpp::Node::NodeList lst = current->get_children();
        xmlpp::Node::NodeList::const_iterator it = lst.begin();
        while (it != lst.end()) {
            root->import_node((*it), true);
            ++it;
        }
    }

    doc.write_to_file_formatted(filename);
}

Glib::ustring write_to_string(xmlpp::Element* current)
{
    Assert(ParseError, current, "Bad argument");

    xmlpp::Document doc;
    xmlpp::Element* root = doc.create_root_node(current->get_name());

    {
        const xmlpp::Element::AttributeList lst = current->get_attributes();
        for (xmlpp::Element::AttributeList::const_iterator it = lst.begin();
             it != lst.end(); ++it)
            root->set_attribute((*it)->get_name(), (*it)->get_value());
    }

    {
        const xmlpp::Node::NodeList lst = current->get_children();
        xmlpp::Node::NodeList::const_iterator it = lst.begin();
        while (it != lst.end()) {
            root->import_node((*it), true);
            ++it;
        }
    }

    return doc.write_to_string_formatted();
}

void import_children_nodes(xmlpp::Element* destination,
                                       xmlpp::Element* origin)
{
    Assert(ParseError, destination and origin, "bad import");

    const xmlpp::Node::NodeList lst = origin->get_children();
    xmlpp::Node::NodeList::const_iterator it = lst.begin();
    while (it != lst.end()) {
        destination->import_node((*it), true);
        ++it;
    }
}

void import_children_nodes_without_parent(
                xmlpp::Element* dest,
                const Glib::ustring& xml)
{
    Assert(ParseError, dest, "bad import");

    try {
	xmlpp::DomParser dom;
	dom.parse_memory(xml);
	xmlpp::Element* origin = dom.get_document()->get_root_node();
        import_children_nodes(dest, origin);
    } catch(const xmlpp::exception& e) {
	throw ParseError(e.what());
    }
}

void import_children_nodes(xmlpp::Element* destination,
                                       const Glib::ustring& xml_content)
{
    Assert(ParseError, destination, "bad import");
    try {
	xmlpp::DomParser dom;
	dom.parse_memory(xml_content);
	xmlpp::Element* origin = dom.get_document()->get_root_node();
        destination->import_node(origin, true);
    } catch(const xmlpp::exception& e) {
	throw ParseError(e.what());
    }
}

}}} // namespace vle utils xml
