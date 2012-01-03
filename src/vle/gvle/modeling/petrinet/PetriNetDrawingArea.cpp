/*
 * @file vle/gvle/modeling/petrinet/PetriNetDrawingArea.cpp
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


#include <vle/gvle/modeling/petrinet/PetriNetDrawingArea.hpp>
#include <vle/gvle/modeling/petrinet/ArcDialog.hpp>
#include <vle/gvle/modeling/petrinet/PlaceDialog.hpp>
#include <vle/gvle/modeling/petrinet/TransitionDialog.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/Settings.hpp>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/radioaction.h>
#include <gtkmm/stock.h>
#include <vle/utils/i18n.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <limits>

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

namespace vle {
namespace gvle {
namespace modeling {
namespace petrinet {

const guint PetriNetDrawingArea::OFFSET = 10;
const guint PetriNetDrawingArea::TRANSITION_HEIGHT = 10;
const guint PetriNetDrawingArea::TRANSITION_WIDTH = 30;
const guint PetriNetDrawingArea::PLACE_RADIUS = 20;

PetriNetDrawingArea::PetriNetDrawingArea(
    BaseObjectType* cobject,
    const Glib::RefPtr < Gnome::Glade::Xml >& xml) :
    Gtk::DrawingArea(cobject),
    mXml(xml),
    mIsRealized(false),
    mNeedRedraw(true),
    mPetriNet(0),
    mState(SELECT),
    mHeight(300 + 2 * OFFSET),
    mWidth(400 + 2 * OFFSET),
    mPreviousX(-1),
    mPreviousY(-1),
    mBreakpoint(0),
    mCurrentArc(0),
    mPetriNetResize(false),
    mStartPlace(0),
    mStartTransition(0)
{
    set_events(Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_MOTION_MASK |
        Gdk::BUTTON1_MOTION_MASK | Gdk::BUTTON2_MOTION_MASK |
        Gdk::BUTTON3_MOTION_MASK | Gdk::BUTTON_PRESS_MASK |
        Gdk::BUTTON_RELEASE_MASK);
    initMenuPopupModels();
}

void PetriNetDrawingArea::addPlace(guint x, guint y)
{
    PlaceDialog dialog(mXml, *mPetriNet);

    if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
        int newWidth = x + PLACE_RADIUS + OFFSET;
        int newHeight = y + PLACE_RADIUS + OFFSET;

        try {
            Place* place = mPetriNet->addPlace(
                    dialog.name(), dialog.output(),
                    boost::lexical_cast < unsigned int > (dialog.
                        initialMarking()),
                    x, y, PLACE_RADIUS);

            if (dialog.delay()) {
                place->delayValue(
                    boost::lexical_cast < double > (dialog.delayValue()));
            }
            place->delay(dialog.delay());
            if (not dialog.outputPortName().empty()) {
                place->outputPortName(dialog.outputPortName());
            } else {
                place->output(false);
            }

            if (newWidth > mPetriNet->width()) {
                mPetriNet->width(newWidth);
                mWidth = mPetriNet->width() + OFFSET;
            }
            if (newHeight > mPetriNet->height()) {
                mPetriNet->height(newHeight);
                mHeight = mPetriNet->height() + OFFSET;
            }
        } catch (...) {
        }
    }
}

void PetriNetDrawingArea::addTransition(guint x, guint y)
{
    TransitionDialog dialog(mXml, *mPetriNet);

    if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
        int newWidth = x + TRANSITION_WIDTH + OFFSET;
        int newHeight = y + TRANSITION_HEIGHT + OFFSET;
        Transition* transition = mPetriNet->addTransition(
                dialog.name(), dialog.input(), dialog.output(),
                x, y, TRANSITION_WIDTH, TRANSITION_HEIGHT);

        if (dialog.delay()) {
            transition->delayValue(
                boost::lexical_cast < double > (dialog.delayValue()));
        }
        transition->delay(dialog.delay());
        transition->priority(
            boost::lexical_cast < unsigned int > (dialog.priority()));
        if (not dialog.inputPortName().empty()) {
            transition->inputPortName(dialog.inputPortName());
        } else {
            transition->input(false);
        }
        if (not dialog.outputPortName().empty()) {
            transition->outputPortName(dialog.outputPortName());
        } else {
            transition->output(false);
        }
        if (newWidth > mPetriNet->width()) {
            mPetriNet->width(newWidth);
            mWidth = mPetriNet->width() + OFFSET;
        }
        if (newHeight > mPetriNet->height()) {
            mPetriNet->height(newHeight);
            mHeight = mPetriNet->height() + OFFSET;
        }
    }
}

void PetriNetDrawingArea::exportPng(const std::string& filename)
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

void PetriNetDrawingArea::exportPdf(const std::string& filename)
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

void PetriNetDrawingArea::exportSvg(const std::string& filename)
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

void PetriNetDrawingArea::exportGraphic()
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

void PetriNetDrawingArea::initMenuPopupModels()
{
    Gtk::Menu::MenuList& menulist(mMenuPopup.items());

    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem(
            _("_Export Graphic"),
            sigc::mem_fun(
                *this,
                &PetriNetDrawingArea::exportGraphic)));

    mMenuPopup.accelerate(*this);
}

points_t PetriNetDrawingArea::computeMarking(const Place* place)
{
    points_t pts;
    double delta = place->radius() / 3.;

    if (place->initialMarking() == 1) {
        pts.push_back(point_t(place->x(), place->y()));
    } else if (place->initialMarking() == 2) {
        pts.push_back(point_t(place->x() - delta, place->y()));
        pts.push_back(point_t(place->x() + delta, place->y()));
    } else if (place->initialMarking() == 3) {
        pts.push_back(point_t(place->x() - delta,
                place->y() + delta));
        pts.push_back(point_t(place->x() + delta,
                place->y() + delta));
        pts.push_back(point_t(place->x(),
                place->y() - delta));
    } else if (place->initialMarking() == 4 or
               place->initialMarking() == 5) {
        pts.push_back(point_t(place->x() - delta,
                place->y() - delta));
        pts.push_back(point_t(place->x() + delta,
                place->y() - delta));
        pts.push_back(point_t(place->x() - delta,
                place->y() + delta));
        pts.push_back(point_t(place->x() + delta,
                place->y() + delta));
        if (place->initialMarking() == 5) {
            pts.push_back(point_t(place->x(), place->y()));
        }
    }

    return pts;
}

void PetriNetDrawingArea::displacePlaces(int oldx, int oldy,
    int newx, int newy,
    bool& xok, bool& yok)
{
    int deltax = newx - oldx;
    int deltay = newy - oldy;
    bool change = false;

    xok = false;
    yok = false;
    if (deltax != 0 or deltay != 0) {
        for (std::vector < Place* >::const_iterator it =
                 mCurrentPlaces.begin();
             it != mCurrentPlaces.end(); ++it) {
            int _deltax = deltax;
            int _deltay = deltay;

            if ((*it)->x() + deltax < (int)(2 * OFFSET)) {
                _deltax = 0;
            } else {
                xok = true;
            }
            if ((*it)->y() + deltay < (int)(2 * OFFSET)) {
                _deltay = 0;
            } else {
                yok = true;
            }

            mPetriNet->displace(*it, _deltax, _deltay);

            {
                int newWidth = (*it)->x() + (*it)->radius();
                int newHeight = (*it)->y() + (*it)->radius();

                if (newWidth > mPetriNet->width()) {
                    mPetriNet->width(newWidth);
                    mWidth = mPetriNet->width() + OFFSET;
                    change = true;
                }
                if (newHeight > mPetriNet->height()) {
                    mPetriNet->height(newHeight);
                    mHeight = mPetriNet->height() + OFFSET;
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

void PetriNetDrawingArea::displaceTransitions(int oldx, int oldy,
    int newx, int newy,
    bool& xok, bool& yok)
{
    int deltax = newx - oldx;
    int deltay = newy - oldy;
    bool change = false;

    xok = false;
    yok = false;
    if (deltax != 0 or deltay != 0) {
        for (std::vector < Transition* >::const_iterator it =
                 mCurrentTransitions.begin();
             it != mCurrentTransitions.end(); ++it) {
            int _deltax = deltax;
            int _deltay = deltay;

            if ((*it)->x() + deltax < (int)(2 * OFFSET)) {
                _deltax = 0;
            } else {
                xok = true;
            }
            if ((*it)->y() + deltay < (int)(2 * OFFSET)) {
                _deltay = 0;
            } else {
                yok = true;
            }

            mPetriNet->displace(*it, _deltax, _deltay);

            {
                int newWidth = (*it)->x() + (*it)->width();
                int newHeight = (*it)->y() + (*it)->height();

                if (newWidth > mPetriNet->width()) {
                    mPetriNet->width(newWidth);
                    mWidth = mPetriNet->width() + OFFSET;
                    change = true;
                }
                if (newHeight > mPetriNet->height()) {
                    mPetriNet->height(newHeight);
                    mHeight = mPetriNet->height() + OFFSET;
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

void PetriNetDrawingArea::draw()
{
    if (mIsRealized and mBuffer) {
        mContext->save();

        mContext->set_line_width(Settings::settings().getLineWidth());

        drawBackground();
        drawPlaces();
        drawTransitions();
        drawArcs();

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

        if (mCurrentArc) {
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

void PetriNetDrawingArea::drawAnchors(Place* place)
{
    const points_t& points = place->anchors();

    mContext->set_source_rgb(0., 0., 1.);
    for (points_t::const_iterator it = points.begin(); it != points.end();
         ++it) {
        mContext->rectangle(it->x - 2, it->y - 2, 5, 5);
        mContext->fill();
    }
}

void PetriNetDrawingArea::drawAnchors(Transition* transition)
{
    const points_t& points = transition->anchors();

    mContext->set_source_rgb(0., 0., 1.);
    for (points_t::const_iterator it = points.begin(); it != points.end();
         ++it) {
        mContext->rectangle(it->x - 2, it->y - 2, 5, 5);
        mContext->fill();
    }
}

void PetriNetDrawingArea::drawArc(Arc* arc)
{
    points_t::const_iterator it = arc->points().begin();
    double xs = it->x;
    double ys = it->y;
    double xd = (++it)->x;
    double yd = it->y;

    if (arc->weight() != 1) {
        drawArcText(arc, xs, ys, xd, yd);
    }

    mContext->move_to(xs, ys);
    if (arc->inhibitor() and * it == arc->points().back()) {
        drawInhibitor(xs, ys, xd, yd);
    } else {
        while (it != arc->points().end()) {
            mContext->line_to(xd, yd);
            ++it;
            if (it != arc->points().end()) {
                xs = xd;
                ys = yd;
                xd = it->x;
                yd = it->y;
                if (arc->inhibitor() and
                    * it == arc->points().back()) {
                    drawInhibitor(xs, ys, xd, yd);
                    break;
                }
            }
        }
    }

    if (not arc->inhibitor()) {
        mContext->stroke();
    }
    drawArrow(xs, ys, xd, yd);
}

void PetriNetDrawingArea::drawArcs()
{
    mContext->set_source_rgb(0., 0., 0.);
    for (arcs_t::const_iterator it = mPetriNet->arcs().begin();
         it != mPetriNet->arcs().end(); ++it) {
        drawArc(*it);
    }
}

void PetriNetDrawingArea::drawArcText(Arc* arc,
    int xs, int ys,
    int xd, int yd)
{
    std::string str((fmt("%1%") % arc->weight()).str());
    double d = std::sqrt((xd - xs) * (xd - xs) + (yd - ys) * (yd - ys));


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

void PetriNetDrawingArea::drawArrow(int xs, int ys, int xd, int yd)
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

void PetriNetDrawingArea::drawBackground()
{
    setColor(Settings::settings().getBackgroundColor());
    mContext->rectangle(0, 0, mWidth, mHeight);
    mContext->fill();
    mContext->stroke();
}

void PetriNetDrawingArea::drawBreakpoints()
{
    points_t::const_iterator it = mCurrentArc->points().begin();

    mContext->set_source_rgb(0., 0., 1.);
    while (it != mCurrentArc->points().end()) {
        mContext->rectangle(it->x - 2, it->y - 2, 5, 5);
        mContext->fill();
        ++it;
    }
}

void PetriNetDrawingArea::drawCurrentBreakpoint()
{
    mContext->set_source_rgb(1., 0., 0.);
    mContext->rectangle(mBreakpoint->x - 2, mBreakpoint->y - 2, 5, 5);
    mContext->fill();
}

void PetriNetDrawingArea::drawInhibitor(double /* xs */,
    double ys,
    double xd,
    double& yd)
{
    if (ys < yd) {
        mContext->line_to(xd, yd - 6);
        mContext->stroke();
        mContext->arc(xd, yd - 3, 3, 0, 2 * M_PI);
        mContext->stroke();
        yd -= 6;
    } else {
        mContext->line_to(xd, yd + 6);
        mContext->stroke();
        mContext->arc(xd, yd + 3, 3, 0, 2 * M_PI);
        mContext->stroke();
        yd += 6;
    }
}

