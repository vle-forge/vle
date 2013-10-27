/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef VLE_GVLE_EXPERIMENTBOX_HPP
#define VLE_GVLE_EXPERIMENTBOX_HPP

#include <vle/utils/Rand.hpp>
#include <gtkmm/dialog.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/box.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/builder.h>

namespace vle { namespace gvle {

    class Modeling;

    class ExperimentBox
    {
    public:
        /**
         * Create a new ExperimentBox with children class ExpConditions,
         * ExpMeasures and ExpSimulation. Parameters modeling and current is to
         * intialise class with for experiment information
         *
         * @throw Internal if modeling is bad
         * @param modeling to get information
         * @param current, current model to show. If current equal NULL then,
         * the modeling top GCoupledModel is used.
         */
        ExperimentBox(const Glib::RefPtr < Gtk::Builder >& xml,
		      Modeling* modeling);

        ~ExperimentBox();

        void run();

    private:
        Modeling*                           mModeling;
        Glib::RefPtr < Gtk::Builder >  mXml;

	Gtk::Entry*         mEntryAuthor;
	Gtk::Entry*         mEntryDate;
	Gtk::Button*        mButtonNow;
	Gtk::Button*        mButtonCalendar;
	Gtk::Entry*         mEntryVersion;
	std::string         mBackupDate;

        Gtk::Dialog*        mDialog;
        Gtk::Entry*         mEntryName;
        Gtk::Entry*         mEntryDuration;
        Gtk::Entry*         mEntryBeginReal;
        Gtk::Entry*         mEntryBeginDate;
        Gtk::Button*        mButtonCalendarBegin;
        Gtk::SpinButton*    mSpinBeginH;
        Gtk::SpinButton*    mSpinBeginM;
        Gtk::SpinButton*    mSpinBeginS;
        double              mBeginRealMin, mBeginRealMax;
        bool                mRealUpdated;
        Gtk::Box*          mHboxCombi;
	Gtk::RadioButton*   mRadioButtonLinear;
	Gtk::RadioButton*   mRadioButtonTotal;

        std::list < sigc::connection > mSigcConnection;

        utils::Rand         mRand;

        bool apply();
	void initExperiment();

	void on_calendar();
	void on_now();

        void on_calendarBegin();
        void on_julianDate_changed();
        void on_time_changed();
        void updateBeginReal();
        void updateBeginTime();

        void on_random_simu();
        void on_random_plan();
    };

}} // namespace vle gvle

#endif
