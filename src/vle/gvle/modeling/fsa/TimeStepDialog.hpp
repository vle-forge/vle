/**
 * @file vle/gvle/modeling/fsa/TimeStepDialog.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2010 ULCO http://www.univ-littoral.fr
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


#ifndef VLE_GVLE_MODELING_FSA_TIME_STEP_DIALOG_HPP
#define VLE_GVLE_MODELING_FSA_TIME_STEP_DIALOG_HPP

#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <libglademm.h>

namespace vle { namespace gvle { namespace modeling {

class TimeStepDialog
{
public:
    TimeStepDialog(const Glib::RefPtr < Gnome::Glade::Xml >& xml);

    virtual ~TimeStepDialog()
    { }

    std::string timeStep() const
    { return mTimeStepEntry->get_text(); }

    int run(const std::string& timeStep);

private:
    Gtk::Dialog* mDialog;
    Gtk::Entry*  mTimeStepEntry;
};

}}} // namespace vle gvle modeling

#endif
