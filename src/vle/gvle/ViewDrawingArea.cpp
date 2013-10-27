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

#include <vle/gvle/GVLEMenuAndToolbar.hpp>
#include <vle/gvle/Settings.hpp>
#include <vle/gvle/ViewDrawingArea.hpp>
#include <vle/gvle/View.hpp>
#include <vle/gvle/GVLE.hpp>
#include <vector>
#include <cmath>
#include <cassert>
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


ViewDrawingArea::ViewDrawingArea(View* view)
    : mMouse(-1, -1), mPrecMouse(-1, -1), mHeight(300), mWidth(450),
    mRectHeight(300), mRectWidth(450), mZoom(1.0), 
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
    mGVLE = view->getGVLE();

    set_has_tooltip();

    m_cntSignalQueryTooltip = signal_query_tooltip().connect(
        sigc::mem_fun(*this, &ViewDrawingArea::onQueryTooltip));
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
    const vpz::ConnectionList ipl =  mCurrent->getInputPortList();
    const vpz::ConnectionList opl =  mCurrent->getOutputPortList();
    vpz::ConnectionList::const_iterator itl;

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

void ViewDrawingArea::preComputeConnection()
{
    mConnections.clear();
    mDirect.clear();
    mInPts.clear();
    mOutPts.clear();

    {
        vpz::ConnectionList& outs(mCurrent->getInternalInputPortList());
        vpz::ConnectionList::const_iterator it;

        for (it = outs.begin(); it != outs.end(); ++it) {
            const vpz::ModelPortList& ports(it->second);
            vpz::ModelPortList::const_iterator jt ;

            for (jt = ports.begin(); jt != ports.end(); ++jt) {
                preComputeConnection(mCurrent, it->first,
                                     jt->first, jt->second);
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
                    preComputeConnection(it->second, jt->first,
                                         kt->first, kt->second);
                }
            }
        }
    }
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

void ViewDrawingArea::computeConnection()
{
    int index = 0;

    mLines.clear();

    {
        vpz::ConnectionList& outs(mCurrent->getInternalInputPortList());
        vpz::ConnectionList::const_iterator it;

        for (it = outs.begin(); it != outs.end(); ++it) {
            const vpz::ModelPortList& ports(it->second);
            vpz::ModelPortList::const_iterator jt ;

            for (jt = ports.begin(); jt != ports.end(); ++jt) {
                computeConnection(mCurrent, it->first, jt->first, jt->second,
                                  index);
                ++index;
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
                    computeConnection(it->second, jt->first,
                                      kt->first, kt->second,
                                      index);
                    ++index;
                }
            }
        }
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
    }
}

