/**
 * @file vle/oov/plugins/cairo/caview/CairoCAView.cpp
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


#include <vle/oov/plugins/cairo/caview/CairoCAView.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/value.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>

namespace vle { namespace oov { namespace plugin {

CairoCAView::CairoCAView(const std::string& location) :
    CairoPlugin(location),
    mRows(0),
    mColumns(0),
    mPen(1),
    mWindowWidth(405),
    mWindowHeight(405),
    mTime(-1.0),
    mReceiveCell(0),
    mReceiveObject(0),
    mValues(0)
{
}

CairoCAView::~CairoCAView()
{
    delete mValues;
}

void CairoCAView::onParameter(const std::string& /* plugin */,
                              const std::string& /* location */,
                              const std::string& /* file */,
                              value::Value* parameters,
                              const double& /* time */)
{
    if (parameters) {
        if (not parameters->isMap()) {
            throw utils::ArgError(
                _("CAView: initialization failed, bad parameters"));
        }

        value::Map* init = dynamic_cast < value::Map* >(parameters);

	const value::Map& size = toMapValue(init->get("size"));

	mColumns = toInteger(size.get("x"));
	mRows = toInteger(size.get("y"));

	mValues = new array_type(boost::extents[mColumns][mRows]);

	mMinX = 0;
	mMinY = 0;
	mMaxX = mWindowWidth;
	mMaxY = mWindowHeight;

	const value::Map& geometry = toMapValue(init->get("geometry"));

	if (toString(geometry.get("type")) == "square") {
	    mGeometry = SQUARE;
	    mStepX = (mMaxX - mMinX) / mColumns;
	    mStepY = (mMaxY - mMinY) / mRows;
	} else {
	    mGeometry = HEXA;
	    mStepX = (mMaxX - mMinX) / (mColumns+1);
	    mStepX2 = (int)(mStepX * 0.5);
	    mStepY = (int)((mMaxY - mMinY) / (mRows * 0.7));
	    mStepY3 = (int)(mStepY * 0.3);
	    mStepY7 = (int)(mStepY * 0.7);
	}

	if (init->existValue("cellName")) {
	    mCellName = toString(init->get("cellName"));
	}

	const value::Map& states = toMapValue(init->get("states"));

	if (states.existValue("name")) {
	    mStateName = toString(states.get("name"));
	}
	if (toString(states.get("type")) == "integer") {
	    mType = INTEGER;
	} else if (toString(states.get("type")) == "real") {
	    mType = REAL;
	} else if (toString(states.get("type")) == "boolean") {
	    mType = BOOLEAN;
	}

	switch (mType) {
	case INTEGER: {
	    const value::Set& values = toSetValue(states.get("values"));

	    for(value::Set::const_iterator it = values.begin();
		it != values.end(); ++it) {
		value::Map* value = toMapValue(*it);

		mColorList[toInteger(value->get("value"))] =
		    CairoCAView::color(toInteger(value->get("red")),
				       toInteger(value->get("green")),
				       toInteger(value->get("blue")));
	    }
	    break;
	}
	case REAL: {
	    const value::Set& values = toSetValue(states.get("values"));

	    for(value::Set::const_iterator it = values.begin();
		it != values.end(); ++it) {
		value::Map* value = toMapValue(*it);

		double v_minValue = toDouble(value->get("min"));
		double v_maxValue = toDouble(value->get("max"));
		std::string v_color = toString(value->get("color"));
		RealColor::color_type v_type = RealColor::LINEAR;
		double v_coef = 0.;

		if (toString(value->get("type")) == "linear") {
		    v_type = RealColor::LINEAR;
		} else if (toString(value->get("type")) == "highvalue") {
		    v_type = RealColor::HIGHVALUE;
		} else if (toString(value->get("type")) == "lowvalue") {
		    v_type = RealColor::LOWVALUE;
		}
		if (v_type == RealColor::HIGHVALUE or
		    v_type == RealColor::LOWVALUE) {
		    v_coef = toDouble(value->get("coef"));
		}
		mRealColorList.push_back(
		    RealColor(v_minValue, v_maxValue, v_color,
			      v_type, v_coef));
	    }
	    break;
	}
	case BOOLEAN: {
	    const value::Set& values = toSetValue(states.get("values"));

	    for(value::Set::const_iterator it = values.begin();
		it != values.end(); ++it) {
		value::Map* value = toMapValue(*it);

		mColorList[0] =
		    CairoCAView::color(toInteger(value->get("red_false")),
				       toInteger(value->get("green_false")),
				       toInteger(value->get("blue_false")));
		mColorList[1] =
		    CairoCAView::color(toInteger(value->get("red_true")),
				       toInteger(value->get("green_true")),
				       toInteger(value->get("blue_true")));
	    }
	    break;
	}
	}

	mObjectNumber = 0;

	if (init->existValue("objects")) {
	    const value::Set& objects = toSetValue(init->get("objects"));
	    std::vector<int> tmp_vector(5, 0);

	    for(value::Set::const_iterator it = objects.begin();
		it != objects.end(); ++it) {
		value::Map* object = toMapValue(*it);
		std::string name = toString(object->get("name"));
		int nb = toInteger(object->get("number"));
		std::string shape = toString(object->get("shape"));
		int red = toInteger(object->get("color_red"));
		int green = toInteger(object->get("color_green"));
		int blue = toInteger(object->get("color_blue"));
		CairoCAView::color color =
		    CairoCAView::color(red, green, blue);

		mObjectList[name] =
		    std::pair < std::string ,CairoCAView::color>(shape, color);

		tmp_vector[2] = red;
		tmp_vector[3] = green;
		tmp_vector[4] = blue;
		for (int i = 0; i < nb; i++)
		    mObjects[name].push_back(tmp_vector);
		mObjectNumber += nb;
	    }
	}
    }

    mMaxX = mStepX * mColumns;
    mMaxY = mStepY * mRows;

    delete parameters;
}

