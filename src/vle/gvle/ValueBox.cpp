/**
 * @file vle/gvle/ValueBox.cpp
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


#include <iostream>
#include <vle/gvle/ValueBox.hpp>

using namespace vle;

namespace vle
{
namespace gvle {
ValueBox::ValueBox(value::MapFactory* map):
        Gtk::Dialog("Map",true,true),
        mTreeView(0),
        mValue(map)
{
    mBackup = map->clone();

    mTreeView = new TreeViewValue();
    mTreeView->makeTreeView(*map);
    get_vbox()->pack_start(*mTreeView);
    makeDialog();
    show_all();
}

ValueBox::ValueBox(value::SetFactory* set):
        Gtk::Dialog("Set",true,true),
        mTreeView(0),
        mValue(set)
{
    mBackup = set->clone();

    mTreeView = new TreeViewValue();
    mTreeView->makeTreeView(*set);
    get_vbox()->pack_start(*mTreeView);
    makeDialog();
    show_all();
}

ValueBox::~ValueBox()
{
    hide_all();
    if (mTreeView)
        delete mTreeView;
}

void ValueBox::run()
{
    int ret = Gtk::Dialog::run();
    if (ret == Gtk::RESPONSE_CANCEL) {
        if (mValue->getType() == value::ValueBase::MAP) {
            value::MapFactory* map = dynamic_cast<value::MapFactory*>(mValue);
            value::MapFactory* backup = dynamic_cast<value::MapFactory*>(&*mBackup);

            map->getValue().clear();

            value::MapValue::const_iterator it = backup->begin();
            while (it != backup->end()) {
                map->addValue(it->first, it->second);
                ++it;
            }
        } else if (mValue->getType() == value::ValueBase::SET) {
            value::SetFactory* set = dynamic_cast<value::SetFactory*>(mValue);
            value::SetFactory* backup = dynamic_cast<value::SetFactory*>(&*mBackup);

            set->getValue().clear();

            value::VectorValue::iterator it = backup->begin();
            while (it != backup->end()) {
                set->addValue(*it);
                ++it;
            }
        }
    }
}

void ValueBox::makeDialog()
{
    add_button(Gtk::Stock::APPLY, Gtk::RESPONSE_APPLY);
    add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
}
}
} // namespace vle gvle
