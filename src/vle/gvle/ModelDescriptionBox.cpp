/**
 * @file vle/gvle/ModelDescriptionBox.cpp
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


#include <vle/gvle/ModelDescriptionBox.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/graph/Model.hpp>
#include <gtkmm/stock.h>
#include <gtkmm/frame.h>
#include <gtkmm/scrolledwindow.h>

using std::string;
using std::set;
using namespace vle;

namespace vle
{
namespace gvle {

ModelDescriptionBox::ModelDescriptionBox(const set < string > & lst,
        graph::Model * model) :
        Gtk::Dialog("Model Description", true, true),
        m_lst(lst),
        m_model(model)
{
    set_position(Gtk::WIN_POS_MOUSE);
    Gtk::Button * cancelbutton, * okbutton;
    cancelbutton = add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    okbutton = add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

    m_entry = Gtk::manage(new Gtk::Entry());

    Gtk::Frame * frameNorth = Gtk::manage(new Gtk::Frame("Model name"));
    frameNorth->set_border_width(5);
    frameNorth->set_shadow_type(Gtk::SHADOW_NONE);
    frameNorth->add(*m_entry);

    Gtk::VBox *vbox1 = Gtk::manage(new class Gtk::VBox(false, 0));
    vbox1->pack_start(*frameNorth, Gtk::PACK_SHRINK, 0);

    get_vbox()->add(*vbox1);
    get_vbox()->set_homogeneous(false);
    get_vbox()->set_spacing(0);

    okbutton->set_flags(Gtk::CAN_DEFAULT);
    okbutton->grab_default();
    m_entry->set_activates_default();

    if (m_model) {
        m_entry->set_text(m_model->getName());
    }
    show_all();
}


bool ModelDescriptionBox::run()
{
    for (;;) {
        int response = Gtk::Dialog::run();

        if (response == Gtk::RESPONSE_OK) {
            if (m_lst.find(getName()) != m_lst.end())
                gvle::Error((boost::format(
                                 "'%1%' already a model name") % getName()).str());
            else if (m_entry->get_text().is_ascii() == false)
                gvle::Error((boost::format(
                                 "'%1%' have no-ascii char") % getName()).str());
            else if (m_entry->get_text().empty() == true)
                gvle::Error("Empty model name ?");
            else {
                hide();
                return true;
            }
        } else {
            hide();
            return false;
        }
    }
}

}
} // namespace vle gvle
