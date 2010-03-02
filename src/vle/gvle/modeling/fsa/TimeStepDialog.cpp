/**
 * @file vle/gvle/modeling/fsa/TimeStepDialog.cpp
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

#include <vle/gvle/modeling/fsa/TimeStepDialog.hpp>

namespace vle { namespace gvle { namespace modeling {

TimeStepDialog::TimeStepDialog(const Glib::RefPtr < Gnome::Glade::Xml >& xml)
{
    xml->get_widget("TimeStepDialog", mDialog);
    xml->get_widget("TimeStepEntry", mTimeStepEntry);
}

int TimeStepDialog::run(const std::string& timeStep)
{
    mTimeStepEntry->set_text(timeStep);
    mDialog->set_default_response(Gtk::RESPONSE_ACCEPT);

    int response = mDialog->run();

    mDialog->hide();
    return response;
}

}}} // namespace vle gvle modeling
