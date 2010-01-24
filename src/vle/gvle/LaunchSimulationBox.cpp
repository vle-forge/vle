/**
 * @file vle/gvle/LaunchSimulationBox.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
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


#include <vle/gvle/LaunchSimulationBox.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/devs/RootCoordinator.hpp>
#include <vle/vpz/Vpz.hpp>
#include <gdkmm/cursor.h>

namespace vle { namespace gvle {

LaunchSimulationBox::LaunchSimulationBox(Glib::RefPtr < Gnome::Glade::Xml > xml,
                                         const vpz::Vpz& vpz)
    : mVpz(vpz), mDialog(0), mMono(0), mMulti(0), mNbProcess(0), mDistant(0),
    mPlay(0), mStop(0), mProgressBar(0), mCurrentTimeLabel(0),mState(Wait),
    mThread(0), mThreadRun(false)
{
    xml->get_widget("DialogSimulation", mDialog);
    xml->get_widget("RadioSimuMono", mMono);
    xml->get_widget("RadioSimuMulti", mMulti);
    xml->get_widget("SpinSimuProcess", mNbProcess);
    xml->get_widget("RadioSimuDistant", mDistant);
    xml->get_widget("simuPlay", mPlay);
    xml->get_widget("simuStop", mStop);
    xml->get_widget("simuProgressBar", mProgressBar);
    xml->get_widget("simuCurrentTime", mCurrentTimeLabel);

    mConnectionPlayButton = mPlay->signal_clicked().connect(
        sigc::mem_fun(*this, &LaunchSimulationBox::onPlayButtonClicked));

    mConnectionStopButton = mStop->signal_clicked().connect(
        sigc::mem_fun(*this, &LaunchSimulationBox::onStopButtonClicked));

    mMono->set_active(true);
    mMulti->set_sensitive(false);
    mDistant->set_sensitive(false);

    updateWidget();
}

LaunchSimulationBox::~LaunchSimulationBox()
{
    mConnectionPlayButton.disconnect();
    mConnectionStopButton.disconnect();
}

void LaunchSimulationBox::run()
{
    mDialog->show_all();
    mDialog->run();
    setState(Close);

    mConnectionTimer.disconnect();

    mDialog->hide();

    resetCursor();

    if (mThread and mThreadRun) {
        mThread->join();
    }
}

void LaunchSimulationBox::onPlayButtonClicked()
{
    mPlay->set_sensitive(false);
    mStop->set_sensitive(false);
    mState = Init;

    mCurrentTime = mVpz.project().experiment().begin();
    mDuration = mVpz.project().experiment().duration();

    mThread = Glib::Thread::create(
        sigc::mem_fun(
            *this, &LaunchSimulationBox::runThread), true);

    mConnectionTimer = Glib::signal_timeout().connect(
        sigc::mem_fun(
            *this, &LaunchSimulationBox::onTimerClock), 100);
}

void LaunchSimulationBox::onStopButtonClicked()
{
    mPlay->set_sensitive(false);
    mStop->set_sensitive(false);
    setState(Finish);

    mThread->join();
    mThread = 0;
}

void LaunchSimulationBox::runThread()
{
    mThreadRun = true;
    vpz::Vpz exp(mVpz);
    devs::RootCoordinator root;

    try {
        root.load(exp);
    } catch (const std::exception &e) {
        setState(Error);
        setErrorMessage((fmt(_("Error while loading project\n%1%")) %
                         e.what()).str());
        return;
    }

    try {
        root.init();
    } catch (const std::exception &e) {
        setState(Error);
        setErrorMessage((fmt(_("Error while initializing project\n%1%")) %
                         e.what()).str());
        return;
    }

    {
        Glib::Mutex::Lock lock(mMutex);
        if (mState != Close) {
            mState = Play;
        }
    }

    State current = state();
    while (current == Play) {
        setCurrentTime(root.getCurrentTime());

        try {
            if (not root.run()) {
                Glib::Mutex::Lock lock(mMutex);
                if (mState != Close) {
                    mCurrentTime = mVpz.project().experiment().begin() +
                        mDuration;
                    mState = Finish;
                }
            }
        } catch (const std::exception &e) {
            setState(Error);
            setErrorMessage((fmt(_("Simulator error\n%1%")) % e.what()).str());
            return;
        }
        current = state();
    }

    if (state() == Finish) {
        try {
            root.finish();
        } catch (const std::exception& e) {
            setState(Error);
            setErrorMessage((fmt(_("Simulator error\n%1%")) % e.what()).str());
        }
    }
    mThreadRun = false;
}

bool LaunchSimulationBox::onTimerClock()
{
    updateWidget();

    switch (mState) {
    case Wait:
    case Init:
    case Play:
        return true;
    case Error:
        showErrorMsg();
    case Finish:
    case Close:
        return false;
    }

    return false;
}

void LaunchSimulationBox::showErrorMsg()
{
    gvle::Error(errorMessage());
}

void LaunchSimulationBox::updateCurrentTime()
{
    double time = currentTime();

    mCurrentTimeLabel->set_label(
        (fmt("%1$=10.2f / %2$=10.2f") % time %
         ((mVpz.project().experiment().begin()) + mDuration)).str());
}

void LaunchSimulationBox::updateProgressBar()
{
    double time = currentTime();
    double p = (time - mVpz.project().experiment().begin()) / mDuration;

    mProgressBar->set_fraction(std::max(0.0, std::min(1.0, p)));
    mProgressBar->set_text((fmt("%1$.0f %%") % (p * 100.)).str());
}

void LaunchSimulationBox::updateWidget()
{
    switch (mState) {
    case LaunchSimulationBox::Wait:
        mPlay->set_sensitive(true);
        mStop->set_sensitive(false);
        mProgressBar->set_fraction(0.0);
        mProgressBar->set_text(_("Wait"));
        mCurrentTimeLabel->set_text("");
        break;
    case LaunchSimulationBox::Init:
        changeToWatchCursor();
        mPlay->set_sensitive(false);
        mStop->set_sensitive(false);
        mProgressBar->set_fraction(0.0);
        mProgressBar->set_text(_("The simulator initializes the models"));
        mCurrentTimeLabel->set_text("");
        break;
    case LaunchSimulationBox::Play:
        mPlay->set_sensitive(false);
        mStop->set_sensitive(true);
        updateCurrentTime();
        updateProgressBar();
        break;
    case LaunchSimulationBox::Error:
        resetCursor();
        mPlay->set_sensitive(true);
        mStop->set_sensitive(false);
        mProgressBar->set_text(_("Simulation error"));
        updateCurrentTime();
        updateProgressBar();
        break;
    case LaunchSimulationBox::Finish:
        resetCursor();
        mPlay->set_sensitive(true);
        mStop->set_sensitive(false);
        mProgressBar->set_text(_("Finished"));
        updateCurrentTime();
        updateProgressBar();
        break;
    case LaunchSimulationBox::Close:
        break;
    }
}

void LaunchSimulationBox::changeToWatchCursor()
{
    if (mDialog->get_window()) {
        mDialog->get_window()->set_cursor(Gdk::Cursor(Gdk::WATCH));
    }
}

void LaunchSimulationBox::resetCursor()
{
    if (mDialog->get_window()) {
        mDialog->get_window()->set_cursor(Gdk::Cursor(Gdk::LEFT_PTR));
    }
}

LaunchSimulationBox::State LaunchSimulationBox::state()
{
    State result;

    {
        Glib::Mutex::Lock lock(mMutex);
        result = mState;
    }

    return result;
}

void LaunchSimulationBox::setState(State state)
{
    {
        Glib::Mutex::Lock lock(mMutex);
        mState = state;
    }
}

double LaunchSimulationBox::currentTime()
{
    double result;

    {
        Glib::Mutex::Lock lock(mMutex);
        result = mCurrentTime;
    }

    return result;
}

void LaunchSimulationBox::setCurrentTime(const double& time)
{
    {
        Glib::Mutex::Lock lock(mMutex);
        mCurrentTime = time;
    }
}

std::string LaunchSimulationBox::errorMessage()
{
    std::string result;
    {
        Glib::Mutex::Lock lock(mMutex);
        result = mErrorMsg;
    }
    return result;
}


void LaunchSimulationBox::setErrorMessage(const std::string& msg)
{
    {
        Glib::Mutex::Lock lock(mMutex);
        mErrorMsg.assign(msg);
    }
}

}} // namespace vle gvle
