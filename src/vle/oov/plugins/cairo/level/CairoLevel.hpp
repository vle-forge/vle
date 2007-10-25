/** 
 * @file CairoLevel.hpp
 * @brief 
 * @author The vle Development Team
 * @date Thu Sep 27 19:06:38 CEST 2007
 */

/*
 * Copyright (C) 2007 - The vle Development Team
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef VLE_OOV_PLUGINS_CAIRO_LEVEL_HPP
#define VLE_OOV_PLUGINS_CAIRO_LEVEL_HPP

#include <vle/oov/CairoPlugin.hpp>
#include <map>
#include <vector>

namespace vle { namespace oov { namespace plugin {

    class CairoLevel: public CairoPlugin
    {
    public:
        CairoLevel(const std::string& location);

        virtual ~CairoLevel();

        virtual void onParameter(const vpz::ParameterTrame& trame);

        virtual void onNewObservable(const vpz::NewObservableTrame& trame);

        virtual void onDelObservable(const vpz::DelObservableTrame& trame);

        virtual void onValue(const vpz::ValueTrame& trame);

        virtual void close(const vpz::EndTrame& trame);

        virtual bool isDrawable() const
        { return m_receive == m_columns.size(); }

    private:
        std::string buildname(const std::string& simulator,
                              const std::string& port)
        {
            std::string result(simulator);
            result += '_';
            result += port;
            return result;
        }

        void draw();

        std::vector < std::string > m_columns;
        std::map < std::string, int > m_columns2;
        std::vector < value::Value > m_buffer;
        double m_time;
        unsigned int m_receive;
        std::map < int, double > m_minList;
        std::map < int, double > m_maxList;
        std::map < int, std::vector <int> > m_colorList;

        unsigned int m_minX;
        unsigned int m_maxX;
        unsigned int m_minY;
        unsigned int m_maxY;

        Cairo::RefPtr < Cairo::ImageSurface > m_img;
    };
    
    DECLARE_OOV_PLUGIN(oov::plugin::CairoLevel);

}}} // namespace vle oov plugin

#endif
