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
#include <cassert>

namespace vle { namespace eov {

Window::Window(Glib::Mutex& mutex, int timer) :
    Gtk::Window(Gtk::WINDOW_TOPLEVEL),
    m_mutex(mutex),
    m_rootBox(true, 5),
    m_isinit(false),
    m_timeout(timer)
{
    set_title("eov - the Eyes of VLE");
    set_border_width(0);
    set_position(Gtk::WIN_POS_CENTER);

    show_all();

    Glib::signal_timeout().connect(
        sigc::mem_fun(*this, &Window::checkPluginTimer), 200);
}

void Window::setPlugin(PluginPtr plg)
{
    Glib::Mutex::Lock lock(m_mutex);

    assert(not m_isinit);
    assert(plg);

    m_plugin = plg;
}

void Window::killClock()
{
    Glib::Mutex::Lock lock(m_mutex);

    m_connection.disconnect();
}

bool Window::checkPluginTimer()
{
    Glib::Mutex::Lock lock(m_mutex);

    if (m_plugin) {
        m_isinit = true;

	m_plugin->drawingWidget().signal_expose_event().connect_notify(
	    sigc::mem_fun(*m_plugin.get(), &Plugin::onExposeEvent));

        m_rootBox.pack_start(m_plugin->widget(), true, true);
	add(m_rootBox);
	resize_children();
	show_all();

        m_connection = Glib::signal_timeout().connect(
            sigc::mem_fun(*this, &Window::redrawTimer), m_timeout);
    }

    return not m_isinit;
}

bool Window::redrawTimer()
{
    assert(m_plugin);

    if (is_realized()) {
	m_plugin->drawingWidget().queue_draw();
    }

    return true;
}

}} // namespace vle eov
