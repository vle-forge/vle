/**
 * @file vle/gvle/ViewBox.hpp
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


#ifndef GUI_VIEWBOX_HPP
#define GUI_VIEWBOX_HPP

#include <gtkmm.h>
#include <vle/vpz/Views.hpp>
#include <vle/vpz/View.hpp>
#include <vle/vpz/Outputs.hpp>

namespace vle
{
namespace gvle {

class ViewBox : public Gtk::Dialog
{
public:
    ViewBox(vpz::Views& views, vpz::View* view, vpz::Outputs* outputs);

    virtual ~ViewBox();

    void run();

private:
    vpz::Views& mViews;
    vpz::View* mView;
    vpz::Outputs* mOutputs;

    //Output
    Gtk::HBox* mHbox2;
    Gtk::HBox* mHbox2_1;
    Gtk::HBox* mHbox2_2;
    Gtk::Frame* mFrame;
    Gtk::VBox* mVbox;
    Gtk::Label* mLabel2;
    Gtk::Button* mAddOutput;
    Gtk::Button* mEdOutput;
    Gtk::Button* mDelOutput;
    Gtk::ComboBoxText* mOutput;

    //Type
    Gtk::HBox* mHbox3;
    Gtk::Label* mLabel3;
    Gtk::ComboBoxText* mType;

    //TimeStep
    Gtk::HBox* mHbox4;
    Gtk::Label* mLabel4;
    Gtk::Entry* mTimeStep;

    //Backup
    vpz::Outputs* mOutputs_backup;

    void makeOutputs();
    void makeType();
    void display_timestep();

    void add_output();
    void edit_output();
    void del_output();
};

}
} // namespace vle gvle

#endif
