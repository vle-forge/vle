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


#ifndef GUI_CALENDARBOX_HPP
#define GUI_CALENDARBOX_HPP

#include <vle/gvle/DllDefines.hpp>
#include <gtkmm/dialog.h>
#include <gtkmm/calendar.h>
#include <gtkmm/builder.h>


namespace vle { namespace gvle {

class GVLE_API CalendarBox
{
public:
    CalendarBox(const Glib::RefPtr < Gtk::Builder >& xml);

    void date(std::string& date);
    void selectDate(int day, int month, int year);
    void dateBegin(std::string& date);

private:
    Glib::RefPtr < Gtk::Builder >       mXml;

    Gtk::Dialog*                        mDialog;
    Gtk::Calendar*                      mCalendar;
};

}} // namespace vle gvle

#endif
