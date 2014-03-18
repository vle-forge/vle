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

#include <gtkmm/stock.h>
#include <vle/value/Boolean.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/value/Null.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Table.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/XML.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/NewTypeBox.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/utils/Tools.hpp>

using namespace vle;

namespace vle {
namespace gvle {

NewTypeBox::NewTypeBox() :
    Gtk::Dialog("?", true, true),
    mVal(0)
{
    add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    add_button(Gtk::Stock::OK, Gtk::RESPONSE_APPLY);

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

value::Value* NewTypeBox::launch()
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
            int cols = utils::to < int32_t >(box.run());
            if (cols < 1) {
                gvle::Error(_("The size can not be null or negative"));
                return mVal;
            }
            SimpleTypeBox box2("rows ?", "1");
            int rows = utils::to < int32_t >(box2.run());
            if (rows < 1) {
                gvle::Error(_("The size can not be null or negative"));
                return mVal;
            }
            mVal = Matrix::create(cols,
                    rows,
                    cols * 2,
                    rows * 2,
                    cols / 5,
                    rows / 5);
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
            int w = utils::to < int32_t >(box.run());
            if (w < 1) {
                gvle::Error(_("The size can not be null or negative"));
                return mVal;
            }
            SimpleTypeBox box2("Height ?", "1");
            int h = utils::to < int32_t >(box2.run());
            if (h < 1) {
                gvle::Error(_("The size can not be null or negative"));
                return mVal;
            }
            mVal = Table::create(w, h);
        } else if (m_Combo->get_active_text() == "Tuple") {
            mVal = Tuple::create();
        } else if (m_Combo->get_active_text() == "Xml") {
            mVal = Xml::create();
        }
    }
    return mVal;
}
}
} // namespace vle gvle
