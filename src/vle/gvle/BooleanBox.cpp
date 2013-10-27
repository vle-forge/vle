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


#include <vle/gvle/BooleanBox.hpp>
#include <gtkmm/stock.h>
#include <gtkmm/comboboxtext.h>


using namespace vle;

namespace vle {
namespace gvle {
BooleanBox::BooleanBox(value::Boolean* boolean) :
    Gtk::Dialog(_("Boolean"), true),
    mValue(boolean)
{
    add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    add_button(Gtk::Stock::OK, Gtk::RESPONSE_APPLY);

    m_Combo = new Gtk::ComboBoxText();
    get_vbox()->pack_start(*m_Combo);

    m_Combo->append("true");
    m_Combo->append("false");
    m_Combo->set_active_text(boolean->writeToString());
    show_all();
}

BooleanBox::~BooleanBox()
{
    hide();
    delete m_Combo;
}

void BooleanBox::run()
{
    int ret = Gtk::Dialog::run();
    if (ret == Gtk::RESPONSE_APPLY) {
        std::string new_val = m_Combo->get_active_text();

        if (new_val == "true") {
            mValue->set(true);
        } else {
            mValue->set(false);
        }
    }

}

}
} // namespace vle gvle
