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


#ifndef GUI_MATRIXBOX_HPP
#define GUI_MATRIXBOX_HPP

#include <gtkmm/dialog.h>
#include <gtkmm/scrolledwindow.h>
//#include <gtkmm/tooltip.h>
#include <gtkmm/grid.h>
#include <vle/value/Matrix.hpp>
#include <vle/value/Table.hpp>
#include <boost/multi_array.hpp>

namespace vle
{
namespace gvle {

class MatrixBox : public Gtk::Dialog
{
public:
    MatrixBox(value::Matrix* matrix);
    virtual ~MatrixBox();

    void run();
private:
    typedef boost::multi_array< std::pair<Gtk::Button*, Gtk::Object*>, 2 > array_type;

    Gtk::ScrolledWindow* mScroll;
    Gtk::Grid* mTable;
    array_type* mArray;

    value::Matrix* mValue;
    value::Value* mBackup;

    void makeTable();

    void on_click(unsigned int i, unsigned int j);
};

}
} // namespace vle gvle

#endif
