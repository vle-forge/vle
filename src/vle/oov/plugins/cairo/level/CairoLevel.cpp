/**
 * @file vle/oov/plugins/cairo/level/CairoLevel.cpp
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


#include <vle/oov/plugins/cairo/level/CairoLevel.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/XML.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/String.hpp>

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

    Assert(utils::InternalError,m_columns2.find(name) == m_columns2.end(),
           boost::format("CairoLevel: observable '%1%' already exist") % name);

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
    std::map < std::string,int >::iterator it = m_columns2.find(name);

    Assert(utils::InternalError,it != m_columns2.end(),boost::format(
            "CairoLevel: columns %1% does not exist. No new Observable ?") %
        name);

    m_buffer[it->second] = value;
    m_receive++;
    draw();
}

void CairoLevel::onParameter(const std::string& /* plugin */,
                             const std::string& /* location */,
                             const std::string& /* file */,
                             const std::string& parameters,
                             const double& /* time */)
{
    m_minX = 1;
    m_maxX = 106;
    m_minY = 1;
    m_maxY = 306;

    if (not parameters.empty()) {
        xmlpp::DomParser parser;
        parser.parse_memory(parameters);

	xmlpp::Element* root = utils::xml::get_root_node(parser, "parameters");

        if (root) {
	    xmlpp::Element* elt = utils::xml::get_children(root, "curves");
	    xmlpp::Node::NodeList lst = elt->get_children("curve");
	    xmlpp::Node::NodeList::iterator it = lst.begin();
	    int i = 0;

	    while (it != lst.end()) {
		xmlpp::Element * elt2 = ( xmlpp::Element* )( *it );
		std::string name = utils::xml::get_attribute(elt2,"name");

//		m_colorList[name] = Gdk::Color(utils::xml::get_attribute(elt2,"color"));
		m_minList[i] = utils::to_double(utils::xml::get_attribute(elt2,"min"));
		m_maxList[i] = utils::to_double(utils::xml::get_attribute(elt2,"max"));
		++it;
		++i;
	    }

	    elt = utils::xml::get_children(root, "size");
            using boost::lexical_cast;
            using utils::xml::get_attribute;
            m_minX = lexical_cast < int >(get_attribute(elt, "minx"));
            m_maxX = lexical_cast < int >(get_attribute(elt, "maxx"));
            m_minY = lexical_cast < int >(get_attribute(elt, "miny"));
            m_maxY = lexical_cast < int >(get_attribute(elt, "maxy"));
        }
    }

    if (not context()) {
        int width = m_maxX - m_minX;
        int height = m_maxY - m_minY;
        m_img = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, width,
                                            height);
        setSurface(m_img);
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
    Assert(utils::InternalError, context(), "Cairo level drawing error");
    Cairo::RefPtr < Cairo::Context > ctx = context();

    if (m_receive == m_columns.size()) {
        int shiftY = 20;
        ctx->rectangle(0, 0, 105, 305);
        ctx->set_source_rgb(1, 1, 1);
        ctx->fill();

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
                    Throw(utils::InternalError,
                          "Can only represent integer or real");
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

                ctx->set_source_rgb(m_colorList[i][0], m_colorList[i][1],
                                    m_colorList[i][2]);
                ctx->rectangle(m_minX + i * stepX,
                               m_maxY - y - shiftY, stepX, y);
                ctx->fill();
                ctx->stroke();

                ctx->set_source_rgb(0., 0., 0.);

                ctx->rectangle(m_minX + i * stepX, m_minY, stepX,
                               (m_maxY - m_minY - shiftY));
                ctx->begin_new_path();
                ctx->move_to(m_minX + i * stepX, m_maxY - y - shiftY);
                ctx->line_to(m_minX + (i + 1) * stepX, m_maxY - y - shiftY);
                ctx->close_path();
                ctx->stroke();

                ctx->set_source_rgb(0., 0., 0.);
                ctx->move_to(m_minX + i * stepX + 1,
                               (int)((m_maxY - m_minY - shiftY) / 2));
                ctx->show_text(utils::to_string(value));

                ctx->move_to(m_minX+i*stepX+1, m_maxY-shiftY);
                ctx->show_text(m_columns[i]);

                delete *it;
                *it = 0;

                ++i;
            }
        }
        m_receive = 0;
    }
}

}}} // namespace vle oov plugin

