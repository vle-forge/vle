/**
 * @file vle/gvle/DialogString.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
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


#include <vle/gvle/DialogString.hpp>
#include <vle/utils/i18n.hpp>
#include <gtkmm/stock.h>

using Glib::ustring;
using std::string;
using std::list;

namespace vle
{
namespace gvle {

DialogString::DialogString(const string& dialog_name,
                           const string& frame_name,
                           const list < ustring >& names) :
        Gtk::Dialog(dialog_name, true, true),
        mList(names)
{
    mFrame = Gtk::manage(new Gtk::Frame(frame_name));
    mEntry = Gtk::manage(new Gtk::Entry());

    add_button(Gtk::Stock::NO, Gtk::RESPONSE_NO);
    Gtk::Button* ok = add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

    mFrame->add(*mEntry);
    mFrame->set_border_width(5);
    get_vbox()->pack_start(*mFrame, true, true, 5);

    ok->set_flags(Gtk::CAN_DEFAULT);
    ok->grab_default();
    mEntry->set_activates_default();
    set_position(Gtk::WIN_POS_MOUSE);
}

std::string DialogString::run()
{
    for (;;) {
        show_all();
        int response = Gtk::Dialog::run();
        if (response == Gtk::RESPONSE_OK) {
            if (get_string().empty()) {
                return _("No empty string");
            } else if (get_string().is_ascii() == false) {
                return _("Only ascii character");
            } else if (std::find(mList.begin(), mList.end(), get_string()) !=
                       mList.end()) {
                return _("Name already exist");
            } else {
                hide();
                return "";
            }
        } else {
            hide();
            return _("Empty string");
        }
    }
}

}
} // namespace vle gvle