void CairoCAView::onNewObservable(const std::string& /* simulator */,
                                  const std::string& /* parent */,
                                  const std::string& /* port */,
                                  const std::string& /* view */,
                                  const double& /* time */)
{
}

void CairoCAView::onDelObservable(const std::string& /* simulator */,
                                  const std::string& /* parent */,
                                  const std::string& /* port */,
                                  const std::string& /* view */,
                                  const double& /* time */)
{
}

void CairoCAView::onValue(const std::string& simulator,
                          const std::string& /* parent */,
                          const std::string& port,
                          const std::string& /* view */,
                          const double& time,
                          value::Value* value)
{
    mTime = time;
    std::vector < std::string > v;

    boost::split(v, simulator, boost::is_any_of("_"));

    if (v[0] == mCellName) {
        (*mValues)[(index)(boost::lexical_cast < int >(v[1])-1)]
	    [(index)(boost::lexical_cast < int >(v[2])-1)] =
	    value;
        ++mReceiveCell;
    } else {
        if (port == "c") {
            value::Set v_color = value->toSet();
            mObjects[v[0]][boost::lexical_cast < int >(v[1])-1][2] =
		v_color.get(0).toInteger().value();
            mObjects[v[0]][boost::lexical_cast < int >(v[1])-1][3] =
		v_color.get(1).toInteger().value();
            mObjects[v[0]][boost::lexical_cast < int >(v[1])-1][4] =
		v_color.get(2).toInteger().value();
        } else {
            if (port == "x") {
                mObjects[v[0]][boost::lexical_cast < int >(v[1])-1][0] =
		    value->toInteger().value();
            } else {
                mObjects[v[0]][boost::lexical_cast < int >(v[1])-1][1] =
		    value->toInteger().value();
            }
            ++mReceiveObject;
        }
	delete value;
    }

    if (mReceiveCell == mRows * mColumns and
	mReceiveObject == mObjectNumber * 2) {
	draw();
        copy();
	mReceiveCell = 0;
	mReceiveObject = 0;
    }
}

void CairoCAView::close(const double& /* time */)
{
    delete mValues;
    mValues = 0;
}

void CairoCAView::onSize(int width, int height)
{
    if (width > 10 and height > 10) {
        mWindowWidth = width;
        mWindowHeight = height;

        mMinX = 0;
        mMinY = 0;
        mMaxX = mWindowWidth;
        mMaxY = mWindowHeight;

        if (mGeometry == SQUARE) {
            mGeometry = SQUARE;
            mStepX = (mMaxX - mMinX) / mColumns;
            mStepY = (mMaxY - mMinY) / mRows;
        } else {
            mStepX = (mMaxX - mMinX) / (mColumns+1);
            mStepX2 = (int)(mStepX * 0.5);
            mStepY = (int)((mMaxY - mMinY) / (mRows * 0.7));
            mStepY3 = (int)(mStepY * 0.3);
            mStepY7 = (int)(mStepY * 0.7);
        }
        mMaxX = mStepX * mColumns;
        mMaxY = mStepY * mRows;
    }
}

void CairoCAView::preferredSize(int& width, int& height)
{
    width = mWindowWidth;
    height = mWindowHeight;
}

