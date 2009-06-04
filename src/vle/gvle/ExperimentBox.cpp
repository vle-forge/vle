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
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
#include <glibmm/miscutils.h>
#include <limits>

namespace vle { namespace gvle {

ExperimentBox::ExperimentBox(Glib::RefPtr<Gnome::Glade::Xml> xml,
                             Modeling* modeling) :
    mModeling(modeling),
    mCalendar(xml),
    mDialog(0)
{
    xml->get_widget("DialogExperiment", mDialog);
    xml->get_widget("EntryAuthor", mEntryAuthor);
    xml->get_widget("EntryDate", mEntryDate);
    xml->get_widget("ButtonCalendar", mButtonCalendar);
    xml->get_widget("ButtonNow", mButtonNow);
    xml->get_widget("EntryVersion", mEntryVersion);
    xml->get_widget("EntryName", mEntryName);
    xml->get_widget("SpinButtonDuration", mSpinDuration);
    xml->get_widget("SpinSimuSeed", mSpinSimuSeed);
    xml->get_widget("ButtonSimuSeed", mButtonSimuSeed);
    xml->get_widget("HBoxCombi", mHboxCombi);
    xml->get_widget("SpinPlanSeed", mSpinPlanSeed);
    xml->get_widget("ButtonPlanSeed", mButtonPlanSeed);
    xml->get_widget("SpinButtonNumber", mButtonNumber);

    initExperiment();

    mButtonCalendar->signal_clicked().connect(
        sigc::mem_fun(*this, &ExperimentBox::on_calendar));
    mButtonNow->signal_clicked().connect(
        sigc::mem_fun(*this, &ExperimentBox::on_now));
    mButtonSimuSeed->signal_clicked().connect(sigc::mem_fun
            (*this, &ExperimentBox::on_random_simu));
    mButtonPlanSeed->signal_clicked().connect(
        sigc::mem_fun (*this, &ExperimentBox::on_random_plan));

    mDialog->show_all();
}

ExperimentBox::~ExperimentBox()
{
    delete mRadioButtonLinear;
    delete mRadioButtonTotal;
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
	mRadioButtonLinear = new Gtk::RadioButton("linear");
        mRadioButtonTotal = new Gtk::RadioButton("total");
        Gtk::RadioButton::Group group = mRadioButtonLinear->get_group();
	mRadioButtonTotal->set_group(group);
	
	if (mModeling->experiment().combination().empty()) {
	    mModeling->experiment().setCombination("linear");
	}

	if (mModeling->experiment().combination() == "linear") {
	  mRadioButtonLinear->set_active();
	} else {
	  mRadioButtonTotal->set_active();
	}

	mHboxCombi->pack_start(*mRadioButtonLinear, true, false, 5);
	mHboxCombi->pack_start(*mRadioButtonTotal, true, false, 5);

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
    mEntryDate->set_text(date);
}

void ExperimentBox::on_now()
{
    mEntryDate->set_text(utils::get_current_date());
}

}} // namespace vle gvle
