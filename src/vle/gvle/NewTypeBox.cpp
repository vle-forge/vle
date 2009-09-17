/**
 * @file vle/gvle/NewTypeBox.cpp
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


#include <vle/gvle/NewTypeBox.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/utils/Tools.hpp>

using namespace vle;

namespace vle
{
namespace gvle {

NewTypeBox::NewTypeBox(value::Value* val):
        Gtk::Dialog("?",true,true),
        mVal(val)
{
    add_button(Gtk::Stock::APPLY, Gtk::RESPONSE_APPLY);
    add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);

    m_Combo = new Gtk::ComboBoxText();
    get_vbox()->pack_start(*m_Combo);

    m_Combo->append_text("Boolean");
    m_Combo->append_text("Double");
    m_Combo->append_text("Integer");
    m_Combo->append_text("Matrix");
    m_Combo->append_text("Map");
    m_Combo->append_text("Null");
    m_Combo->append_text("Set");
    m_Combo->append_text("String");
    m_Combo->append_text("Table");
    m_Combo->append_text("Tuple");
    m_Combo->append_text("Xml");
    show_all();
}

NewTypeBox::~NewTypeBox()
{
    hide_all();
    delete m_Combo;
}

void NewTypeBox::run()
{
    using namespace value;
    int ret = Gtk::Dialog::run();
    if (ret == Gtk::RESPONSE_APPLY) {
        if (m_Combo->get_active_text() == "Boolean") {
            mVal = Boolean::create();
        } else if (m_Combo->get_active_text() == "Double") {
            mVal = Double::create();
        } else if (m_Combo->get_active_text() == "Integer") {
            mVal = Integer::create();
        } else if (m_Combo->get_active_text() == "Matrix") {
            SimpleTypeBox box("columns ?", "1");
            int cols = utils::to_int(box.run());
            SimpleTypeBox box2("rows ?", "1");
            int rows = utils::to_int(box2.run());
            mVal = Matrix::create(cols, rows, cols*2, rows*2, cols/5, rows/5);
        } else if (m_Combo->get_active_text() == "Map") {
            mVal = Map::create();
        } else if (m_Combo->get_active_text() == "Null") {
            mVal = Null::create();
        } else if (m_Combo->get_active_text() == "Set") {
            mVal = Set::create();
        } else if (m_Combo->get_active_text() == "String") {
            mVal = String::create();
        } else if (m_Combo->get_active_text() == "Table") {
            SimpleTypeBox box("Width ?", "1");
            int w = utils::to_int(box.run());
            SimpleTypeBox box2("Height ?", "1");
            int h = utils::to_int(box2.run());
            mVal = Table::create(w, h);
        } else if (m_Combo->get_active_text() == "Tuple") {
            mVal = Tuple::create();
        } else if (m_Combo->get_active_text() == "Xml") {
            mVal = Xml::create();
        }
    }
}

}
} // namespace vle gvle
