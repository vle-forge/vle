/**
 * @file vle/gvle/MatrixBox.hpp
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


#ifndef GUI_MATRIXBOX_HPP
#define GUI_MATRIXBOX_HPP

#include <gtkmm.h>
#include <vle/value/Matrix.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Table.hpp>
#include <boost/multi_array.hpp>
#include <vle/gvle/GVLE.hpp>

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
    typedef boost::multi_array< std::pair<Gtk::Button*, Gtk::Tooltips*>, 2 > array_type;

    Gtk::ScrolledWindow* mScroll;
    Gtk::Table* mTable;
    array_type* mArray;

    value::Matrix* mValue;
    value::Value* mBackup;

    void makeTable();

    void on_click(unsigned int i, unsigned int j);
};

}
} // namespace vle gvle

#endif
