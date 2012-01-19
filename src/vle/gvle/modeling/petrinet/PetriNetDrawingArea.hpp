/*
 * @file vle/gvle/modeling/petrinet/PetriNetDrawingArea.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2012 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2012 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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


#ifndef VLE_GVLE_MODELING_PETRINET_DRAWING_AREA_HPP
#define VLE_GVLE_MODELING_PETRINET_DRAWING_AREA_HPP

#include <vle/gvle/modeling/petrinet/PetriNet.hpp>
#include <gdkmm/color.h>
#include <gtkmm.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/builder.h>

namespace vle {
namespace gvle {
namespace modeling {
namespace petrinet {

class PetriNetDrawingArea : public Gtk::DrawingArea
{
public:
    enum tool_states {
        SELECT, ADD_ARC, ADD_PLACE, ADD_TRANSITION, DELETE,
        HELP
    };

    PetriNetDrawingArea(BaseObjectType* cobject,
                        const Glib::RefPtr < Gtk::Builder >& xml);
    virtual ~PetriNetDrawingArea()
    {
    }

    void setState(int state)
    {
        mState = state;
        mCurrentPlaces.clear();
        mCurrentTransitions.clear();
    }

    void setPetriNet(PetriNet* petrinet)
    {
        mPetriNet = petrinet;
    }

private:
    void addPlace(guint x, guint y);
    void addTransition(guint x, guint y);
    void exportPng(const std::string& filename);
    void exportPdf(const std::string& filename);
    void exportSvg(const std::string& filename);
    void exportGraphic();
    void initMenuPopupModels();
    points_t computeMarking(const Place* place);
    void displacePlaces(int oldx, int oldy, int newx, int newy,
                        bool& xok, bool& yok);
    void displaceTransitions(int oldx, int oldy, int newx, int newy,
                             bool& xok, bool& yok);

    void draw();
    void drawAnchors(Place* place);
    void drawAnchors(Transition* transition);
    void drawArc(Arc* arc);
    void drawArcs();
    void drawArcText(Arc* arc, int xs, int ys, int xd, int yd);
    void drawArrow(int xs, int ys, int xd, int yd);
    void drawBackground();
    void drawBreakpoints();
    void drawCurrentBreakpoint();
    void drawInhibitor(double xs, double ys, double xd,
                       double& yd);
    void drawPlace(Place* place);
    void drawPlaceName(Place* place);
    void drawPlaces();
    void drawTransition(Transition* transition);
    void drawTransitionName(Transition* transition);
    void drawTransitions();

    void makeBreakpoint(guint mx, guint my, bool ctrl);

    bool modifyCurrentArc();
    bool modifyCurrentPlace();
    bool modifyCurrentTransition();

    bool on_button_press_event(GdkEventButton* event);
    bool on_button_release_event(GdkEventButton* event);
    bool on_configure_event(GdkEventConfigure* event);
    bool on_expose_event(GdkEventExpose* event);
    bool on_motion_notify_event(GdkEventMotion* event);
    void on_realize();
    void queueRedraw()
    {
        mNeedRedraw = true;
        queue_draw();
    }
    void removeBreakpoint();
    point_t searchAnchor(const Place* place, guint x, guint y);
    point_t searchAnchor(const Transition* transition, guint x, guint y);
    point_t searchAnchor(const points_t& anchors, guint x, guint y);
    bool selectArc(guint x, guint y);
    bool selectPlace(guint x, guint y, bool ctrl);
    bool selectTransition(guint x, guint y, bool ctrl);
    void setColor(const Gdk::Color& color);

    static const guint OFFSET;
    static const guint TRANSITION_HEIGHT;
    static const guint TRANSITION_WIDTH;
    static const guint PLACE_RADIUS;

    Glib::RefPtr < Gtk::Builder > mXml;
    Glib::RefPtr < Gdk::Pixmap >       mBuffer;
    Cairo::RefPtr < Cairo::Context >   mContext;
    Glib::RefPtr < Gdk::Window >       mWin;
    Glib::RefPtr < Gdk::GC >           mWingc;
    bool mIsRealized;
    bool mNeedRedraw;

    PetriNet* mPetriNet;
    int mState;

    int mHeight;
    int mWidth;

    std::vector < Place* >             mCurrentPlaces;
    std::vector < Transition* >        mCurrentTransitions;
    int mPreviousX;
    int mPreviousY;
    point_t mMouse;
    point_t mBegin;
    point_t mEnd;
    point_t mStartPoint;
    point_t* mBreakpoint;
    point_t* mFirstBreakpoint;
    point_t* mLastBreakpoint;
    Arc* mCurrentArc;
    bool mPetriNetResize;
    Place* mStartPlace;
    Transition* mStartTransition;
    Gtk::Menu mMenuPopup;

};

}
}
}
}    // namespace vle gvle modeling petrinet

#endif
