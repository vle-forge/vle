/**
 * @file vle/gvle/ViewDrawingArea.hpp
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


#ifndef GUI_VIEWDRAWINGAREA_HPP
#define GUI_VIEWDRAWINGAREA_HPP

#include <gtkmm/drawingarea.h>

namespace vle
{
namespace graph {

class Model;
class CoupledModel;

}
} // namespace vle graph

namespace vle
{
namespace gvle {

class View;
class Modeling;

/**
 * @brief A Gtk::DrawingArea to draw the gui::GModel onto the screen. This
 * objet is attached to a gui::View that manage menu and global Gtk::Window
 * around this Gtk::DrawingArea.
 */
class ViewDrawingArea : public Gtk::DrawingArea
{
public:
    ViewDrawingArea(View* view);

    virtual ~ViewDrawingArea() {}

    void draw();

    static const guint MODEL_WIDTH;
    static const guint MODEL_DECAL;
    static const guint MODEL_PORT;
    static const guint MODEL_PORT_SPACING_LABEL;
    static const guint MODEL_SPACING_PORT;
    static const guint PORT_SPACING_LABEL;
    static const guint MODEL_HEIGHT;
    static const guint CURRENT_MODEL_PORT;
    static const guint CURRENT_MODEL_SPACE;
    static const double ZOOM_FACTOR_SUP;
    static const double ZOOM_FACTOR_INF;
    static const double ZOOM_MIN;
    static const double ZOOM_MAX;

private:
    void drawCurrentCoupledModel();
    void drawCurrentModelPorts();
    void drawConnection();
    void drawConnection(graph::Model* src, const std::string& srcport,
                        graph::Model* dst, const std::string& dstport);
    void drawChildrenModels();
    void drawChildrenModel(graph::Model* model,
                           Glib::RefPtr < Gdk::GC > color);
    void drawChildrenPorts(graph::Model* model,
                           Glib::RefPtr < Gdk::GC > color);
    void drawLink();
    void drawZoomFrame();

    bool on_button_press_event(GdkEventButton* event);
    bool on_button_release_event(GdkEventButton* event);
    bool on_configure_event(GdkEventConfigure* event);
    bool on_expose_event(GdkEventExpose* event);
    bool on_motion_notify_event(GdkEventMotion* event);
    void on_realize();

    void on_gvlepointer_button_1(graph::Model* mdl, bool state);
    void delUnderMouse(int x, int y);

public:
    void getCurrentModelInPosition(const std::string& p, int& x, int& y);
    void getCurrentModelOutPosition(const std::string& p, int& x, int& y);
    void getModelInPosition(graph::Model* model, const std::string& p,
                            int& x, int& y);
    void getModelOutPosition(graph::Model* model, const std::string& p,
                             int& x, int& y);
    /*        void getModelInitPosition(graph::Model* model, const std::string&
                                      p, int& x, int& y); */
    /*        void getModelStatePosition(graph::Model* model, const std::string&
                                       p, int& x, int& y);*/

    void calcSize(graph::Model* m);

private:
    /**
     * add a link between two model prec Model and model under mouse x, y
     *
     * @param x mouse position
     * @param y mouse position
     */
    void addLinkOnButtonPress(int x, int y);
    void addLinkOnMotion(int x, int y);
    void addLinkOnButtonRelease(int x, int y);

    /**
     * Return heigth between segment (x1,y1) and (x2,y2) and mouse
     * position. If mouse position is not include in rectangle (min(x1,x2),
     * min(y1,y2) and max(x1,x2), max(y1,y2) then return negative value.
     *
     * @param x1 x position of origin model
     * @param y1 y position of origin model
     * @param x2 x position of destination model
     * @param y2 y position of destination model
     * @param mx x position of mouse cursor
     * @param my y position of mouse cursor
     * @return height between connection and mouse position. A negative
     * value if position are too far.
     */
    int distance(int x1, int y1, int x2, int y2, int mx, int my);

    /**
     * Return nearest connection between all connection and mouse position
     *
     * @param x x position of mouse cursor
     * @param y y position of mouse cursor
     * @return nearest connection, otherwise 0
     */
    void delConnection(int x, int y);

    /**
     * @brief Return the distance between the connection provided by the
     * fourth function parameters and the user position x, y.
     * @param src model source of the connection.
     * @param portsrc port of the model source.
     * @param dst destination of the connection.
     * @param portdst port of the model destination.
     * @param x position of the mouse.
     * @param y position of the mouse.
     * @return an number of pixel greather than zero or a negative value if
     * error.
     */
    int distanceConnection(graph::Model* src, const std::string& portsrc,
                           graph::Model* dst, const std::string& portdst,
                           int x, int y);

public:
    //
    //
    // MANAGE ZOOM / UNZOOM IN VIEW
    //
    //

    /**
     * when zoom click, if left button then zoom, if right button then
     * unzoom, otherwise default size
     *
     * @param button 1 for left, 2 for right, otherwise other
     */
    void onZoom(int button);

    /**
     * Get current zoom value
     *
     * @return zoom value
     */
    inline double getZoom() const {
        return mZoom;
    }

    /** Add a constant to zoom limit by 4.0 */
    void addCoefZoom();

    /** Del a constant to zoom limit by 0.1 */
    void delCoefZoom();

    /** restore zoom to default value 1.0 */
    void restoreZoom();

    /**
     * Calculate a new Zoom factor using 2 points and apply to the
     * DrawingArea.
     *
     * @param xmin left position
     * @param ymin top position
     * @param xmax right position
     * @param ymax bottom position
     */
    void selectZoom(int xmin, int ymin, int xmax, int ymax);

    /** Calculate the new size of DrawingArea. */
    void newSize();

private:
    View*                           mView;
    graph::CoupledModel*            mCurrent;
    Modeling*                       mModeling;

    Gdk::Point                      mMouse;
    Gdk::Point                      mPrecMouse;

    int                             mHeight;
    int                             mWidth;
    double                          mZoom;
    Glib::RefPtr < Gdk::Pixmap >    mBuffer;
    Glib::RefPtr < Gdk::Window >    mWin;
    Glib::RefPtr < Gdk::GC >        mWingc;
    Glib::RefPtr < Gdk::GC >        mBlack;
    Glib::RefPtr < Gdk::GC >        mWhite;
    Glib::RefPtr < Gdk::GC >        mRed;
    Glib::RefPtr < Gdk::GC >        mGreen;
    Glib::RefPtr < Gdk::GC >        mBlue;
    Glib::RefPtr < Pango::Layout>   mPango;
    bool                            mIsRealized;
};

}
} // namespace vle gvle

#endif