void PetriNetDrawingArea::drawPlace(Place* place)
{
    if (std::find(mCurrentPlaces.begin(), mCurrentPlaces.end(), place) ==
        mCurrentPlaces.end()) {
        setColor(Settings::settings().getAtomicColor());
    } else {
        setColor(Settings::settings().getSelectedColor());
    }

    mContext->arc(place->x(), place->y(), place->radius(), 0, 2 * M_PI);
    mContext->stroke();

    if (place->output()) {
        mContext->arc(place->x(), place->y(),
            place->radius() - 2, 0, 2 * M_PI);
        mContext->stroke();
    }

    if (place->initialMarking() > 0) {
        if (place->initialMarking() <= 5) {
            points_t pts = computeMarking(place);

            for (points_t::const_iterator it = pts.begin(); it != pts.end();
                 ++it) {
                mContext->arc(it->x, it->y, 3, 0, 2 * M_PI);
                mContext->fill();
                mContext->stroke();
            }
        } else {
            mContext->arc(place->x(), place->y(), 3, 0, 2 * M_PI);
            mContext->fill();
            mContext->stroke();

            mContext->move_to(place->x() + 4,
                place->y() - 4);
            mContext->show_text((fmt("%1%") % place->initialMarking()).str());
            mContext->stroke();
        }
    }

    drawPlaceName(place);

    if (mState == ADD_ARC and not mCurrentPlaces.empty() and
        place == *mCurrentPlaces.begin()) {
        drawAnchors(place);
    }
}

