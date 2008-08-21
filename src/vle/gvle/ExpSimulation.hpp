/**
 * @file vle/gvle/ExpSimulation.hpp
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


#ifndef GUI_EXPSIMULATION_HPP
#define GUI_EXPSIMULATION_HPP

#include <vle/utils/Tools.hpp>
#include <gtkmm/box.h>
#include <gtkmm/frame.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/progressbar.h>
#include <list>
#include <string>

namespace vle
{
namespace gvle {

class Modeling;

/**
 * @brief ExpSimulation is a VBox to allow user to launch simulation using
 * VLE simulators in differents way: local, pipe or socket.
 *
 */
class ExpSimulation : public Gtk::VBox
{
public:
    ExpSimulation(Modeling* m);

    virtual ~ExpSimulation() {}

    /**
     * Return duration from Entry widget.
     *
     * @return a double transformation
     */
    double getDuration() {
        return utils::to_double(mEntryDuration.get_text());
    }

private:
    /** When user clicked on button start. */
    void on_button_start();

    /** When user clicked on button pause. */
    void on_button_pause();

    /** When user clicked on button stop. */
    void on_button_stop();

    /** When user select local radio button, we change label. */
    void on_radiobutton_local_mono();

    /** When user select localpipe radio button, we change label. */
    void on_radiobutton_local_multi();

    /** When user select socket radio button, we change label. */
    void on_radiobutton_socket();

    /** Connect all widget signal to slot. */
    void connectSignal();

    /** When a local simulation start. */
    void localPipeSimulationStarted();

    /**
    * Run a local simulation using internal VLE libs.
    *
    * @param name filename to read.
    */
    void runMono(const std::string& name);

    /**
    * Run a local simulation using external processus.
    *
    * @param name filename to read.
    */
    void runMulti(const std::string& name);

    /**
    * Run a simulation on distant host using networks.
    *
    * @param name filename to read.
    */
    void runSocket(const std::string& name);

    void on_click_button_file();

    Modeling*               mModeling;
    Glib::Pid               mChildPid;

    Gtk::Frame              mFrameLabel;
    Gtk::VBox               mVBoxLabel;
    Gtk::HBox               mHBoxDuration;
    Gtk::Label              mLabelDuration;
    Gtk::Entry              mEntryDuration;

    // Gtk::HBox               mHBoxFile;
    //Gtk::Entry              mEntryFile;
    //Gtk::Button             mButtonFile;

    Gtk::Label              mLabel2;
    Gtk::Label              mLabel3;

    Gtk::Frame              mFrameSimulation;
    Gtk::VBox               mVBoxSimulation;
    Gtk::HBox               mHBoxRadio;
    Gtk::VBox               mVBoxRadio;
    Gtk::RadioButtonGroup   mRadioButtonGroup;
    Gtk::Label              mLabelRadioButton;
    Gtk::RadioButton        mRadioButtonMono;


    Gtk::HBox             mHBoxMulti;
    Gtk::Label            mLabelNbProcess;
    Gtk::Entry            mNbProcess;

    Gtk::RadioButton        mRadioButtonMulti;
    Gtk::RadioButton        mRadioButtonSocket;
    //Gtk::ProgressBar        mProgressBar;
    Gtk::HBox               mButtonControl;
    Gtk::Button             mStart;
    //Gtk::Button             mPause;
    //Gtk::Button             mStop;

    int                     mSocket;
};

}
} // namespace vle gvle

#endif
