/**
 * @file vle/oov/plugins/cairo/netview/CairoNetView.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
 * Copyright (C) 2003-2009 ULCO http://www.univ-littoral.fr
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


#ifndef VLE_OOV_PLUGINS_CAIRO_NETVIEW_HPP
#define VLE_OOV_PLUGINS_CAIRO_NETVIEW_HPP

#include <vle/oov/plugins/cairo/netview/Graph.hpp>
#include <vle/oov/plugins/cairo/netview/Colors.hpp>
#include <vle/oov/CairoPlugin.hpp>
#include <map>
#include <vector>
#include <utility>
#include <boost/multi_array.hpp>

namespace vle { namespace oov { namespace plugin {

    class CairoNetView: public CairoPlugin
    {
    public:
        typedef std::map < std::string ,
                std::pair < std::string , color > > ObjectList;

        CairoNetView(const std::string& location);

        virtual ~CairoNetView();

        virtual void onParameter(const std::string& plugin,
                                 const std::string& location,
                                 const std::string& file,
                                 value::Value* parameters,
                                 const double& time);

        virtual void onNewObservable(const std::string& simulator,
                                     const std::string& parent,
                                     const std::string& port,
                                     const std::string& view,
                                     const double& time);

        virtual void onDelObservable(const std::string& simulator,
                                     const std::string& parent,
                                     const std::string& port,
                                     const std::string& view,
                                     const double& time);

        virtual void onValue(const std::string& simulator,
                             const std::string& parent,
                             const std::string& port,
                             const std::string& view,
                             const double& time,
                             value::Value* value);

        virtual void close(const double& time);

        ///
        ////
        ///

        virtual void preferredSize(int& width, int& height);

        virtual void onSize(int width, int height);

    private:
        cairo_color mColors;
        void draw();
        void scale_positions();
        void draw_arrow(const int start_x, const int start_y,
                        int end_x, int end_y, int rayon);
        double get_length(const int Ax, const int Ay, const int Bx, const int By);
        bool m_display_node_names;
        void draw_node_name(const std::string &node_name, double x,
                            double y, double font_size);

        std::string mExecutiveName;
        std::string mStateName;

        int mWidth;
        int mHeight;
        double mMaxX;
        double mMaxY;

        double mPen;

        int mWindowWidth;
        int mWindowHeight;

        double mTime;
        unsigned int mReceiveCell;

        typedef std::map<std::string, std::string> map_strings;
        map_strings mValues;

        Graph *mGraph;
    };

    DECLARE_OOV_PLUGIN(oov::plugin::CairoNetView)

}}} // namespace vle oov plugin

#endif
