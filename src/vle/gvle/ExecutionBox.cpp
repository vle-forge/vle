/**
 * @file vle/gvle/ExecutionBox.cpp
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


#include <vle/gvle/ExecutionBox.hpp>
#include <vle/gvle/GVLE.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Tools.hpp>
#include <glibmm.h>
#include <string>
#include <cassert>
#include <list>
#include <gtkmm/stock.h>
#include <gtkmm/fileselection.h>

using std::string;
using std::list;

namespace vle { namespace gvle {

ExecutionBox::ExecutionBox(const std::string & projectFilename) :
        mProjectFileName(projectFilename)
{
    /*
    mLabelName = Gtk::manage(new class Gtk::Label("Project path"));
    mEntryName = Gtk::manage(new class Gtk::Entry());

    //mEntryName->set_text(Glib::build_filename(utils::Path::path().getBinDir(), "vle"));

    mButtonName = Gtk::manage(new class Gtk::Button("  ...   "));
    mHBoxName = Gtk::manage(new class Gtk::HBox(false, 5));
    mFrameName = Gtk::manage(new class Gtk::Frame("VLE path"));

    mHBoxName->add(*mLabelName);
    mHBoxName->add(*mEntryName);
    mHBoxName->add(*mButtonName);
    mFrameName->add(*mHBoxName);

    mLocalExecution = Gtk::manage(
        new class Gtk::RadioButton(mGroupExecution, "local"));
    mRemoteExecution = Gtk::manage(
        new class Gtk::RadioButton(mGroupExecution, "remote"));
    mHBoxExecution = Gtk::manage(new class Gtk::HBox(false, 5));
    mFrameExecution = Gtk::manage(new class Gtk::Frame("Execution type"));

    mHBoxExecution->add(*mLocalExecution);
    mHBoxExecution->add(*mRemoteExecution);
    mFrameExecution->add(*mHBoxExecution);

    mCancel = add_button(Gtk::Stock::NO, Gtk::RESPONSE_NO);
    mValid = add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

    mButtonName->signal_clicked().connect(
        sigc::mem_fun(*this, &ExecutionBox::onButtonName));
    mValid->signal_clicked().connect(
 sigc::mem_fun(*this, &ExecutionBox::launchSimulation));

    get_vbox()->pack_start(*mFrameName, Gtk::PACK_SHRINK, 10);
    get_vbox()->pack_start(*mFrameExecution, Gtk::PACK_SHRINK, 10);
    set_title("Execution dialog");
    set_default_size(300, 150);
    show_all();
          */
}

void ExecutionBox::onButtonName()
{
    Gtk::FileSelection file_selection("Open project file");

    if (file_selection.run() == Gtk::RESPONSE_OK) {
        string project_file = file_selection.get_filename();
        mEntryName->set_text(project_file);
    }
}

void ExecutionBox::launchSimulation()
{
    if (Glib::file_test(mProjectFileName, Glib::FILE_TEST_IS_REGULAR |
                        Glib::FILE_TEST_EXISTS)) {
        try {
            std::list < string > argv;
            std::list < string > envp;

            argv.push_back(mProjectFileName);

            //Glib::spawn_sync(utils::Path::path().getBinDir(), argv, envp,
            //Glib::SpawnFlags(0),
            //sigc::slot0<void>(), 0, 0, 0);
        } catch (const std::exception& e) {
            Error((boost::format(
                       "An error ocurred simulation:\n%1") % e.what()).str());
        }
    } else {
        Error("Project file not found");
    }
}

}
} // namespace vle gvle
