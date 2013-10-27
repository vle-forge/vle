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


#include <vle/gvle/CompleteViewDrawingArea.hpp>
#include <vle/gvle/GVLE.hpp>
#include <vle/gvle/Settings.hpp>
#include <vle/gvle/View.hpp>
#include <vle/gvle/ViewDrawingArea.hpp>
#include <boost/lexical_cast.hpp>
#include <cairomm/scaledfont.h>

namespace vle { namespace gvle {

CompleteViewDrawingArea::CompleteViewDrawingArea(View* view) :
    ViewDrawingArea(view)
{
}

void CompleteViewDrawingArea::draw()
{
    if (mContext) {
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

void CompleteViewDrawingArea::preComputeConnection(int xs, int ys,
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

void CompleteViewDrawingArea::preComputeConnection(vpz::BaseModel* src,
                                           const std::string& portsrc,
                                           vpz::BaseModel* dst,
                                           const std::string& portdst)
{
    int xs, ys, xd, yd;
    int ytms = src->y();
    int ybms = src->y() + src->height();

    if (src == mCurrent) {
        getCurrentModelInPosition(portsrc, xs, ys);
        getModelInPosition(dst, portdst, xd, yd);
        preComputeConnection(xs, ys, xd, yd, ys - SPACING_MODEL,
                             ys + SPACING_MODEL);
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




ViewDrawingArea::StraightLine CompleteViewDrawingArea::computeConnection(
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

void CompleteViewDrawingArea::computeConnection(vpz::BaseModel* src,
                                        const std::string& portsrc,
                                        vpz::BaseModel* dst,
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

void CompleteViewDrawingArea::drawChildrenModel(vpz::BaseModel* model,
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

void CompleteViewDrawingArea::drawChildrenPorts(vpz::BaseModel* model,
                                        const Gdk::Color& color)
{
    const vpz::ConnectionList ipl =  model->getInputPortList();
    const vpz::ConnectionList opl =  model->getOutputPortList();
    vpz::ConnectionList::const_iterator itl;

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

void CompleteViewDrawingArea::getCurrentModelInPosition(const std::string& port, int& x,
                                                int& y)
{
    x = 2 * MODEL_PORT;

    y = ((int)mRectHeight / (mCurrent->getInputPortList().size() + 1)) *
    (mCurrent->getInputPortIndex(port) + 1);

}

void CompleteViewDrawingArea::getCurrentModelOutPosition(const std::string& port, int&
                                                 x, int& y)
{
    x = (int)mRectWidth - MODEL_PORT;

    y = ((int)mRectHeight / (mCurrent->getOutputPortNumber() + 1)) *
    (mCurrent->getOutputPortIndex(port) + 1);
}

void CompleteViewDrawingArea::getModelInPosition(vpz::BaseModel* model,
                                         const std::string& port,
                                         int& x, int& y)
{
    x = model->x();
    y = model->y() + (model->height() / (model->getInputPortNumber() + 1)) *
        (model->getInputPortIndex(port) + 1);
}

void CompleteViewDrawingArea::getModelOutPosition(vpz::BaseModel* model,
                                          const std::string& port,
                                          int& x, int& y)
{
    x = model->x() + model->width();
    y = model->y() + (model->height() / (model->getOutputPortNumber() + 1)) *
        (model->getOutputPortIndex(port) + 1);
}

bool CompleteViewDrawingArea::on_button_press_event(GdkEventButton* event)
{
    GVLE::ButtonType currentbutton = mView->getCurrentButton();
    mMouse.set_x((int)(event->x / mZoom));
    mMouse.set_y((int)(event->y / mZoom));
    bool shiftOrControl = (event->state & GDK_SHIFT_MASK) or(event->state &
                                                             GDK_CONTROL_MASK);
    vpz::BaseModel* model = mCurrent->find(mMouse.get_x(), mMouse.get_y());

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

bool CompleteViewDrawingArea::on_button_release_event(GdkEventButton* event)
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
                    vpz::BaseModel* model = mCurrent->find(x, y);
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

void CompleteViewDrawingArea::onOrder()
{
    mGrid.clear();

    std::string key;
    int x, y;
    int compteur = 0;

    setUndefinedModels();

    mCasesWidth = (mRectWidth - (MODEL_PORT + mOffset)) / (mMaxWidth + 15);
    mCasesHeight = (mRectHeight - (MODEL_PORT + mOffset)) / (mMaxHeight + 15);

    vpz::ModelList::const_iterator it = mCurrent->getModelList().begin();

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

void CompleteViewDrawingArea::preComputeConnectInfo()
{
    mConnectionInfo.clear();

    {
        vpz::ConnectionList& outs(mCurrent->getInternalInputPortList());
        vpz::ConnectionList::const_iterator it;

        for (it = outs.begin(); it != outs.end(); ++it) {
            const  vpz::ModelPortList& ports(it->second);
            vpz::ModelPortList::const_iterator jt ;

            for (jt = ports.begin(); jt != ports.end(); ++jt) {
                record.src = mCurrent;
                record.srcport = it->first;
                record.dst = jt->first;
                record.dstport = jt->second;
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
                    record.src = it->second;
                    record.srcport = jt->first+"";
                    record.dst = kt->first;
                    record.dstport = kt->second;
                    mConnectionInfo.push_back(record);
                }
            }
        }
    }
}

std::string CompleteViewDrawingArea::getConnectionInfo(int mHighlightLine)
{
    return mModeling->getIdCardConnection(
        mConnectionInfo[mHighlightLine].src,
        mConnectionInfo[mHighlightLine].srcport,
        mConnectionInfo[mHighlightLine].dst,
        mConnectionInfo[mHighlightLine].dstport,
        mCurrent);
}

}} // namespace vle gvle
