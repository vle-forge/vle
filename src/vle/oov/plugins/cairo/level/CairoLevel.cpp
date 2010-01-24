/**
 * @file vle/oov/plugins/cairo/level/CairoLevel.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
 * Copyright (C) 2003-2010 ULCO http://www.univ-littoral.fr
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


#include <vle/oov/plugins/cairo/level/CairoLevel.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/value.hpp>
#include <gdkmm/color.h>

namespace vle { namespace oov { namespace plugin {

CairoLevel::CairoLevel(const std::string& location) :
    CairoPlugin(location),
    m_time(-1.0),
    m_receive(0)
{
}

CairoLevel::~CairoLevel()
{
}

void CairoLevel::onNewObservable(const std::string& simulator,
                                 const std::string& parent,
                                 const std::string& /* port */,
                                 const std::string& /* view */,
                                 const double& /* time */)
{
    std::string name(buildname(parent, simulator));

    if (m_columns2.find(name) != m_columns2.end()) {
        throw utils::InternalError(fmt(
                _("CairoLevel: observable '%1%' already exist")) % name);
    }

    m_columns.push_back(name);
    m_columns2[name] = m_buffer.size();
    m_buffer.push_back(0);
}

void CairoLevel::onDelObservable(const std::string& /* simulator */,
                                 const std::string& /* parent */,
                                 const std::string& /* port */,
                                 const std::string& /* view */,
                                 const double& /* time */)
{
}

void CairoLevel::onValue(const std::string& simulator,
                         const std::string& parent,
                         const std::string& /* port */,
                         const std::string& /* view */,
                         const double& time,
                         value::Value* value)
{
    m_time = time;

    std::string name(buildname(parent, simulator));
    std::map < std::string, int >::iterator it = m_columns2.find(name);

    if (it == m_columns2.end()) {
        throw utils::InternalError(fmt(
                _("CairoLevel: columns %1% does not exist. No new Observable ?")) %
            name);
    }

    m_buffer[it->second] = value;
    m_receive++;
    draw();
    copy();
}

void CairoLevel::onParameter(const std::string& /* plugin */,
                             const std::string& /* location */,
                             const std::string& /* file */,
                             value::Value* parameters,
                             const double& /* time */)
{
    m_minX = 1;
    m_maxX = 106;
    m_minY = 1;
    m_maxY = 306;

    if (parameters) {
        if (not parameters->isMap()) {
            throw utils::ArgError(
                _("Level: initialization failed, bad parameters"));
        }
        value::Map* init = dynamic_cast < value::Map* >(parameters);

	const value::Set& curves = toSetValue(init->get("curves"));
	int i = 0;

	for(value::Set::const_iterator it = curves.begin();
	    it != curves.end(); ++it, ++i) {
	    value::Map* curve = toMapValue(*it);
	    Gdk::Color color = Gdk::Color(toString(curve->get("color")));

	    m_colorList[i][0] = color.get_red();
	    m_colorList[i][1] = color.get_green();
	    m_colorList[i][2] = color.get_blue();
	    m_minList[i] = toDouble(curve->get("min"));
	    m_maxList[i] = toDouble(curve->get("max"));
	}

	const value::Map& size = toMapValue(init->get("size"));

	m_minX = toInteger(size.get("minx"));
	m_maxX = toInteger(size.get("maxx"));
	m_minY = toInteger(size.get("miny"));
	m_maxY = toInteger(size.get("maxy"));

        delete parameters;
    }

    m_colorList[0] = std::vector<int>(3);
    m_colorList[0][0] = 255;
    m_colorList[0][1] = 0;
    m_colorList[0][2] = 0;
}

void CairoLevel::close(const double& /* time */)
{
}

void CairoLevel::preferredSize(int& width, int& height)
{
    width = 105;
    height = 305;
}

void CairoLevel::draw()
{
    if (m_receive == m_columns.size()) {
        int shiftY = 20;
        m_ctx->rectangle(0, 0, 105, 305);
        m_ctx->set_source_rgb(1, 1, 1);
        m_ctx->fill();

        if (not m_buffer.empty()) {
            unsigned int stepX = (m_maxX - m_minX - 10) / m_columns.size();
            unsigned int i = 0;
            std::vector < value::Value* >::iterator it;

            for (it = m_buffer.begin(); it != m_buffer.end(); ++it) {
                double value;
                if ((*it)->isInteger()) {
                    value = value::toLong(*it);
                } else if ((*it)->isDouble()) {
                    value = value::toDouble(*it);
                } else {
                    throw utils::InternalError(
                          _("Can only represent integer or real"));
                }
                unsigned int y = (int)((value - m_minList[i]) *
                                       (m_maxY - m_minY - shiftY) /
                                       (m_maxList[i] - m_minList[i]));

                if (y < m_minY) {
                    y = 0;
                }

                if (y > m_maxY - m_minY - shiftY) {
                    y = m_maxY - m_minY - shiftY;
                }

                m_ctx->set_source_rgb(m_colorList[i][0], m_colorList[i][1],
                                    m_colorList[i][2]);
                m_ctx->rectangle(m_minX + i * stepX,
                               m_maxY - y - shiftY, stepX, y);
                m_ctx->fill();
                m_ctx->stroke();

                m_ctx->set_source_rgb(0., 0., 0.);

                m_ctx->rectangle(m_minX + i * stepX, m_minY, stepX,
                               (m_maxY - m_minY - shiftY));
                m_ctx->begin_new_path();
                m_ctx->move_to(m_minX + i * stepX, m_maxY - y - shiftY);
                m_ctx->line_to(m_minX + (i + 1) * stepX, m_maxY - y - shiftY);
                m_ctx->close_path();
                m_ctx->stroke();

                m_ctx->set_source_rgb(0., 0., 0.);
                m_ctx->move_to(m_minX + i * stepX + 1,
                               (int)((m_maxY - m_minY - shiftY) / 2));
                m_ctx->show_text(utils::toString(value));

                m_ctx->move_to(m_minX+i*stepX+1, m_maxY-shiftY);
                m_ctx->show_text(m_columns[i]);

                delete *it;
                *it = 0;

                ++i;
            }
        }
        m_receive = 0;
    }
}

}}} // namespace vle oov plugin