CairoCAView::cairo_color CairoCAView::build_color(value::Value* p_value)
{
    CairoCAView::cairo_color v_color;

    switch (mType) {
    case INTEGER: {
	v_color.r = mColorList[toInteger(p_value)].r / 65535.;
	v_color.g = mColorList[toInteger(p_value)].g / 65535.;
	v_color.b = mColorList[toInteger(p_value)].b / 65535.;
	break;
    }
    case REAL: {
	double v_value = toDouble(p_value);
	std::list < RealColor >::iterator it = mRealColorList.begin();
	bool v_found = false;

	while (not v_found and it != mRealColorList.end()) {
	    if (it->m_min <= v_value && it->m_max > v_value)
		v_found = true;
	    else ++it;
	}
	if (v_found) {
	    double d = it->m_max - it->m_min;
	    double v = v_value - it->m_min;
	    double c = v/d;

	    if (it->m_type == RealColor::HIGHVALUE)
		c = pow(c,it->m_coef);
	    else if (it->m_type == RealColor::LOWVALUE)
		c = exp(-(it->m_coef)*c);

	    if (it->m_color == "red") {
		v_color.r = 1.;
		v_color.g = 1. - c;
		v_color.b = 0.;
	    }
	    else if (it->m_color == "red_only") {
		v_color.r = 1.;
		v_color.g = 0.;
		v_color.b = 0.;
	    }
	    else if (it->m_color == "white") {
		v_color.r = 1.;
		v_color.g = 1.;
		v_color.b = 1.;
	    }
	    else if (it->m_color == "blue") {
		v_color.r = c;
		v_color.g = c;
		v_color.b = 1.;
	    }
	    else if (it->m_color == "green") {
		v_color.r = c;
		v_color.g = 1.;
		v_color.b = c;
	    }
	    else if (it->m_color == "yellow") {
		v_color.r = 1.;
		v_color.g = 1.;
		v_color.b = 1. - c;
	    }
	    else if (it->m_color == "white_green") {
		v_color.r = 1. - c;
		v_color.g = 1.;
		v_color.b = 1. - c;
	    }
	    else if (it->m_color == "green_only") {
		v_color.r = 0.;
		v_color.g = 1.;
		v_color.b = 0.;
	    }
	} else {
	    v_color.r = 0.;
	    v_color.g = 0.;
	    v_color.b = 0.;
	}
	break;
    }
    case BOOLEAN: {
	v_color.r = mColorList[toBoolean(p_value)].r / 65535.;
	v_color.g = mColorList[toBoolean(p_value)].g / 65535.;
	v_color.b = mColorList[toBoolean(p_value)].b / 65535.;
	break;
    }
    }
    return v_color;
}

void CairoCAView::draw()
{
    m_ctx->rectangle(0, 0, mWindowWidth, mWindowHeight);
    m_ctx->set_source_rgb(0.745098, 0.745098, 0.745098);
    m_ctx->fill();
    if (mGeometry == SQUARE) {
	int midX = ((mMaxX - mMinX) - (mColumns * mStepX)) / 2;
	int midY = ((mMaxY - mMinY) - (mRows * mStepY)) / 2;

	for (index y = 0; y < (index)mRows ; ++y)
	    for (index x = 0; x < (index)mColumns ; ++x) {
		cairo_color v_color = build_color((*mValues)[x][y]);

		m_ctx->set_source_rgb(v_color.r, v_color.g, v_color.b);
		m_ctx->rectangle(x * mStepX + 2 + midX,
			       y * mStepY + 2 + midY,
			       mStepX - 1, mStepY - 1 );
		m_ctx->fill();
		delete (*mValues)[x][y];
	    }
	draw_objects(m_ctx);
    }
    else if (mGeometry == HEXA) {
	for (index y = 0; y < (index)mRows ; ++y)
	    for (index x = 0; x < (index)mColumns ; ++x) {
		cairo_color v_color = build_color((*mValues)[x][y]);

		m_ctx->set_source_rgb(v_color.r, v_color.g, v_color.b);
		draw_hexa(m_ctx, x, y);
	    }
	draw_hexa_objects(m_ctx);
    }
}

