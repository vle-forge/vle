/**
 * @file vle/oov/plugins/cairo/caview/CairoCAView.hpp
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


#ifndef VLE_OOV_PLUGINS_CAIRO_CAVIEW_HPP
#define VLE_OOV_PLUGINS_CAIRO_CAVIEW_HPP

#include <vle/oov/CairoPlugin.hpp>
#include <map>
#include <vector>
#include <boost/multi_array.hpp>

namespace vle { namespace oov { namespace plugin {

    class RealColor
    {
    public:
        enum color_type { LINEAR, HIGHVALUE, LOWVALUE };

        double m_min;
        double m_max;
        std::string m_color;
        color_type m_type;
        double m_coef;

        RealColor(double p_min, double p_max, const std::string & p_color,
                  color_type p_type, double p_coef):
            m_min(p_min), m_max(p_max), m_color(p_color),
            m_type(p_type), m_coef(p_coef) { }
    };

    class CairoCAView: public CairoPlugin
    {
    public:
        enum geometry { SQUARE, HEXA };
        enum type { INTEGER, REAL, BOOLEAN };

        struct color
        {
            int r; int g; int b;
            color(int _r = 0, int _g = 0, int _b = 0)
                : r(_r), g(_g), b(_b) {}
        };

        struct cairo_color
        {
            double r; double g; double b;
            cairo_color(double _r = 0, double _g = 0, double _b = 0)
                : r(_r), g(_g), b(_b) {}
        };

        typedef std::map < std::string,
                std::pair < std::string , color > > ObjectList;

    public:
        CairoCAView(const std::string& location);

        virtual ~CairoCAView();

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
        cairo_color build_color(value::Value* p_value);
        void draw();
        void draw_hexa(Cairo::RefPtr < Cairo::Context > ctx, int x, int p_y);
        void draw_objects(Cairo::RefPtr < Cairo::Context > ctx);
        void draw_hexa_objects(Cairo::RefPtr < Cairo::Context > ctx);

        geometry mGeometry;
        type mType;
        std::string mCellName;
        std::string mStateName;
        std::map < int, color > mColorList;
        std::list < RealColor > mRealColorList;
        ObjectList mObjectList;

        unsigned int mRows;
        unsigned int mColumns;

        int mWidth;
        int mHeight;
        int mMinX;
        int mMaxX;
        int mMinY;
        int mMaxY;
        int mStepX;
        int mStepX2;
        int mStepY;
        int mStepY3;
        int mStepY7;

        double mPen;

        int mWindowWidth;
        int mWindowHeight;

        double mTime;
        unsigned int mReceiveCell;
        unsigned int mReceiveObject;
        unsigned int mObjectNumber;

        typedef boost::multi_array < value::Value*, 2> array_type;
        typedef array_type::index index;
        array_type* mValues;

        typedef std::map < std::string, std::vector < std::vector < int > > > object_type;
        object_type mObjects;
    };

    DECLARE_OOV_PLUGIN(oov::plugin::CairoCAView)

}}} // namespace vle oov plugin

#endif
