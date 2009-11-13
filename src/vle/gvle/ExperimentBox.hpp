/**
 * @file vle/gvle/ExperimentBox.hpp
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


#ifndef VLE_GVLE_EXPERIMENTBOX_HPP
#define VLE_GVLE_EXPERIMENTBOX_HPP

#include <vle/utils/Rand.hpp>
#include <vle/gvle/CalendarBox.hpp>
#include <gtkmm/dialog.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/button.h>
#include <gtkmm/frame.h>
#include <gtkmm/box.h>
#include <gtkmm/comboboxtext.h>
#include <libglademm.h>

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
        ExperimentBox(Glib::RefPtr<Gnome::Glade::Xml> xml,
		      Modeling* modeling);

        ~ExperimentBox();

        void run();

    private:
        Modeling*                           mModeling;
        Glib::RefPtr < Gnome::Glade::Xml >  mXml;

	Gtk::Entry*         mEntryAuthor;
	Gtk::Entry*         mEntryDate;
	Gtk::Button*        mButtonNow;
	Gtk::Button*        mButtonCalendar;
	Gtk::Entry*         mEntryVersion;
	std::string         mBackupDate;

        Gtk::Dialog*        mDialog;
        Gtk::Entry*         mEntryName;
        Gtk::SpinButton*    mSpinDuration;
        Gtk::SpinButton*    mSpinBeginReal;
        Gtk::Entry*         mEntryBeginDate;
        Gtk::Button*        mButtonCalendarBegin;
        Gtk::SpinButton*    mSpinBeginH;
        Gtk::SpinButton*    mSpinBeginM;
        Gtk::SpinButton*    mSpinBeginS;
        double              mBeginRealMin, mBeginRealMax;
        bool                mRealUpdated;
        Gtk::SpinButton*    mSpinSimuSeed;
        Gtk::Button*        mButtonSimuSeed;
        Gtk::HBox*          mHboxCombi;
	Gtk::RadioButton*   mRadioButtonLinear;
	Gtk::RadioButton*   mRadioButtonTotal;
        Gtk::SpinButton*    mSpinPlanSeed;
        Gtk::Button*        mButtonPlanSeed;
        Gtk::SpinButton*    mButtonNumber;

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
