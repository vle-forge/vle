/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <vle/gvle/ConnectionBox.hpp>
#include <vle/vpz/CoupledModel.hpp>
#include <vle/utils/i18n.hpp>
#include <gtkmm/stock.h>
#include <cassert>

using std::list;
using std::string;
using namespace vle;

namespace vle { namespace gvle {

ConnectionBox::ConnectionBox(vpz::CoupledModel* parent,
                             vpz::BaseModel* src,
                             vpz::BaseModel* dst) :
        Gtk::Dialog(_("Connection Box"), true),
        m_vbox(Gtk::ORIENTATION_VERTICAL),
        m_hbox(Gtk::ORIENTATION_HORIZONTAL),
        m_left(_("Source model")),
        m_left2(Gtk::ORIENTATION_VERTICAL),
        m_right(_("Destination model")),
        m_right2(Gtk::ORIENTATION_VERTICAL)
{
    m_vbox.set_homogeneous(false);
    m_right2.set_homogeneous(false);
    m_left2.set_homogeneous(false);
    m_hbox.set_homogeneous(true);
     	
    assert(parent and src and dst);

    m_label.set_markup(_("<big><b>Connection</b></big>"));
    m_labelInput.set_markup(src->getName());
    m_labelOutput.set_markup(dst->getName());

    assingComboInputPort(parent, src);
    assingComboOutputPort(parent, dst);

    m_left.set_border_width(5);
    m_right.set_border_width(5);

    m_vbox.add(m_label);
    m_vbox.add(m_hbox);
    m_hbox.add(m_left);
    m_hbox.add(m_right);
    m_left.add(m_left2);
    m_right.add(m_right2);
    m_left2.pack_start(m_labelInput, true, true, 0);
    m_left2.pack_start(m_comboInput, false, true, 0);
    m_right2.pack_start(m_labelOutput, true, true, 0);
    m_right2.pack_start(m_comboOutput, false, true, 0);

    add_button(Gtk::Stock::NO, Gtk::RESPONSE_NO);
    add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

    get_vbox()->add(m_vbox);
    show_all();
}

void ConnectionBox::assingComboOutputPort(vpz::CoupledModel* parent,
        vpz::BaseModel* dst)
{
    if (dst == parent) {
        const vpz::ConnectionList& input = dst->getOutputPortList();
        vpz::ConnectionList::const_iterator it = input.begin();
        while (it != input.end()) {
            m_comboOutput.append_string((*it).first);
            ++it;
        }
    } else {
        const vpz::ConnectionList& input = dst->getInputPortList();
        vpz::ConnectionList::const_iterator it = input.begin();
        while (it != input.end()) {
            m_comboOutput.append_string((*it).first);
            ++it;
        }
    }
}

void ConnectionBox::assingComboInputPort(vpz::CoupledModel* parent,
        vpz::BaseModel* src)
{
    if (src == parent) {
        const vpz::ConnectionList& output = src->getInputPortList();
        vpz::ConnectionList::const_iterator it = output.begin();
        while (it != output.end()) {
            m_comboInput.append_string((*it).first);
            ++it;
        }
    } else {
        const vpz::ConnectionList& output = src->getOutputPortList();
        vpz::ConnectionList::const_iterator it = output.begin();
        while (it != output.end()) {
            m_comboInput.append_string((*it).first);
            ++it;
        }
    }
}

}
} // namespace vle gvle
