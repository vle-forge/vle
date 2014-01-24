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


#include <vle/gvle/CalendarBox.hpp>
#include <cassert>

namespace vle { namespace gvle {

CalendarBox::CalendarBox(const Glib::RefPtr < Gtk::Builder >& xml)
    : mXml(xml)
{
    xml->get_widget("DialogCalendar", mDialog);
    xml->get_widget("Calendar", mCalendar);

    assert(mDialog);
    assert(mCalendar);
}

void CalendarBox::date(std::string& date)
{
    if (mDialog->run() == Gtk::RESPONSE_OK) {
#if GTKMM_MAJOR_VERSION == 2 && GTKMM_MINOR_VERSION <= 10
        guint year, month, day;
        mCalendar->get_date(year, month, day);
        Glib::Date d((Glib::Date::Day)day, (Glib::Date::Month)month,
                     (Glib::Date::Year)year);
#else
        Glib::Date d;
        mCalendar->get_date(d);
#endif
        Glib::ustring format = "%a, %d %b %Y";
        date = d.format_string(format);
    }
    mDialog->hide();
}

void CalendarBox::selectDate(int day, int month, int year)
{
    mCalendar->select_month((guint)month - 1, (guint)year);
    mCalendar->select_day((guint) day);
}

void CalendarBox::dateBegin(std::string& date)
{
    if (mDialog->run() == Gtk::RESPONSE_OK) {
#if GTKMM_MAJOR_VERSION == 2 && GTKMM_MINOR_VERSION <= 10
        guint year, month, day;
        mCalendar->get_date(year, month, day);
        Glib::Date d((Glib::Date::Day)day, (Glib::Date::Month)month,
                     (Glib::Date::Year)year);
#else
        Glib::Date d;
        mCalendar->get_date(d);
#endif
        Glib::ustring format = "%Y-%m-%d";
        date = d.format_string(format);
    }
    mDialog->hide();
}

}} // namespace vle gvle
