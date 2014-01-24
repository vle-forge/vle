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


#include <vle/oov/CairoPlugin.hpp>

namespace vle { namespace oov {

void CairoPlugin::init()
{
    int height, width;

    preferredSize(width, height);

    m_img = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, width, height);
    m_store = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, width, height);
    m_ctx = Cairo::Context::create(m_img);

    Cairo::RefPtr < Cairo::Context > ctx = Cairo::Context::create(m_store);

    ctx->save();
    ctx->set_source_rgb(1.0, 1.0, 1.0);
    ctx->set_operator(Cairo::OPERATOR_SOURCE);
    ctx->paint();
    ctx->set_operator(Cairo::OPERATOR_OVER);
    ctx->restore();

    m_ctx->save();
    m_ctx->set_source_rgb(1.0, 1.0, 1.0);
    m_ctx->set_operator(Cairo::OPERATOR_SOURCE);
    m_ctx->paint();
    m_ctx->set_operator(Cairo::OPERATOR_OVER);
    m_ctx->restore();

    m_init = false;
    m_need = true;
}

void CairoPlugin::copy()
{
    if (m_init) {
        init();
    }

    if (m_need) {
        Cairo::RefPtr < Cairo::Context > ctx = Cairo::Context::create(m_store);

        ctx->save();
        ctx->set_source(m_img, 0.0, 0.0);
        ctx->paint();
        ctx->restore();

        m_need = false;
        m_copydone = true;
    }
}

}} // namespace vle oov
