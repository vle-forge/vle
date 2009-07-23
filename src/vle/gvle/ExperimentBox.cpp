/**
 * @file vle/gvle/ExperimentBox.cpp
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


#include <vle/gvle/ExperimentBox.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <vle/graph/Model.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/DateTime.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
#include <glibmm/miscutils.h>
#include <limits>

namespace vle { namespace gvle {

ExperimentBox::ExperimentBox(Glib::RefPtr<Gnome::Glade::Xml> xml,
                             Modeling* modeling) :
    mModeling(modeling),
    mCalendar(xml),
    mDialog(0),
    mCalendarBegin(xml)
{
    xml->get_widget("DialogExperiment", mDialog);
    xml->get_widget("EntryAuthor", mEntryAuthor);
    xml->get_widget("EntryDate", mEntryDate);
    xml->get_widget("ButtonCalendar", mButtonCalendar);
    xml->get_widget("ButtonNow", mButtonNow);
    xml->get_widget("EntryVersion", mEntryVersion);
    xml->get_widget("EntryName", mEntryName);
    xml->get_widget("SpinButtonDuration", mSpinDuration);
    xml->get_widget("SpinBeginReal", mSpinBeginReal);
    xml->get_widget("EntryBeginDate", mEntryBeginDate);
    xml->get_widget("ButtonCalendarBegin", mButtonCalendarBegin);
    xml->get_widget("SpinBeginH", mSpinBeginH);
    xml->get_widget("SpinBeginM", mSpinBeginM);
    xml->get_widget("SpinBeginS", mSpinBeginS);
    xml->get_widget("SpinSimuSeed", mSpinSimuSeed);
    xml->get_widget("ButtonSimuSeed", mButtonSimuSeed);
    xml->get_widget("HBoxCombi", mHboxCombi);
    xml->get_widget("RadioButtonLinear", mRadioButtonLinear);
    xml->get_widget("RadioButtonTotal", mRadioButtonTotal);
    xml->get_widget("SpinPlanSeed", mSpinPlanSeed);
    xml->get_widget("ButtonPlanSeed", mButtonPlanSeed);
    xml->get_widget("SpinButtonNumber", mButtonNumber);

    mBeginRealMin = utils::DateTime::toJulianDay("1400-01-01 00:00:00");
    mBeginRealMax = utils::DateTime::toJulianDay("9999-12-31 23:59:59");

    initExperiment();

    mSigcConnection.push_back(mButtonCalendar->signal_clicked().connect(
        sigc::mem_fun(*this, &ExperimentBox::on_calendar)));
    mSigcConnection.push_back(mButtonNow->signal_clicked().connect(
        sigc::mem_fun(*this, &ExperimentBox::on_now)));
    mSigcConnection.push_back(mButtonSimuSeed->signal_clicked().connect(
        sigc::mem_fun(*this, &ExperimentBox::on_random_simu)));
    mSigcConnection.push_back(mButtonPlanSeed->signal_clicked().connect(
        sigc::mem_fun (*this, &ExperimentBox::on_random_plan)));
    mSigcConnection.push_back(mButtonCalendarBegin->signal_clicked().connect(
        sigc::mem_fun(*this, &ExperimentBox::on_calendarBegin)));
    mSigcConnection.push_back(mSpinBeginH->signal_value_changed().connect(
        sigc::mem_fun(*this, &ExperimentBox::on_time_changed)));
    mSigcConnection.push_back(mSpinBeginM->signal_value_changed().connect(
        sigc::mem_fun(*this, &ExperimentBox::on_time_changed)));
    mSigcConnection.push_back(mSpinBeginS->signal_value_changed().connect(
        sigc::mem_fun(*this, &ExperimentBox::on_time_changed)));
    mSigcConnection.push_back(mSpinBeginReal->signal_value_changed().connect(
        sigc::mem_fun(*this, &ExperimentBox::on_julianDate_changed)));

    mDialog->show_all();
}

ExperimentBox::~ExperimentBox()
{
    for (std::list < sigc::connection >::iterator it = mSigcConnection.begin();
        it != mSigcConnection.end(); ++it) {
        it->disconnect();
    }
}

void ExperimentBox::initExperiment()
{
    // Project frame
    {
        std::string name(mModeling->vpz().project().author());

        if (name.empty()) {
            name = Glib::get_real_name();
            if (name.empty()) {
                name = Glib::get_user_name();
            }
        }
        mEntryAuthor->set_text(name);

	if (mModeling->vpz().project().date().empty()) {
	    on_now();
	} else {
	    mEntryDate->set_text(mModeling->vpz().project().date());
	}

	mEntryVersion->set_text(utils::to_string(
				    mModeling->vpz().project().version()));
    }

    // Experiment frame
    {
        if (mModeling->experiment().name().empty()) {
	    mEntryName->set_text("exp");
	} else {
	    mEntryName->set_text(mModeling->experiment().name());
	}

	mSpinDuration->set_range(
	    std::numeric_limits < double >::epsilon(),
	    std::numeric_limits < double >::max());
	mSpinDuration->set_value(mModeling->experiment().duration());

        mSpinBeginReal->set_range(std::numeric_limits < int >::min(),
                                  std::numeric_limits < int >::max());
        mSpinBeginReal->set_value(mModeling->experiment().begin());
        mEntryBeginDate->set_text("");
        mSpinBeginH->set_value(0);
        mSpinBeginM->set_value(0);
        mSpinBeginS->set_value(0);
    }

    // Simulation frame
    {
	mSpinSimuSeed->set_range(0, std::numeric_limits < guint32 >::max());
	if (mModeling->experiment().seed() != 1) {
	    mSpinSimuSeed->set_value(mModeling->experiment().seed());
	} else {
	    on_random_simu();
	}
    }

    // Plan frame
    {
	if (mModeling->experiment().combination().empty()) {
	    mModeling->experiment().setCombination("linear");
	}

	if (mModeling->experiment().combination() == "linear") {
	    mRadioButtonLinear->set_active();
	} else {
	    mRadioButtonTotal->set_active();
	}

	mSpinPlanSeed->set_range(0, std::numeric_limits < guint32 >::max());
	if (mModeling->experiment().replicas().seed() != 0) {
	    mSpinPlanSeed->set_value(mModeling->experiment().replicas().seed());
	} else {
	    on_random_plan();
	}

	mButtonNumber->set_range(1, std::numeric_limits < guint32 >::max());
	mButtonNumber->set_value(mModeling->experiment().replicas().number());
    }
}

void ExperimentBox::run()
{
    bool ok = false;

    while (!ok) {
	if (mDialog->run() == Gtk::RESPONSE_OK) {
	    ok = apply();
	} else {
	    ok = true;
	}
    }
    mDialog->hide();
}

bool ExperimentBox::apply()
{
    // Project frame
    {
	std::string error;

	if (mEntryAuthor->get_text().empty()) {
	    error += _("Set an author for this project.\n");
	}

	if (mEntryVersion->get_text().empty()) {
	    error += _("Set an number of version for this project.\n");
	}

	if (mEntryName->get_text().empty()) {
	    error += _("Set a name for this experiment.");
	}

	if (error.empty()) {
	    mModeling->vpz().project().setAuthor(mEntryAuthor->get_text());
	    mModeling->vpz().project().setDate(mEntryDate->get_text());
	    mModeling->vpz().project().setVersion(mEntryVersion->get_text());
	} else {
	    Error(error);
	    return false;
	}
    }

    vpz::Experiment& exp = mModeling->experiment();
    vpz::Replicas& rep = exp.replicas();

    {
	exp.setName(mEntryName->get_text());

	exp.setDuration(mSpinDuration->get_value() <= 0.0 ?
			std::numeric_limits < double >::epsilon() :
			mSpinDuration->get_value());
        exp.setBegin(mSpinBeginReal->get_value());
    }

    {
        long seed = (long)std::floor(std::abs(mSpinSimuSeed->get_value()));
	exp.setSeed(seed);
    }

    {
        long seed = (long)std::floor(std::abs(mSpinPlanSeed->get_value()));
        long number = (long)std::floor(std::abs(mButtonNumber->get_value()));
        exp.setCombination(mRadioButtonLinear->get_active()?"linear":"total");
	rep.setSeed(seed);
	rep.setNumber(number);
    }
    return true;
}

void ExperimentBox::on_random_simu()
{
    mSpinSimuSeed->set_value(mRand.getInt());
}

void ExperimentBox::on_random_plan()
{
    mSpinPlanSeed->set_value(mRand.getInt());
}

void ExperimentBox::on_calendar()
{
    std::string date;

    mCalendar.get_date(date);
    if (not date.empty()) {
        mEntryDate->set_text(date);
    }
}

void ExperimentBox::updateBeginReal()
{
    if (!mRealUpdated) {
        std::string date;
        if (mEntryBeginDate->get_text() != "") {
            date = mEntryBeginDate->get_text();
        } else {
            date = "1400-01-01";
        }

        date += " ";
        std::string h = boost::lexical_cast <
               std::string >(mSpinBeginH->get_value());
        date += h;
        date += ":";
        std::string m = boost::lexical_cast <
               std::string >(mSpinBeginM->get_value());
        date += m;
        date += ":";
        std::string s = boost::lexical_cast <
               std::string >(mSpinBeginS->get_value());
        date += s;
        date += ".000";
        double realDate = utils::DateTime::toJulianDay(date);
        mSpinBeginReal->set_value(realDate);
    }
}

void ExperimentBox::updateBeginDate()
{
    if (mSpinBeginReal->get_value() != mBeginRealMin) {
        mEntryBeginDate->set_text(
          utils::DateTime::toJulianDayNumber(
             mSpinBeginReal->get_value()));
    }
}

void ExperimentBox::updateBeginTime()
{
    if (!mTimeUpdated) {
        std::string d = utils::DateTime::toJulianDay
           (mSpinBeginReal->get_value());

        std::string h;
        h = d[12];
        h += d[13];
        mSpinBeginH->set_value(
	   boost::lexical_cast <int>(h));

        std::string m;
        m = d[15];
        m += d[16];
        mSpinBeginM->set_value(
	   boost::lexical_cast <int>(m));

        std::string s;
        s = d[12];
        s += d[13];
        mSpinBeginS->set_value(
	   boost::lexical_cast <int>(s));
    }
}

void ExperimentBox::on_julianDate_changed()
{
    if (mSpinBeginReal->get_value() < mBeginRealMin or
        mSpinBeginReal->get_value() > mBeginRealMax) {
        mEntryBeginDate->set_text("");
        mSpinBeginH->set_value(0);
        mSpinBeginM->set_value(0);
        mSpinBeginS->set_value(0);
    } else {
        mRealUpdated = true;
        updateBeginDate();
        updateBeginTime();
        mRealUpdated = false;
    }
}

void ExperimentBox::on_time_changed()
{
    mTimeUpdated = true;
    if (mSpinBeginReal->get_value() != 0.0) {
        updateBeginReal();
    }
    mTimeUpdated = false;
}

void ExperimentBox::on_calendarBegin()
{
    std::string date;

    mCalendarBegin.get_dateBegin(date);
    if (not date.empty()) {
        mEntryBeginDate->set_text(date);
        updateBeginReal();
    }
}

void ExperimentBox::on_now()
{
    mEntryDate->set_text(utils::get_current_date());
}

}} // namespace vle gvle
