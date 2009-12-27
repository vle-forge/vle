/**
 * @file vle/gvle/ConnectionBox.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
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


#include <vle/gvle/ConnectionBox.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <vle/utils/Debug.hpp>
#include <gtkmm/stock.h>

using std::list;
using std::string;
using namespace vle;

namespace vle { namespace gvle {

ConnectionBox::ConnectionBox(graph::CoupledModel* parent,
                             graph::Model* src,
                             graph::Model* dst) :
        Gtk::Dialog(_("Connection Box"), true, true),
        m_vbox(false),
        m_hbox(true),
        m_left(_("Source model")),
        m_left2(false),
        m_right(_("Destination model")),
        m_right2(false)
{
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

void ConnectionBox::assingComboOutputPort(graph::CoupledModel* parent,
        graph::Model* dst)
{
    if (dst == parent) {
        const graph::ConnectionList& input = dst->getOutputPortList();
        graph::ConnectionList::const_iterator it = input.begin();
        while (it != input.end()) {
            m_comboOutput.append_string((*it).first);
            ++it;
        }
    } else {
        const graph::ConnectionList& input = dst->getInputPortList();
        graph::ConnectionList::const_iterator it = input.begin();
        while (it != input.end()) {
            m_comboOutput.append_string((*it).first);
            ++it;
        }
    }
}

void ConnectionBox::assingComboInputPort(graph::CoupledModel* parent,
        graph::Model* src)
{
    if (src == parent) {
        const graph::ConnectionList& output = src->getInputPortList();
        graph::ConnectionList::const_iterator it = output.begin();
        while (it != output.end()) {
            m_comboInput.append_string((*it).first);
            ++it;
        }
    } else {
        const graph::ConnectionList& output = src->getOutputPortList();
        graph::ConnectionList::const_iterator it = output.begin();
        while (it != output.end()) {
            m_comboInput.append_string((*it).first);
            ++it;
        }
    }
}

}
} // namespace vle gvle
