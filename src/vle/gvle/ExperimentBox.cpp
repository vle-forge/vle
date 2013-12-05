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


#include <vle/gvle/ExperimentBox.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/gvle/CalendarBox.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/DateTime.hpp>
#include <glibmm/miscutils.h>

namespace vz = vle::vpz;
namespace vu = vle::utils;

namespace vle { namespace gvle {

ExperimentBox::ExperimentBox(const Glib::RefPtr < Gtk::Builder >& xml,
                             Modeling* modeling)
    : mModeling(modeling), mXml(xml), mDialog(0)
{
    xml->get_widget("EntryName", mEntryName);
    xml->get_widget("EntryAuthor", mEntryAuthor);
    xml->get_widget("DialogExperiment", mDialog);

    xml->get_widget("EntryDate", mEntryDate);
    xml->get_widget("ButtonCalendar", mButtonCalendar);
    xml->get_widget("ButtonNow", mButtonNow);
    xml->get_widget("EntryVersion", mEntryVersion);
    xml->get_widget("HBoxCombi", mHboxCombi);
    xml->get_widget("RadioButtonLinear", mRadioButtonLinear);
    xml->get_widget("RadioButtonTotal", mRadioButtonTotal);

    mBeginRealMin = utils::DateTime::toJulianDay("1400-01-01 00:00:00");
    mBeginRealMax = utils::DateTime::toJulianDay("9999-12-31 23:59:59");

    initExperiment();

    mSigcConnection.push_back(mButtonCalendar->signal_clicked().connect(
        sigc::mem_fun(*this, &ExperimentBox::on_calendar)));
    mSigcConnection.push_back(mButtonNow->signal_clicked().connect(
        sigc::mem_fun(*this, &ExperimentBox::on_now)));

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

    // Experiment frame
    {
        if (experiment.name().empty()) {
            mEntryName->set_text("exp");
        } else {
            mEntryName->set_text(experiment.name());
        }
    }

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
        mEntryVersion->set_text(vpz.project().version());
    }

    // Plan frame
    {
        if (experiment.combination().empty() or
                experiment.combination() == "linear") {
            mRadioButtonLinear->set_active();
        } else {
            mRadioButtonTotal->set_active();
        }
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

    exp.setName(mEntryName->get_text());

    {
        exp.setCombination(mRadioButtonLinear->get_active()?"linear":"total");
    }
    return true;
}

void ExperimentBox::on_calendar()
{
    std::string date;
    long year, month, day;

    utils::DateTime::currentDate(year, month, day);

    CalendarBox cal(mXml);
    cal.selectDate((int)day, (int)month, (int)year);

    cal.date(date);
    if (not date.empty()) {
        mEntryDate->set_text(date);
    }
}



void ExperimentBox::on_now()
{
    try {
        mEntryDate->set_text(utils::DateTime::currentDate());
    } catch(...) {
    }
}

}} // namespace vle gvle
