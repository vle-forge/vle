/**
 * @file vle/gvle/Paned.hpp
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


#include <vle/gvle/EovContainer.hpp>
#include <vle/gvle/PluginWidget.hpp>
#include <vle/gvle/EmptyWidget.hpp>
#include <vle/utils/XML.hpp>

namespace vle
{
namespace gvle {

class EovContainer;

template < class Class >
class Paned : public Class
{
public:
    /**
     * Constructor
     * @param eovc the EovContainer parent
     */
    Paned(EovContainer &eovc);

    /**
     * Set the first child
     * @param child the widget
     */
    void set_child1(Gtk::Widget &child);

    /**
     * Set the second child
     * @param child the widget
     */
    void set_child2(Gtk::Widget &child);

    /**
     * Check if w if the first child
     * @param w the checked widget
     * @return true if w is the first child else false
     */
    bool is_first_child(Gtk::Widget *w);

    /**
     * Add the xml of this class to node
     * @param node the xml node
     */
    void toXml(xmlpp::Element* node) const;

protected :
    EovContainer &mEovContainer;

    /*
     * return true if child is a child of the paned
     * @param child the child
     * @return true, false
     */
    bool exist_child(Gtk::Widget *child);
};

template < class Class >
void Paned < Class >::toXml(xmlpp::Element* node) const
{
    const Paned < Gtk::HPaned > *child_hp = 0;
    const Paned < Gtk::VPaned > *child_vp = 0;
    const EmptyWidget *child_empty = 0;
    const PluginWidget *child_plugin = 0;
    const Gtk::Widget *child;

    xmlpp::Element *child_node = node->add_child("CHILD");
    xmlpp::Element *widget_node = child_node->add_child("WIDGET");

    if (mEovContainer.getType(this) == EovContainer::HPANED)
        widget_node->set_attribute("TYPE", "hpaned");
    else if (mEovContainer.getType(this) == EovContainer::VPANED)
        widget_node->set_attribute("TYPE", "vpaned");

    child = Class::get_child1();
    switch (mEovContainer.getType(child)) {
    case EovContainer::HPANED :
        child_hp = dynamic_cast < const Paned < Gtk::HPaned > * >(child);
        child_hp->toXml(widget_node);
        break;
    case EovContainer::VPANED :
        child_vp = dynamic_cast < const Paned < Gtk::VPaned > * >(child);
        child_vp->toXml(widget_node);
        break;
    case EovContainer::PLUGIN :
        child_plugin = dynamic_cast < const PluginWidget * >(child);
        child_plugin->toXml(widget_node);
        break;
    case EovContainer::EMPTY_WIDGET :
        child_empty = dynamic_cast < const EmptyWidget * >(child);
        child_empty->toXml(widget_node);
        break;
    }

    child = Class::get_child2();
    switch (mEovContainer.getType(child)) {
    case EovContainer::HPANED :
        child_hp = dynamic_cast < const Paned < Gtk::HPaned > * >(child);
        child_hp->toXml(widget_node);
        break;
    case EovContainer::VPANED :
        child_vp = dynamic_cast < const Paned < Gtk::VPaned > * >(child);
        child_vp->toXml(widget_node);
        break;
    case EovContainer::PLUGIN :
        child_plugin = dynamic_cast < const PluginWidget * >(child);
        child_plugin->toXml(widget_node);
        break;
    case EovContainer::EMPTY_WIDGET :
        child_empty = dynamic_cast < const EmptyWidget * >(child);
        child_empty->toXml(widget_node);
        break;
    }

}

template < class Class >
Paned < Class >::Paned(EovContainer &eovc) : Class(), mEovContainer(eovc)
{
    Gtk::Widget *w;

    w = mEovContainer.createWidget(EovContainer::EMPTY_WIDGET);
    set_child1(*w);
    w = mEovContainer.createWidget(EovContainer::EMPTY_WIDGET);
    set_child2(*w);

    Class::check_resize();
    Class::show_all();

    if (mEovContainer.getType(this) == EovContainer::HPANED)
        Class::set_position(Class::get_width()/2);
    else
        Class::set_position(Class::get_height()/2);
}

template < class Class >
void Paned < Class >::set_child1(Gtk::Widget &child)
{
    Gtk::Widget *old_child;
    old_child = Class::get_child1();
    if (old_child) {
        Class::remove(*old_child);
        mEovContainer.eraseWidget(old_child);
        delete old_child;
    }
    Class::pack1(child);

    Class::check_resize();
    child.show_all();
}

template < class Class >
void Paned < Class >::set_child2(Gtk::Widget &child)
{

    Gtk::Widget *old_child;
    old_child = Class::get_child2();
    if (old_child) {
        Class::remove(*old_child);
        mEovContainer.eraseWidget(old_child);
        delete old_child;
    }

    Class::pack2(child);

    Class::check_resize();
    child.show_all();

}

template < class Class >
bool Paned < Class >::is_first_child(Gtk::Widget *w)
{
    return (Class::get_child1() == w);
}

template < class Class >
bool Paned < Class >:: exist_child(Gtk::Widget *child)
{
    bool is_child_1, is_child_2;

    is_child_1 = (Class::get_child1() == child);
    if (!is_child_1)
        is_child_2 = (Class::get_child2() == child);

    return (is_child_1 || is_child_2);
}

}
} // namespace vle gvle

