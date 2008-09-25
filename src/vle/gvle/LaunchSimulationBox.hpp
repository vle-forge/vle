/**
 * @file vle/gvle/LaunchSimulationBox.hpp
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


#ifndef VLE_GVLE_LAUNCHSIMULATIONBOX_HPP
#define VLE_GVLE_LAUNCHSIMULATIONBOX_HPP

#include <libglademm.h>
#include <gtkmm/dialog.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/button.h>
#include <gtkmm/progressbar.h>
#include <gtkmm/label.h>
#include <vle/vpz/Vpz.hpp>

namespace vle { namespace gvle {

    class Modeling;

    class LaunchSimulationBox
    {
    public:
        LaunchSimulationBox(Glib::RefPtr < Gnome::Glade::Xml > xml,
                            Modeling* modeling);

        void show();

    private:
        Modeling*                         mModeling;

        Gtk::Dialog*                      mDialog;
        Gtk::RadioButton*                 mMono;
        Gtk::RadioButton*                 mMulti;
        Gtk::SpinButton*                  mNbProcess;
        Gtk::RadioButton*                 mDistant;
        Gtk::Button*                      mPlay;
        Gtk::Button*                      mPause;
        Gtk::Button*                      mStop;
        Gtk::ProgressBar*                 mProgressBar;
        Gtk::Label*                       mCurrentTime;

        double                            m_vle_time, m_gvle_time;
        double                            m_max_time;

        bool                              m_stop, m_pause, m_vle_error;

        vle::vpz::Vpz                     *file;
        vle::vpz::Vpz                     *m_vpzfile;

        Glib::Mutex                       m_mutex;
        Glib::Cond                        m_cond_set_time;
        Glib::Mutex                       m_mutex_stop;
        Glib::Cond                        m_cond_set_stop;
        Glib::Mutex                       m_mutex_pause;
        Glib::Cond                        m_cond_set_pause;
        Glib::Mutex                       m_mutex_exception;
        Glib::Cond                        m_cond_set_exception;
        std::string                       m_exception;

        void catch_vle_exception();
        void updateCurrentTime();
        void updateProgressBar();

        void on_play();
        void on_pause();
        void on_stop();
        void run_local_simu();
        bool timer();
    };

}} // namespace vle gvle

#endif
