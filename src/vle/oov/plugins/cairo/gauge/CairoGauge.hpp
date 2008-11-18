/**
 * @file vle/oov/plugins/cairo/gauge/CairoGauge.hpp
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


#ifndef VLE_OOV_PLUGINS_CAIRO_GAUGE_HPP
#define VLE_OOV_PLUGINS_CAIRO_GAUGE_HPP

#include <vle/oov/CairoPlugin.hpp>
#include <map>
#include <vector>

namespace vle { namespace oov { namespace plugin {

    class CairoGauge: public CairoPlugin
    {
    public:
        CairoGauge(const std::string& location);

        virtual ~CairoGauge();

        virtual void onParameter(const std::string& plugin,
                                 const std::string& location,
                                 const std::string& file,
                                 const std::string& parameters,
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
	void draw_line(Cairo::RefPtr < Cairo::Context > ctx);
	void draw_text(Cairo::RefPtr < Cairo::Context > ctx);
	void draw_background(Cairo::RefPtr < Cairo::Context > ctx);

	double mLine;
	double mMin;
	double mMax;
	double mValue;
	double mScale;
	int mWindowWidth;
	int mWindowHeight;

	std::string mName;
	double mTime;
    };

    DECLARE_OOV_PLUGIN(oov::plugin::CairoGauge)

}}} // namespace vle oov plugin

#endif
