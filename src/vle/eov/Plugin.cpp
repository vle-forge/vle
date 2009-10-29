/**
 * @file vle/eov/Plugin.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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


#include <vle/eov/Plugin.hpp>
#include <vle/eov/NetStreamReader.hpp>
#include <vle/utils/Tools.hpp>
#include <iostream>

namespace vle { namespace eov {

void Plugin::onExposeEvent(GdkEventExpose* /* event */)
{
    if (drawingSurface() and drawingWidget().is_realized() and m_cairoplugin) {
        int width, height;
        m_cairoplugin->preferredSize(width, height);

        if (not m_buffer or not m_ctx) {
            buildBuffer(width, height);
        }

        if (m_buffer and m_ctx and m_cairoplugin->stored()) {
            Glib::Mutex::Lock lock(m_net->mutex());

            m_ctx->save();
            m_ctx->set_source(m_cairoplugin->stored(), 0.0, 0.0);
            m_ctx->paint();
            m_ctx->restore();

            drawingSurface()->draw_drawable(
                Gdk::GC::create(drawingSurface()), m_buffer, 0, 0, 0,
                0, width, height);
        }
    }
}

void Plugin::onConfigureEvent(GdkEventConfigure* event)
{
    if (drawingSurface() and drawingWidget().is_realized() and m_cairoplugin) {
        Glib::Mutex::Lock lock(m_net->mutex());

        int width = event->width;
        int height = event->height;
        m_cairoplugin->onSize(width, height);
        m_cairoplugin->preferredSize(width, height);

        buildBuffer(width, height);

        m_cairoplugin->needInit();
    }
}

void Plugin::buildBuffer(int width, int height)
{
    if (m_buffer) {
        m_buffer.clear();
    }

    if (m_ctx) {
        m_ctx.clear();
    }

    m_buffer = Gdk::Pixmap::create(drawingSurface(), width, height);
    if (m_buffer) {
        m_ctx = m_buffer->create_cairo_context();
    }
}

}} // namespace vle eov
