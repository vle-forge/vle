/**
 * @file vle/gvle/LaunchSimulationBox.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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
#include <vle/gvle/Modeling.hpp>
#include <vle/manager/JustRun.hpp>
#include <vle/utils/Tools.hpp>

namespace vle { namespace gvle {

LaunchSimulationBox::LaunchSimulationBox(Glib::RefPtr < Gnome::Glade::Xml > xml,
                                         Modeling* modeling) :
    mModeling(modeling),
    mDialog(0),
    m_vpzfile(&modeling->vpz())
{
    xml->get_widget("DialogSimulation", mDialog);
    xml->get_widget("RadioSimuMono", mMono);
    xml->get_widget("RadioSimuMulti", mMulti);
    xml->get_widget("SpinSimuProcess", mNbProcess);
    xml->get_widget("RadioSimuDistant", mDistant);
    xml->get_widget("simuPlay", mPlay);
    xml->get_widget("simuPause", mPause);
    xml->get_widget("simuStop", mStop);
    xml->get_widget("simuProgressBar", mProgressBar);
    xml->get_widget("simuCurrentTime", mCurrentTime);

    mPlay->signal_clicked().connect(
        sigc::mem_fun (*this, &LaunchSimulationBox::on_play));

    mPause->signal_clicked().connect(
        sigc::mem_fun (*this, &LaunchSimulationBox::on_pause));

    mStop->signal_clicked().connect(
        sigc::mem_fun (*this, &LaunchSimulationBox::on_stop));

    mMono->set_active();
    mMulti->set_sensitive(false);
    mDistant->set_sensitive(false);

    mPlay->set_sensitive(true);
    mPause->set_sensitive(false);
    mStop->set_sensitive(false);
}

void LaunchSimulationBox::show()
{
    mDialog->show_all();
    mDialog->run();
    mDialog->hide();
}

void LaunchSimulationBox::on_play()
{
    if (mMono->get_active()) {
        bool pause;
        {
            Glib::Mutex::Lock lock(m_mutex_pause);
            pause = m_pause;
            if (pause)
                m_cond_set_pause.signal();
        }

        if (not pause) {
            m_stop = true;
            m_gvle_time = m_vle_time = 0.;
            mProgressBar->set_fraction(0.);
            updateCurrentTime();

            std::string tmpFile = vle::utils::build_temp("tmpGvleSimu.vpz");
            m_vpzfile->write(tmpFile);
            file = new vle::vpz::Vpz(tmpFile);
            m_max_time = file->project().experiment().duration();

            Glib::Thread::create(sigc::mem_fun(
                    *this,&LaunchSimulationBox::run_local_simu),false);
            Glib::signal_timeout().connect(sigc::mem_fun(
                    *this,&LaunchSimulationBox::timer), 200);
        } else {
            Glib::Mutex::Lock lock(m_mutex_pause);
            m_pause = false;
        }
        mPlay->set_sensitive(false);
        mPause->set_sensitive(true);
        mStop->set_sensitive(true);
    }
}

void LaunchSimulationBox::on_pause()
{
    Glib::Mutex::Lock lock(m_mutex_pause);
    m_pause = true;
    mPlay->set_sensitive(true);
    mPause->set_sensitive(false);
    mStop->set_sensitive(false);
}

void LaunchSimulationBox::on_stop()
{
    Glib::Mutex::Lock lock(m_mutex_stop);
    m_stop = true;
    mPlay->set_sensitive(true);
    mPause->set_sensitive(false);
    mStop->set_sensitive(false);
    m_cond_set_stop.signal();
}

void LaunchSimulationBox::run_local_simu()
{
    double duration = m_max_time;
    vle::devs::RootCoordinator root_coordinator;
    try {
        root_coordinator.load(*file);
    } catch (const std::exception &e) {
        {
            Glib::Mutex::Lock lock(m_mutex_exception);
            m_exception = (fmt(_("Error while loading project\n%1%")) %
                           e.what()).str();
            m_vle_error = true;
        }
        delete file;
        return;
    }
    delete file;
    try {
        root_coordinator.init();
    } catch (const std::exception &e) {
        {
            Glib::Mutex::Lock lock(m_mutex_exception);
            m_exception = (fmt(_("Error while initializing project\n%1%")) %
                           e.what()).str();
            m_vle_error = true;
        }
        return;
    }
    {
        Glib::Mutex::Lock lock(m_mutex_stop);
        m_cond_set_stop.wait(m_mutex_stop);
        m_stop = false;
    }

    bool running = true;
    while (running) {
        {
            Glib::Mutex::Lock lock(m_mutex);
            m_vle_time = root_coordinator.getCurrentTime().getValue();
            m_cond_set_time.signal();
        }

        try {
            running = root_coordinator.run();
        } catch (const std::exception &e) {
            {
                Glib::Mutex::Lock lock(m_mutex_exception);
                m_exception = (fmt(_("Simulator error\n%1%")) % e.what()).str();
                m_vle_error = true;
            }
            return;
        }
        {
            Glib::Mutex::Lock lock(m_mutex_stop);
            if (m_stop)
                running=false;
        }

        {
            Glib::Mutex::Lock lock(m_mutex_pause);
            if (m_pause)
                m_cond_set_pause.wait(m_mutex_pause);
        }

    }

    {
        Glib::Mutex::Lock lock(m_mutex);
        {
            Glib::Mutex::Lock lock_stop(m_mutex_stop);
            if (m_stop) {
                root_coordinator.finish();
                m_gvle_time = m_vle_time;
                m_cond_set_stop.wait(m_mutex_stop);
            } else {
                root_coordinator.finish();
                m_gvle_time = duration;
            }
            m_stop = true;
        }
        m_cond_set_time.signal();
    }
}

bool LaunchSimulationBox::timer()
{
    {
        Glib::Mutex::Lock lock(m_mutex_exception);
        if (m_vle_error) {
            catch_vle_exception();
            return false;
        }
    }

    {
        Glib::Mutex::Lock lock(m_mutex_pause);
        if (m_pause) {
            mProgressBar->set_text(_("PAUSE"));
            return true;
        }
    }

    bool stop;
    {
        Glib::Mutex::Lock lock(m_mutex_stop);
        stop = m_stop;
        m_cond_set_stop.signal();
    }
    if (stop and m_gvle_time == 0.) {
        mProgressBar->set_text(_("The simulator initializes the models"));
        return true;
    }

    if (stop and(m_gvle_time < m_max_time)) {
        mProgressBar->set_fraction(0);
        mProgressBar->set_text(_("Simulation stopped"));
        return false;
    }
    if (not stop) {
        {
            Glib::Mutex::Lock lock(m_mutex);
            m_cond_set_time.wait(m_mutex);
            m_gvle_time = m_vle_time;
        }
        updateCurrentTime();
        updateProgressBar();
        return true;
    }

    updateProgressBar();
    updateCurrentTime();
    mPlay->set_sensitive(true);
    mPause->set_sensitive(false);
    mStop->set_sensitive(false);
    return false;
}

void LaunchSimulationBox::catch_vle_exception()
{
    Gtk::MessageDialog d(m_exception,false,Gtk::MESSAGE_ERROR);
    d.run();
    m_vle_error = false;
}

void LaunchSimulationBox::updateCurrentTime()
{
    std::string s = (fmt("%1$10.0f") % m_gvle_time).str();
    mCurrentTime->set_label(s);
}

void LaunchSimulationBox::updateProgressBar()
{
    double p = m_gvle_time / m_max_time;
    mProgressBar->set_fraction(p);
    mProgressBar->set_text((fmt("%1$.0f %%")%(p*100.)).str());
}

}} // namespace vle gvle