void ViewDrawingArea::drawChildrenModels()
{
    const vpz::ModelList& children = mCurrent->getModelList();
    vpz::ModelList::const_iterator it = children.begin();
    while (it != children.end()) {
        vpz::BaseModel* model = it->second;

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




void ViewDrawingArea::drawLink()
{
    if (mView->getCurrentButton() == GVLE::VLE_GVLE_ADDLINK and
        mView->isEmptySelectedModels() == false) {
        vpz::BaseModel* src = mView->getFirstSelectedModels();
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
    if ((mView->getCurrentButton() == GVLE::VLE_GVLE_ZOOM
         or mView->getCurrentButton() == GVLE::VLE_GVLE_POINTER)
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

    if (change) {
        set_size_request(mRectWidth * mZoom, mRectHeight * mZoom);
        queueRedraw();
    }
    return true;
}

void ViewDrawingArea::queueRedraw()
{ 
	queue_draw(); 
}

bool ViewDrawingArea::on_draw(const Cairo::RefPtr<Cairo::Context>& context)
{
    mContext = context;
    mContext->set_line_width(Settings::settings().getLineWidth());
    mOffset = (Settings::settings().getLineWidth() < 1.1)
        ? 0.5 : 0.0;
    draw();
    
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
    case GVLE::VLE_GVLE_POINTER :
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
    case GVLE::VLE_GVLE_ZOOM:
        if (button == 1) {
            queueRedraw();
        }
        break;
    case GVLE::VLE_GVLE_ADDLINK :
        if (button == 1) {
            addLinkOnMotion((int)mMouse.get_x(), (int)mMouse.get_y());
            queueRedraw();
        }
        mPrecMouse = mMouse;
        break;
    case GVLE::VLE_GVLE_DELETE:
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
    
    newSize();
    queueRedraw();
}

void ViewDrawingArea::on_gvlepointer_button_1(vpz::BaseModel* mdl,
                                              bool state)
{
    if (mdl) {
        if (mView->existInSelectedModels(mdl) == false) {
            if (state == false) {
                mView->clearSelectedModels();
                if (mView->isClassView()) {
                    mGVLE->getModelTreeBox()->selectNone();
                    mGVLE->getModelClassBox()->showRow(mdl);
                } else{
                    mGVLE->getModelClassBox()->selectNone();
                    mGVLE->getModelTreeBox()->showRow(mdl);
                }
            } else {
                vpz::BaseModel* mod = mView->getFirstSelectedModels();
                if (mod == 0) {
                    if (mView->isClassView()) {
                        mGVLE->getModelTreeBox()->selectNone();
                        mGVLE->getModelClassBox()->showRow(mdl);
                    } else{
                        mGVLE->getModelClassBox()->selectNone();
                        mGVLE->getModelTreeBox()->showRow(mdl);
                    }
                } else if (mView->getAllSelectedModels().size() == 1) {
                    if (mView->isClassView()) {
                        mGVLE->getModelTreeBox()->selectNone();
                        mGVLE->getModelClassBox()->selectNone();
                    } else {
                        mGVLE->getModelClassBox()->selectNone();
                        mGVLE->getModelTreeBox()->selectNone();
                    }
                    mView->addModelToSelectedModels(mod);
                }
            }
            mGVLE->getMenu()->showCopyCut();
            mView->addModelToSelectedModels(mdl);
        }
    } else {
        if (mView->isClassView()) {
        } else {
            mGVLE->getModelTreeBox()->selectNone();
        }
        mGVLE->getMenu()->hideCopyCut();
        mView->clearSelectedModels();
        mPrecMouse = mMouse;
        queueRedraw();
    }
    queueRedraw();
}

void ViewDrawingArea::delUnderMouse(int x, int y)
{
    vpz::BaseModel* model = mCurrent->find(x, y);
    if (model) {
        mView->delModel(model);
    } else {
        delConnection();
        mGVLE->setModified(true);
    }

    mGVLE->getMenu()->showCopyCut();
    queueRedraw();
}

void ViewDrawingArea::calcRectSize()
{
    if ((mCurrent->getModelList()).size() != 0) {
        const vpz::ModelList& children = mCurrent->getModelList();
        vpz::ModelList::const_iterator it;
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

            const vpz::ConnectionList& opl =
        it->second->getOutputPortList();

            vpz::ConnectionList::const_iterator itl;

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

    vpz::BaseModel* mdl = mCurrent->find(x, y);
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
        vpz::BaseModel* mdl = mCurrent->find(x, y);
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
        vpz::BaseModel* mdl = mCurrent->find(x, y);
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
    vpz::BaseModel* src = 0;
    vpz::BaseModel* dst = 0;
    std::string portsrc, portdst;
    bool internal = false;
    bool external = false;
    int i = 0;

    {
        vpz::ConnectionList& outs(mCurrent->getInternalInputPortList());
        vpz::ConnectionList::const_iterator it;

        for (it = outs.begin(); it != outs.end(); ++it) {
            const vpz::ModelPortList& ports(it->second);
            vpz::ModelPortList::const_iterator jt ;

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
        const vpz::ModelList& children(mCurrent->getModelList());
        vpz::ModelList::const_iterator it;

        for (it = children.begin(); it != children.end(); ++it) {
            const vpz::ConnectionList& outs(it->second->getOutputPortList());
            vpz::ConnectionList::const_iterator jt;

            for (jt = outs.begin(); jt != outs.end(); ++jt) {
                const vpz::ModelPortList&  ports(jt->second);
                vpz::ModelPortList::const_iterator kt;

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

    mGVLE->updateAdjustment(xmin * mZoom, ymin * mZoom);
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

bool ViewDrawingArea::onQueryTooltip(int wx,int wy, bool,  // keyboard_tooltip
                                     const Glib::RefPtr<Gtk::Tooltip>& tooltip)
{
    vpz::BaseModel* model = mCurrent->find(wx/mZoom, wy/mZoom);
    Glib::ustring card;

    if (mHighlightLine != -1) {
        card = getConnectionInfo(mHighlightLine);
        tooltip->set_markup(card);
        return true;
    } else if (model) {
        if (mView->isClassView()) {
            card = mModeling->getClassIdCard(model);
        } else {
            card = mModeling->getIdCard(model);
        }
        tooltip->set_markup(card);
        return true;
    } else {
        return false;
    }
}

void ViewDrawingArea::setUndefinedModels()
{
    vpz::ModelList::const_iterator it = mCurrent->getModelList().begin();
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

void ViewDrawingArea::drawConnection()
{
    preComputeConnection();
    preComputeConnectInfo();
    computeConnection();
    drawLines();
}

void ViewDrawingArea::maxModelWidthHeight()
{
    mMaxWidth = 0;
    mMaxHeight = 0;

    vpz::ModelList::const_iterator it =
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
