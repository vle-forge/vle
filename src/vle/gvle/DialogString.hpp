/**
 * @file vle/gvle/DialogString.hpp
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


#ifndef GVLE_WIDGETS_DIALOGSTRING_HPP
#define GVLE_WIDGETS_DIALOGSTRING_HPP

#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <gtkmm/frame.h>
#include <glibmm/ustring.h>
#include <list>

namespace vle
{
namespace gvle {

class DialogString : public Gtk::Dialog
{
public:
    /**
     * Build a new Dialog with string Entry widget, and test if name already
     * exist in list
     *
     * @param names a list of string not available in entry widget.
     * @param dialog_name Dialog title
     * @param frame_name Frame title
     */
    DialogString(const std::string& dialog_name,
                 const std::string& frame_name,
                 const std::list < Glib::ustring >& names);

    /** Nothing to delete */
    virtual ~DialogString() {}

    /**
     * Get string from entry widget
     *
     * @return string from entry widget, no test
     */
    inline Glib::ustring get_string() {
        return mEntry->get_text();
    }

    /**
     * While user don't have a good response or quit dialog, wait
     *
     * @return string reprensetation of error, empty if no error found.
     */
    std::string run();

protected:
    Gtk::Frame*                         mFrame;
    Gtk::Entry*                         mEntry;
    const std::list < Glib::ustring >&  mList;
};

}
} // namespace vle gvle

#endif
