/**
 * @file vle/eov/Window.cpp
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


#include <vle/eov/Window.hpp>
#include <vle/utils/Debug.hpp>
#include <iostream>

namespace vle { namespace eov {

Window::Window(PluginPtr plg) :
    Gtk::Window(Gtk::WINDOW_TOPLEVEL),
    m_rootBox(true, 5),
    m_plugin(plg),
    m_timeout(100)
{
    //set_size_request(400, 300);
    set_title("eov - the Eyes of VLE");
    set_border_width(0);
    set_position(Gtk::WIN_POS_CENTER);

    m_rootBox.pack_start(m_plugin->widget(), true, true);
    add(m_rootBox);

    resize_children();
    show_all();
}

Window::~Window()
{
}

void Window::on_realize()
{
    Gtk::Window::on_realize();

    m_connection = Glib::signal_timeout().connect(
        sigc::mem_fun(*this, &Window::runTimeout), m_timeout);
}

bool Window::on_expose_event(GdkEventExpose* event)
{
    bool r = Gtk::Window::on_expose_event(event);

    if (m_plugin and is_realized()) {
        m_plugin->redraw();
    }

    return r;
}

bool Window::on_configure_event(GdkEventConfigure* event)
{
    bool r = Gtk::Window::on_configure_event(event);

    if (m_plugin and is_realized()) {
        m_plugin->resize();
    }

    return r;
}

bool Window::runTimeout()
{
    if (m_plugin and is_realized()) {
        m_plugin->redraw();
    }

    return true;
}

}} // namespace vle eov
