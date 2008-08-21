/**
 * @file vle/gvle/ValueBox.hpp
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


#ifndef GUI_VALUEBOX_HPP
#define GUI_VALUEBOX_HPP

#include <gtkmm.h>
#include <vle/value/Value.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Set.hpp>
#include <vle/vpz/Condition.hpp>
#include <vle/gvle/TreeViewValue.hpp>

namespace vle
{
namespace gvle {

class ValueBox : public Gtk::Dialog
{
public:
    ValueBox(value::MapFactory* map);
    ValueBox(value::SetFactory* set);

    virtual ~ValueBox();

    void run();
private:
    TreeViewValue* mTreeView;
    value::ValueBase* mValue;
    value::Value mBackup;

    void makeDialog();
};

}
} // namespace vle gvle

#endif