void PetriNetDrawingArea::drawPlaceName(Place* place)
{
    setColor(Settings::settings().getForegroundColor());
    mContext->select_font_face(Settings::settings().getFont(),
        Cairo::FONT_SLANT_NORMAL,
        Cairo::FONT_WEIGHT_NORMAL);
    mContext->set_font_size(Settings::settings().getFontSize());

    std::string str(place->name());
    Cairo::TextExtents textExtents;

    if (place->output()) {
        str += (fmt(" [%1%]") % place->outputPortName()).str();
    }
    if (place->delay()) {
        str += (fmt(" (%1%)") % place->delayValue()).str();
    }
    mContext->get_text_extents(str, textExtents);
    mContext->move_to(place->x() + place->radius() + 2,
        place->y() + textExtents.height / 2);
    mContext->show_text(str);
    mContext->stroke();
}

void PetriNetDrawingArea::drawPlaces()
{
    for (places_t::const_iterator it = mPetriNet->places().begin();
         it != mPetriNet->places().end(); ++it) {
        drawPlace(it->second);
    }
}

void PetriNetDrawingArea::drawTransition(Transition* transition)
{
    if (std::find(mCurrentTransitions.begin(), mCurrentTransitions.end(),
            transition) == mCurrentTransitions.end()) {
        setColor(Settings::settings().getAtomicColor());
    } else {
        setColor(Settings::settings().getSelectedColor());
    }

    mContext->rectangle(transition->x(),
        transition->y(), transition->width(),
        transition->height());
    mContext->stroke();

    if (transition->input()) {
        mContext->rectangle(transition->x() + 2, transition->y() + 2,
            transition->width() - 4,
            transition->height() - 4);
        mContext->stroke();
    }

    if (transition->output()) {
        mContext->move_to(transition->x() + transition->width() / 2,
            transition->y());
        mContext->line_to(transition->x() + transition->width(),
            transition->y() + transition->height() / 2);
        mContext->line_to(transition->x() + transition->width() / 2,
            transition->y() + transition->height());
        mContext->line_to(transition->x(),
            transition->y() + transition->height() / 2);
        mContext->line_to(transition->x() + transition->width() / 2,
            transition->y());
        mContext->stroke();
    }

    drawTransitionName(transition);

    if (mState == ADD_ARC and not mCurrentTransitions.empty() and
        transition == *mCurrentTransitions.begin()) {
        drawAnchors(transition);
    }
}

