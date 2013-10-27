/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <vle/gvle/SimpleViewDrawingArea.hpp>
#include <vle/gvle/Settings.hpp>
#include <vle/gvle/View.hpp>
#include <vle/gvle/GVLE.hpp>

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

namespace vle { namespace gvle {

const gint SimpleViewDrawingArea::MODEL_RADIUS = 22;

SimpleViewDrawingArea::SimpleViewDrawingArea(View* view) :
    ViewDrawingArea(view)
{

}

void SimpleViewDrawingArea::draw()
{
    if (mContext) {
	mContext->save();
	mContext->scale(mZoom, mZoom);
	calcRectSize();
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

void SimpleViewDrawingArea::drawChildrenModel(vpz::BaseModel* model,
					      const Gdk::Color& color)
{
    setColor(color);

    mContext->arc(model->x() + (mOffset + MODEL_RADIUS),
		  model->y() + (mOffset + MODEL_RADIUS),
		  MODEL_RADIUS,
		  0.0,
		  2.0 * M_PI);


    mContext->stroke();

    mContext->select_font_face(Settings::settings().getFont(),
			       Cairo::FONT_SLANT_NORMAL,
			       Cairo::FONT_WEIGHT_NORMAL);
    mContext->set_font_size(Settings::settings().getFontSize());

    if (mZoom >= 1.0) {
        Cairo::TextExtents textExtents;
        mContext->get_text_extents(model->getName(), textExtents);
	mContext->move_to((model->x() + MODEL_RADIUS -
                           (textExtents.width / 2)),
			  model->y() + (MODEL_RADIUS * 2) +
			  MODEL_SPACING_PORT + 10);

	mContext->show_text(model->getName());
	mContext->stroke();

    }
}

void SimpleViewDrawingArea::preComputeConnection(int xs, int ys,
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

void SimpleViewDrawingArea::preComputeConnection(vpz::BaseModel* src,
						 const std::string& portsrc,
						 vpz::BaseModel* dst,
						 const std::string& portdst)
{
    int xs = 0, ys = 0, xd = 0, yd = 0;
    int ytms = src->y();
    int ybms = src->y() + src->height();

    if (src == mCurrent) {
        getCurrentModelInPosition(portsrc, xs, ys);
	getModelInPosition(xs, ys, dst, dst, xd, yd);
        preComputeConnection(xs, ys, xd, yd, ys - SPACING_MODEL,
                             ys + SPACING_MODEL);
    } else if (dst == mCurrent) {
	getCurrentModelOutPosition(portdst, xd, yd);
	getModelOutPosition(src, xd, yd, src, xs, ys);
        preComputeConnection(xs, ys, xd, yd, ytms, ybms);
    } else {
	getModelOutPosition(src, xd, yd, src, xs, ys);
	getModelInPosition(xs, ys, dst, dst, xd, yd);
        preComputeConnection(xs, ys, xd, yd, ytms, ybms);
    }
}

void SimpleViewDrawingArea::computeConnection(vpz::BaseModel* src,
					      const std::string& portsrc,
					      vpz::BaseModel* dst,
					      const std::string& portdst,
					      int index)
{
    int xs, ys, xd, yd;

    if (src == mCurrent) {
        getCurrentModelInPosition(portsrc, xs, ys);
	getModelInPosition(xs, ys, dst, dst, xd, yd);
        mLines.push_back(computeConnection(xs, ys, xd, yd, index));
    } else if (dst == mCurrent) {
        getCurrentModelOutPosition(portdst, xd, yd);
	getModelOutPosition(src, xd, yd, src, xs, ys);
	mLines.push_back(computeConnection(xs, ys, xd, yd, index));
    } else {
	getModelOutPosition(src, dst->x(), dst->y(), src, xs, ys);
	getModelInPosition(src->x(), src->y(), dst, dst, xd, yd);
        mLines.push_back(computeConnection(xs, ys, xd, yd, index));
    }
}

ViewDrawingArea::StraightLine
SimpleViewDrawingArea::computeConnection(int xs, int ys, int xd, int yd,
                                         int /* aindex */)
{
    StraightLine list;

    list.push_back(Point((int)(xs), (int)(ys)));
    list.push_back(Point((int)(xd), (int)(yd)));

    float x = xs + (getNegativeDelta(xs, ys, xd, yd, xd, yd) * (xd - xs));
    float y = ys + (getNegativeDelta(xs, ys, xd, yd, xd, yd) * (yd - ys));

    float x2, y2;

    if (directorCoefficient(xs, ys, xd, yd) >= 0.005 or
	directorCoefficient(xs, ys, xd, yd) <= -0.005) {
	x2 = x + 10;
	y2 = (-1) / directorCoefficient(xs, ys, xd, yd) * x2 + y
	    - ((-1) / directorCoefficient(xs, ys, xd, yd) * x);
    } else {
	x2 = x;
	y2 = x + 1;
    }

    mContext->move_to(xd, yd);
    mContext->line_to(x + (getNegativeDelta(x, y, x2, y2, x, y) * (x2 - x)),
		      y + (getNegativeDelta(x, y, x2, y2, x, y) * (y2 - y)));
    mContext->line_to(x + (getPositiveDelta(x, y, x2, y2, x, y) * (x2 - x)),
		      y + (getPositiveDelta(x, y, x2, y2, x, y) * (y2 - y)));
    mContext->line_to(xd, yd);
    mContext->fill();
    mContext->stroke();

    return list;
}


void SimpleViewDrawingArea::getCurrentModelInPosition(const std::string& port,
						      int& x, int& y)
{
    x = 2 * MODEL_PORT;

    y = ((int)mRectHeight / (mCurrent->getInputPortList().size() + 1)) *
    (mCurrent->getInputPortIndex(port) + 1);

}

void SimpleViewDrawingArea::getCurrentModelOutPosition(const std::string& port,
						       int& x, int& y)
{
    x = (int)mRectWidth - MODEL_PORT;

    y = ((int)mRectHeight / (mCurrent->getOutputPortNumber() + 1)) *
    (mCurrent->getOutputPortIndex(port) + 1);
}

void SimpleViewDrawingArea::getModelInPosition(int xs, int ys,
					       vpz::BaseModel* dst,
					       vpz::BaseModel* center,
					       int&x, int& y)
{
    int a = ((dst->x() - xs) * (dst->x() - xs))
	+ ((dst->y() - ys) * (dst->y() - ys));
    int b = 2 * (((dst->x() - xs) * (xs - center->x()) )
		 + ((dst->y() - ys) * (ys - center->y())));
    int c = ((center->x() + MODEL_RADIUS) * (center->x() + MODEL_RADIUS))
	+ ((center->y() + MODEL_RADIUS) * (center->y() + MODEL_RADIUS))
	+ ((xs + MODEL_RADIUS) * (xs + MODEL_RADIUS))
	+ ((ys + MODEL_RADIUS) * (ys + MODEL_RADIUS))
	- 2 * (((center->x() + MODEL_RADIUS) * (xs + MODEL_RADIUS))
	       + ((center->y() + MODEL_RADIUS) * (ys + MODEL_RADIUS)))
	- (MODEL_RADIUS * MODEL_RADIUS);

    float k = (-b - sqrt((float)((b * b) - (4 * a * c)))) / (2 * (float)(a));
    x = (xs + MODEL_RADIUS) + (k * (dst->x() - xs));
    y = (ys + MODEL_RADIUS) + (k * (dst->y() - ys));

}


void SimpleViewDrawingArea::getModelOutPosition(vpz::BaseModel* src,
						int xd, int yd,
						vpz::BaseModel* center,
						int&x, int& y)
{
    int a = ((xd - src->x()) * (xd - src->x()))
	+ ((yd - src->y()) * (yd - src->y()));
    int b = 2 * (((xd - src->x()) * (src->x() - center->x()) )
		 + ((yd - src->y()) * (src->y() - center->y())));
    int c = ((center->x() + MODEL_RADIUS) * (center->x() + MODEL_RADIUS))
	+ ((center->y() + MODEL_RADIUS) * (center->y() + MODEL_RADIUS))
	+ ((src->x() + MODEL_RADIUS) * (src->x() + MODEL_RADIUS))
	+ ((src->y() + MODEL_RADIUS) * (src->y() + MODEL_RADIUS))
	- 2 * (((center->x() + MODEL_RADIUS) * (src->x() + MODEL_RADIUS))
	       + ((center->y() + MODEL_RADIUS) * (src->y() + MODEL_RADIUS)))
	- (MODEL_RADIUS * MODEL_RADIUS);
    float k = (-b + sqrt((float)((b * b) - (4 * a * c)))) / (2 * (float)(a));
    x = (src->x() + MODEL_RADIUS) + (k * (xd - src->x()));
    y = (src->y() + MODEL_RADIUS) + (k * (yd - src->y()));

}

void SimpleViewDrawingArea::preComputeConnectInfo()
{
    mConnectionInfo.clear();

    {
        vpz::ConnectionList& outs(mCurrent->getInternalInputPortList());
        vpz::ConnectionList::const_iterator it;

        for (it = outs.begin(); it != outs.end(); ++it) {
            const vpz::ModelPortList& ports(it->second);
            vpz::ModelPortList::const_iterator jt ;

            for (jt = ports.begin(); jt != ports.end(); ++jt) {
                record.source = mCurrent;
                record.destination = jt->first;
                mConnectionInfo.push_back(record);
            }
        }
    }

    {
        const vpz::ModelList& children(mCurrent->getModelList());
        vpz::ModelList::const_iterator it;

        for (it = children.begin(); it != children.end(); ++it) {
            const vpz::ConnectionList& outs(it->second->getOutputPortList());
            vpz::ConnectionList::const_iterator jt;

            for (jt = outs.begin(); jt != outs.end(); ++jt) {
                const vpz::ModelPortList&  ports(jt->second);
                vpz::ModelPortList::const_iterator kt;

                for (kt = ports.begin(); kt != ports.end(); ++kt) {
                    record.source = it->second;
                    record.destination = kt->first;
                    mConnectionInfo.push_back(record);
                }
            }
        }
    }
}

std::string SimpleViewDrawingArea::getConnectionInfo(int mHighlightLine)
{
	return mModeling->getIdCardConnection(
								mConnectionInfo[mHighlightLine].source,
								mConnectionInfo[mHighlightLine].destination,
								mCurrent);
}

float SimpleViewDrawingArea::getPositiveDelta(int xs, int ys, int xd, int yd,
					 int xc, int yc)
{
    int a = ((xd - xs) * (xd - xs)) + ((yd - ys) * (yd - ys));
    int b = 2 * (((xd - xs) * (xs - xc) ) + ((yd - ys) * (ys - yc)));
    int c = (xc * xc) + (yc * yc) + (xs * xs) + (ys * ys)
	- 2 * ((xc * xs) + (yc * ys))
	- (MODEL_PORT * MODEL_PORT);
    int delta =  (b * b) - (4 * a * c);
    return (-b + sqrt((float)(delta))) / (2 * a);
}

float SimpleViewDrawingArea::getNegativeDelta(int xs, int ys, int xd, int yd,
					 int xc, int yc)
{
    int a = ((xd - xs) * (xd - xs)) + ((yd - ys) * (yd - ys));
    int b = 2 * (((xd - xs) * (xs - xc) ) + ((yd - ys) * (ys - yc)));
    int c = (xc * xc) + (yc * yc) + (xs * xs) + (ys * ys)
	- 2 * ((xc * xs) + (yc * ys))
	- (MODEL_PORT * MODEL_PORT);
    int delta =  (b * b) - (4 * a * c);
    return (-b - sqrt((float)(delta))) / (2 * a);
}

bool SimpleViewDrawingArea::on_button_press_event(GdkEventButton* event)
{
    GVLE::ButtonType currentbutton = mView->getCurrentButton();
    mMouse.set_x((int)(event->x / mZoom));
    mMouse.set_y((int)(event->y / mZoom));
    bool shiftOrControl = (event->state & GDK_SHIFT_MASK) or(event->state &
                                                             GDK_CONTROL_MASK);
    vpz::BaseModel* model = mCurrent->find(mMouse.get_x(), mMouse.get_y(),
					 2 * MODEL_RADIUS, 2 * MODEL_RADIUS);

    switch (currentbutton) {
    case GVLE::VLE_GVLE_POINTER:
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
    case GVLE::VLE_GVLE_ADDMODEL:
        mView->addAtomicModel(mMouse.get_x(), mMouse.get_y());
        queueRedraw();
        break;
    case GVLE::VLE_GVLE_ADDLINK:
        if (event->button == 1) {
            addLinkOnButtonPress(mMouse.get_x(), mMouse.get_y());
            mPrecMouse = mMouse;
            queueRedraw();
        }
        break;
    case GVLE::VLE_GVLE_DELETE:
        delUnderMouse(mMouse.get_x(), mMouse.get_y());
        queueRedraw();
        break;
    case GVLE::VLE_GVLE_QUESTION:
        mModeling->showDynamics((model) ? model->getName() :
                                mCurrent->getName());
        break;
    case GVLE::VLE_GVLE_ZOOM:
        if (event->button == 1) {
            mPrecMouse = mMouse;
            queueRedraw();
        }
        queueRedraw();
        break;
    case GVLE::VLE_GVLE_PLUGINMODEL:
        mView->addPluginModel(mMouse.get_x(), mMouse.get_y());
        queueRedraw();
        break;
    case GVLE::VLE_GVLE_ADDCOUPLED:
        if (event->button == 1) {
            if (model) {
                mView->addModelInListModel(model, shiftOrControl);
            } else {
                mView->addCoupledModel(mMouse.get_x(), mMouse.get_y());
            }
        } else if (event->button == 3) {
            mView->clearSelectedModels();
        }
        queueRedraw();
        break;
    default:
        break;
    }
    return true;
}

bool SimpleViewDrawingArea::on_button_release_event(GdkEventButton* event)
{
    mMouse.set_x((int)(event->x / mZoom));
    mMouse.set_y((int)(event->y / mZoom));

    switch (mView->getCurrentButton()) {
    case GVLE::VLE_GVLE_POINTER:
	if (event->button == 1) {
	    for(int x = std::min(mMouse.get_x(), mPrecMouse.get_x());
		x <= std::max(mMouse.get_x(), mPrecMouse.get_x());
		++x) {
		for (int y = std::min(mMouse.get_y(), mPrecMouse.get_y());
		     y <= std::max(mMouse.get_y(), mPrecMouse.get_y());
		     ++y) {
		    vpz::BaseModel* model = mCurrent->find(x, y,
							 2 * MODEL_RADIUS,
							 2 * MODEL_RADIUS);
		    if (model)
			if (not mView->existInSelectedModels(model))
			    mView->addModelToSelectedModels(model);
		}
	    }
	    queueRedraw();
        } else if (event->button == 3) {
            mView->clearSelectedModels();
	    queueRedraw();
        }

        if (mView->getAllSelectedModels().size() == 1) {
            vpz::BaseModel* mod = mView->getFirstSelectedModels();
            if (mView->isClassView()) {
                mGVLE->getModelTreeBox()->selectNone();
                mGVLE->getModelClassBox()->showRow(mod);
            } else{
                mGVLE->getModelClassBox()->selectNone();
                mGVLE->getModelTreeBox()->showRow(mod);
            }
        }
	mPrecMouse.set_x(-1);
	mPrecMouse.set_y(-1);
	break;
    case GVLE::VLE_GVLE_ADDLINK:
        addLinkOnButtonRelease(mMouse.get_x(), mMouse.get_y());
        queueRedraw();
        break;
    case GVLE::VLE_GVLE_ZOOM:
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


void SimpleViewDrawingArea::onOrder()
{
    mCurrent->order();
    queueRedraw();
}

}} // namespace vle gvle
