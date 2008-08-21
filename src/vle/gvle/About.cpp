/**
 * @file vle/gvle/About.cpp
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


#include <vle/gvle/About.hpp>
#include <vle/utils/Debug.hpp>
#include <gtkmm/label.h>
#include <gtkmm/stock.h>
#include "config.h"

namespace vle
{
namespace gvle {

About::About() :
        Gtk::Dialog("About", true, true)
{
    set_position(Gtk::WIN_POS_MOUSE);
    Gtk::Label* label = Gtk::manage(
                            new Gtk::Label("", Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER, true));
    label->set_line_wrap(true);
    label->set_justify(Gtk::JUSTIFY_CENTER);

    label->set_markup((boost::format(
                           "<big><big><big><b>GVLE %1%%2% "
                           "</b></big></big></big>\n"
                           "\n<b>A Modelling application to VLE</b>\n"
                           "<b>Virtual Laboritory Environment</b>"
                           "\n\nCopyright (C) 2004, 2005, 2006 The vle "
                           "Development Team\n\n"
                           "LIL - Laboratoire d'Informatique du Littoral\n"
                           "Université du Littoral Cote d'Opale\n\n"
                           ": <u>Web</u> :\n"
                           "http://vle.univ-littoral.fr\n"
                           "http://sourceforge.net/projects/vle/\n\n"
                           ": <u>Authors</u> :\n"
                           "Gauthier Quesnel quesnel@users.sourceforge.net\n"
                           "Eric Ramat eramat@users.sourceforge.net\n\n"
                           ": <u>Copying</u> :\n"
                           "Free software under GNU GPL license") %
            VLE_VERSION % VLE_EXTRA_VERSION).str());

    Gtk::Button* mm = add_button(Gtk::Stock::OK, 0);
    mm->signal_clicked().connect(sigc::mem_fun(*this, &Gtk::Dialog::hide));

    get_vbox()->add(*label);
    set_border_width(8);
    show_all();
}

}
} // namespace vle gvle
