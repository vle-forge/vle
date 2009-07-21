/**
 * @file vle/gvle/CalendarBox.cpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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


#include <vle/gvle/CalendarBox.hpp>
#include <gtkmmconfig.h>

namespace vle
{
namespace gvle {

CalendarBox::CalendarBox(Glib::RefPtr<Gnome::Glade::Xml> xml):
        mXml(xml)
{
    xml->get_widget("DialogCalendar", mDialog);
    xml->get_widget("Calendar", mCalendar);
}

void CalendarBox::get_date(std::string& date)
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

void CalendarBox::get_dateBegin(std::string& date)
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

}
} // namespace vle gvle
