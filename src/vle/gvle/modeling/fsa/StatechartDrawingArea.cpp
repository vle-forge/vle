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

#include <vle/gvle/modeling/fsa/StatechartDrawingArea.hpp>
#include <vle/gvle/modeling/fsa/StateDialog.hpp>
#include <vle/gvle/modeling/fsa/TransitionDialog.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/Settings.hpp>
#include <vle/utils/i18n.hpp>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/radioaction.h>
#include <gtkmm/stock.h>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <limits>

namespace vle {
namespace gvle {
namespace modeling {
namespace fsa {

const guint StatechartDrawingArea::OFFSET = 10;
const guint StatechartDrawingArea::HEADER_HEIGHT = 20;
const guint StatechartDrawingArea::STATE_NAME_HEIGHT = 20;
const guint StatechartDrawingArea::STATE_HEIGHT = 50;
const guint StatechartDrawingArea::STATE_WIDTH = 50;
const guint StatechartDrawingArea::SELF_TRANSITION_SIZE = 60;

StatechartDrawingArea::StatechartDrawingArea(
    BaseObjectType* cobject,
    const Glib::RefPtr < Gnome::Glade::Xml >& xml) :
    Gtk::DrawingArea(cobject),
    mXml(xml),
    mIsRealized(false),
    mNeedRedraw(true),
    mStatechart(0),
    mState(SELECT),
    mHeight(300 + 2 * OFFSET),
    mWidth(400 + 2 * OFFSET),
    mPreviousX(-1),
    mPreviousY(-1),
    mBreakpoint(0),
    mCurrentTransition(0),
    mStatechartResize(false),
    mStartState(0)
{
    set_events(Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_MOTION_MASK |
        Gdk::BUTTON1_MOTION_MASK | Gdk::BUTTON2_MOTION_MASK |
        Gdk::BUTTON3_MOTION_MASK | Gdk::BUTTON_PRESS_MASK |
        Gdk::BUTTON_RELEASE_MASK);
    initMenuPopupModels();
}

void StatechartDrawingArea::addState(guint x, guint y)
{
    NewStateDialog dialog(mXml, *mStatechart);
    if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
        int newWidth = x + STATE_WIDTH + OFFSET;
        int newHeight = y + STATE_HEIGHT + OFFSET;

        mStatechart->addState(dialog.name(), dialog.initial(),
            x, y, STATE_WIDTH, STATE_HEIGHT);
        if (newWidth > mStatechart->width()) {
            mStatechart->width(newWidth);
            mWidth = mStatechart->width() + OFFSET;
        }
        if (newHeight > mStatechart->height()) {
            mStatechart->height(newHeight);
            mHeight = mStatechart->height() + OFFSET;
        }
    }
}

void StatechartDrawingArea::exportPng(const std::string& filename)
{
    Cairo::RefPtr < Cairo::ImageSurface > surface =
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

void StatechartDrawingArea::exportPdf(const std::string& filename)
{
    Cairo::RefPtr < Cairo::PdfSurface > surface =
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

void StatechartDrawingArea::exportSvg(const std::string& filename)
{
    Cairo::RefPtr < Cairo::SvgSurface > surface =
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

void StatechartDrawingArea::exportGraphic()
{
    Gtk::FileChooserDialog file(_(
	    "Image file"),
                                Gtk::FILE_CHOOSER_ACTION_SAVE);
    file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
    Gtk::FileFilter filterAuto;
    Gtk::FileFilter filterPng;
    Gtk::FileFilter filterPdf;
    Gtk::FileFilter filterSvg;
    filterAuto.set_name(_("Guess type from file name"));
    filterAuto.add_pattern("*");
    filterPng.set_name(_("Portable Newtork Graphics (.png)"));
    filterPng.add_pattern("*.png");
    filterPdf.set_name(_("Portable Format Document (.pdf)"));
    filterPdf.add_pattern("*.pdf");
    filterSvg.set_name(_("Scalable Vector Graphics (.svg)"));
    filterSvg.add_pattern("*.svg");
    file.add_filter(filterAuto);
    file.add_filter(filterPng);
    file.add_filter(filterPdf);
    file.add_filter(filterSvg);


    if (file.run() == Gtk::RESPONSE_OK) {
        std::string filename(file.get_filename());
        std::string extension(file.get_filter()->get_name());

        if (extension == _("Guess type from file name")) {
            size_t ext_pos = filename.find_last_of('.');
            if (ext_pos != std::string::npos) {
                std::string type(filename, ext_pos + 1);
                filename.resize(ext_pos);
                if (type == "png") {
                    exportPng(filename);
                } else if (type == "pdf") {
                    exportPdf(filename);
                } else if (type == "svg") {
                    exportSvg(filename);
                } else {
                    Error(_("Unsupported file format"));
                }
            }
        } else if (extension == _("Portable Newtork Graphics (.png)")) {
            exportPng(filename);
        } else if (extension == _("Portable Format Document (.pdf)")) {
            exportPdf(filename);
        } else if (extension == _("Scalable Vector Graphics (.svg)")) {
            exportSvg(filename);
        }
    }
}

void StatechartDrawingArea::initMenuPopupModels()
{
    Gtk::Menu::MenuList& menulist(mMenuPopup.items());

    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem(
            _("_Export Graphic"),
            sigc::mem_fun(
                *this,
                &StatechartDrawingArea::exportGraphic)));

    mMenuPopup.accelerate(*this);
}

void StatechartDrawingArea::checkSize(State* state)
{
    int height = STATE_NAME_HEIGHT;

    checkSize(state, state->name());
    if (not state->inAction().empty()) {
        height += checkSize(state, (fmt(
	        "in: %1%") %
                                    state->inAction()).str());
    }
    if (not state->outAction().empty()) {
        height += checkSize(state,
                (fmt("out: %1%") % state->outAction()).str());
    }
    if (not state->activity().empty()) {
        height += checkSize(state,
                (fmt("activity: %1%") % state->activity()).str());
    }
    if (not state->eventInStates().empty()) {
        for (eventInStates_t::const_iterator it =
                 state->eventInStates().begin();
             it != state->eventInStates().end(); ++it) {
            height += checkSize(state,
                    (fmt("%1%: %2%") % it->first
                     % it->second).str());
        }
    }
    if (height > (int)STATE_HEIGHT) {
        mStatechart->resizeState(state, state->width(), height);
    }
}

int StatechartDrawingArea::checkSize(State* state, const std::string& str)
{
    Cairo::TextExtents textExtents;

    mContext->get_text_extents(str, textExtents);
    if (state->width() < textExtents.width + 2 * OFFSET) {
        mStatechart->resizeState(state, textExtents.width + 2 * OFFSET,
            state->height());
    }
    return textExtents.height + OFFSET;
}

void StatechartDrawingArea::displaceStates(int oldx, int oldy,
    int newx, int newy,
    bool& xok, bool& yok)
{
    int deltax = newx - oldx;
    int deltay = newy - oldy;
    bool change = false;

    xok = false;
    yok = false;
    if (deltax != 0 or deltay != 0) {
        for (std::vector < State* >::const_iterator it =
                 mCurrentStates.begin();
             it != mCurrentStates.end(); ++it) {
            int _deltax = deltax;
            int _deltay = deltay;

            if ((*it)->x() + deltax < (int)(2 * OFFSET)) {
                _deltax = 0;
            } else {
                xok = true;
            }
            if ((*it)->y() + deltay < (int)(2 * OFFSET + HEADER_HEIGHT)) {
                _deltay = 0;
            } else {
                yok = true;
            }

            mStatechart->displace(*it, _deltax, _deltay);

            {
                int newWidth = (*it)->x() + (*it)->width();
                int newHeight = (*it)->y() + (*it)->height();

                if (newWidth > mStatechart->width()) {
                    mStatechart->width(newWidth);
                    mWidth = mStatechart->width() + OFFSET;
                    change = true;
                }
                if (newHeight > mStatechart->height()) {
                    mStatechart->height(newHeight);
                    mHeight = mStatechart->height() + OFFSET;
                    change = true;
                }
            }
        }
    } else {
        xok = true;
        yok = true;
    }
    if (change) {
        mBuffer = Gdk::Pixmap::create(mWin, mWidth, mHeight, -1);
    }
}

void StatechartDrawingArea::draw()
{
    if (mIsRealized and mBuffer) {
        mContext->save();

        mContext->set_line_width(Settings::settings().getLineWidth());

        drawBackground();
        drawRectangle();
        drawName();
        drawStates();
        drawTransitions();

        if (mMouse.valid() and mBegin.valid()) {
            mContext->set_source_rgb(1., 0., 0.);
            mContext->rectangle(mBegin.x - 2, mBegin.y - 2, 5, 5);
            mContext->fill();

            if (mEnd.valid()) {
                mContext->rectangle(mEnd.x - 2, mEnd.y - 2, 5, 5);
                mContext->fill();
            }

            mContext->set_source_rgb(0., 0., 0.);
            mContext->move_to(mBegin.x, mBegin.y);
            mContext->line_to(mMouse.x, mMouse.y);
            mContext->stroke();
        }

        if (mCurrentTransition) {
            if (mBreakpoint) {
                drawCurrentBreakpoint();
            } else {
                drawBreakpoints();
            }
        }

        mContext->restore();
        set_size_request(mWidth, mHeight);
    }
}

void StatechartDrawingArea::drawAnchors(State* state)
{
    const points_t& points = state->anchors();

    mContext->set_source_rgb(0., 0., 1.);
    for (points_t::const_iterator it = points.begin(); it != points.end();
         ++it) {
        mContext->rectangle(it->x - 2, it->y - 2, 5, 5);
        mContext->fill();
    }
}

void StatechartDrawingArea::drawArrow(int xs, int ys, int xd, int yd)
{
    double d = std::sqrt((xd - xs) * (xd - xs) + (yd - ys) * (yd - ys));
    double angle = std::acos((xd - xs) / d);

    if (ys > yd) {
        angle *= -1;
    }

    mContext->set_identity_matrix();
    mContext->move_to(xd, yd);
    mContext->rotate(angle + 0.3);
    mContext->rel_line_to(-8, 0);

    mContext->set_identity_matrix();
    mContext->move_to(xd, yd);
    mContext->rotate(angle - 0.3);
    mContext->rel_line_to(-8, 0);
    mContext->stroke();

    mContext->set_identity_matrix();
}

void StatechartDrawingArea::drawBackground()
{
    setColor(Settings::settings().getBackgroundColor());
    mContext->rectangle(0, 0, mWidth, mHeight);
    mContext->fill();
    mContext->stroke();
}

void StatechartDrawingArea::drawBreakpoints()
{
    points_t::const_iterator it = mCurrentTransition->points().begin();

    mContext->set_source_rgb(0., 0., 1.);
    while (it != mCurrentTransition->points().end()) {
        mContext->rectangle(it->x - 2, it->y - 2, 5, 5);
        mContext->fill();
        ++it;
    }
}

void StatechartDrawingArea::drawCurrentBreakpoint()
{
    mContext->set_source_rgb(1., 0., 0.);
    mContext->rectangle(mBreakpoint->x - 2, mBreakpoint->y - 2, 5, 5);
    mContext->fill();
}

void StatechartDrawingArea::drawDirectConnection(int xs, int ys,
    int xd, int yd)
{
    mContext->move_to(xs, ys);
    mContext->line_to(xd, yd);

    if (xs == xd) {
        if (ys < yd) {
            mContext->move_to(xd - 5, yd - 5);
            mContext->rel_line_to(5, 5);
            mContext->rel_line_to(5, -5);
        } else {
            mContext->move_to(xd + 5, yd + 5);
            mContext->rel_line_to(-5, -5);
            mContext->rel_line_to(-5, 5);
        }
    } else {
        if (xs < xd) {
            mContext->move_to(xd - 5, yd - 5);
            mContext->rel_line_to(5, 5);
            mContext->rel_line_to(-5, 5);
        } else {
            mContext->move_to(xd + 5, yd - 5);
            mContext->rel_line_to(-5, 5);
            mContext->rel_line_to(5, 5);
        }
    }
    mContext->set_line_join(Cairo::LINE_JOIN_MITER);
    mContext->stroke();
}

void StatechartDrawingArea::drawInitialState(State* state)
{
    int x;
    int y;
    int xs;
    int ys;
    int xd;
    int yd;

    if (state->y() - 25 < (int)(2 * OFFSET + HEADER_HEIGHT)) {
        if (state->x() - 25 < (int)(2 * OFFSET)) {
            x = state->x() + state->width() + 25;
            y = state->y() + state->height() / 2;
            xs = x - 5;
            ys = y;
            xd = state->x() + state->width() + 1;
            yd = y;
        } else {
            x = state->x() - 25;
            y = state->y() + state->height() / 2;
            xs = x + 5;
            ys = y;
            xd = state->x() - 1;
            yd = y;
        }
    } else {
        x = state->x() + state->width() / 2;
        y = state->y() - 25;
        xs = x;
        ys = y + 5;
        xd = x;
        yd = state->y() - 1;
    }

    // draw initial state
    mContext->set_source_rgb(0., 0., 0.);
    mContext->arc(x, y, 5, 0, 2 * M_PI);
    mContext->fill();
    mContext->stroke();

    // draw transition
    drawDirectConnection(xs, ys, xd, yd);
}

void StatechartDrawingArea::drawName()
{
    mContext->select_font_face(Settings::settings().getFont(),
        Cairo::FONT_SLANT_NORMAL,
        Cairo::FONT_WEIGHT_NORMAL);
    mContext->set_font_size(Settings::settings().getFontSize());

    Cairo::TextExtents textExtents;
    mContext->get_text_extents(mStatechart->name(), textExtents);

    mContext->move_to(
        (mStatechart->width() - textExtents.width) / 2 + OFFSET,
        OFFSET + (HEADER_HEIGHT + textExtents.height) / 2);
    mContext->show_text(mStatechart->name());
    mContext->stroke();
}

void StatechartDrawingArea::drawRectangle()
{
    setColor(Settings::settings().getForegroundColor());

    drawRoundedRectangle(OFFSET, OFFSET, mStatechart->width(),
        mStatechart->height(), 1.0, 20,
        1., 1., 1.);

    mContext->move_to(OFFSET, OFFSET + HEADER_HEIGHT);
    mContext->line_to(
        mStatechart->width() + OFFSET, OFFSET + HEADER_HEIGHT);
    mContext->stroke();
}

void StatechartDrawingArea::drawRoundedRectangle(guint x, guint y,
    guint width, guint height,
    double aspect,
    double corner_radius,
    double red,
    double green,
    double blue)
{
    double radius = corner_radius / aspect;
    double degrees = M_PI / 180.0;

    mContext->set_line_width(Settings::settings().getLineWidth());
    mContext->begin_new_sub_path();
    mContext->arc(x + width - radius, y + radius, radius,
        -90 * degrees, 0 * degrees);
    mContext->arc(x + width - radius, y + height - radius,
        radius, 0 * degrees, 90 * degrees);
    mContext->arc(x + radius, y + height - radius, radius,
        90 * degrees, 180 * degrees);
    mContext->arc(x + radius, y + radius, radius,
        180 * degrees, 270 * degrees);
    mContext->close_path();

    mContext->set_source_rgb(red, green, blue);
    mContext->fill_preserve();
    setColor(Settings::settings().getForegroundColor());
    mContext->stroke();
}

void StatechartDrawingArea::drawSelfConnection(Transition* transition)
{
    const point_t& source = *transition->points().begin();
    State* state = mStatechart->state(transition->source());
    int xs, ys;
    int xt, yt;

    mContext->move_to(source.x, source.y);
    if (source.x == state->x()) { // left
        xs = source.x - SELF_TRANSITION_SIZE;
        ys = source.y - SELF_TRANSITION_SIZE;
        xt = source.x - SELF_TRANSITION_SIZE;
        yt = source.y + SELF_TRANSITION_SIZE;
    } else if (source.x == state->x() + state->width()) { // right
        xs = source.x + SELF_TRANSITION_SIZE;
        ys = source.y + SELF_TRANSITION_SIZE;
        xt = source.x + SELF_TRANSITION_SIZE;
        yt = source.y - SELF_TRANSITION_SIZE;
    } else if (source.y == state->y()) { // top
        xs = source.x + SELF_TRANSITION_SIZE;
        ys = source.y - SELF_TRANSITION_SIZE;
        xt = source.x - SELF_TRANSITION_SIZE;
        yt = source.y - SELF_TRANSITION_SIZE;
    } else { // bottom
        xs = source.x - SELF_TRANSITION_SIZE;
        ys = source.y + SELF_TRANSITION_SIZE;
        xt = source.x + SELF_TRANSITION_SIZE;
        yt = source.y + SELF_TRANSITION_SIZE;
    }
    mContext->curve_to(xt, yt, xs, ys, source.x, source.y);
    mContext->stroke();

    drawTransitionText(transition, source.x, source.y, xt, yt);

    drawArrow(xs, ys, source.x, source.y);
}

void StatechartDrawingArea::drawState(State* state)
{
    if (std::find(mCurrentStates.begin(), mCurrentStates.end(), state) ==
        mCurrentStates.end()) {
        setColor(Settings::settings().getAtomicColor());
    } else {
        setColor(Settings::settings().getSelectedColor());
    }

    drawRoundedRectangle(state->x(), state->y(),
        state->width(), state->height(), 1.0, 10,
        1., 1., 0.75);
    mContext->stroke();

    mContext->move_to(state->x(), state->y() + STATE_NAME_HEIGHT);
    mContext->line_to(state->x() + state->width(),
        state->y() + STATE_NAME_HEIGHT);
    mContext->stroke();

    drawStateName(state);

    int pos = state->y() + STATE_NAME_HEIGHT + OFFSET;

    mContext->move_to(state->x() + OFFSET, pos);
    if (not state->inAction().empty()) {
        Cairo::TextExtents textExtents;
        std::string str((fmt("in: %1%") % state->inAction()).str());

        mContext->get_text_extents(str, textExtents);
        mContext->show_text(str);
        pos += textExtents.height + OFFSET;
        mContext->move_to(state->x() + OFFSET, pos);
    }
    if (not state->outAction().empty()) {
        Cairo::TextExtents textExtents;
        std::string str((fmt("out: %1%") % state->outAction()).str());

        mContext->get_text_extents(str, textExtents);
        mContext->show_text(str);
        pos += textExtents.height + OFFSET;
        mContext->move_to(state->x() + OFFSET, pos);
    }
    if (not state->activity().empty()) {
        Cairo::TextExtents textExtents;
        std::string str((fmt("activity: %1%") % state->activity()).str());

        mContext->get_text_extents(str, textExtents);
        mContext->show_text(str);
        pos += textExtents.height + OFFSET;
        mContext->move_to(state->x() + OFFSET, pos);
    }
    if (not state->eventInStates().empty()) {
        for (eventInStates_t::const_iterator it =
                 state->eventInStates().begin();
             it != state->eventInStates().end(); ++it) {
            Cairo::TextExtents textExtents;
            std::string str((fmt("%1%: %2%") % it->first % it->second).str());

            mContext->get_text_extents(str, textExtents);
            mContext->show_text(str);
            pos += textExtents.height + OFFSET;
            mContext->move_to(state->x() + OFFSET, pos);
        }
    }

    if (state->initial()) {
        drawInitialState(state);
    }

    if (mState == ADD_TRANSITION and not mCurrentStates.empty() and
        state == *mCurrentStates.begin()) {
        drawAnchors(state);
    }
}

void StatechartDrawingArea::drawStateName(State* state)
{
    setColor(Settings::settings().getForegroundColor());
    mContext->select_font_face(Settings::settings().getFont(),
        Cairo::FONT_SLANT_NORMAL,
        Cairo::FONT_WEIGHT_NORMAL);
    mContext->set_font_size(Settings::settings().getFontSize());

    Cairo::TextExtents textExtents;

    mContext->get_text_extents(state->name(), textExtents);
    mContext->move_to(
        state->x() + (state->width() - textExtents.width) / 2,
        state->y() + (STATE_NAME_HEIGHT +
                      textExtents.height) / 2);
    mContext->show_text(state->name());
    mContext->stroke();
}

void StatechartDrawingArea::drawStates()
{
    for (states_t::const_iterator it = mStatechart->states().begin();
         it != mStatechart->states().end(); ++it) {
        drawState(it->second);
    }
}

void StatechartDrawingArea::drawTransition(Transition* transition)
{
    if (transition->source() == transition->destination()) {
        drawSelfConnection(transition);
    } else {
        points_t::const_iterator it = transition->points().begin();
        double xs = it->x;
        double ys = it->y;
        double xd = (++it)->x;
        double yd = it->y;

        drawTransitionText(transition, xs, ys, xd, yd);

        mContext->move_to(xs, ys);
        while (it != transition->points().end()) {
            mContext->line_to(xd, yd);
            ++it;
            if (it != transition->points().end()) {
                xs = xd;
                ys = yd;
                xd = it->x;
                yd = it->y;
            }
        }
        mContext->stroke();

        drawArrow(xs, ys, xd, yd);
    }
}

void StatechartDrawingArea::drawTransitions()
{
    mContext->set_source_rgb(0., 0., 0.);
    for (transitions_t::const_iterator it =
             mStatechart->transitions().begin();
         it != mStatechart->transitions().end(); ++it) {
        drawTransition(*it);
    }
}

void StatechartDrawingArea::drawTransitionText(Transition* transition,
    int xs, int ys,
    int xd, int yd)
{
    std::string str = transition->event();
    double d = std::sqrt((xd - xs) * (xd - xs) + (yd - ys) * (yd - ys));

    if (not transition->after().empty()) {
        str += (fmt("after(%1%)") % transition->after()).str();
    }
    if (not transition->when().empty()) {
        str += (fmt("when(%1%)") % transition->when()).str();
    }
    if (not transition->guard().empty()) {
        str += (fmt("[%1%]") % transition->guard()).str();
    }
    if (not transition->action().empty()) {
        str += (fmt("/%1%") % transition->action()).str();
    }
    if (not transition->send().empty()) {
        str += (fmt("^%1%") % transition->send()).str();
    }

    mContext->set_source_rgb(0., 0., 0.);
    mContext->select_font_face(Settings::settings().getFont(),
        Cairo::FONT_SLANT_NORMAL,
        Cairo::FONT_WEIGHT_NORMAL);
    mContext->set_font_size(Settings::settings().getFontSize());

    Cairo::TextExtents textExtents;

    mContext->get_text_extents(str, textExtents);

    double angle = std::acos((xd - xs) / d);

    if (ys > yd) {
        angle *= -1;
    }

    mContext->move_to(xs, ys);
    if (xs < xd) {
        mContext->rotate(angle);
        mContext->rel_move_to(10, -2);
    } else {
        mContext->rotate(angle - M_PI);
        mContext->rel_move_to(-10 - textExtents.width, -2);
    }
    mContext->show_text(str);
    mContext->stroke();
    mContext->set_identity_matrix();
}

function_type StatechartDrawingArea::getFunctionType(
    const std::string& value) const
{
    try {
        boost::lexical_cast < double > (value);
    } catch (...) {
        return FUNCTION;
    }
    return CONSTANT;
}

void StatechartDrawingArea::makeBreakpoint(guint mx, guint my, bool ctrl)
{
    transitions_t::const_iterator itt = mStatechart->transitions().begin();
    bool found = false;

    mBreakpoint = 0;
    while (itt != mStatechart->transitions().end() and not found) {
        int xs2, ys2;
        points_t::iterator it = (*itt)->points().begin();
        double deltax = (it->x + 2.) - mx;
        double deltay = (it->y + 2.) - my;

        if (std::sqrt(deltax * deltax + deltay * deltay) < 10) {
            mCurrentTransition = *itt;
            if (ctrl) {
                mCurrentStates.clear();
                mCurrentStates.push_back(mStatechart->state((*itt)->source()));
                mFirstBreakpoint = &(*it);
            }
            found = true;
        } else {
            xs2 = it->x;
            ys2 = it->y;
            ++it;
            while (it != (*itt)->points().end() and not found) {
                int xs, ys, xd, yd;

                if (xs2 == it->x) {
                    xs = xs2 - 5;
                    xd = it->x + 5;
                } else {
                    xs = xs2;
                    xd = it->x;
                }

                if (ys2 == it->y) {
                    ys = ys2 - 5;
                    yd = it->y + 5;
                } else {
                    ys = ys2;
                    yd = it->y;
                }

                deltax = (double)xd - mx;
                deltay = (double)yd - my;
                if (std::sqrt(deltax * deltax + deltay * deltay) < 10) {
                    found = true;
                    mCurrentTransition = *itt;
                    mBreakpoint = &(*it);
                    ++it;
                    if (it == (*itt)->points().end()) {
                        if (ctrl) {
                            mCurrentStates.clear();
                            mCurrentStates.push_back(
	mStatechart->state((*itt)->destination()));
                            mLastBreakpoint = mBreakpoint;
                        }
                        mBreakpoint = 0;
                    }
                } else {
                    double h = -1;
                    if (std::min(xs, xd) <= (int)mx
                        and (int)mx <= std::max(xs, xd)
                        and std::min(ys, yd) <= (int)my
                        and (int)my <= std::max(ys, yd)) {
                        const double a = (ys - yd) / (double)(xs - xd);
                        const double b = ys - a * xs;
                        h = std::abs((my - (a * mx) - b) /
	std::sqrt(1 + a * a));
                        if (h <= 10) {
                            point_t pt(mx, my);

                            found = true;
                            mCurrentTransition = *itt;
                            mBreakpoint =
                                &(*mCurrentTransition->addPoint(it,
	      pt));
                        }
                    }
                }
                if (not found) {
                    xs2 = it->x;
                    ys2 = it->y;
                    ++it;
                }
            }
        }
        if (not found) {
            ++itt;
        }
    }
}

bool StatechartDrawingArea::modifyCurrentState()
{
    bool result = false;
    if (not mCurrentStates.empty()) {
        State* state = *mCurrentStates.begin();
        std::string oldName = state->name();
        StateDialog dialog(mXml, mStatechart, state);

        if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
            state->name(dialog.name());
            state->initial(dialog.initial());
            state->inAction(dialog.getInAction());
            state->outAction(dialog.getOutAction());
            state->activity(dialog.getActivity());
            state->eventInStates(dialog.eventInStates());
            if(oldName != dialog.name()){
                mStatechart->changeStateName(oldName,
                                             dialog.name());
            }
            for (eventInStates_t::const_iterator it =
                     state->eventInStates().begin();
                 it != state->eventInStates().end(); ++it) {
                if (std::find(mStatechart->inputPorts().begin(),
                        mStatechart->inputPorts().end(), it->first)
                    == mStatechart->inputPorts().end()) {
                    mStatechart->addInputPort(it->first);
                }
            }

            checkSize(state);
            result = true;
        }
    }
    return result;
}

bool StatechartDrawingArea::modifyCurrentTransition()
{
    bool result = false;

    if (mCurrentTransition) {
        TransitionDialog dialog(mXml, mCurrentTransition, mStatechart);

        if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
            mCurrentTransition->action(dialog.action());
            mCurrentTransition->after(dialog.after(),
                getFunctionType(dialog.after()));
            mCurrentTransition->event(dialog.event());
            mCurrentTransition->guard(dialog.guard());
            mCurrentTransition->send(dialog.send(), dialog.typeSend());
            mCurrentTransition->when(dialog.when(),
                getFunctionType(dialog.when()));

            if (not dialog.event().empty() and
                std::find(mStatechart->inputPorts().begin(),
                    mStatechart->inputPorts().end(),
                    dialog.event()) == mStatechart->inputPorts().end()) {
                mStatechart->addInputPort(dialog.event());
            }
            if (dialog.typeSend() == CONSTANT and
                std::find(mStatechart->outputPorts().begin(),
                    mStatechart->outputPorts().end(),
                    dialog.send()) == mStatechart->outputPorts().end()) {
                mStatechart->addOutputPort(dialog.send());
            }
            result = true;
        }
    }
    return result;
}

bool StatechartDrawingArea::on_button_press_event(GdkEventButton* event)
{
    if (event->button == 3) {
        mMenuPopup.popup(event->button, event->time);
    }
    switch (mState)
    {
    case SELECT:
        if (event->type == GDK_BUTTON_PRESS and event->button == 1) {
            if (not selectState(event->x, event->y,
                    event->state & GDK_CONTROL_MASK)) {
                int x = event->x - OFFSET;
                int y = event->y - OFFSET;

                mStatechartResize = (x >= mStatechart->width() - 20) and
                    x <= mStatechart->width() and
                    y >= mStatechart->height() - 20 and
                    y <= mStatechart->height();
            }
        } else if (event->type == GDK_2BUTTON_PRESS and event->button ==
                   1) {
            if (selectState(event->x, event->y, false)) {
                if (modifyCurrentState()) {
                    queueRedraw();
                }
            } else if (selectTransition(event->x, event->y)) {
                if (modifyCurrentTransition()) {
                    mCurrentTransition = 0;
                    queueRedraw();
                } else {
                    mCurrentTransition = 0;
                }
            }
        } else if (event->button == 2) {
        }
        break;
    case ADD_STATE:
        if (event->type == GDK_BUTTON_PRESS and event->button == 1) {
            addState(event->x, event->y);
            queueRedraw();
        }
        break;
    case ADD_TRANSITION:
        if (event->type == GDK_BUTTON_PRESS and event->button == 1) {
            if (not (event->state & GDK_CONTROL_MASK) and
                selectState(event->x, event->y, false)) {
                mBegin = searchAnchor(*mCurrentStates.begin(),
                        event->x, event->y);
                mStartPoint.x = event->x;
                mStartPoint.y = event->y;
                mStartState = mCurrentStates.front();
            } else {
                makeBreakpoint(event->x, event->y,
                    event->state & GDK_CONTROL_MASK);
            }
        }
        break;
    case DELETE:
        if (selectState(event->x, event->y, false)) {
            if (Question(_("Are you sure to delete state?"))) {
                mStatechart->removeState(*mCurrentStates.begin());
            }
            queueRedraw();
        } else if (selectTransition(event->x, event->y)) {
            if (Question(_("Are you sure to delete transition?"))) {
                mStatechart->removeTransition(mCurrentTransition);
            }
            queueRedraw();
        }
        break;
    default:
        break;
    };
    return true;
}

bool StatechartDrawingArea::on_button_release_event(GdkEventButton* event)
{
    switch (mState) {
    case SELECT:
        mPreviousX = -1;
        mPreviousY = -1;
        mStatechartResize = false;
        break;
    case ADD_TRANSITION:
        if (event->button == 1) {
            if (mCurrentTransition) {
                if (mBreakpoint) {
                    removeBreakpoint();
                    mBreakpoint = 0;
                } else if (mFirstBreakpoint) {
                    mFirstBreakpoint = 0;
                } else if (mLastBreakpoint) {
                    mLastBreakpoint = 0;
                }
            } else if (not mCurrentStates.empty() and mStartPoint.x !=
                       event->x
                       and mStartPoint.y != event->y) {
                if (selectState(event->x, event->y, false)) {
                    State* dst = *mCurrentStates.begin();
                    points_t pts;

                    if (mStartState == dst) {
                        pts.push_back(mBegin);
                    } else {
                        point_t anchor = searchAnchor(dst,
	event->x,
	event->y);

                        pts.push_back(mBegin);
                        pts.push_back(anchor);
                    }
                    mStatechart->addTransition(mStartState->name(),
                        dst->name(), pts);
                }
            }

            mMouse.invalid();
            mBegin.invalid();
            mStartPoint.invalid();
            mCurrentStates.clear();
            mCurrentTransition = 0;
            mStartState = 0;
            queueRedraw();
            break;
        }
    default:
        break;
    }
    return true;
}

bool StatechartDrawingArea::on_configure_event(GdkEventConfigure* event)
{
    bool change = false;

    if (event->width > mWidth) {
        change = true;
        mWidth = event->width;
    }
    if (event->height > mHeight) {
        change = true;
        mHeight = event->height;
    }
    if (change and mIsRealized) {
        set_size_request(mWidth, mHeight);
        mBuffer = Gdk::Pixmap::create(mWin, mWidth, mHeight, -1);
        queueRedraw();
    }
    return true;
}

bool StatechartDrawingArea::on_expose_event(GdkEventExpose*)
{
    if (mIsRealized) {
        if (!mBuffer) {
            mBuffer = Gdk::Pixmap::create(mWin, mWidth, mHeight, -1);
        }
        if (mBuffer) {
            if (mNeedRedraw) {
                mContext = mBuffer->create_cairo_context();
                mContext->set_line_width(Settings::settings().getLineWidth());
                draw();
                mNeedRedraw = false;
            }
            mWin->draw_drawable(mWingc, mBuffer, 0, 0, 0, 0, -1, -1);
        }
    }
    return true;
}

bool StatechartDrawingArea::on_motion_notify_event(GdkEventMotion* event)
{
    int button;

    if (event->state & GDK_BUTTON1_MASK) {
        button = 1;
    } else if (event->state & GDK_BUTTON2_MASK) {
        button = 2;
    } else {
        button = 0;
    }

    switch (mState) {
    case SELECT:
        if (button == 1) {
            if (not mCurrentStates.empty()) {
                bool xok, yok;

                displaceStates(mPreviousX == -1 ? event->x : mPreviousX,
                    mPreviousY == -1 ? event->y : mPreviousY,
                    event->x, event->y, xok, yok);
                if (xok) {
                    mPreviousX = event->x;
                }
                if (yok) {
                    mPreviousY = event->y;
                }
                queueRedraw();
            } else if (mStatechartResize) {
                bool xok, yok;

                resizeStatechart(mPreviousX == -1 ? event->x : mPreviousX,
                    mPreviousY == -1 ? event->y : mPreviousY,
                    event->x, event->y, xok, yok);
                if (xok) {
                    mPreviousX = event->x;
                }
                if (yok) {
                    mPreviousY = event->y;
                }
                queueRedraw();
            }
        }
        break;
    case ADD_TRANSITION:
        if (button == 1) {
            if (mCurrentTransition) {
                if (mBreakpoint) {
                    mBreakpoint->x = event->x;
                    mBreakpoint->y = event->y;
                } else if (mFirstBreakpoint) {
                    point_t pt = searchAnchor(mCurrentStates.front(),
                            event->x, event->y);

                    if (pt != *mFirstBreakpoint) {
                        *mFirstBreakpoint = pt;
                    }
                } else if (mLastBreakpoint) {
                    point_t pt = searchAnchor(mCurrentStates.front(),
                            event->x, event->y);

                    if (pt != *mLastBreakpoint) {
                        *mLastBreakpoint = pt;
                    }
                }
            } else if (mBegin.valid()) {
                if (selectState(event->x, event->y, false)) {
                    mEnd = searchAnchor(mCurrentStates.front(),
                            event->x, event->y);
                }
            }
        } else if (button == 0) {
            mCurrentStates.clear();
            if (not selectTransition(event->x, event->y)) {
                mCurrentTransition = 0;
                selectState(event->x, event->y, false);
            }
        }
        mMouse.x = event->x;
        mMouse.y = event->y;
        queueRedraw();
        break;
    default:
        break;
    }
    return true;
}

void StatechartDrawingArea::on_realize()
{
    Gtk::DrawingArea::on_realize();
    mWin = get_window();
    mWingc = Gdk::GC::create(mWin);
    mIsRealized = true;
    queueRedraw();
}

void StatechartDrawingArea::removeBreakpoint()
{
    points_t::iterator it = mCurrentTransition->points().begin();

    if (*it != *mBreakpoint) {
        points_t::iterator itb;
        guint xs = it->x;
        guint ys = it->y;
        bool found = false;

        ++it;
        while (it != mCurrentTransition->points().end() and not found) {
            found = (*it == *mBreakpoint);
            if (found) {
                itb = it;
            }
            ++it;
        }
        if (it != mCurrentTransition->points().end()) {
            guint xd = it->x;
            guint yd = it->y;
            double a = (ys - yd) / (double)(xs - xd);
            double b = ys - a * xs;
            double h = std::abs(
                    (mBreakpoint->y - (a * mBreakpoint->x) - b) /
                    std::sqrt(1 + a * a));

            if (h < 5) {
                mCurrentTransition->points().erase(itb);
            }
        }
    }
}

void StatechartDrawingArea::resizeStatechart(int oldx, int oldy,
    int newx, int newy,
    bool& xok, bool& yok)
{
    int deltax = newx - oldx;
    int deltay = newy - oldy;

    xok = true;
    yok = true;
    if (deltax != 0 or deltay != 0) {
        for (states_t::const_iterator it = mStatechart->states().begin();
             it != mStatechart->states().end(); ++it) {
            if ((int)(it->second->x() + it->second->width() + OFFSET) >
                mStatechart->width() + deltax) {
                deltax = 0;
                xok = false;
            }
            if ((int)(it->second->y() + it->second->height() + OFFSET) >
                mStatechart->height() + deltay) {
                deltay = 0;
                yok = false;
            }
        }
    }
    if (mStatechart->width() + deltax < Statechart::MINIMAL_WIDTH) {
        deltax = 0;
        xok = false;
    }
    if (mStatechart->height() + deltay < Statechart::MINIMAL_HEIGHT) {
        deltay = 0;
        yok = false;
    }
    mStatechart->resize(deltax, deltay);
}

point_t StatechartDrawingArea::searchAnchor(const State* state,
    guint x, guint y)
{
    const points_t& anchors = state->anchors();
    point_t anchor;
    double min = std::numeric_limits < double >::max();

    for (points_t::const_iterator it = anchors.begin(); it != anchors.end();
         ++it) {
        double deltax = (double)x - it->x;
        double deltay = (double)y - it->y;
        double dst = std::sqrt(deltax * deltax + deltay * deltay);

        if (dst < min) {
            anchor = *it;
            min = dst;
        }
    }
    return anchor;
}

bool StatechartDrawingArea::selectState(guint x, guint y, bool ctrl)
{
    bool found = false;
    states_t::const_iterator it = mStatechart->states().begin();

    while (not found and it != mStatechart->states().end()) {
        found = it->second->select(x, y);
        if (not found) {
            ++it;
        }
    }
    if (found) {
        if (not ctrl) {
            mCurrentStates.clear();
        }
        if (std::find(mCurrentStates.begin(), mCurrentStates.end(),
                it->second) == mCurrentStates.end()) {
            mCurrentStates.push_back(it->second);
        }
    } else {
        mCurrentStates.clear();
        queueRedraw();
    }
    return found;
}

bool StatechartDrawingArea::selectTransition(guint mx, guint my)
{
    transitions_t::const_iterator itt = mStatechart->transitions().begin();
    bool found = false;

    while (itt != mStatechart->transitions().end() and not found) {
        if ((*itt)->source() == (*itt)->destination()) {
            State* state = mStatechart->state((*itt)->source());
            const point_t& pt = (*itt)->points().front();
            int x = (int)mx - pt.x;
            int y = (int)my - pt.y;

            if (state->x() == pt.x
                and x >= -(int)SELF_TRANSITION_SIZE and x + y <= 0
                and x - y <= 0) { // left
                found = true;
                mCurrentTransition = *itt;
            } else if (state->x() + state->width() == pt.x
                       and x <= (int)SELF_TRANSITION_SIZE and x + y >= 0
                       and x - y >= 0) { // right
                found = true;
                mCurrentTransition = *itt;
            } else if (state->y() == pt.y
                       and y >= -(int)SELF_TRANSITION_SIZE and x + y <= 0
                       and x - y >= 0) { // top
                found = true;
                mCurrentTransition = *itt;
            } else if (state->y() + state->height() == pt.y
                       and y <= (int)SELF_TRANSITION_SIZE and x + y >= 0
                       and x - y <= 0) { // bottom
                found = true;
                mCurrentTransition = *itt;
            }
        } else {
            int xs2, ys2;
            points_t::iterator it = (*itt)->points().begin();

            xs2 = it->x;
            ys2 = it->y;
            ++it;
            while (it != (*itt)->points().end() and not found) {
                int xs, ys, xd, yd;

                if (xs2 == it->x) {
                    xs = xs2 - 5;
                    xd = it->x + 5;
                } else {
                    xs = xs2;
                    xd = it->x;
                }

                if (ys2 == it->y) {
                    ys = ys2 - 5;
                    yd = it->y + 5;
                } else {
                    ys = ys2;
                    yd = it->y;
                }

                double h = -1;
                if (std::min(xs,
                        xd) <= (int)mx and (int)mx <= std::max(xs, xd)
                    and std::min(ys, yd) <= (int)my
                    and (int)my <= std::max(ys, yd)) {
                    const double a = (ys - yd) / (double)(xs - xd);
                    const double b = ys - a * xs;
                    h = std::abs((my - (a * mx) - b) / std::sqrt(1 + a * a));
                    if (h <= 10) {
                        mCurrentTransition = *itt;
                        found = true;
                    }
                }
                if (not found) {
                    xs2 = it->x;
                    ys2 = it->y;
                    ++it;
                }
            }
        }
        if (not found) {
            ++itt;
        }
    }
    return found;
}

void StatechartDrawingArea::setColor(const Gdk::Color& color)
{
    mContext->set_source_rgb(color.get_red_p(),
        color.get_green_p(),
        color.get_blue_p());
}

}
}
}
}    // namespace vle gvle modeling fsa
