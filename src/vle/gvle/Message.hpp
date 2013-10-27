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


#ifndef VLE_GVLE_WIDGETS_MESSAGE_HPP
#define VLE_GVLE_WIDGETS_MESSAGE_HPP 1

#include <vle/gvle/DllDefines.hpp>
#include <vle/utils/i18n.hpp>
#include <glibmm.h>

namespace vle { namespace gvle {

/**
 * @brief Show a Gtk dialog box with warning icon and error message.
 *
 * @param str a string representation of debugging message.
 */
void GVLE_API Debug(const std::string& str);

/**
 * @brief Show a Gtk dialog box with warning icon and error message.
 * @code
 * vle::gvle::Debug(vle::fmt("%1% - %2%") % 1.23 % "a string");
 * @endcode
 *
 * @param argv A boost::format argument.
 */
inline void Debug(const boost::format& str)
{
    Debug(str.str());
}

/**
 * @brief Show a Gtk dialog box with warning icon and information message.
 *
 * @param str a string representation of information message.
 */
void GVLE_API Info(const std::string& str);

/**
 * @brief Show a Gtk dialog box with warning icon and information message.
 * @code
 * vle::gvle::Debug(vle::fmt("%1% - %2%") % 1.23 % "a string");
 * @endcode
 *
 * @param argv A boost::format argument.
 */
inline void Info(const boost::format& str)
{
    Info(str.str());
}

/**
 * @brief Show a Gtk dialog box with error icon and error message.
 *
 * @param msg a string representation of error message.
 * @param use_markup to enhance or not the text presentation by
 * inserting html tags inside the message string.
 */
void GVLE_API Error(const std::string& msg, bool use_markup = true);

/**
 * @brief Show a Gtk dialog box with error icon and error message.
 *
 * @param msg a boost::format of error message.
 * @param use_markup to enhance or not the text presentation by
 * inserting html tags inside the message string.
 */
inline void Error(const boost::format& msg,
                                  bool use_markup = true)
{
    Error(msg.str(), use_markup);
}

/**
 * @brief Show a Gtk dialog box with a question. User have two choices, YES or
 * NO.
 *
 * @param str a string representation of question.
 *
 * @return true if user have clicked on YES, otherwise false.
 */
bool GVLE_API Question(const std::string& str);

/**
 * @brief Show a Gtk dialog box with a question. User have two choices, YES or
 * NO.
 *
 * @param str a boost::format representation of question.
 *
 * @return true if user have clicked on YES, otherwise false.
 */
inline bool Question(const boost::format& str)
{
    return Question(str.str());
}

}} // namespace vle gvle

#endif
