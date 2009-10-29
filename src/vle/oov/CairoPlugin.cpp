/**
 * @file vle/oov/CairoPlugin.cpp
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


#include <vle/oov/CairoPlugin.hpp>
#include <iostream>

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
