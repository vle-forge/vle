/**
 * @file vle/gvle/ParameterExecutionBox.hpp
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


#ifndef GUI_PARAMETEREXECUTIONBOX_HPP
#define GUI_PARAMETEREXECUTIONBOX_HPP

#include <gtkmm/dialog.h>
#include <gtkmm/button.h>
#include <gtkmm/frame.h>
#include <gtkmm/box.h>
#include <gtkmm/entry.h>
#include <gtkmm/label.h>
#include <string>

namespace vle
{
namespace gvle {

class Modeling;

/**
 * @brief A Gtk::Dialog to parameter the execution of VLE project.
 */
class ParameterExecutionBox : public Gtk::Dialog
{
public:
    ParameterExecutionBox(Modeling* modeling);

    virtual ~ParameterExecutionBox() { }

    /**
     * show the and wait return
     *
     * @return true if all data in widget is ok, otherwise false.
     */
    bool run();

private:
    /** Built graphic interface of this Dialog. */
    void buildInterface();

    /** When user click to change Path of VLE binary. */
    void onClickButtonPath();

    Gtk::Entry*     mEntryPath;
    Gtk::Entry*     mEntryPort;
    Modeling*       mModeling;
};

}
} // namespace vle gvle

#endif
