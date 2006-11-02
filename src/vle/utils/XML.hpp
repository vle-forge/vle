/**
 * @file utils/XML.hpp
 * @author The VLE Development Team.
 * @brief libxml++ VLE extension.
 */

/*
 * Copyright (c) 2004, 2005 The vle Development Team
 *
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifndef UTILS_XML_HPP
#define UTILS_XML_HPP

#include <glibmm/ustring.h>
#include <libxml++/libxml++.h>



namespace vle { namespace utils { namespace xml {

    /**
     * return an attribute from an element node.
     *
     * @throw ParseError.
     * @param elt element node to examine.
     * @param name attribute name to find.
     * @return string value of attribute.
     */
    Glib::ustring get_attribute(xmlpp::Element* elt,
				const Glib::ustring& name);

    /**
     * return an Attribute from a node after a cast into element.
     *
     * @throw ParseError.
     * @param node node to examine.
     * @param name name attribute name to find.
     * @return string value of attribute.
     */
    Glib::ustring get_attribute(xmlpp::Node* node, const Glib::ustring& name);

    /**
     * return an Attribute with name attribute from children node with
     * name children of element elt.
     *
     * @throw ParseError.
     * @param elt node to get children attribute.
     * @param children a string of children node.
     * @param attribute a string of attribute in children node.
     * @return
     */
    Glib::ustring get_attribute_from_children(xmlpp::Element* elt,
					      const Glib::ustring& children,
					      const Glib::ustring& attribute);

    /**
     * return int conversion of an attribute value from an element node.
     *
     * @throw ParseError.
     * @param elt element node to examine.
     * @param name attribute name to find.
     * @return int translation.
     */
    int get_int_attribute(xmlpp::Element* elt,
			  const Glib::ustring& name);

    /**
     * return double conversion of an attribute value from an element node.
     *
     * @throw ParseError.
     * @param elt element node to examine.
     * @param name attribute name to find.
     * @return double translation.
     */
    double get_double_attribute(xmlpp::Element* elt,
				const Glib::ustring& name);

    /** 
     * @brief Get size_t conversion of an attribute value from an element node.
     * 
     * @param elt element node to examine.
     * @param name attribute name to find.
     * 
     * @return size_t translation.
     *
     * @throw ParseError.
     */
    size_t get_size_t_attribute(xmlpp::Element* elt,
                                const Glib::ustring& name);

    /**
     * return true if attribute exist in xmlpp::Element node.
     *
     * @param elt xmlpp::Element to scan.
     * @param name attribute name to test.
     * @return true if attribute exist, otherwise false.
     */
    bool exist_attribute(xmlpp::Element* elt, const Glib::ustring& name);

    /**
     * return first children of an xml node.
     *
     * @throw ParseError.
     * @param node node to get children.
     * @param name children name to get.
     * @return a children element node.
     */
    xmlpp::Element* get_children(xmlpp::Node* node,
				 const Glib::ustring& name);

    /**
     * return existing of a children.
     *
     * @param node node to get children.
     * @param name children name to test existing.
     * @return true if a children node exist, otherwise false.
     */
    bool exist_children(xmlpp::Node* node, const Glib::ustring& name);

    /**
     * return existing of children.
     *
     * @param node node to test.
     * @return true if node have children.
     */
    bool has_children(xmlpp::Node* node);

    /**
     * test if node is an element node.
     *
     * @param node xmlpp::node to test.
     * @return true if node is an element, otherwise false.
     */
    bool is_element(xmlpp::Node* node);

    /**
     * return a cast of a Node to a Element.
     *
     * @throw ParseError if node == NULL or cast fail.
     * @param node node to change into element.
     * @return element node result.
     */
    xmlpp::Element* cast_node_element(xmlpp::Node* node);

    /**
     * return root element from DomParser.
     *
     * @throw ParseError.
     * @param dom DomParser parent.
     * @param name test name with root's name to validate.
     * @return root element node.
     */
    xmlpp::Element* get_root_node(xmlpp::DomParser& dom,
				  const Glib::ustring& name);

    /**
     * Add a XML file contents under current node.
     *
     * @throw ParseError.
     * @param current no to copy and insert newly tree.
     * @param filename XML file to copy.
     */
    void add_child_file(xmlpp::Element* current,
			const Glib::ustring& filename);

    /**
     * Add an XML string contents under current node.
     *
     * @throw ParseError.
     * @param current node to copy and insert newly tree.
     * @param xml string to copy.
     */
    void add_child_string(xmlpp::Element* current, const Glib::ustring& xml);

    /**
     * Save a current node into a file.
     *
     * @throw ParseError.
     * @param current node to save into file.
     * @param filename XML file where save.
     */
    void write_to_file(xmlpp::Element* current, const Glib::ustring& filename);

    /**
     * Save a current node into a string.
     *
     * @throw Exception::Pase.
     * @param current node to save into a string.
     * @return string.
     */
    Glib::ustring write_to_string(xmlpp::Element* current);

    /**
     * Import children nodes.
     *
     * @param destination root node to import.
     * @param origin root node.
     */
    void import_children_nodes(xmlpp::Element* destination,
			       xmlpp::Element* origin);

    /** 
     * @brief Import children nodes from string. Parent node is forget.
     * 
     * @param dest root node to import.
     * @param xml string to translate.
     */
    void import_children_nodes_without_parent(xmlpp::Element* dest,
                                              const Glib::ustring& xml);

    /**
     * Translate Glib::ustring into xmlpp nodes and attach it under node
     * destination.
     *
     * @throw ParseError if xml_content is not a valid XML.
     * @param destination root node to import.
     * @param xml_content string to translate.
     */
    void import_children_nodes(xmlpp::Element* destination, const
			       Glib::ustring& xml_content);

}}} // namespace vle utils xml

#endif
