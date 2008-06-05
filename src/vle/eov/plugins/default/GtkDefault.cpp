/**
 * @file src/vle/eov/plugins/default/GtkDefault.cpp
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


#include <vle/eov/plugins/default/GtkDefault.hpp>

namespace vle { namespace eov {

GtkDefault::GtkDefault(oov::CairoPluginPtr cairoplugin,
                       NetStreamReader* net) :
    Plugin(cairoplugin, net),
    m_vbox(false, 5),
    m_label("Default plugin")
{
    int width, height;

    m_cairoplugin->preferredSize(width, height);

    if (width > 0 and height > 0) {
        m_da.set_size_request(width, height);
    } else {
        m_da.set_size_request(400, 300);
    }

    m_da.signal_expose_event().connect(
        sigc::mem_fun(*this, &GtkDefault::on_expose_event));

    m_vbox.pack_start(m_da, true, true, 0);
    m_vbox.pack_start(m_label, false, false, 0);
}

GtkDefault::~GtkDefault()
{
}

Glib::RefPtr < Gdk::Window > GtkDefault::drawingSurface()
{
    return m_da.get_window();
}

Gtk::Widget& GtkDefault::widget()
{
    return m_vbox;
}

bool GtkDefault::on_expose_event(GdkEventExpose* /* event */)
{
    return true;
}

int GtkDefault::width()
{
    int width, height;
    drawingSurface()->get_size(width, height);
    return width;
}

int GtkDefault::height()
{
    int width, height;
    drawingSurface()->get_size(width, height);
    return height;
}

}} // namespace vle eov
