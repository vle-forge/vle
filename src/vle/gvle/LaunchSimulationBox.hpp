/**
 * @file vle/gvle/LaunchSimulationBox.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
 * Copyright (C) 2003-2009 ULCO http://www.univ-littoral.fr
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


#ifndef VLE_GVLE_LAUNCHSIMULATIONBOX_HPP
#define VLE_GVLE_LAUNCHSIMULATIONBOX_HPP

#include <libglademm.h>
#include <gtkmm/dialog.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/button.h>
#include <gtkmm/progressbar.h>
#include <gtkmm/label.h>

namespace vle { namespace vpz {

    class Vpz;

}} // namespace vle vpz

namespace vle { namespace gvle {

class LaunchSimulationBox
{
public:
    LaunchSimulationBox(Glib::RefPtr < Gnome::Glade::Xml > xml,
			const vpz::Vpz& file);

    ~LaunchSimulationBox();

    void run();

private:
    enum State { Wait, Init, Play, Error, Finish, Close };

    /* The VPZ to execute */
    const vpz::Vpz& mVpz;

    /* Gtk Widgets */
    Gtk::Dialog      *mDialog;
    Gtk::RadioButton *mMono;
    Gtk::RadioButton *mMulti;
    Gtk::SpinButton  *mNbProcess;
    Gtk::RadioButton *mDistant;
    Gtk::Button      *mPlay;
    Gtk::Button      *mStop;
    Gtk::ProgressBar *mProgressBar;
    Gtk::Label       *mCurrentTimeLabel;

    /* Signal to disconnect before closing */
    sigc::connection mConnectionPlayButton;
    sigc::connection mConnectionStopButton;
    sigc::connection mConnectionTimer;

    /* Simulation attributes */
    double      mCurrentTime;
    double      mDuration;
    State       mState;
    std::string mErrorMsg;

    /* thread mutex */
    Glib::Thread* mThread;
    Glib::Mutex   mMutex;
    bool          mThreadRun;

    /* accessors to the protected variables */
    void setState(State state);
    State state();
    double currentTime();
    void setCurrentTime(const double& time);
    std::string errorMessage();
    void setErrorMessage(const std::string& msg);

    void showErrorMsg();
    void updateCurrentTime();
    void updateProgressBar();

    void onPlayButtonClicked();
    void onPauseButtonClicked();
    void onStopButtonClicked();

    /**
     * @brief Thread slot to execute simulation.
     */
    void runThread();

    /**
     * @brief Timer slot to update widgets from simulation.
     *
     * @return true to continue the timer, stop otherwise.
     */
    bool onTimerClock();

    /**
     * @brief Adapt all widgets to the current State.
     */
    void updateWidget();

    //
    // Cursors
    //

    /**
     * @brief Switch cursor to the WATCH cursor.
     */
    void changeToWatchCursor();

    /**
     * @brief Switch cursor to the classical LEFT_PTR.
     */
    void resetCursor();
};

}} // namespace vle gvle

#endif
