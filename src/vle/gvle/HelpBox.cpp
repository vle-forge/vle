/**
 * @file vle/gvle/HelpBox.cpp
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


#include <vle/gvle/HelpBox.hpp>

namespace vle
{
namespace gvle {

HelpBox::HelpBox()
{
    mTextView = Gtk::manage(new class Gtk::TextView());
    mScrolledWindow = Gtk::manage(new class Gtk::ScrolledWindow());

    mTextView->set_flags(Gtk::CAN_FOCUS);
    mTextView->set_editable(true);
    mTextView->set_cursor_visible(true);
    mTextView->set_pixels_above_lines(0);
    mTextView->set_pixels_below_lines(0);
    mTextView->set_pixels_inside_wrap(0);
    mTextView->set_left_margin(0);
    mTextView->set_right_margin(0);
    mTextView->set_indent(0);
    mTextView->set_wrap_mode(Gtk::WRAP_NONE);
    mTextView->set_justification(Gtk::JUSTIFY_LEFT);
    mScrolledWindow->set_flags(Gtk::CAN_FOCUS);
    mScrolledWindow->set_shadow_type(Gtk::SHADOW_NONE);
    mScrolledWindow->set_policy(Gtk::POLICY_ALWAYS, Gtk::POLICY_ALWAYS);
    mScrolledWindow->property_window_placement().
    set_value(Gtk::CORNER_TOP_LEFT);
    mScrolledWindow->add(*mTextView);

    set_title("Help");
    set_modal(false);
    property_window_position().set_value(Gtk::WIN_POS_NONE);
    set_resizable(true);
    property_destroy_with_parent().set_value(false);
    add(*mScrolledWindow);
    set_default_size(300, 400);
    show_all();
}



HelpBox::~HelpBox()
{
}

}} // namespace vle gvle
