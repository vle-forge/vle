/**
 * @file vle/gvle/ViewDrawingArea.cpp
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


#include <vle/gvle/ViewDrawingArea.hpp>
#include <vle/gvle/View.hpp>
#include <vle/utils/Debug.hpp>
#include <gdkmm/types.h>
#include <gdkmm/pixbuf.h>
#include <vector>
#include <cmath>

using namespace vle;

namespace vle
{
namespace gvle {
    
const guint ViewDrawingArea::MODEL_WIDTH = 40;
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

ViewDrawingArea::ViewDrawingArea(View* view) :
        mMouse(-1, -1),
        mPrecMouse(-1, -1),
        mHeight(300),
        mWidth(450),
        mZoom(1.0),
        mIsRealized(false)
{
    AssertI(view);

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
        drawCurrentCoupledModel();
        drawCurrentModelPorts();
        drawConnection();
        drawChildrenModels();
        drawLink();
        drawZoomFrame();
    }
}

void ViewDrawingArea::drawCurrentCoupledModel()
{

    if (mView->existInSelectedModels(mCurrent)) {
        mBuffer->draw_rectangle(mRed, true, 0, 0,
                                (int)(mWidth * mZoom),
                                (int)(mHeight * mZoom));
        mBuffer->draw_rectangle(mWhite, true,
                                (int)(MODEL_PORT * mZoom),
                                (int)(MODEL_PORT * mZoom),
                                (int)((mWidth - 2 * MODEL_PORT) * mZoom),
                                (int)((mHeight - 2 * MODEL_PORT) * mZoom));
        mBuffer->draw_rectangle(mBlack, false,
                                (int)(MODEL_PORT * mZoom),
                                (int)(MODEL_PORT * mZoom),
                                (int)((mWidth - 2 * MODEL_PORT) * mZoom),
                                (int)((mHeight - 2 * MODEL_PORT) * mZoom));
    } else {
        mBuffer->draw_rectangle(mWhite, true, 0, 0,
                                (int)(mWidth * mZoom),
                                (int)(mHeight * mZoom));
        mBuffer->draw_rectangle(mBlack, false,
                                (int)(MODEL_PORT * mZoom),
                                (int)(MODEL_PORT * mZoom),
                                (int)((mWidth - 2 * MODEL_PORT) * mZoom),
                                (int)((mHeight - 2 * MODEL_PORT) * mZoom));
    }
}

void ViewDrawingArea::drawCurrentModelPorts()
{

    const graph::ConnectionList ipl =  mCurrent->getInputPortList();
    const graph::ConnectionList opl =  mCurrent->getOutputPortList();
    graph::ConnectionList::const_iterator itl;

    const size_t maxInput = ipl.size();
    const size_t maxOutput = opl.size();
    const size_t stepInput = (int)((mHeight * mZoom) / (maxInput + 1));
    const size_t stepOutput = (int)((mHeight * mZoom) / (maxOutput + 1));
    std::vector < Gdk::Point > array(4);

    mPango->set_width((int)(mZoom * MODEL_WIDTH));
    mPango->set_alignment(Pango::ALIGN_LEFT);
    Pango::FontDescription desc;
    desc.set_size((int)(10000 * mZoom));
    desc.set_style(Pango::STYLE_OBLIQUE);
    mPango->set_font_description(desc);

    itl = ipl.begin();

    for (size_t i = 0; i < maxInput; ++i) {

        // to draw the port
        array[0].set_x((int)(mZoom * (MODEL_PORT)));
        array[0].set_y((int)(stepInput * (i + 1) - MODEL_PORT));
        array[1].set_x((int)(mZoom * (MODEL_PORT)));
        array[1].set_y((int)(stepInput * (i + 1) + MODEL_PORT));
        array[2].set_x((int)(mZoom * (MODEL_PORT + MODEL_PORT)));
        array[2].set_y((int)(stepInput * (i + 1)));
        array[3] = array[0];
        mBuffer->draw_polygon(mBlack, true, array);

        // to draw the label of the port
        mPango->set_markup(itl->first);
        mBuffer->draw_layout(mBlack,
                             (int)(mZoom * (MODEL_PORT + MODEL_PORT_SPACING_LABEL)),
                             (int)(stepInput * (i + 1)),
                             mPango);

        itl++;
    }

    itl = opl.begin();

    mPango->set_alignment(Pango::ALIGN_RIGHT);

    for (guint i = 0; i < maxOutput; ++i) {

        // to draw the port
        array[0].set_x((int)(mZoom * (mWidth - MODEL_PORT)));
        array[0].set_y((int)(stepOutput * (i + 1) - MODEL_PORT));
        array[1].set_x((int)(mZoom * (mWidth - MODEL_PORT)));
        array[1].set_y((int)(stepOutput * (i + 1) + MODEL_PORT));
        array[2].set_x((int)(mZoom * (MODEL_PORT + mWidth - MODEL_PORT)));
        array[2].set_y((int)(stepOutput * (i + 1)));
        array[3] = array[0];
        mBuffer->draw_polygon(mBlack, true, array);

        // to draw the label of the port
        mPango->set_markup(itl->first);
        mBuffer->draw_layout(mBlack,
                             (int)(mZoom * (mWidth - MODEL_PORT - MODEL_PORT_SPACING_LABEL)),
                             (int)(stepOutput * (i + 1)),
                             mPango);




        itl++;
    }

    mPango->set_alignment(Pango::ALIGN_LEFT);
    desc.set_style(Pango::STYLE_NORMAL);
}

void ViewDrawingArea::drawConnection(graph::Model* src,
                                     const std::string& portsrc,
                                     graph::Model* dst,
                                     const std::string& portdst)
{
    int xs, ys, xd, yd;

    if (src == mCurrent) {
        getCurrentModelInPosition(portsrc, xs, ys);
        getModelInPosition(dst, portdst, xd, yd);
        mBuffer->draw_line(mBlack, (int)(xs * mZoom), (int)(ys * mZoom),
                           (int)(xd * mZoom), (int)(yd * mZoom));
    } else if (dst == mCurrent) {
        getModelOutPosition(src, portsrc, xs, ys);
        getCurrentModelOutPosition(portdst, xd, yd);
        mBuffer->draw_line(mBlack, (int)(xs * mZoom), (int)(ys * mZoom),
                           (int)(xd * mZoom), (int)(yd * mZoom));
    } else {
        getModelOutPosition(src, portsrc, xs, ys);
        getModelInPosition(dst, portdst, xd, yd);
        mBuffer->draw_line(mBlack, (int)(xs * mZoom), (int)(ys * mZoom),
                           (int)(xd * mZoom), (int)(yd * mZoom));
    }
}

void ViewDrawingArea::drawConnection()
{
    using namespace graph;
    {
        ConnectionList& outs(mCurrent->getInternalInputPortList());
        ConnectionList::const_iterator it;

        for (it = outs.begin(); it != outs.end(); ++it) {
            const ModelPortList& ports(it->second);
            ModelPortList::const_iterator jt ;

            for (jt = ports.begin(); jt != ports.end(); ++jt) {
                drawConnection(mCurrent, it->first, jt->first, jt->second);
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
                    drawConnection(it->second, jt->first, kt->first, kt->second);
                }
            }
        }
    }
}

void ViewDrawingArea::drawChildrenModels()
{
    const graph::ModelList& children = mCurrent->getModelList();
    graph::ModelList::const_iterator it = children.begin();
    while (it != children.end()) {
        graph::Model* model = it->second;
        calcSize(model);
        if (mView->existInSelectedModels(model))
            drawChildrenModel(model, mRed);
        else
            if (model->isAtomic())
                drawChildrenModel(model, mBlack);
            else
                drawChildrenModel(model, mGreen);

        ++it;
    }
    if (mView->getDestinationModel() != NULL and mView->getDestinationModel() !=
            mCurrent) {
        drawChildrenModel(mView->getDestinationModel(), mBlue);
    }
}

void ViewDrawingArea::drawChildrenModel(graph::Model* model,
                                        Glib::RefPtr < Gdk::GC > color)
{
    mBuffer->draw_rectangle(mBlack, false, (int)(mZoom * model->x()),
                            (int)(mZoom * model->y()), (int)(mZoom * model->width()),
                            (int)(mZoom * model->height()));
    mBuffer->draw_rectangle(color, false, (int)(mZoom * model->x()),
                            (int)(mZoom * model->y()), (int)(mZoom * model->width()),
                            (int)(mZoom * model->height()));
    model->setWidth(100);
    //if (model->isAtomic()) {
    //Plugin* plg = mModeling->getPlugin(
    //mModeling->vpz().project().dynamics().find(
    //model->getName()).formalism());
    //
    //if (plg) {
    //Glib::RefPtr < Gdk::Pixbuf > px = plg->getImage();
    //
    //if (px) {
    //Glib::RefPtr<Gdk::Pixbuf> copy;
    //
    //copy = px->scale_simple((int)(mZoom * (model->width() - 2)),
    //(int)(mZoom * (model->height() - 2)),
    //Gdk::INTERP_BILINEAR);
    //
    //mBuffer->draw_pixbuf(color, copy, 0, 0,
    //(int)((model->x() + 1) * mZoom),
    //(int)((model->y() + 1) * mZoom),
    //(int)((model->width() - 2) * mZoom),
    //(int)((model->height() - 2) * mZoom),
    //Gdk::RGB_DITHER_NONE, 0, 0);
    //}
    //}
    //}
    drawChildrenPorts(model, color);
}

void ViewDrawingArea::drawChildrenPorts(graph::Model* model,
                                        Glib::RefPtr < Gdk::GC > color)
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
    Gdk::Point   array[4];

    mPango->set_width((int)(mZoom * model->width()));
    mPango->set_alignment(Pango::ALIGN_LEFT);
    Pango::FontDescription desc;
    desc.set_size((int)(10000 * mZoom));
    desc.set_style(Pango::STYLE_OBLIQUE);
    mPango->set_font_description(desc);

    itl = ipl.begin();

    for (size_t i = 0; i < maxInput; ++i) {

        // to draw the port
        array[0].set_x((int)(mZoom * (mX)));
        array[0].set_y((int)(mZoom * (mY + stepInput * (i + 1) -
                                      MODEL_SPACING_PORT)));
        array[1].set_x((int)(mZoom * (mX)));
        array[1].set_y((int)(mZoom * (mY + stepInput * (i + 1) +
                                      MODEL_SPACING_PORT)));
        array[2].set_x((int)(mZoom * (mX + MODEL_SPACING_PORT)));
        array[2].set_y((int)(mZoom * (mY + stepInput * (i + 1))));
        array[3] = array[0];


        // to draw the label of the port
        mPango->set_markup(itl->first);
        mBuffer->draw_layout(color,
                             (int)(mZoom * (mX + PORT_SPACING_LABEL)),
                             (int)(mZoom * (mY + stepInput * (i + 1))),
                             mPango);

        mBuffer->draw_polygon(color, true, array);

        itl++;
    }

    itl = opl.begin();

    for (size_t i = 0; i < maxOutput; ++i) {

        // to draw the port
        array[0].set_x((int)(mZoom * (mX + model->width())));
        array[0].set_y((int)(mZoom * (mY + stepOutput * (i + 1) -
                                      MODEL_SPACING_PORT)));
        array[1].set_x((int)(mZoom * (mX + model->width())));
        array[1].set_y((int)(mZoom * (mY + stepOutput * (i + 1) +
                                      MODEL_SPACING_PORT)));
        array[2].set_x((int)(mZoom * (mX + MODEL_SPACING_PORT +
                                      model->width())));
        array[2].set_y((int)(mZoom * (mY + stepOutput * (i + 1))));
        array[3] = array[0];

        // to draw the label of the port
        mPango->set_markup(itl->first);
        mBuffer->draw_layout(color,
                             (int)(mZoom * (mX + model->width()+ PORT_SPACING_LABEL)),
                             (int)(mZoom * (mY + stepOutput * (i + 1))),
                             mPango);

        mBuffer->draw_polygon(color, true, array);

        itl++;
    }


    desc.set_style(Pango::STYLE_NORMAL);
    mPango->set_font_description(desc);

    mPango->set_width((int)(mZoom * model->width()));
    mPango->set_alignment(Pango::ALIGN_CENTER);
    mPango->set_markup(model->getName());
    mBuffer->draw_layout(color,
                         (int)((model->x() + (model->width() / 2)) * mZoom),
                         (int)((model->y() + model->height() + MODEL_SPACING_PORT) *
                               mZoom),
                         mPango);
}

void ViewDrawingArea::drawLink()
{
    if (mView->getCurrentButton() == GVLE::ADDLINK and
            mView->isEmptySelectedModels() == false) {
        graph::Model* src = mView->getFirstSelectedModels();
        if (src == mCurrent) {
            mBuffer->draw_line(mBlack, (int)(MODEL_PORT * mZoom), (int)(mZoom *
                               mHeight
                               / 2),
                               (int)(mMouse.get_x() * mZoom),
                               (int)(mZoom * mMouse.get_y()));
        } else {
            int w = src->height();
            int h = src->width();
            int x = src->x();
            int y = src->y();

            mBuffer->draw_line(mBlack, (int)(mZoom * (x + w / 2)),
                               (int)(mZoom * (y + h / 2)), (int)(mMouse.get_x() * mZoom),
                               (int)(mMouse.get_y() * mZoom));
        }
    }
}

void ViewDrawingArea::drawZoomFrame()
{
    if (mView->getCurrentButton() == GVLE::ZOOM and mPrecMouse.get_x() != -1
            and mPrecMouse.get_y() != -1) {
        int xmin = std::min(mMouse.get_x(), mPrecMouse.get_x());
        int xmax = std::max(mMouse.get_x(), mPrecMouse.get_x());
        int ymin = std::min(mMouse.get_y(), mPrecMouse.get_y());
        int ymax = std::max(mMouse.get_y(), mPrecMouse.get_y());

        mBuffer->draw_rectangle(mBlack, false,
                                (int)(mZoom * xmin),
                                (int)(mZoom * ymin),
                                (int)(mZoom * (xmax - xmin)),
                                (int)(mZoom * (ymax - ymin)));
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
            //std::cout << "Pointer 1\n";
            on_gvlepointer_button_1(model, shiftOrControl);
        } else if (event->type == GDK_2BUTTON_PRESS and event->button == 1) {
            //std::cout << "Pointer 2\n";
            mView->showModel(model);
        } else if (event->button == 2) {
            //std::cout << "Pointer 3\n";
            mView->showModel(model);
        }
        queue_draw();
        break;
    case GVLE::ADDMODEL:
        mView->addAtomicModel(mMouse.get_x(), mMouse.get_y());
        queue_draw();
        break;
    case GVLE::ADDLINK:
        if (event->button == 1) {
            addLinkOnButtonPress(mMouse.get_x(), mMouse.get_y());
            mPrecMouse = mMouse;
            queue_draw();
        }
        break;
    case GVLE::DELETE:
        delUnderMouse(mMouse.get_x(), mMouse.get_y());
        queue_draw();
        break;
    case GVLE::QUESTION:
        mModeling->showDynamics((model) ? model->getName() :
                                mCurrent->getName());
        break;
    case GVLE::ZOOM:
        if (event->button == 1) {
            mPrecMouse = mMouse;
            queue_draw();
        }
        queue_draw();
        break;
    case GVLE::PLUGINMODEL:
        mView->addPluginModel(mMouse.get_x(), mMouse.get_y());
        queue_draw();
        break;
    case GVLE::ADDCOUPLED:
        if (event->button == 1) {
            mView->addModelInListModel(model, shiftOrControl);
        } else if (event->button == 2) {
            mView->addCoupledModel(mMouse.get_x(), mMouse.get_y());
        }
        queue_draw();
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
    case GVLE::ADDLINK:
        addLinkOnButtonRelease(mMouse.get_x(), mMouse.get_y());
        queue_draw();
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
                queue_draw();
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

    if (change and mIsRealized) {
        set_size_request((int)(mWidth * mZoom), (int)(mHeight * mZoom));
        mBuffer = Gdk::Pixmap::create(mWin, (int)(mWidth * mZoom),
                                      (int)(mHeight * mZoom), -1);
        queue_draw();
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
            draw();
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
            mView->displaceModel(mPrecMouse.get_x(), mPrecMouse.get_y(),
                                 mMouse.get_x(), mMouse.get_y());
            queue_draw();
        }
        mPrecMouse = mMouse;
        break;
    case GVLE::ZOOM:
        if (button == 1) {
            queue_draw();
        }
        break;
    case GVLE::ADDLINK :
        if (button == 1) {
            addLinkOnMotion(mMouse.get_x(), mMouse.get_y());
            queue_draw();
        }
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
    AssertI(mWin);
    mWingc = Gdk::GC::create(mWin);
    mBlack = get_style()->get_black_gc();
    mWhite = get_style()->get_white_gc();
    Gdk::Color red;
    Gdk::Color blue;
    Gdk::Color green;
    red.set_rgb(54484, 44718, 46517);
    blue.set_rgb(44718, 46517, 54484);
    green.set_rgb(46517, 54484, 44718);
    mRed = Gdk::GC::create(mWin);
    mRed->set_fill(Gdk::STIPPLED);
    mRed->set_rgb_fg_color(red);
    mBlue = Gdk::GC::create(mWin);
    mBlue->set_fill(Gdk::STIPPLED);
    mBlue->set_rgb_fg_color(blue);
    mGreen = Gdk::GC::create(mWin);
    mGreen->set_fill(Gdk::STIPPLED);
    mGreen->set_rgb_fg_color(green);
    Pango::FontDescription desc;
    desc.set_size(10000);
    mPango = create_pango_layout("");
    mPango->set_font_description(desc);
    mIsRealized = true;
    queue_draw();
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
        if (mView->isEmptySelectedModels()) {
            mView->addModelToSelectedModels(mCurrent);
        } else {
            mView->clearSelectedModels();
        }
    }
    queue_draw();
}

void ViewDrawingArea::delUnderMouse(int x, int y)
{
    graph::Model* model = mCurrent->find(x, y);
    if (model) {
        mView->delModel(model);
    } else {
        delConnection(x, y);
    }

    queue_draw();
}

void ViewDrawingArea::getCurrentModelInPosition(const std::string& port, int& x,
        int& y)
{
    x = 2 * MODEL_PORT;

    y = ((int)mHeight / (mCurrent->getInputPortList().size() + 1)) *
        (mCurrent->getInputPortIndex(port) + 1);

}

void ViewDrawingArea::getCurrentModelOutPosition(const std::string& port, int&
        x, int& y)
{
    x = (int)mWidth - MODEL_PORT;

    y = ((int)mHeight / (mCurrent->getOutputPortNumber() + 1)) *
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

void ViewDrawingArea::calcSize(graph::Model* m)
{
    m->setHeight(MODEL_HEIGHT +
                 std::max(m->getInputPortNumber(), m->getOutputPortNumber()) *
                 (MODEL_SPACING_PORT + MODEL_PORT));
}

void ViewDrawingArea::addLinkOnButtonPress(int x, int y)
{
    if (mView->isEmptySelectedModels()) {
        mView->clearSelectedModels();
    }

    graph::Model* mdl = mCurrent->find(x, y);
    if (mdl) {
        mView->addModelToSelectedModels(mdl);
    } else {
        mView->addModelToSelectedModels(mCurrent);
    }
    queue_draw();
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
        queue_draw();
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
    queue_draw();
}


void ViewDrawingArea::delConnection(int x, int y)
{
    using namespace graph;
    Model *src = 0, *dst = 0;
    std::string portsrc, portdst;
    int height = -1, h;
    bool internal = false, external = false;

    {
        ConnectionList& outs(mCurrent->getInternalInputPortList());
        ConnectionList::const_iterator it;

        for (it = outs.begin(); it != outs.end(); ++it) {
            const ModelPortList& ports(it->second);
            ModelPortList::const_iterator jt ;

            for (jt = ports.begin(); jt != ports.end(); ++jt) {
                h = distanceConnection(mCurrent, it->first, jt->first,
                                       jt->second, x, y);
                if ((h > 0 and height == -1) or(h > 0 and h < height)) {
                    portsrc = it->first;
                    dst = jt->first;
                    portdst = jt->second;
                    height = h;
                    internal = true;
                }
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
                    h = distanceConnection(it->second, jt->first, kt->first,
                                           kt->second, x, y);
                    if ((h > 0 and height == -1) or(h > 0 and h < height)) {
                        src = it->second;
                        portsrc = jt->first;
                        dst = kt->first;
                        portdst = kt->second;
                        height = h;
                        external = (dst == mCurrent);
                    }
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
}

int ViewDrawingArea::distanceConnection(graph::Model* src,
                                        const std::string& portsrc,
                                        graph::Model* dst,
                                        const std::string& portdst,
                                        int x, int y)
{
    int xs, ys, xd, yd;

    if (src == mCurrent) {
        getCurrentModelInPosition(portsrc, xs, ys);
        getModelInPosition(dst, portdst, xd, yd);
    } else if (dst == mCurrent) {
        getModelOutPosition(src, portsrc, xs, ys);
        getCurrentModelOutPosition(portdst, xd, yd);
    } else {
        getModelOutPosition(src, portsrc, xs, ys);
        getModelInPosition(dst, portdst, xd, yd);
    }
    return distance(xs, ys, xd, yd, x, y);
}

int ViewDrawingArea::distance(int x1, int y1, int x2, int y2, int mx, int my)
{
    double h = -1;
    if (std::min(x1, x2) <= mx and mx <= std::max(x1, x2) and std::min(y1, y2)
            <= my and my <= std::max(y1, y2)) {
        const double a = (y1 - y2) / (double)(x1 - x2);
        const double b = y1 - a * x1;
        h = std::abs((my - (a * mx) - b) / std::sqrt(1 + a * a));
        if (h <= 10) {
            return static_cast < int >(h);
        }
    }
    return -1;
}

void ViewDrawingArea::newSize()
{
    int tWidth = (int)(mWidth * mZoom);
    int tHeight = (int)(mHeight * mZoom);
    set_size_request(tWidth, tHeight);
    mBuffer = Gdk::Pixmap::create(mWin, tWidth, tHeight, -1);
    queue_draw();
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

    Pango::FontDescription desc;
    desc.set_size((int)(10000 * mZoom));
    mPango->set_font_description(desc);
    newSize();
}

void ViewDrawingArea::delCoefZoom()
{
    if (mZoom > 1.0)
        mZoom = mZoom - ZOOM_FACTOR_SUP;
    else
        mZoom = (mZoom <= ZOOM_MIN) ? ZOOM_MIN : mZoom - ZOOM_FACTOR_INF;
    Pango::FontDescription desc;

    desc.set_size((int)(10000 * mZoom));
    mPango->set_font_description(desc);
    newSize();
}

void ViewDrawingArea::restoreZoom()
{
    mZoom = 1.0;
    Pango::FontDescription desc;
    desc.set_size((int)(10000));
    mPango->set_font_description(desc);
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
    Pango::FontDescription desc;
    desc.set_size((int)(10000));
    mPango->set_font_description(desc);
    newSize();

    mView->updateAdjustment(xmin * mZoom, ymin * mZoom);

}

}
} // namespace vle gvle
