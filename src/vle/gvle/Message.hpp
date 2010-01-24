/**
 * @file vle/gvle/Message.hpp
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


#ifndef GVLE_WIDGETS_MESSAGE_HPP
#define GVLE_WIDGETS_MESSAGE_HPP

#include <vle/gvle/DllDefines.hpp>
#include <glibmm/ustring.h>

namespace vle { namespace gvle {

/**
 * Show a Gtk dialog box with warning icon and output error on console.
 *
 * @param debug a string representation of debugging message.
 */
void VLE_GVLE_EXPORT Debug(const Glib::ustring& debug);

/**
 * Show a Gtk dialog box with info icon and output information on
 * console.
 *
 * @param information a string representation of information message.
 */
void VLE_GVLE_EXPORT Info(const Glib::ustring& information);

/**
 * Show a Gtk dialog box with error icon and output information on
 * consolse.
 *
 * @param msg a string representation of error message.
 */
void VLE_GVLE_EXPORT Error(const Glib::ustring& msg);

/**
 * Show a Gtk dialog box with a question. User have two choices YES or
 * NO.
 *
 * @param question a string representation of question.
 *
 * @return true if user have clicked on YES, otherwise false.
 */
bool VLE_GVLE_EXPORT Question(const Glib::ustring& question);

}} // namespace vle gvle

#endif
