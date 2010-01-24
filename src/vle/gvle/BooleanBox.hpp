/**
 * @file vle/gvle/BooleanBox.hpp
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


#ifndef GUI_BOOLEANBOX_HPP
#define GUI_BOOLEANBOX_HPP

#include <gtkmm.h>
#include <vle/value/Boolean.hpp>

namespace vle
{
namespace gvle {

class BooleanBox : public Gtk::Dialog
{
public:
    BooleanBox(value::Boolean* boolean);
    virtual ~BooleanBox();

    void run();
private:
    Gtk::ComboBoxText* m_Combo;
    value::Boolean* mValue;
};

}
} // namespace vle gvle

#endif
