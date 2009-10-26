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

ExperimentBox::ExperimentBox(Glib::RefPtr < Gnome::Glade::Xml > xml,
                             Modeling* modeling)
    : mModeling(modeling), mCalendar(xml), mDialog(0), mCalendarBegin(xml)
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
    const Modeling& modeling(*mModeling);
    const vle::vpz::Vpz& vpz(modeling.vpz());
    const vle::vpz::Experiment& experiment(modeling.experiment());

    // Project frame
    {
        std::string name(vpz.project().author());

        if (name.empty()) {
            name = Glib::get_real_name();
            if (name.empty()) {
                name = Glib::get_user_name();
            }
        }
        mEntryAuthor->set_text(name);

	if (vpz.project().date().empty()) {
	    on_now();
	} else {
	    mEntryDate->set_text(vpz.project().date());
	}

	mEntryVersion->set_text(utils::to_string(
				    vpz.project().version()));
    }

    // Experiment frame
    {
        if (experiment.name().empty()) {
	    mEntryName->set_text("exp");
	} else {
	    mEntryName->set_text(experiment.name());
	}

	mSpinDuration->set_range(
	    std::numeric_limits < double >::epsilon(),
	    std::numeric_limits < double >::max());
	mSpinDuration->set_value(experiment.duration());

        mSpinBeginReal->set_range(std::numeric_limits < int >::min(),
                                  std::numeric_limits < int >::max());
        mSpinBeginReal->set_value(experiment.begin());
        updateBeginTime();
    }

    // Simulation frame
    {
	mSpinSimuSeed->set_range(0, std::numeric_limits < guint32 >::max());
	if (experiment.seed() != 1) {
	    mSpinSimuSeed->set_value(experiment.seed());
	} else {
	    on_random_simu();
	}
    }

    // Plan frame
    {
	if (experiment.combination().empty() or
	    experiment.combination() == "linear") {
	    mRadioButtonLinear->set_active();
	} else {
	    mRadioButtonTotal->set_active();
	}

	mSpinPlanSeed->set_range(0, std::numeric_limits < guint32 >::max());
	if (experiment.replicas().seed() != 0) {
	    mSpinPlanSeed->set_value(experiment.replicas().seed());
	} else {
	    on_random_plan();
	}

	mButtonNumber->set_range(1, std::numeric_limits < guint32 >::max());
	mButtonNumber->set_value(experiment.replicas().number());
    }
}

void ExperimentBox::run()
{
    bool ok = false;

    while (not ok) {
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
    std::string date = mEntryBeginDate->get_text();

    if (date.empty()) {
        mEntryBeginDate->set_text("1400-01-01");
        date = mEntryBeginDate->get_text();
    }

    double begin = utils::DateTime::toJulianDayNumber(date);

    begin += (mSpinBeginH->get_value() * 1 / 24);
    begin += (mSpinBeginM->get_value() * 1 / (24 * 60));
    begin += (mSpinBeginS->get_value() * 1 / (24 * 60 * 60));

    mSpinBeginReal->set_value(begin);
}

void ExperimentBox::updateBeginTime()
{
    try {
        long year, month, day, hours, minutes, seconds;
        double remain;

        remain = utils::DateTime::toTime(mSpinBeginReal->get_value(), year,
                                         month, day, hours, minutes, seconds);

        mSpinBeginH->set_value(hours);
        mSpinBeginM->set_value(minutes);

        if (remain > .5) {
            mSpinBeginS->set_value(seconds + 1);
        } else {
            mSpinBeginS->set_value(seconds);
        }

        mEntryBeginDate->set_text(
            utils::DateTime::toJulianDayNumber(mSpinBeginReal->get_value()));
    } catch (const std::exception& e) {
        mSpinBeginH->set_value(0);
        mSpinBeginM->set_value(0);
        mSpinBeginS->set_value(0);
        mEntryBeginDate->set_text("");
    }
}

void ExperimentBox::on_julianDate_changed()
{
    updateBeginTime();
}

void ExperimentBox::on_time_changed()
{
    updateBeginReal();
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
