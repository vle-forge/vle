/**
 * @file vle/gvle/ViewDrawingArea.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
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


#include <vle/gvle/Settings.hpp>
#include <vle/gvle/ViewDrawingArea.hpp>
#include <vle/gvle/View.hpp>
#include <vle/utils/Debug.hpp>
#include <gdkmm/gc.h>
#include <gdkmm/types.h>
#include <gdkmm/pixbuf.h>
#include <vector>
#include <cmath>
#include <cairomm/scaledfont.h>

namespace vle { namespace gvle {

const guint ViewDrawingArea::MODEL_WIDTH = 100;
const guint ViewDrawingArea::MODEL_DECAL = 20;
const guint ViewDrawingArea::MODEL_PORT  = 10;
const guint ViewDrawingArea::MODEL_PORT_SPACING_LABEL  = 10;
const guint ViewDrawingArea::MODEL_SPACING_PORT = 5;
const guint ViewDrawingArea::PORT_SPACING_LABEL = 5;
const guint ViewDrawingArea::MODEL_HEIGHT = 30;
const guint ViewDrawingArea::CURRENT_MODEL_PORT = 10;
const guint ViewDrawingArea::CURRENT_MODEL_SPACE = 10;
const double ViewDrawingArea::ZOOM_FACTOR_SUP = 0.5;
const double ViewDrawingArea::ZOOM_FACTOR_INF = 0.1;
const double ViewDrawingArea::ZOOM_MIN = 0.5;
const double ViewDrawingArea::ZOOM_MAX = 4.0;
const guint ViewDrawingArea::SPACING_MODEL = 25;
const guint ViewDrawingArea::SPACING_LINE = 5;
const guint ViewDrawingArea::SPACING_MODEL_PORT = 10;


ViewDrawingArea::ViewDrawingArea(View* view) :
    mMouse(-1, -1),
    mPrecMouse(-1, -1),
    mHeight(300),
    mWidth(450),
    mRectHeight(300),
    mRectWidth(450),
    mZoom(1.0),
    mIsRealized(false),
    mHighlightLine(-1)
{
    assert(view);

    set_events(Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_MOTION_MASK |
               Gdk::BUTTON1_MOTION_MASK | Gdk::BUTTON2_MOTION_MASK |
               Gdk::BUTTON3_MOTION_MASK | Gdk::BUTTON_PRESS_MASK |
               Gdk::BUTTON_RELEASE_MASK);

    mView = view;
    mCurrent = view->getGCoupledModel();
    mModeling = view->getModeling();
}

void ViewDrawingArea::draw()
{
    if (mIsRealized and mBuffer) {
	maxModelWidthHeight();
        calcRectSize();
	mContext->save();
	mContext->scale(mZoom, mZoom);
	drawCurrentCoupledModel();
        drawCurrentModelPorts();
        drawConnection();
        drawChildrenModels();
        drawLink();
        drawZoomFrame();
        drawHighlightConnection();
	set_size_request(mRectWidth * mZoom, mRectHeight * mZoom);
	mContext->restore();
    }
}

void ViewDrawingArea::drawCurrentCoupledModel()
{
    setColor(Settings::settings().getBackgroundColor());
    mContext->rectangle(0 + mOffset, 0 + mOffset, mWidth, mHeight);
    mContext->fill();
    mContext->stroke();

    if (mView->existInSelectedModels(mCurrent)) {
	setColor(Settings::settings().getSelectedColor());
	mContext->rectangle(0 + mOffset, 0 + mOffset,
			    mRectWidth,
			    mRectHeight);
	mContext->fill();
	mContext->stroke();

	setColor(Settings::settings().getBackgroundColor());
	mContext->rectangle(MODEL_PORT + mOffset,
			    MODEL_PORT + mOffset,
			    (mRectWidth - 2 * MODEL_PORT),
			    (mRectHeight - 2 * MODEL_PORT));
	mContext->fill();
	mContext->stroke();

	setColor(Settings::settings().getForegroundColor());
	mContext->rectangle(MODEL_PORT + mOffset,
			    MODEL_PORT + mOffset,
			    (mRectWidth - 2 * MODEL_PORT),
			    (mRectHeight - 2 * MODEL_PORT));
	mContext->stroke();
    } else {
	setColor(Settings::settings().getBackgroundColor());
	mContext->rectangle(0 + mOffset, 0 + mOffset,
			    mRectWidth,
			    mRectHeight);
	mContext->fill();
	mContext->stroke();

	setColor(Settings::settings().getForegroundColor());
	mContext->rectangle(MODEL_PORT + mOffset,
			    MODEL_PORT + mOffset,
			    (mRectWidth - 2 * MODEL_PORT),
			    (mRectHeight - 2 * MODEL_PORT));
	mContext->stroke();
    }
}

void ViewDrawingArea::drawCurrentModelPorts()
{

    const graph::ConnectionList ipl =  mCurrent->getInputPortList();
    const graph::ConnectionList opl =  mCurrent->getOutputPortList();
    graph::ConnectionList::const_iterator itl;

    const size_t maxInput = ipl.size();
    const size_t maxOutput = opl.size();
    const size_t stepInput = (int)(mRectHeight / (maxInput + 1));
    const size_t stepOutput = (int)(mRectHeight / (maxOutput + 1));

    mContext->select_font_face(Settings::settings().getFont(),
			  Cairo::FONT_SLANT_OBLIQUE,
			  Cairo::FONT_WEIGHT_NORMAL);
    mContext->set_font_size(Settings::settings().getFontSize());

    itl = ipl.begin();

    for (size_t i = 0; i < maxInput; ++i) {

        // to draw the port
	setColor(Settings::settings().getForegroundColor());
	mContext->move_to((MODEL_PORT),
			  (stepInput * (i + 1) - MODEL_PORT));
	mContext->line_to((MODEL_PORT),
			  (stepInput * (i + 1) + MODEL_PORT));
	mContext->line_to((MODEL_PORT + MODEL_PORT),
			  (stepInput * (i + 1)));
	mContext->line_to((MODEL_PORT),
			  (stepInput * (i + 1) - MODEL_PORT));
	mContext->fill();
	mContext->stroke();

	setColor(Settings::settings().getForegroundColor());
	mContext->move_to((MODEL_PORT),
			  (stepInput * (i + 1) - MODEL_PORT));
	mContext->line_to((MODEL_PORT),
			  (stepInput * (i + 1) + MODEL_PORT));
	mContext->line_to((MODEL_PORT + MODEL_PORT),
			  (stepInput * (i + 1)));
	mContext->line_to((MODEL_PORT),
			  (stepInput * (i + 1) - MODEL_PORT));
	mContext->fill();
	mContext->stroke();

        // to draw the label of the port
	if (mZoom >= 1.0) {
	    setColor(Settings::settings().getForegroundColor());
	    mContext->move_to((MODEL_PORT + MODEL_PORT_SPACING_LABEL),
			      (stepInput * (i + 1) + 10));
	    mContext->show_text(itl->first);
	    mContext->stroke();
	}
	itl++;
    }

    itl = opl.begin();

    for (guint i = 0; i < maxOutput; ++i) {

	setColor(Settings::settings().getForegroundColor());
	mContext->move_to((mRectWidth - MODEL_PORT),
			  (stepOutput * (i + 1) - MODEL_PORT));
	mContext->line_to((mRectWidth - MODEL_PORT),
			  (stepOutput * (i + 1) + MODEL_PORT));
	mContext->line_to((MODEL_PORT + mRectWidth - MODEL_PORT),
			  (stepOutput * (i + 1)));
	mContext->line_to((mRectWidth - MODEL_PORT),
			  (stepOutput * (i + 1) - MODEL_PORT));
	mContext->fill();
	mContext->stroke();

        // to draw the label of the port
	if (mZoom >= 1.0) {
	    mContext->move_to((mRectWidth),
			      (stepOutput * (i + 1) + 10));
	    mContext->show_text(itl->first);
	    mContext->stroke();
	}

	itl++;

    }
}

void ViewDrawingArea::preComputeConnection(int xs, int ys,
                                           int xd, int yd,
                                           int ytms, int ybms)
{
    Connection con(xs, ys, xd, yd, xs + SPACING_MODEL_PORT, xd -
                   SPACING_MODEL_PORT, 0, 0, 0);
    Point inpt(xd, yd);

    if (mDirect.find(inpt) == mDirect.end()) {
        mDirect[inpt] = false;
    }

    if (mInPts.find(xd) == mInPts.end()) {
        mInPts[xd] = std::pair < int, int >(0, 0);
    }

    if (ys > yd) {
        int p = ++(mInPts[xd].first);

        if (p == 1) {
            if (mDirect[inpt]) {
                p = ++(mInPts[xd].first);
            } else {
                mDirect[inpt] = true;
            }
        }
        con.y3 = yd;
        if (p > 1) {
            con.x2 -= (p - 1) * SPACING_LINE;
            con.y3 += (p - 1) * SPACING_LINE;
        }
        con.top = false;
    } else {
        int p = ++(mInPts[xd].second);

        if (p == 1) {
            if (mDirect[inpt]) {
                p = ++(mInPts[xd].second);
            } else {
                mDirect[inpt] = true;
            }
        }
        con.y3 = yd;
        if (p > 1) {
            con.x2 -= (p - 1) * SPACING_LINE;
            con.y3 -= (p - 1) * SPACING_LINE;
        }
        con.top = true;
    }

    Point outpt(xs, ys);

    if (mOutPts.find(xs) == mOutPts.end()) {
        mOutPts[xs] = std::pair < int, int >(0, 0);
    }
    if (ys > yd) {
        int p = ++(mOutPts[xs].first);

        con.y4 = ys;
        con.x1 += (p - 1) * SPACING_LINE;
        con.y4 -= (p - 1) * SPACING_LINE;
        if (con.x2 <= con.x1) {
            con.y1 = ytms - SPACING_MODEL - (p - 1) * SPACING_LINE;
        }
    } else {
        int p = ++(mOutPts[xs].second);

        con.y4 = ys;
        con.x1 += (p - 1) * SPACING_LINE;
        con.y4 += (p - 1) * SPACING_LINE;
        if (con.x2 <= con.x1) {
            con.y1 = ybms + SPACING_MODEL + (p - 1) * SPACING_LINE;
        }
    }

    mConnections.push_back(con);
}

void ViewDrawingArea::preComputeConnection(graph::Model* src,
                                           const std::string& portsrc,
                                           graph::Model* dst,
                                           const std::string& portdst)
{
    int xs, ys, xd, yd;
    int ytms = src->y();
    int ybms = src->y() + src->height();

    if (src == mCurrent) {
        getCurrentModelInPosition(portsrc, xs, ys);
        getModelInPosition(dst, portdst, xd, yd);
        preComputeConnection(xs, ys, xd, yd, ytms, ybms);
    } else if (dst == mCurrent) {
        getModelOutPosition(src, portsrc, xs, ys);
        getCurrentModelOutPosition(portdst, xd, yd);
        preComputeConnection(xs, ys, xd, yd, ytms, ybms);
    } else {
        getModelOutPosition(src, portsrc, xs, ys);
        getModelInPosition(dst, portdst, xd, yd);
        preComputeConnection(xs, ys, xd, yd, ytms, ybms);
    }
}

void ViewDrawingArea::preComputeConnection()
{
    using namespace graph;

    mConnections.clear();
    mDirect.clear();
    mInPts.clear();
    mOutPts.clear();
    mText.clear();

    {
        ConnectionList& outs(mCurrent->getInternalInputPortList());
        ConnectionList::const_iterator it;

        for (it = outs.begin(); it != outs.end(); ++it) {
            const ModelPortList& ports(it->second);
            ModelPortList::const_iterator jt ;

            for (jt = ports.begin(); jt != ports.end(); ++jt) {
                preComputeConnection(mCurrent, it->first,
				     jt->first, jt->second);
                mText.push_back(mCurrent->getName() +
                                ":" + it->first +
                                " -> " + jt->first->getName() +
                                ":" + jt->second);
	    }
	}
    }

    {
        const ModelList& children(mCurrent->getModelList());
        ModelList::const_iterator it;

        for (it = children.begin(); it != children.end(); ++it) {
            const ConnectionList& outs(it->second->getOutputPortList());
            ConnectionList::const_iterator jt;

            for (jt = outs.begin(); jt != outs.end(); ++jt) {
                const ModelPortList&  ports(jt->second);
                ModelPortList::const_iterator kt;

                for (kt = ports.begin(); kt != ports.end(); ++kt) {
                    preComputeConnection(it->second, jt->first,
                                         kt->first, kt->second);
                    mText.push_back(it->second->getName() +
                                    ":" + jt->first +
                                    " -> " + kt->first->getName() +
                                    ":" + kt->second);
                }
            }
        }
    }
}

ViewDrawingArea::StraightLine ViewDrawingArea::computeConnection(
    int xs, int ys,
    int xd, int yd,
    int index)
{
    StraightLine list;
    Connection con = mConnections[index];
    Point inpt(xd, yd);
    Point outpt(xs, ys);

    list.push_back(Point((int)(xs),
			 (int)(ys)));
    list.push_back(Point((int)((xs + SPACING_MODEL_PORT)),
			 (int)(ys)));
    if (con.x2 >= con.x1) {
	list.push_back(Point((int)(con.x1),
			     (int)(con.y4)));

	if (con.x2 > con.x1) {
	    list.push_back(Point((int)(con.x1),
				 (int)(con.y3)));
	}
    } else {
	list.push_back(Point((int)(con.x1),
			     (int)(con.y4)));
	list.push_back(Point((int)(con.x1),
			     (int)(con.y1)));
	list.push_back(Point((int)(con.x2),
			     (int)(con.y1)));
    }
    list.push_back(Point((int)(con.x2),
			 (int)(con.y3)));
    list.push_back(Point((int)(xd - SPACING_MODEL_PORT),
			 (int)(yd)));
    list.push_back(Point((int)(xd),
			 (int)(yd)));
    return list;
}

void ViewDrawingArea::computeConnection(graph::Model* src,
                                        const std::string& portsrc,
                                        graph::Model* dst,
                                        const std::string& portdst,
                                        int index)
{
    int xs, ys, xd, yd;

    if (src == mCurrent) {
        getCurrentModelInPosition(portsrc, xs, ys);
        getModelInPosition(dst, portdst, xd, yd);
        mLines.push_back(computeConnection(xs, ys, xd, yd, index));
    } else if (dst == mCurrent) {
        getModelOutPosition(src, portsrc, xs, ys);
        getCurrentModelOutPosition(portdst, xd, yd);
        mLines.push_back(computeConnection(xs, ys, xd, yd, index));
    } else {
        getModelOutPosition(src, portsrc, xs, ys);
        getModelInPosition(dst, portdst, xd, yd);
        mLines.push_back(computeConnection(xs, ys, xd, yd, index));
    }
}

void ViewDrawingArea::computeConnection()
{
    using namespace graph;

    int index = 0;

    mLines.clear();

    {
        ConnectionList& outs(mCurrent->getInternalInputPortList());
        ConnectionList::const_iterator it;

        for (it = outs.begin(); it != outs.end(); ++it) {
            const ModelPortList& ports(it->second);
            ModelPortList::const_iterator jt ;

            for (jt = ports.begin(); jt != ports.end(); ++jt) {
                computeConnection(mCurrent, it->first, jt->first, jt->second,
                                  index);
                ++index;
            }
        }
    }

    {
        const ModelList& children(mCurrent->getModelList());
        ModelList::const_iterator it;

        for (it = children.begin(); it != children.end(); ++it) {
            const ConnectionList& outs(it->second->getOutputPortList());
            ConnectionList::const_iterator jt;

            for (jt = outs.begin(); jt != outs.end(); ++jt) {
                const ModelPortList&  ports(jt->second);
                ModelPortList::const_iterator kt;

                for (kt = ports.begin(); kt != ports.end(); ++kt) {
                    computeConnection(it->second, jt->first,
                                      kt->first, kt->second,
                                      index);
                    ++index;
                }
            }
        }
    }
}

void ViewDrawingArea::drawConnection()
{
    preComputeConnection();
    computeConnection();
    drawLines();
}

void ViewDrawingArea::drawLines()
{
    std::vector < StraightLine >::const_iterator itl = mLines.begin();
    int i =0;

    while (itl != mLines.end()) {
        if (i != mHighlightLine) {
	    mContext->set_line_join(Cairo::LINE_JOIN_ROUND);
	    setColor(Settings::settings().getConnectionColor());
        }
	mContext->move_to(itl->begin()->first + mOffset,
                          itl->begin()->second + mOffset);
	std::vector <Point>::const_iterator iter = itl->begin();
	while (iter != itl->end()) {
	    mContext->line_to(iter->first + mOffset, iter->second + mOffset);
	    ++iter;
	}

	mContext->stroke();
        ++i;
        ++itl;
    }
}

void ViewDrawingArea::drawHighlightConnection()
{
    if (mHighlightLine != -1) {

	mContext->set_line_width(Settings::settings().getLineWidth());
	mContext->set_line_cap(Cairo::LINE_CAP_ROUND);
	mContext->set_line_join(Cairo::LINE_JOIN_ROUND);

	Color color(0.41, 0.34, 0.35);
	std::vector <Point>::const_iterator iter =
            mLines[mHighlightLine].begin();
	mContext->move_to(iter->first + mOffset, iter->second + mOffset);
	while (iter != mLines[mHighlightLine].end()) {
	    mContext->line_to(iter->first + mOffset, iter->second + mOffset);
	    ++iter;
	}
	mContext->stroke();

	mContext->set_line_width(Settings::settings().getLineWidth());
	mContext->select_font_face(Settings::settings().getFont(),
				   Cairo::FONT_SLANT_NORMAL,
				   Cairo::FONT_WEIGHT_NORMAL);
	mContext->set_font_size(Settings::settings().getFontSize());

	Cairo::TextExtents textExtents;
	mContext->get_text_extents(mText[mHighlightLine],textExtents);

        {
            Gdk::Color c;

            c.set_rgb_p(1.0, 1.0, 0.25);
            setColor(c);
        }

	mContext->rectangle(mMouse.get_x(),
			    (mMouse.get_y() - textExtents.height - 6),
			    (textExtents.width + 5),
			    (textExtents.height + 5));
	mContext->fill();
	mContext->stroke();

	setColor(Settings::settings().getForegroundColor());
	mContext->rectangle((mMouse.get_x()),
			    (mMouse.get_y() - textExtents.height - 6),
			    (textExtents.width + 5),
			    (textExtents.height + 5));

	mContext->move_to((mMouse.get_x() + 2),
			  (mMouse.get_y() - textExtents.height + 5));
	mContext->show_text(mText[mHighlightLine]);

	mContext->stroke();
    }
}

void ViewDrawingArea::drawChildrenModels()
{
    const graph::ModelList& children = mCurrent->getModelList();
    graph::ModelList::const_iterator it = children.begin();
    while (it != children.end()) {
        graph::Model* model = it->second;

        if (model->isCoupled()) {
            model->setWidth(ViewDrawingArea::MODEL_WIDTH);
            model->setHeight(MODEL_HEIGHT +
                             std::max(model->getInputPortNumber(),
                                      model->getOutputPortNumber())
                             * (MODEL_SPACING_PORT + MODEL_PORT));
        }

        if (mView->existInSelectedModels(model)) {
            drawChildrenModel(model, Settings::settings().getSelectedColor());
        } else {
            if (model->isAtomic()) {
                drawChildrenModel(model, Settings::settings().getAtomicColor());
            } else {
                drawChildrenModel(model,
                                  Settings::settings().getCoupledColor());
            }
        }
        ++it;
    }
    if (mView->getDestinationModel() != NULL and
	mView->getDestinationModel() != mCurrent) {
        drawChildrenModel(mView->getDestinationModel(),
			  Settings::settings().getAtomicColor());
    }
}

void ViewDrawingArea::drawChildrenModel(graph::Model* model,
                                        const Gdk::Color& color)
{
    setColor(color);
    mContext->rectangle(model->x() + mOffset,
			(model->y()) + mOffset,
			(model->width()),
			(model->height()));
    mContext->stroke();

    drawChildrenPorts(model, color);
}

void ViewDrawingArea::drawChildrenPorts(graph::Model* model,
                                        const Gdk::Color& color)
{
    const graph::ConnectionList ipl =  model->getInputPortList();
    const graph::ConnectionList opl =  model->getOutputPortList();
    graph::ConnectionList::const_iterator itl;

    const size_t maxInput = model->getInputPortList().size();
    const size_t maxOutput = model->getOutputPortList().size();
    const size_t stepInput = model->height() / (maxInput + 1);
    const size_t stepOutput = model->height() / (maxOutput + 1);
    const int    mX = model->x();
    const int    mY = model->y();

    mContext->select_font_face(Settings::settings().getFont(),
			       Cairo::FONT_SLANT_OBLIQUE,
			       Cairo::FONT_WEIGHT_NORMAL);
    mContext->set_font_size(Settings::settings().getFontSize());

    itl = ipl.begin();

    for (size_t i = 0; i < maxInput; ++i) {

        // to draw the port
	setColor(color);
	mContext->move_to(mX,
			  (mY + stepInput * (i + 1) -
			   MODEL_SPACING_PORT));
	mContext->line_to((mX),
			  (mY + stepInput * (i + 1) +
			   MODEL_SPACING_PORT));
	mContext->line_to((mX + MODEL_SPACING_PORT),
			  (mY + stepInput * (i + 1)));
	mContext->line_to(mX,
			  (mY + stepInput * (i + 1) -
					 MODEL_SPACING_PORT));
	mContext->fill();
	mContext->stroke();

	// to draw the label of the port
	if (mZoom >= 1.0) {
	    mContext->move_to((mX + PORT_SPACING_LABEL),
			      (mY + stepInput * (i + 1) + 10));
	    mContext->show_text(itl->first);
	    mContext->stroke();
	}
	itl++;
    }

    itl = opl.begin();

    for (size_t i = 0; i < maxOutput; ++i) {

        // to draw the port
	setColor(color);
	mContext->move_to((mX + model->width()),
			  (mY + stepOutput * (i + 1) -
			   MODEL_SPACING_PORT));
	mContext->line_to((mX + model->width()),
			  (mY + stepOutput * (i + 1) +
			   MODEL_SPACING_PORT));
	mContext->line_to((mX + MODEL_SPACING_PORT +
			   model->width()),
			  (mY + stepOutput * (i + 1)));
	mContext->line_to((mX + model->width()),
			  (mY + stepOutput * (i + 1) -
			    MODEL_SPACING_PORT));
	mContext->fill();
	mContext->stroke();

	// to draw the label of the port
	if (mZoom >= 1.0) {
	    setColor(color);
	    mContext->move_to((mX + model->width() +
			     PORT_SPACING_LABEL),
			    (mY + stepOutput * (i + 1) + 10));
	    mContext->show_text(itl->first);
	    mContext->stroke();
	}
	itl++;
    }

    mContext->select_font_face(Settings::settings().getFont(),
			       Cairo::FONT_SLANT_NORMAL,
			       Cairo::FONT_WEIGHT_NORMAL);
    mContext->set_font_size(Settings::settings().getFontSize());

    if (mZoom >= 1.0) {
        Cairo::TextExtents textExtents;
        mContext->get_text_extents(model->getName(), textExtents);
        mContext->move_to((model->x() + (model->width() / 2) -
                           (textExtents.width / 2)),
			  (model->y() + model->height() +
			   MODEL_SPACING_PORT) + 10);
        mContext->show_text(model->getName());
    }
}

void ViewDrawingArea::drawLink()
{
    if (mView->getCurrentButton() == GVLE::ADDLINK and
        mView->isEmptySelectedModels() == false) {
        graph::Model* src = mView->getFirstSelectedModels();
	setColor(Settings::settings().getForegroundColor());
        if (src == mCurrent) {
	    mContext->move_to(MODEL_PORT,
			      (mHeight / 2));
	    mContext->line_to(mMouse.get_x(),
			      mMouse.get_y());

        } else {
            int w = src->width();
            int h = src->height();
            int x = src->x();
            int y = src->y();

	    mContext->move_to((x + w / 2) + mOffset,
			      (y + h / 2) + mOffset);
	    mContext->line_to(mMouse.get_x() + mOffset,
			      mMouse.get_y() + mOffset);
        }
	mContext->stroke();
    }
}

void ViewDrawingArea::drawZoomFrame()
{
    if ((mView->getCurrentButton() == GVLE::ZOOM
	 or mView->getCurrentButton() == GVLE::POINTER)
	 and mPrecMouse.get_x() != -1 and mPrecMouse.get_y() != -1) {
        int xmin = std::min(mMouse.get_x(), mPrecMouse.get_x());
        int xmax = std::max(mMouse.get_x(), mPrecMouse.get_x());
        int ymin = std::min(mMouse.get_y(), mPrecMouse.get_y());
        int ymax = std::max(mMouse.get_y(), mPrecMouse.get_y());

	setColor(Settings::settings().getForegroundColor());
	mContext->rectangle(xmin,
			    ymin,
			    (xmax - xmin),
			    (ymax - ymin));
	mContext->stroke();
    }
}

void ViewDrawingArea::highlightLine(int mx, int my)
{
    std::vector < StraightLine >::const_iterator itl = mLines.begin();
    bool found = false;
    int i = 0;

    while (itl != mLines.end() and not found) {
        int xs2, ys2;
        StraightLine::const_iterator it = itl->begin();

        xs2 = it->first;
        ys2 = it->second;
        ++it;
        while (it != itl->end() and not found) {
            int xs, ys, xd, yd;

            if (xs2 == it->first) {
                xs = xs2 - 5;
                xd = it->first + 5;
            } else {
                xs = xs2;
                xd = it->first;
            }

            if (ys2 == it->second) {
                ys = ys2 - 5;
                yd = it->second + 5;
            } else {
                ys = ys2;
                yd = it->second;
            }

            double h = -1;
            if (std::min(xs, xd) <= mx and mx <= std::max(xs, xd)
                and std::min(ys, yd) <= my and my <= std::max(ys, yd)) {
                const double a = (ys - yd) / (double)(xs - xd);
                const double b = ys - a * xs;
                h = std::abs((my - (a * mx) - b) / std::sqrt(1 + a * a));
                if (h <= 10) {
                    found = true;
                    mHighlightLine = i;
                }
            }
            xs2 = it->first;
            ys2 = it->second;
            ++it;
        }
        ++itl;
        ++i;
	}
    if (found) {
        queueRedraw();
    } else {
        if (mHighlightLine != -1) {
            mHighlightLine = -1;
            queueRedraw();
        }
    }
}

bool ViewDrawingArea::on_button_press_event(GdkEventButton* event)
{
    GVLE::ButtonType currentbutton = mView->getCurrentButton();
    mMouse.set_x((int)(event->x / mZoom));
    mMouse.set_y((int)(event->y / mZoom));
    bool shiftOrControl = (event->state & GDK_SHIFT_MASK) or(event->state &
                                                             GDK_CONTROL_MASK);
    graph::Model* model = mCurrent->find(mMouse.get_x(), mMouse.get_y());

    switch (currentbutton) {
    case GVLE::POINTER:
        if (event->type == GDK_BUTTON_PRESS and event->button == 1) {
            on_gvlepointer_button_1(model, shiftOrControl);
            queueRedraw();
        } else if (event->type == GDK_2BUTTON_PRESS and event->button == 1) {
	    mView->showModel(model);
        } else if (event->button == 2) {
	    mView->showModel(model);
        }
        if (not model) {
            queueRedraw();
        }
        break;
    case GVLE::ADDMODEL:
        mView->addAtomicModel(mMouse.get_x(), mMouse.get_y());
        queueRedraw();
        break;
    case GVLE::ADDLINK:
        if (event->button == 1) {
            addLinkOnButtonPress(mMouse.get_x(), mMouse.get_y());
            mPrecMouse = mMouse;
            queueRedraw();
        }
        break;
    case GVLE::DELETE:
        delUnderMouse(mMouse.get_x(), mMouse.get_y());
        queueRedraw();
        break;
    case GVLE::QUESTION:
        mModeling->showDynamics((model) ? model->getName() :
                                mCurrent->getName());
        break;
    case GVLE::ZOOM:
        if (event->button == 1) {
            mPrecMouse = mMouse;
            queueRedraw();
        }
        queueRedraw();
        break;
    case GVLE::PLUGINMODEL:
        mView->addPluginModel(mMouse.get_x(), mMouse.get_y());
        queueRedraw();
        break;
    case GVLE::ADDCOUPLED:
        if (event->button == 1) {
            mView->addModelInListModel(model, shiftOrControl);
        } else if (event->button == 2) {
            mView->addCoupledModel(mMouse.get_x(), mMouse.get_y());
        }
        queueRedraw();
        break;
    default:
        break;
    }
    return true;
}

bool ViewDrawingArea::on_button_release_event(GdkEventButton* event)
{
    mMouse.set_x((int)(event->x / mZoom));
    mMouse.set_y((int)(event->y / mZoom));

    switch (mView->getCurrentButton()) {
    case GVLE::POINTER:
	if (event->button == 1) {
	    for(int x = std::min(mMouse.get_x(), mPrecMouse.get_x());
		x <= std::max(mMouse.get_x(), mPrecMouse.get_x());
		++x) {
		for (int y = std::min(mMouse.get_y(), mPrecMouse.get_y());
		     y <= std::max(mMouse.get_y(), mPrecMouse.get_y());
		     ++y) {
		    graph::Model* model = mCurrent->find(x, y);
		    if (model)
			if (not mView->existInSelectedModels(model))
			    mView->addModelToSelectedModels(model);
		}
	    }
	    queueRedraw();
	}
	mPrecMouse.set_x(-1);
	mPrecMouse.set_y(-1);
	break;
    case GVLE::ADDLINK:
        addLinkOnButtonRelease(mMouse.get_x(), mMouse.get_y());
        queueRedraw();
        break;
    case GVLE::ZOOM:
        if (event->button == 1) {
            // to not zoom in case of very small selection.
            if (std::max(mMouse.get_x(), mPrecMouse.get_x()) -
                std::min(mMouse.get_x(), mPrecMouse.get_x()) > 5 &&
                std::max(mMouse.get_y(), mPrecMouse.get_y()) -
                std::min(mMouse.get_y(), mPrecMouse.get_y()) > 5) {
                selectZoom(std::min(mMouse.get_x(), mPrecMouse.get_x()),
                           std::min(mMouse.get_y(), mPrecMouse.get_y()),
                           std::max(mMouse.get_x(), mPrecMouse.get_x()),
                           std::max(mMouse.get_y(), mPrecMouse.get_y()));
                queueRedraw();
            }
            mPrecMouse.set_x(-1);
            mPrecMouse.set_y(-1);

        } else
            onZoom(event->button);
        break;
    default:
        break;
    }

    return true;
}

bool ViewDrawingArea::on_configure_event(GdkEventConfigure* event)
{
    bool change = false;

    if (event->width > mWidth * mZoom) {
        change = true;
        mWidth = event->width;
        mCurrent->setWidth(mWidth);
    }

    if (event->height > mHeight * mZoom) {
        change = true;
        mHeight = event->height;
        mCurrent->setHeight(mHeight);
    }

    if (mRectWidth > mWidth * mZoom) {
        change = true;
        mWidth = mRectWidth;
        mCurrent->setWidth(mWidth);
    }

    if (mRectHeight > mHeight * mZoom) {
        change = true;
        mHeight = mRectHeight;
        mCurrent->setHeight(mHeight);
    }

    if (change and mIsRealized) {
	set_size_request(mRectWidth * mZoom, mRectHeight * mZoom);
        mBuffer = Gdk::Pixmap::create(mWin, (int)(mWidth * mZoom),
                                      (int)(mHeight * mZoom), -1);
        queueRedraw();
    }
    return true;
}

bool ViewDrawingArea::on_expose_event(GdkEventExpose*)
{
    if (mIsRealized) {
        if (!mBuffer) {
            mBuffer = Gdk::Pixmap::create(mWin, (int)(mWidth * mZoom),
                                          (int)(mHeight * mZoom), -1);
        }
        if (mBuffer) {
            if (mNeedRedraw) {
		mContext = mBuffer->create_cairo_context();
		mContext->set_line_width(Settings::settings().getLineWidth());
		mOffset = (Settings::settings().getLineWidth() < 1.1)
                    ? 0.5 : 0.0;
                draw();
                mNeedRedraw = false;
            }
            mWin->draw_drawable(mWingc, mBuffer, 0, 0, 0, 0, -1, -1);
	}
    }
    return true;
}

bool ViewDrawingArea::on_motion_notify_event(GdkEventMotion* event)
{
    mMouse.set_x((int)(event->x / mZoom));
    mMouse.set_y((int)(event->y / mZoom));
    int button;

    if (event->state & GDK_BUTTON1_MASK) {
        button = 1;
    } else if (event->state & GDK_BUTTON2_MASK) {
        button = 2;
    } else {
        button = 0;
    }

    switch (mView->getCurrentButton()) {
    case GVLE::POINTER :
        if (button == 1) {
	    if (not mView->isEmptySelectedModels()) {
                mView->displaceModel(
                    mPrecMouse.get_x() == -1 ? 0 : mPrecMouse.get_x(),
                    mPrecMouse.get_y() == -1 ? 0 : mPrecMouse.get_y(),
                    mMouse.get_x(), mMouse.get_y());
		mPrecMouse = mMouse;
		queueRedraw();
	    } else {
		queueRedraw();
	    }
        } else {
            highlightLine((int)mMouse.get_x(), (int)mMouse.get_y());
	    mPrecMouse = mMouse;
        }
        break;
    case GVLE::ZOOM:
        if (button == 1) {
            queueRedraw();
        }
        break;
    case GVLE::ADDLINK :
        if (button == 1) {
            addLinkOnMotion((int)mMouse.get_x(), (int)mMouse.get_y());
            queueRedraw();
        }
        mPrecMouse = mMouse;
        break;
    case GVLE::DELETE:
        highlightLine((int)event->x, (int)event->y);
        mPrecMouse = mMouse;
        break;
    default:
        break;
    }
    return true;
}

void ViewDrawingArea::on_realize()
{
    Gtk::DrawingArea::on_realize();
    mWin = get_window();
    assert(mWin);
    mWingc = Gdk::GC::create(mWin);
    mIsRealized = true;
    queueRedraw();
}

void ViewDrawingArea::on_gvlepointer_button_1(graph::Model* mdl,
                                              bool state)
{
    if (mdl) {
        if (mView->existInSelectedModels(mdl) == false) {
            if (state == false) {
                mView->clearSelectedModels();
            }
            mView->addModelToSelectedModels(mdl);
	}
    } else {
	mView->clearSelectedModels();
	mPrecMouse = mMouse;
	queueRedraw();
    }
    queueRedraw();
}

void ViewDrawingArea::delUnderMouse(int x, int y)
{
    graph::Model* model = mCurrent->find(x, y);
    if (model) {
        mView->delModel(model);
    } else {
        delConnection();
	mModeling->setModified(true);
    }

    queueRedraw();
}

void ViewDrawingArea::getCurrentModelInPosition(const std::string& port, int& x,
                                                int& y)
{
    x = 2 * MODEL_PORT;

    y = ((int)mRectHeight / (mCurrent->getInputPortList().size() + 1)) *
    (mCurrent->getInputPortIndex(port) + 1);

}

void ViewDrawingArea::getCurrentModelOutPosition(const std::string& port, int&
                                                 x, int& y)
{
    x = (int)mRectWidth - MODEL_PORT;

    y = ((int)mRectHeight / (mCurrent->getOutputPortNumber() + 1)) *
    (mCurrent->getOutputPortIndex(port) + 1);
}

void ViewDrawingArea::getModelInPosition(graph::Model* model,
                                         const std::string& port,
                                         int& x, int& y)
{
    x = model->x();
    y = model->y() + (model->height() / (model->getInputPortNumber() + 1)) *
        (model->getInputPortIndex(port) + 1);
}

void ViewDrawingArea::getModelOutPosition(graph::Model* model,
                                          const std::string& port,
                                          int& x, int& y)
{
    x = model->x() + model->width();
    y = model->y() + (model->height() / (model->getOutputPortNumber() + 1)) *
        (model->getOutputPortIndex(port) + 1);
}

void ViewDrawingArea::calcRectSize()
{
    if ((mCurrent->getModelList()).size() != 0) {
        const graph::ModelList& children = mCurrent->getModelList();
        graph::ModelList::const_iterator it;
        Cairo::TextExtents textExtents;

        int xMax = 0;
        int yMax = 0;

        for (it = children.begin() ; it != children.end() ; ++it) {
	    if (it->second->x() + it->second->width() > xMax) {
	        xMax = it->second->x() + it->second->width();
            }
            if ((it->second->y() + it->second->height()) > yMax) {
		yMax = it->second->y() + it->second->height();
            }

            const graph::ConnectionList& opl =
	        it->second->getOutputPortList();

            graph::ConnectionList::const_iterator itl;

	    for (itl = opl.begin() ; itl != opl.end() ; ++itl) {
	        mContext->get_text_extents(itl->first, textExtents);
		if (it->second->x() +
		    it->second->width() +
		    textExtents.width +
		    MODEL_PORT
		    > xMax) {
	            xMax = it->second->x() +
		            it->second->width() +
		            textExtents.width + MODEL_PORT;
	        }
	    }
	}
        mRectWidth = xMax + mOffset + 15;
        mRectHeight = yMax + mOffset + SPACING_MODEL + 15;
    }
}

void ViewDrawingArea::addLinkOnButtonPress(int x, int y)
{
    if (not mView->isEmptySelectedModels()) {
        mView->clearSelectedModels();
    }

    graph::Model* mdl = mCurrent->find(x, y);
    if (mdl) {
        mView->addModelToSelectedModels(mdl);
    } else {
        mView->addModelToSelectedModels(mCurrent);
    }
    queueRedraw();
}

void ViewDrawingArea::addLinkOnMotion(int x, int y)
{
    if (mView->isEmptySelectedModels() == false) {
        graph::Model* mdl = mCurrent->find(x, y);
        if (mdl == mView->getFirstSelectedModels()) {
            mMouse.set_x(mdl->x() + mdl->width() / 2);
            mMouse.set_y(mdl->y() + mdl->height() / 2);
            mView->addDestinationModel(NULL);
        } else if (mdl == NULL) {
            mMouse.set_x(x);
            mMouse.set_y(y);
            mView->addDestinationModel(NULL);
        } else {
            mMouse.set_x(mdl->x() + mdl->width() / 2);
            mMouse.set_y(mdl->y() + mdl->height() / 2);
            mView->addDestinationModel(mdl);
        }
        queueRedraw();
    }
}

void ViewDrawingArea::addLinkOnButtonRelease(int x, int y)
{
    if (mView->isEmptySelectedModels() == false) {
        graph::Model* mdl = mCurrent->find(x, y);
        if (mdl == NULL)
            mView->makeConnection(mView->getFirstSelectedModels(), mCurrent);
        else
            mView->makeConnection(mView->getFirstSelectedModels(), mdl);
    }
    mView->clearSelectedModels();
    mView->addDestinationModel(NULL);
    queueRedraw();
}


void ViewDrawingArea::delConnection()
{
    using namespace graph;
    Model* src = 0;
    Model* dst = 0;
    std::string portsrc, portdst;
    bool internal = false;
    bool external = false;
    int i = 0;

    {
        ConnectionList& outs(mCurrent->getInternalInputPortList());
        ConnectionList::const_iterator it;

        for (it = outs.begin(); it != outs.end(); ++it) {
            const ModelPortList& ports(it->second);
            ModelPortList::const_iterator jt ;

            for (jt = ports.begin(); jt != ports.end(); ++jt) {
                if (i == mHighlightLine) {
                    portsrc = it->first;
                    dst = jt->first;
                    portdst = jt->second;
                    internal = true;
                }
                ++i;
            }
        }
    }

    {
        const ModelList& children(mCurrent->getModelList());
        ModelList::const_iterator it;

        for (it = children.begin(); it != children.end(); ++it) {
            const ConnectionList& outs(it->second->getOutputPortList());
            ConnectionList::const_iterator jt;

            for (jt = outs.begin(); jt != outs.end(); ++jt) {
                const ModelPortList&  ports(jt->second);
                ModelPortList::const_iterator kt;

                for (kt = ports.begin(); kt != ports.end(); ++kt) {
                    if (i == mHighlightLine) {
                        src = it->second;
                        portsrc = jt->first;
                        dst = kt->first;
                        portdst = kt->second;
                        external = (dst == mCurrent);
                    }
                    ++i;
                }
            }
        }
    }

    if (src or dst) {
        if (internal) {
            mCurrent->delInputConnection(portsrc, dst, portdst);
        } else if (external) {
            mCurrent->delOutputConnection(src, portsrc, portdst);
        } else {
            mCurrent->delInternalConnection(src, portsrc, dst, portdst);
        }
    }
    mHighlightLine = -1;
}

void ViewDrawingArea::newSize()
{
    int tWidth = (int)(mWidth * mZoom);
    int tHeight = (int)(mHeight * mZoom);
    set_size_request(tWidth, tHeight);
    mBuffer = Gdk::Pixmap::create(mWin, tWidth, tHeight, -1);
    queueRedraw();
}

void ViewDrawingArea::onZoom(int button)
{
    switch (button) {
    case 2:
        addCoefZoom();
        break;
    case 3:
        delCoefZoom();
        break;
    }
    newSize();
}

void ViewDrawingArea::addCoefZoom()
{
    mZoom = (mZoom >= ZOOM_MAX) ? ZOOM_MAX : mZoom + ZOOM_FACTOR_SUP;
    mContext->set_font_size(Settings::settings().getFontSize() * mZoom);
    newSize();
}

void ViewDrawingArea::delCoefZoom()
{
    if (mZoom > 1.0)
        mZoom = mZoom - ZOOM_FACTOR_SUP;
    else
        mZoom = (mZoom <= ZOOM_MIN) ? ZOOM_MIN : mZoom - ZOOM_FACTOR_INF;
    mContext->set_font_size(Settings::settings().getFontSize() * mZoom);
    newSize();
}

void ViewDrawingArea::restoreZoom()
{
    mZoom = 1.0;
    mContext->set_font_size(Settings::settings().getFontSize());
    newSize();
}

void ViewDrawingArea::selectZoom(int xmin, int ymin, int xmax, int ymax)
{
    double xt = (xmax - xmin) / (double)mWidth;
    double yt = (ymax - ymin) / (double)mHeight;
    double z = std::abs(1 - std::max(xt, yt));
    mZoom = 4 * z;
    if (mZoom <= 0.1)
        mZoom = 0.1;
    if (mZoom >= 4.0)
        mZoom = 4.0;
    mContext->set_font_size(Settings::settings().getFontSize());
    newSize();

    mModeling->updateAdjustment(xmin * mZoom, ymin * mZoom);
}

void ViewDrawingArea::setCoefZoom(double coef)
{
    mZoom = coef;
    newSize();
}

void ViewDrawingArea::setColor(const Gdk::Color& color)
{
    mContext->set_source_rgb(color.get_red_p(),
                             color.get_green_p(),
                             color.get_blue_p());
}

void ViewDrawingArea::exportPng(const std::string& filename)
{

    Cairo::RefPtr<Cairo::ImageSurface> surface =
        Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, mWidth, mHeight);
    mContext = Cairo::Context::create(surface);
    mContext->set_line_width(Settings::settings().getLineWidth());
    mContext->select_font_face(Settings::settings().getFont(),
			       Cairo::FONT_SLANT_NORMAL,
			       Cairo::FONT_WEIGHT_NORMAL);
    mContext->set_font_size(Settings::settings().getFontSize());
    draw();
    surface->write_to_png(filename + ".png");
}

void ViewDrawingArea::exportPdf(const std::string& filename)
{
    Cairo::RefPtr<Cairo::PdfSurface> surface =
        Cairo::PdfSurface::create(filename + ".pdf", mWidth, mHeight);
    mContext = Cairo::Context::create(surface);
    mContext->set_line_width(Settings::settings().getLineWidth());
    mContext->select_font_face(Settings::settings().getFont(),
			       Cairo::FONT_SLANT_NORMAL,
			       Cairo::FONT_WEIGHT_NORMAL);
    mContext->set_font_size(Settings::settings().getFontSize());
    draw();
    mContext->show_page();
    surface->finish();
}

void ViewDrawingArea::exportSvg(const std::string& filename)
{
    Cairo::RefPtr<Cairo::SvgSurface> surface =
        Cairo::SvgSurface::create(filename + ".svg", mWidth, mHeight);

    mContext = Cairo::Context::create(surface);
    mContext->set_line_width(Settings::settings().getLineWidth());
    mContext->select_font_face(Settings::settings().getFont(),
			       Cairo::FONT_SLANT_NORMAL,
			       Cairo::FONT_WEIGHT_NORMAL);
    mContext->set_font_size(Settings::settings().getFontSize());
    draw();
    mContext->show_page();
    surface->finish();
}

void ViewDrawingArea::setUndefinedModels()
{
    graph::ModelList::const_iterator it = mCurrent->getModelList().begin();
    bool found = false;

    while (it != mCurrent->getModelList().end()) {
	if (it->second->width() <= 0) {
	    found = true;
	    it->second->setWidth(100);
	    if (mMaxWidth <= 0) {
		mMaxWidth = 100;
	    }
	}
	if (it->second->height() <= 0) {
	    found = true;
	    it->second->setHeight(MODEL_HEIGHT +
				  std::max(it->second->getInputPortNumber(),
					   it->second->getOutputPortNumber()) *
				  (MODEL_SPACING_PORT + MODEL_PORT));
	    if (mMaxHeight <= 0 or mMaxHeight < it->second->height()) {
		mMaxHeight = it->second->height();
	    }
	}
	++it;
    }
    if (found) {
	mRectWidth = (mMaxWidth + SPACING_MODEL) *
	    std::floor(std::sqrt(mCurrent->getModelList().size()) + 1);
	mRectHeight = (mMaxHeight + SPACING_MODEL) *
	    std::floor(std::sqrt(mCurrent->getModelList().size()) + 1);
    }
}

void ViewDrawingArea::onRandomOrder()
{
    mGrid.clear();

    std::string key;
    int x, y;
    int compteur = 0;

    setUndefinedModels();

    mCasesWidth = (mRectWidth - (MODEL_PORT + mOffset)) / (mMaxWidth + 15);
    mCasesHeight = (mRectHeight - (MODEL_PORT + mOffset)) / (mMaxHeight + 15);

    graph::ModelList::const_iterator it = mCurrent->getModelList().begin();

    while (it != mCurrent->getModelList().end()) {
	do {
	    ++compteur;
	    x = mModeling->getRand()->getInt(0, mCasesWidth - 1);
	    y = mModeling->getRand()->getInt(0, mCasesHeight - 1);
	    key = boost::lexical_cast < std::string > (x) + ":" +
		boost::lexical_cast < std::string > (y);
	    if (compteur == 100) {
		mRectWidth += mMaxWidth + 15;
		mRectHeight += mMaxHeight + 15;
		mCasesWidth = (mRectWidth - (MODEL_PORT + mOffset)) /
		    (mMaxWidth + 15);
		mCasesHeight = (mRectHeight - (MODEL_PORT + mOffset)) /
		    (mMaxHeight + 15);
		compteur = 0;
	    }
	} while (std::find(mGrid.begin(), mGrid.end(), key) != mGrid.end());

	compteur = 0;
	mGrid.push_back(key);
	it->second->setX((x * (mMaxWidth + 15)) + MODEL_PORT + mOffset + 15);
	it->second->setY((y * (mMaxHeight + 15)) + MODEL_PORT + mOffset + 15);
	++it;
    }
    queueRedraw();
}

void ViewDrawingArea::maxModelWidthHeight()
{
    mMaxWidth = 0;
    mMaxHeight = 0;

    graph::ModelList::const_iterator it =
	mCurrent->getModelList().begin();
    while(it != mCurrent->getModelList().end()) {
	if (it->second->width() > mMaxWidth)
	    mMaxWidth = it->second->width();
	if (it->second->height() > mMaxHeight)
	    mMaxHeight = it->second->height();
	++it;
    }
}


}} // namespace vle gvle
