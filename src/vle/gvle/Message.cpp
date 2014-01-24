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


#include <vle/gvle/Message.hpp>
#include <gtkmm/messagedialog.h>

namespace vle { namespace gvle {

void Debug(const std::string& debug)
{
    Gtk::MessageDialog* box;

    std::string::size_type pos = debug.find("----------\n");
    if (pos == std::string::npos) {
        box = new Gtk::MessageDialog(debug, false, Gtk::MESSAGE_WARNING,
                                     Gtk::BUTTONS_OK, true);
    } else {
        std::string left(debug, 0, pos);
        std::string right(debug, pos + 12);

        box = new Gtk::MessageDialog(left, false, Gtk::MESSAGE_WARNING,
                                     Gtk::BUTTONS_OK, true);

#if GTK_VERSION_GT(2,4)
        box->set_secondary_text(right);
#endif
    }

    box->run();
    delete box;
}

void Info(const std::string& information)
{
    Gtk::MessageDialog* box;

    std::string::size_type pos = information.find("----------\n");
    if (pos == std::string::npos) {
        box = new Gtk::MessageDialog(information, false, Gtk::MESSAGE_INFO,
                                     Gtk::BUTTONS_OK, true);
    } else {
        std::string left(information, 0, pos);
        std::string right(information, pos + 12);

        box = new Gtk::MessageDialog(left, false, Gtk::MESSAGE_INFO,
                                     Gtk::BUTTONS_OK, true);
#if GTK_VERSION_GT(2,4)
        box->set_secondary_text(right);
#endif
    }

    box->run();
    delete box;
}

void Error(const std::string& msg, bool use_markup)
{
    Gtk::MessageDialog* box;
    std::string msg_first;
    std::string msg_second;

    if (use_markup) {

        std::string::size_type pos1 = msg.find("----------");
        if (pos1 == std::string::npos) {
            msg_first.assign(msg);
        } else {
            msg_first.assign(msg, 0, pos1);
            msg_second.assign(msg, pos1 + 10, std::string::npos);
            msg_second = Glib::Markup::escape_text(msg_second);

            std::string::size_type pos2 = msg_second.find("----------");
            if (pos2 == std::string::npos) {
                msg_second.insert(0, "<i>");
                msg_second.append("</i>");
            } else {
                msg_second.erase(pos2, 10);
                msg_second.insert(pos2, "</i>\n<tt>");
                msg_second.insert(0, "<i>");
                msg_second.append("</tt>");
            }
        }

        msg_first.insert(0, "<b>");
        msg_first.append("</b>\n");
        msg_first += msg_second;
    } else {
        msg_first.assign(msg);
    }

    box = new Gtk::MessageDialog(
        msg_first, use_markup, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);

    box->run();
    delete box;
}

bool Question(const std::string& question)
{
    Gtk::MessageDialog* box = NULL;
    int r;

    box = new Gtk::MessageDialog(question, false, Gtk::MESSAGE_QUESTION,
                                 Gtk::BUTTONS_YES_NO, true);
    r = box->run();
    delete box;

    return (r == Gtk::RESPONSE_YES);
}

}} // namespace vle gvle
