/** 
 * @file CairoGauge.hpp
 * @brief 
 * @author The vle Development Team
 * @date Thu Oct 25 13:57:22 CEST 2007
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

        virtual void onParameter(const vpz::ParameterTrame& trame);

        virtual void onNewObservable(const vpz::NewObservableTrame& trame);

        virtual void onDelObservable(const vpz::DelObservableTrame& trame);

        virtual void onValue(const vpz::ValueTrame& trame);

        virtual void close(const vpz::EndTrame& trame);

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

        Cairo::RefPtr < Cairo::ImageSurface > mImageSurface;
    };
    
    DECLARE_OOV_PLUGIN(oov::plugin::CairoGauge);

}}} // namespace vle oov plugin

#endif
