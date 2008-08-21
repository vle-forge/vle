/**
 * @file vle/gvle/LaunchSimulationBox.cpp
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


#include <vle/gvle/LaunchSimulationBox.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/manager/JustRun.hpp>
#include <vle/utils/Tools.hpp>

using namespace vle;

namespace vle
{
namespace gvle {

LaunchSimulationBox::LaunchSimulationBox(Glib::RefPtr<Gnome::Glade::Xml> xml, Modeling* modeling) :
        mModeling(modeling),
        mDialog(0)
{
    xml->get_widget("DialogSimulation", mDialog);

    xml->get_widget("RadioSimuMono", mMono);
    //mMono->signal_toggled().connect(sigc::mem_fun
    //         (*this, &LaunchSimulationBox::on_mono));

    xml->get_widget("RadioSimuMulti", mMulti);
    //mMulti->signal_toggled().connect(sigc::mem_fun
    //          (*this, &LaunchSimulationBox::on_multi));

    xml->get_widget("SpinSimuProcess", mNbProcess);

    xml->get_widget("RadioSimuDistant", mDistant);
    //mDistant->signal_toggled().connect(sigc::mem_fun
    //     (*this, &LaunchSimulationBox::on_distant));

    xml->get_widget("simuPlay", mPlay);
    mPlay->signal_pressed().connect(sigc::mem_fun
                                    (*this, &LaunchSimulationBox::on_play));

    xml->get_widget("simuPause", mPause);
    mPause->signal_pressed().connect(sigc::mem_fun
                                     (*this, &LaunchSimulationBox::on_pause));

    xml->get_widget("simuStop", mStop);
    mStop->signal_pressed().connect(sigc::mem_fun
                                    (*this, &LaunchSimulationBox::on_stop));

    xml->get_widget("simuProgressBar", mProgressBar);

    xml->get_widget("simuCurrentTime", mCurrentTime);

    xml->get_widget("ButtonSimuClose", mClose);
    mClose->signal_pressed().connect(sigc::mem_fun
                                     (*this, &LaunchSimulationBox::on_close));

    mMono->set_active();

    m_vpzfile = &mModeling->vpz();
}

void LaunchSimulationBox::show()
{
    mDialog->show_all();
    mDialog->run();
}

//void LaunchSimulationBox::on_mono() {
//}

//void LaunchSimulationBox::on_multi() {
//}

//void LaunchSimulationBox::on_distant() {
//}

void LaunchSimulationBox::on_close()
{
    mDialog->hide_all();
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

            Glib::Thread::create(sigc::mem_fun(*this,&LaunchSimulationBox::run_local_simu),false);
            Glib::signal_timeout().connect(sigc::mem_fun(*this,&LaunchSimulationBox::timer), 200);
        } else {
            Glib::Mutex::Lock lock(m_mutex_pause);
            m_pause = false;
        }
    } else {
        std::cout << "Not Implemented Yet\n";
    }

}

void LaunchSimulationBox::on_pause()
{
    Glib::Mutex::Lock lock(m_mutex_pause);
    m_pause = true;
}

void LaunchSimulationBox::on_stop()
{
    Glib::Mutex::Lock lock(m_mutex_stop);
    m_stop = true;
    m_cond_set_stop.signal();
}

void LaunchSimulationBox::run_local_simu()
{
    /*
    using namespace manager;
    int process = utils::to_int( mNbProcess->get_text() );
    if (process == 1) {
    JustRunMono mono(std::cout);
    CmdArgs args;
    args.push_back(mModeling->vpz().filename().c_str());
    mono(args);
    } else {
    JustRunThread multi(std::cout, process);
    CmdArgs args;
    args.push_back(mModeling->vpz().filename().c_str());
    multi(args);
    }
    */
    double duration = m_max_time;
    vle::devs::RootCoordinator root_coordinator;
    try {
        root_coordinator.load(*file);
    } catch (const std::exception &e) {
        {
            Glib::Mutex::Lock lock(m_mutex_exception);
            m_exception = (boost::format(
                               "Error while loading project\n%1%") %
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
            m_exception = (boost::format(
                               "Error while initializing project\n%1%") %
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
                m_exception = (boost::format("Simulator error\n%1%") % e.what()).str();
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
            } else
                m_gvle_time = duration;
            m_stop = true;
        }
        m_cond_set_time.signal();
    }
}

//void LaunchSimulationBox::run_local_manager() {
//
//}

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
            mProgressBar->set_text("PAUSE");
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
        mProgressBar->set_text("The simulator initializes the models");
        return true;
    }

    if (stop and(m_gvle_time < m_max_time)) {
        mProgressBar->set_fraction(0);
        mProgressBar->set_text("Simulation stoped");
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
    std::string s = (boost::format("%1$10.0f") % m_gvle_time).str();
    mCurrentTime->set_label(s);
}

void LaunchSimulationBox::updateProgressBar()
{
    double p = m_gvle_time / m_max_time;
    mProgressBar->set_fraction(p);
    mProgressBar->set_text((boost::format("%1$.0f %%")%(p*100.)).str());
}

}
} // namespace vle gvle