void PetriNetDrawingArea::drawTransitionName(Transition* transition)
{
    setColor(Settings::settings().getForegroundColor());
    mContext->select_font_face(Settings::settings().getFont(),
        Cairo::FONT_SLANT_NORMAL,
        Cairo::FONT_WEIGHT_NORMAL);
    mContext->set_font_size(Settings::settings().getFontSize());

    std::string str(transition->name());
    Cairo::TextExtents textExtents;

    if (transition->delay()) {
        str += (fmt(" (%1%)") % transition->delayValue()).str();
    }
    if (transition->priority() > 0) {
        str += (fmt(" {%1%}") % transition->priority()).str();
    }
    if (transition->input()) {
        str += (fmt(" [%1%]") % transition->inputPortName()).str();
    }
    if (transition->output()) {
        str += (fmt(" [%1%]") % transition->outputPortName()).str();
    }
    mContext->get_text_extents(str, textExtents);
    mContext->move_to(transition->x() + transition->width() + 2,
        transition->y() + (transition->height() +
                           textExtents.height / 2));
    mContext->show_text(str);
    mContext->stroke();
}

void PetriNetDrawingArea::drawTransitions()
{
    for (transitions_t::const_iterator it = mPetriNet->transitions().begin();
         it != mPetriNet->transitions().end(); ++it) {
        drawTransition(it->second);
    }
}

