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


#ifndef GUI_TABLEBOX_HPP
#define GUI_TABLEBOX_HPP

#include <gtkmm/dialog.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/grid.h>
#include <gtkmm/entry.h>
#include <vle/value/Table.hpp>
#include <boost/multi_array.hpp>

namespace vle
{
namespace gvle {

class TableBox : public Gtk::Dialog
{
public:
    TableBox(value::Table* table);
    virtual ~TableBox();

    void run();
private:
    typedef boost::multi_array<Gtk::Entry*, 2> array_type;
    typedef array_type::index index;

    Gtk::ScrolledWindow* mScroll;
    Gtk::Grid* mTable;
    array_type* mArray;

    value::Table* mValue;


    void makeTable();

};

}
} // namespace vle gvle

#endif
