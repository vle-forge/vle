/**
 * @file vle/gvle/modeling/fsa/StatechartDrawingArea.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2010 ULCO http://www.univ-littoral.fr
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


#ifndef VLE_GVLE_MODELING_FSA_STATECHART_DRAWING_AREA_HPP
#define VLE_GVLE_MODELING_FSA_STATECHART_DRAWING_AREA_HPP

#include <vle/gvle/modeling/fsa/Statechart.hpp>
#include <gdkmm/color.h>
#include <gtkmm.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/scrolledwindow.h>
#include <libglademm.h>

namespace vle { namespace gvle { namespace modeling { namespace fsa {

class StatechartDrawingArea : public Gtk::DrawingArea
{
public:
    enum tool_states { SELECT, ADD_STATE, ADD_TRANSITION, DELETE, HELP};

    StatechartDrawingArea(BaseObjectType* cobject,
                          const Glib::RefPtr < Gnome::Glade::Xml >& xml);

    virtual ~StatechartDrawingArea() { }

    void setState(int state)
    { mState = state; mCurrentStates.clear(); }

    void setStatechart(Statechart* statechart)
    { mStatechart = statechart; }

private:
    void addState(guint x, guint y);
    void exportPng(const std::string& filename);
    void exportPdf(const std::string& filename);
    void exportSvg(const std::string& filename);
    void exportGraphic();
    void initMenuPopupModels();
    void checkSize(State* state);
    int checkSize(State* state, const std::string& str);
    void displaceStates(int oldx, int oldy, int newx, int newy,
                        bool& xok, bool& yok);

    void draw();
    void drawAnchors(State* state);
    void drawArrow(int xs, int ys, int xd, int yd);
    void drawBackground();
    void drawBreakpoints();
    void drawCurrentBreakpoint();
    void drawDirectConnection(int xs, int ys, int xd, int yd);
    void drawInitialState(State* state);
    void drawName();
    void drawRectangle();
    void drawRoundedRectangle(guint x, guint y, guint width, guint height,
                              double aspect, double radius, double red,
                              double green, double blue);
    void drawSelfConnection(Transition* transition);
    void drawState(State* state);
    void drawStateName(State* state);
    void drawStates();
    void drawTransition(Transition* transition);
    void drawTransitions();
    void drawTransitionText(Transition* transition, int xs, int ys,
                            int xd, int yd);

    function_type getFunctionType(const std::string& value) const;
    void makeBreakpoint(guint mx, guint my, bool ctrl);
    bool modifyCurrentState();
    bool modifyCurrentTransition();

    bool on_button_press_event(GdkEventButton* event);
    bool on_button_release_event(GdkEventButton* event);
    bool on_configure_event(GdkEventConfigure* event);
    bool on_expose_event(GdkEventExpose* event);
    bool on_motion_notify_event(GdkEventMotion* event);
    void on_realize();
    void queueRedraw()
    { mNeedRedraw = true; queue_draw(); }
    void removeBreakpoint();
    void resizeStatechart(int oldx, int oldy, int newx, int newy,
                          bool& xok, bool& yok);
    point_t searchAnchor(const State* state, guint x, guint y);
    void setColor(const Gdk::Color& color);
    bool selectState(guint x, guint y, bool ctrl);
    bool selectTransition(guint x, guint y);

    static const guint OFFSET;
    static const guint HEADER_HEIGHT;
    static const guint STATE_NAME_HEIGHT;
    static const guint STATE_HEIGHT;
    static const guint STATE_WIDTH;
    static const guint SELF_TRANSITION_SIZE;

    Glib::RefPtr < Gnome::Glade::Xml > mXml;
    Glib::RefPtr < Gdk::Pixmap >       mBuffer;
    Cairo::RefPtr < Cairo::Context >   mContext;
    Glib::RefPtr < Gdk::Window >       mWin;
    Glib::RefPtr < Gdk::GC >           mWingc;
    bool                               mIsRealized;
    bool                               mNeedRedraw;

    Statechart*                        mStatechart;
    int                                mState;

    int                                mHeight;
    int                                mWidth;

    std::vector < State* >             mCurrentStates;
    int                                mPreviousX;
    int                                mPreviousY;
    point_t                            mMouse;
    point_t                            mBegin;
    point_t                            mEnd;
    point_t                            mStartPoint;
    point_t*                           mBreakpoint;
    point_t*                           mFirstBreakpoint;
    point_t*                           mLastBreakpoint;
    Transition*                        mCurrentTransition;
    bool                               mStatechartResize;
    State*                             mStartState;

    Gtk::Menu                          mMenuPopup;
};

}}}} // namespace vle gvle modeling fsa

#endif