void CairoCAView::draw_hexa(Cairo::RefPtr < Cairo::Context > m_ctx,
			    int x, int p_y)
{
    double y;

    m_ctx->begin_new_path();
    if (p_y%2 == 0) {
	y = p_y * 0.7;
	m_ctx->move_to(x*mStepX+mPen,y*mStepY+mStepY3);
	m_ctx->line_to(x*mStepX+mStepX2,y*mStepY);
	m_ctx->line_to((x+1)*mStepX-mPen,y*mStepY+mStepY3);
	m_ctx->line_to((x+1)*mStepX-mPen,y*mStepY+mStepY7);
	m_ctx->line_to(x*mStepX+mStepX2,(y+1)*mStepY);
	m_ctx->line_to(x*mStepX+mPen,y*mStepY+mStepY7);
    }else{
	y = p_y * 0.7;
	m_ctx->move_to(x*mStepX+mStepX2+mPen,y*mStepY+mStepY3);
	m_ctx->line_to((x+1)*mStepX,y*mStepY);
	m_ctx->line_to((x+1)*mStepX+mStepX2-1,y*mStepY+mStepY3);
	m_ctx->line_to((x+1)*mStepX+mStepX2-1,y*mStepY+mStepY7);
	m_ctx->line_to((x+1)*mStepX,(y+1)*mStepY);
	m_ctx->line_to(x*mStepX+mStepX2+1,y*mStepY+mStepY7);
    }
    m_ctx->close_path();

    m_ctx->fill_preserve();
    m_ctx->set_source_rgb( 0.745098, 0.745098, 0.745098);
    m_ctx->set_line_width(mPen);
    m_ctx->stroke();
}

void CairoCAView::draw_objects(Cairo::RefPtr < Cairo::Context > m_ctx)
{
    ObjectList::iterator ito = mObjectList.begin();

    while (ito != mObjectList.end()) {
	std::string v_name = ito->first;

        for(unsigned int i=0; i < mObjects[v_name].size(); ++i){
            int x = mObjects[v_name][i][0];
            int y = mObjects[v_name][i][1];

	    int midX = ((mMaxX - mMinX) - (mColumns * mStepX)) / 2;
	    int midY = ((mMaxY - mMinY) - (mRows * mStepY)) / 2;

            m_ctx->set_source_rgb(mObjects[v_name][i][2]/65535.,
				mObjects[v_name][i][3]/65535.,
				mObjects[v_name][i][4]/65535.);

	    if (ito->second.first == "square"){
		m_ctx->rectangle( (x-1)*mStepX+3+midX,(y-1)*mStepY+3+midY,
				mStepX-4, mStepY-4 );
		m_ctx->fill();

	    }else {
		if (ito->second.first == "circle"){
		    m_ctx->arc( ((x-1)*mStepX+3+midX) + (mStepX-4)/2,
			      ((y-1)*mStepY+3+midY) + (mStepY-4)/2,
			      mStepY>mStepX?( (mStepX-4)/2 ):( (mStepY-4)/2 ),
			      0,
			      2*M_PI );
		    m_ctx->fill();
		}
	    }
	}
	++ito;
    }
}

void CairoCAView::draw_hexa_objects(Cairo::RefPtr < Cairo::Context > m_ctx)
{
    ObjectList::iterator ito = mObjectList.begin();

    while (ito != mObjectList.end()) {
	std::string v_name = ito->first;

        for(unsigned int i=0; i < mObjects[v_name].size(); ++i){
            int x = mObjects[v_name][i][0];
	    int y = mObjects[v_name][i][1];

            m_ctx->set_source_rgb(mObjects[v_name][i][2]/65535.,
				mObjects[v_name][i][3]/65535.,
				mObjects[v_name][i][4]/65535.);

	    double p_y = y *0.7;
	    if (y % 2 == 0) {
		if (ito->second.first == "square"){
		    m_ctx->rectangle( x*mStepX + 1 ,
				   p_y*mStepY + mStepY3 + 1 ,
				   2*(mStepX2-1) ,
				   mStepY7-mStepY3 - 2
			);
		    m_ctx->fill();

		}else{
		    if (ito->second.first == "circle"){
			m_ctx->arc(  x*mStepX  + mStepX2,
				  (p_y)*mStepY+(mStepY3+mStepY7)/2,
				  mStepX2/2,
				  0,
				  2 * M_PI );
			m_ctx->fill();
		    }
		}
	    } else {
		if (ito->second.first == "square"){
		    m_ctx->rectangle( x*mStepX + mStepX2 +1,
				   p_y*mStepY + mStepY3 +1,
				   2*(mStepX2-1) ,
				   mStepY7-mStepY3 -2
			);
		    m_ctx->fill();
		}else{
		    if (ito->second.first == "circle"){
			m_ctx->arc(  (x+1)*mStepX ,
				  p_y*mStepY +(mStepY3+mStepY7)/2 ,
				  mStepX2/2,
				  0,
				  2 * M_PI );
			m_ctx->fill();
		    }
		}

	    }
	}
	++ito;
    }
}

}}} // namespace vle oov plugin

