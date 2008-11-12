/**
 * @file vle/oov/plugins/cairo/gauge/CairoGauge.cpp
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


#include <vle/oov/plugins/cairo/gauge/CairoGauge.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/XML.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/String.hpp>

namespace vle { namespace oov { namespace plugin {

CairoGauge::CairoGauge(const std::string& location) :
    CairoPlugin(location),
    mLine(1.),
    mMin(0.0),
    mMax(1.0),
    mValue(0.0),
    mScale(1.8),
    mWindowWidth(175),
    mWindowHeight(120),
    mTime(-1.0)
{
}

CairoGauge::~CairoGauge()
{
}

void CairoGauge::onParameter(const std::string& /* plugin */,
                             const std::string& /* location */,
                             const std::string& /* file */,
                             const std::string& parameters,
                             const double& /* time */)
{
    if (not context()) {
        mImageSurface =
            Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32,
                                        mWindowWidth, mWindowHeight);
	setSurface(mImageSurface);
    }

    xmlpp::DomParser parser;

    parser.parse_memory(parameters);
    xmlpp::Element* root = utils::xml::get_root_node(parser, "parameters");
    xmlpp::Element * elt = utils::xml::get_children(root,"min");
    mMin = utils::to_double(utils::xml::get_attribute(elt,"value").c_str());
    xmlpp::Element * elt2 = utils::xml::get_children(root,"max");
    mMax = utils::to_double(utils::xml::get_attribute(elt2,"value").c_str());
}

void CairoGauge::onNewObservable(const std::string& simulator,
                                 const std::string& /* parent */,
                                 const std::string& port,
                                 const std::string& /* view */,
                                 const double& /* time */)
{
    std::string name(buildname(simulator, port));

    Assert(utils::InternalError, mName == "",
           boost::format("CairoGauge: observable '%1%' already exists")
           % name);

    mName = name;
}

void CairoGauge::onDelObservable(const std::string& /* simulator */,
                                 const std::string& /* parent */,
                                 const std::string& /* port */,
                                 const std::string& /* view */,
                                 const double& /* time */)
{
}

void CairoGauge::onValue(const std::string& simulator,
                         const std::string& /* parent */,
                         const std::string& port,
                         const std::string& /* view */,
                         const double& time,
                         value::Value* value)
{
    mTime = time;

    std::string name(buildname(simulator, port));

    if (name == mName) {
        mValue = value->toDouble().value();
        delete value;
    } else {
        Throw(utils::InternalError, boost::format(
                "CairoGauge: columns %1% does not exist") % name);
    }

    draw();
}

void CairoGauge::close(const double& /* time */)
{
}

void CairoGauge::preferredSize(int& width, int& height)
{
    width = mWindowWidth;
    height = mWindowHeight;
}

void CairoGauge::draw()
{
    Assert(utils::InternalError, context(), "Cairo gauge drawing error");
    Cairo::RefPtr < Cairo::Context > ctx = context();

    draw_background(ctx);
    draw_text(ctx);
    draw_line(ctx);
}

void CairoGauge::draw_background(Cairo::RefPtr < Cairo::Context > ctx)
{
    double sy = mWindowHeight * mScale;

    ctx->rectangle(0, 0, mWindowWidth, mWindowHeight);
    ctx->set_source_rgb(1.,1.,1.);
    ctx->fill();

    ctx->begin_new_path();
    ctx->set_source_rgb(0.,0.,0.);
    ctx->arc(mWindowWidth/2, mWindowHeight, (mWindowWidth>sy)?sy/2:mWindowWidth/2 , M_PI, 0);
    ctx->close_path();
    ctx->stroke();
}

void CairoGauge::draw_line(Cairo::RefPtr < Cairo::Context > ctx)
{
    double nx = 0.;
    double ny = 0.;
    double mx = mWindowWidth / 2;
    double my = mWindowHeight - mLine ;

    double sy = mWindowHeight * mScale;
    double rayon = (mWindowWidth>sy)?sy/2:mWindowWidth/2;

//		std::cout << "mx : " << mx << " / " <<  mWindowWidth << " my : " << my << " / " << mWindowHeight << "\n";

    if (mValue <= mMin) {
	nx = mx - rayon  + mLine ;
	ny = mWindowHeight;
    } else if (mValue >= mMax) {
	nx = mx + rayon - mLine ;
	ny = mWindowHeight;
    } else {
	double angle = M_PI + (M_PI * mValue / (mMax - mMin));
	nx = mx + ( std::cos(angle) * rayon ) - mLine;
	ny = my + ( std::sin(angle) * rayon ) - mLine;
    }

    ctx->set_line_width(mLine);
    ctx->set_source_rgb(0.,0.,1.);
    ctx->begin_new_path();
    ctx->move_to(mx, my);
    ctx->line_to(nx, ny);
    ctx->close_path();
    ctx->stroke();
}

void CairoGauge::draw_text(Cairo::RefPtr < Cairo::Context > ctx)
{
//    int nx, ny;

    double sy = mWindowHeight * mScale;
    double rayon = (mWindowWidth>sy)?sy/2:mWindowWidth/2;

    ctx->set_source_rgb(0.,0.,0.);
    ctx->move_to( mWindowWidth / 2 - rayon + 5 , mWindowHeight - 3*mLine);
    ctx->show_text((boost::format("min: %1%") % mMin).str());

    ctx->move_to(mWindowWidth / 2 - 15, mWindowHeight - 3*mLine ) ;
    ctx->show_text(vle::utils::to_string( mValue));

    ctx->move_to(mWindowWidth / 2 + rayon - 40, mWindowHeight - 3*mLine );
    ctx->show_text( (boost::format("max: %1%") % mMax).str() );
}

}}} // namespace vle oov plugin