void PetriNetDrawingArea::makeBreakpoint(guint mx, guint my, bool ctrl)
{
    arcs_t::const_iterator itt = mPetriNet->arcs().begin();
    bool found = false;

    mBreakpoint = 0;
    while (itt != mPetriNet->arcs().end() and not found) {
        int xs2, ys2;
        points_t::iterator it = (*itt)->points().begin();
        double deltax = (it->x + 2.) - mx;
        double deltay = (it->y + 2.) - my;

        if (std::sqrt(deltax * deltax + deltay * deltay) < 10) {
            mCurrentArc = *itt;
            if (ctrl) {
                Place* p = mPetriNet->place((*itt)->source());

                if (p) {
                    mCurrentPlaces.clear();
                    mCurrentPlaces.push_back(p);
                    mFirstBreakpoint = &(*it);
                } else {
                    Transition* t = mPetriNet->transition((*itt)->source());

                    if (t) {
                        mCurrentTransitions.clear();
                        mCurrentTransitions.push_back(t);
                        mFirstBreakpoint = &(*it);
                    }
                }
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
                    mCurrentArc = *itt;
                    mBreakpoint = &(*it);
                    ++it;
                    if (it == (*itt)->points().end()) {
                        if (ctrl) {
                            Place* p = mPetriNet->place((*itt)->source());

                            if (p) {
	mCurrentPlaces.clear();
	mCurrentPlaces.push_back(p);
	mLastBreakpoint = mBreakpoint;
                            } else {
	Transition* t =
	    mPetriNet->transition((*itt)->source());

	if (t) {
	    mCurrentTransitions.clear();
	    mCurrentTransitions.push_back(t);
	    mLastBreakpoint = mBreakpoint;
	}
                            }
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
                            mCurrentArc = *itt;
                            mBreakpoint = &(*mCurrentArc->addPoint(it, pt));
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

bool PetriNetDrawingArea::modifyCurrentArc()
{
    bool result = false;

    if (mCurrentArc) {
        ArcDialog dialog(mXml, mCurrentArc);

        if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
            try {
                mCurrentArc->weight(boost::lexical_cast < unsigned int > (
                        dialog.weight()));
                mCurrentArc->inhibitor(dialog.inhibitor());
                result = true;
            } catch (...) {
            }
        }
    }
    return result;
}

bool PetriNetDrawingArea::modifyCurrentPlace()
{
    bool result = false;

    if (not mCurrentPlaces.empty()) {
        Place* place = *mCurrentPlaces.begin();
        std::string oldName = place->name();
        PlaceDialog dialog(mXml, *mPetriNet, place);

        if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
            try {
                place->initialMarking(
                    boost::lexical_cast < unsigned int > (
                        dialog.initialMarking()));
                place->name(dialog.name());
                if (dialog.delay()) {
                    place->delayValue(
                        boost::lexical_cast < double > (dialog.delayValue()));
                }
                place->delay(dialog.delay());
                place->output(dialog.output());
                if(oldName != dialog.name()){
                    mPetriNet->changePlaceName(oldName, dialog.name());
                }
                if (not dialog.outputPortName().empty()) {
                    place->outputPortName(dialog.outputPortName());
                } else {
                    place->output(false);
                }
                result = true;
            } catch (...) {
            }
        }
    }
    return result;
}

bool PetriNetDrawingArea::modifyCurrentTransition()
{
    bool result = false;

    if (not mCurrentTransitions.empty()) {
        Transition* transition = *mCurrentTransitions.begin();
        std::string oldName = transition->name();
        TransitionDialog dialog(mXml, *mPetriNet, transition);

        if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
            try {
                if (dialog.delay()) {
                    transition->delayValue(
                        boost::lexical_cast < double > (dialog.delayValue()));
                }
                transition->delay(dialog.delay());
                transition->name(dialog.name());
                transition->input(dialog.input());
                transition->output(dialog.output());
                if(oldName != dialog.name()){
                    mPetriNet->changeTransitionName(oldName, dialog.name());
                }
                if (not dialog.inputPortName().empty()) {
                    transition->inputPortName(dialog.inputPortName());
                } else {
                    transition->input(false);
                }
                if (not dialog.outputPortName().empty()) {
                    transition->outputPortName(dialog.outputPortName());
                } else {
                    transition->output(false);
                }
                transition->priority(
                    boost::lexical_cast < unsigned int > (dialog.priority()));
                result = true;
            } catch (...) {
            }
        }
    }
    return result;
}

bool PetriNetDrawingArea::on_button_press_event(GdkEventButton* event)
{
    if (event->button == 3) {
        mMenuPopup.popup(event->button, event->time);
    }

    switch (mState)
    {
    case SELECT:
        if (event->type == GDK_BUTTON_PRESS and event->button == 1) {
            if (not selectPlace(event->x, event->y,
                    event->state & GDK_CONTROL_MASK)) {
                if (not selectTransition(event->x, event->y,
                        event->state & GDK_CONTROL_MASK)) {
                }
            }
        } else if (event->type == GDK_2BUTTON_PRESS and event->button ==
                   1) {
            if (selectPlace(event->x, event->y, false)) {
                if (modifyCurrentPlace()) {
                    queueRedraw();
                }
            } else if (selectTransition(event->x, event->y, false)) {
                if (modifyCurrentTransition()) {
                    queueRedraw();
                }
            } else if (selectArc(event->x, event->y)) {
                if (modifyCurrentArc()) {
                    mCurrentArc = 0;
                    queueRedraw();
                } else {
                    mCurrentArc = 0;
                }
            }
        } else if (event->button == 2) {
        }
        break;
    case ADD_PLACE:
        if (event->type == GDK_BUTTON_PRESS and event->button == 1) {
            addPlace(event->x, event->y);
            queueRedraw();
        }
        break;
    case ADD_TRANSITION:
        if (event->type == GDK_BUTTON_PRESS and event->button == 1) {
            addTransition(event->x, event->y);
            queueRedraw();
        }
        break;
    case ADD_ARC:
        if (event->type == GDK_BUTTON_PRESS and event->button == 1) {
            if (not (event->state & GDK_CONTROL_MASK) and
                selectPlace(event->x, event->y, false)) {
                mBegin = searchAnchor(*mCurrentPlaces.begin(),
                        event->x, event->y);
                mStartPoint.x = event->x;
                mStartPoint.y = event->y;
                mStartPlace = mCurrentPlaces.front();
            } else if (not (event->state & GDK_CONTROL_MASK) and
                       selectTransition(event->x, event->y, false)) {
                mBegin = searchAnchor(*mCurrentTransitions.begin(),
                        event->x, event->y);
                mStartPoint.x = event->x;
                mStartPoint.y = event->y;
                mStartTransition = mCurrentTransitions.front();
            } else {
                makeBreakpoint(event->x, event->y,
                    event->state & GDK_CONTROL_MASK);
            }
        }
        break;
    case DELETE:
        if (selectPlace(event->x, event->y, false)) {
            if (Question(_("Are you sure to delete place?"))) {
                mPetriNet->removePlace(*mCurrentPlaces.begin());
            }
            queueRedraw();
        } else if (selectTransition(event->x, event->y, false)) {
            if (Question(_("Are you sure to delete transition?"))) {
                mPetriNet->removeTransition(*mCurrentTransitions.begin());
            }
            queueRedraw();
        } else if (selectArc(event->x, event->y)) {
            if (Question(_("Are you sure to delete arc?"))) {
                mPetriNet->removeArc(mCurrentArc);
                mCurrentArc = 0;
            }
            queueRedraw();
        }
        break;
    default:
        break;
    };
    return true;
}

bool PetriNetDrawingArea::on_button_release_event(GdkEventButton* event)
{
    switch (mState) {
    case SELECT:
        mPreviousX = -1;
        mPreviousY = -1;
        mPetriNetResize = false;
        break;
    case ADD_ARC:
        if (mCurrentArc) {
            if (mBreakpoint) {
                removeBreakpoint();
                mBreakpoint = 0;
            } else if (mFirstBreakpoint) {
                mFirstBreakpoint = 0;
            } else if (mLastBreakpoint) {
                mLastBreakpoint = 0;
            }
        } else if (mStartPlace and mStartPoint.x != event->x and
                   mStartPoint.y != event->y) {
            if (selectTransition(event->x, event->y, false)) {
                Transition* dst = *mCurrentTransitions.begin();
                points_t pts;
                point_t anchor = searchAnchor(dst, event->x, event->y);

                pts.push_back(mBegin);
                pts.push_back(anchor);
                mPetriNet->addArc(mStartPlace->name(),
                    dst->name(), pts);
            }
        } else if (mStartTransition and mStartPoint.x != event->x and
                   mStartPoint.y != event->y) {
            if (selectPlace(event->x, event->y, false)) {
                Place* dst = *mCurrentPlaces.begin();
                points_t pts;
                point_t anchor = searchAnchor(dst, event->x, event->y);

                pts.push_back(mBegin);
                pts.push_back(anchor);
                mPetriNet->addArc(mStartTransition->name(),
                    dst->name(), pts);
            }
        }

        mMouse.invalid();
        mBegin.invalid();
        mStartPoint.invalid();
        mCurrentPlaces.clear();
        mCurrentTransitions.clear();
        mCurrentArc = 0;
        mStartPlace = 0;
        mStartTransition = 0;
        queueRedraw();
        break;
    default:
        break;
    }
    return true;
}

bool PetriNetDrawingArea::on_configure_event(GdkEventConfigure* event)
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

bool PetriNetDrawingArea::on_expose_event(GdkEventExpose*)
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

bool PetriNetDrawingArea::on_motion_notify_event(GdkEventMotion* event)
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
            if (not mCurrentPlaces.empty()) {
                bool xok, yok;

                displacePlaces(mPreviousX == -1 ? event->x : mPreviousX,
                    mPreviousY == -1 ? event->y : mPreviousY,
                    event->x, event->y, xok, yok);
                if (xok) {
                    mPreviousX = event->x;
                }
                if (yok) {
                    mPreviousY = event->y;
                }
                queueRedraw();
            } else if (not mCurrentTransitions.empty()) {
                bool xok, yok;

                displaceTransitions(
                    mPreviousX == -1 ? event->x : mPreviousX,
                    mPreviousY == -1 ? event->y : mPreviousY,
                    event->x,
                    event->y,
                    xok,
                    yok);
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
    case ADD_ARC:
        if (button == 1) {
            if (mCurrentArc) {
                if (mBreakpoint) {
                    mBreakpoint->x = event->x;
                    mBreakpoint->y = event->y;
                } else if (mFirstBreakpoint) {
                    if (not mCurrentPlaces.empty()) {
                        point_t pt = searchAnchor(mCurrentPlaces.front(),
	event->x, event->y);

                        if (pt != *mFirstBreakpoint) {
                            *mFirstBreakpoint = pt;
                        }
                    } else if (not mCurrentTransitions.empty()) {
                        point_t pt = searchAnchor(
	mCurrentTransitions.front(),
	event->x,
	event->y);

                        if (pt != *mFirstBreakpoint) {
                            *mFirstBreakpoint = pt;
                        }
                    }
                } else if (mLastBreakpoint) {
                    if (not mCurrentPlaces.empty()) {
                        point_t pt = searchAnchor(mCurrentPlaces.front(),
	event->x, event->y);

                        if (pt != *mLastBreakpoint) {
                            *mLastBreakpoint = pt;
                        }
                    } else if (not mCurrentTransitions.empty()) {
                        point_t pt = searchAnchor(
	mCurrentTransitions.front(),
	event->x,
	event->y);

                        if (pt != *mLastBreakpoint) {
                            *mLastBreakpoint = pt;
                        }
                    }
                }
            } else if (mBegin.valid()) {
                if (selectPlace(event->x, event->y, false)) {
                    mEnd = searchAnchor(mCurrentPlaces.front(),
                            event->x, event->y);
                } else if (selectTransition(event->x, event->y, false)) {
                    mEnd = searchAnchor(mCurrentTransitions.front(),
                            event->x, event->y);
                }
            }
        } else if (button == 0) {
            mCurrentPlaces.clear();
            mCurrentTransitions.clear();
            if (not selectArc(event->x, event->y)) {
                mCurrentArc = 0;
                if (not selectPlace(event->x, event->y, false)) {
                    selectTransition(event->x, event->y, false);
                }
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

void PetriNetDrawingArea::on_realize()
{
    Gtk::DrawingArea::on_realize();
    mWin = get_window();
    mWingc = Gdk::GC::create(mWin);
    mIsRealized = true;
    queueRedraw();
}

void PetriNetDrawingArea::removeBreakpoint()
{
    points_t::iterator it = mCurrentArc->points().begin();

    if (*it != *mBreakpoint) {
        points_t::iterator itb;
        guint xs = it->x;
        guint ys = it->y;
        bool found = false;

        ++it;
        while (it != mCurrentArc->points().end() and not found) {
            found = (*it == *mBreakpoint);
            if (found) {
                itb = it;
            }
            ++it;
        }
        if (it != mCurrentArc->points().end()) {
            guint xd = it->x;
            guint yd = it->y;
            double a = (ys - yd) / (double)(xs - xd);
            double b = ys - a * xs;
            double h = std::abs(
                    (mBreakpoint->y - (a * mBreakpoint->x) - b) /
                    std::sqrt(1 + a * a));

            if (h < 5) {
                mCurrentArc->points().erase(itb);
            }
        }
    }
}

point_t PetriNetDrawingArea::searchAnchor(const Place* place,
    guint x, guint y)
{
    return searchAnchor(place->anchors(), x, y);
}

point_t PetriNetDrawingArea::searchAnchor(const Transition* transition,
    guint x, guint y)
{
    return searchAnchor(transition->anchors(), x, y);
}

point_t PetriNetDrawingArea::searchAnchor(const points_t& anchors,
    guint x, guint y)
{
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

bool PetriNetDrawingArea::selectArc(guint mx, guint my)
{
    arcs_t::const_iterator itt = mPetriNet->arcs().begin();
    bool found = false;

    while (itt != mPetriNet->arcs().end() and not found) {
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
            if (std::min(xs, xd) <= (int)mx and (int)mx <= std::max(xs, xd)
                and std::min(ys, yd) <= (int)my
                and (int)my <= std::max(ys, yd)) {
                const double a = (ys - yd) / (double)(xs - xd);
                const double b = ys - a * xs;
                h = std::abs((my - (a * mx) - b) / std::sqrt(1 + a * a));
                if (h <= 10) {
                    mCurrentArc = *itt;
                    found = true;
                }
            }
            if (not found) {
                xs2 = it->x;
                ys2 = it->y;
                ++it;
            }
        }
        if (not found) {
            ++itt;
        }
    }
    return found;
}

bool PetriNetDrawingArea::selectPlace(guint x, guint y, bool ctrl)
{
    bool found = false;
    places_t::const_iterator it = mPetriNet->places().begin();

    while (not found and it != mPetriNet->places().end()) {
        found = it->second->select(x, y);
        if (not found) {
            ++it;
        }
    }
    if (found) {
        mCurrentTransitions.clear();
        if (not ctrl) {
            mCurrentPlaces.clear();
        }
        if (std::find(mCurrentPlaces.begin(), mCurrentPlaces.end(),
                it->second) == mCurrentPlaces.end()) {
            mCurrentPlaces.push_back(it->second);
        }
    } else {
        mCurrentPlaces.clear();
        queueRedraw();
    }
    return found;
}

bool PetriNetDrawingArea::selectTransition(guint x, guint y, bool ctrl)
{
    bool found = false;
    transitions_t::const_iterator it = mPetriNet->transitions().begin();

    while (not found and it != mPetriNet->transitions().end()) {
        found = it->second->select(x, y);
        if (not found) {
            ++it;
        }
    }
    if (found) {
        mCurrentPlaces.clear();
        if (not ctrl) {
            mCurrentTransitions.clear();
        }
        if (std::find(mCurrentTransitions.begin(), mCurrentTransitions.end(),
                it->second) == mCurrentTransitions.end()) {
            mCurrentTransitions.push_back(it->second);
        }
    } else {
        mCurrentTransitions.clear();
        queueRedraw();
    }
    return found;
}

void PetriNetDrawingArea::setColor(const Gdk::Color& color)
{
    mContext->set_source_rgb(color.get_red_p(),
        color.get_green_p(),
        color.get_blue_p());
}

}
}
}
}    // namespace vle gvle modeling petrinet
