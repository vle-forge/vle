/**
 * @file vle/gvle/ExecutionBox.hpp
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


#ifndef GUI_EXECUTIONBOX_HPP
#define GUI_EXECUTIONBOX_HPP

#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <gtkmm/button.h>
#include <gtkmm/box.h>
#include <gtkmm/frame.h>

namespace vle
{
namespace gvle {

class GVLE;

/**
 * @brief A Gtk::Dialog to launch a VLE simulator with the current VLE
 * project.
 */
class ExecutionBox : public Gtk::Dialog
{
public:
    ExecutionBox(const std::string & projectFilename);

    virtual ~ExecutionBox() { }

private:
    Gtk::Label*             mLabelName;
    Gtk::Entry*             mEntryName;
    Gtk::Button*            mButtonName;
    Gtk::HBox*              mHBoxName;
    Gtk::Frame*             mFrameName;
    Gtk::RadioButton*       mLocalExecution;
    Gtk::RadioButton*       mRemoteExecution;
    Gtk::RadioButton::Group mGroupExecution;
    Gtk::HBox*              mHBoxExecution;
    Gtk::Frame*             mFrameExecution;
    Gtk::Button*            mCancel;
    Gtk::Button*            mValid;
    std::string             mProjectFileName;

    /**
     * When click on button name. Show a file selection to select project
     * file to run
     */
    void onButtonName();

    /** launch a vle process on project file. */
    void launchSimulation();
};

}
} // namespace vle gvle

#endif
