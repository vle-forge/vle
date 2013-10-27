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
#include <vle/gvle/ValueBox.hpp>

namespace vle { namespace gvle {

ValueBox::ValueBox(value::Map* map) :
    Gtk::Dialog("Map", true),
    mTreeView(0),
    mValue(map)
{
    mBackup = map->clone();

    mTreeView = new TreeViewValue();
    mTreeView->makeTreeView(*map);
    set_default_size(200, 200);
    get_vbox()->pack_start(*mTreeView);
    makeDialog();
    show_all();
}

ValueBox::ValueBox(value::Set* set) :
    Gtk::Dialog("Set", true),
    mTreeView(0),
    mValue(set)
{
    mBackup = set->clone();

    mTreeView = new TreeViewValue();
    mTreeView->makeTreeView(*set);
    set_default_size(200, 200);
    get_vbox()->pack_start(*mTreeView);
    makeDialog();
    show_all();
}

ValueBox::~ValueBox()
{
    // No hide_all in GTK 3
    hide();
    delete mTreeView;
}

void ValueBox::run()
{
    int ret = Gtk::Dialog::run();
    if (ret == Gtk::RESPONSE_CANCEL) {
        if (mValue->getType() == value::Value::MAP) {
            value::Map* map = dynamic_cast < value::Map* > (mValue);
            value::Map* backup = dynamic_cast < value::Map* > (&*mBackup);

            map->value().clear();

            value::MapValue::const_iterator it = backup->begin();
            while (it != backup->end()) {
                map->add(it->first, it->second);
                ++it;
            }
        } else if (mValue->getType() == value::Value::SET) {
            value::Set* set = dynamic_cast < value::Set* > (mValue);
            value::Set* backup = dynamic_cast < value::Set* > (&*mBackup);

            set->value().clear();

            value::VectorValue::iterator it = backup->begin();
            while (it != backup->end()) {
                set->add(*it);
                ++it;
            }
        }
    }
}

void ValueBox::makeDialog()
{
    add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    add_button(Gtk::Stock::OK, Gtk::RESPONSE_APPLY);
}

} } // namespace vle gvle
