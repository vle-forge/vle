/**
 * @file vle/gvle/BooleanBox.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
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


#include <vle/gvle/BooleanBox.hpp>

using namespace vle;

namespace vle {
namespace gvle {
BooleanBox::BooleanBox(value::Boolean* boolean) :
    Gtk::Dialog(_("Boolean"), true, true),
    mValue(boolean)
{
    add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    add_button(Gtk::Stock::OK, Gtk::RESPONSE_APPLY);

    m_Combo = new Gtk::ComboBoxText();
    get_vbox()->pack_start(*m_Combo);

    m_Combo->append_text("true");
    m_Combo->append_text("false");
    m_Combo->set_active_text(boolean->writeToString());
    show_all();
}

BooleanBox::~BooleanBox()
{
    hide_all();
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
