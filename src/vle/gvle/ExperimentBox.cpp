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
#include <vle/gvle/Modeling.hpp>
#include <vle/gvle/WarningBox.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <vle/graph/Model.hpp>
#include <vle/utils/Tools.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

using namespace vle;

namespace vle
{
namespace gvle {

ExperimentBox::ExperimentBox(Glib::RefPtr<Gnome::Glade::Xml> xml, Modeling* modeling) :
        mModeling(modeling),
        mDialog(0),
        mButtonApply(0),
        mButtonCancel(0)
{
    xml->get_widget("DialogExperiment", mDialog);

    xml->get_widget("EntryName", mEntryName);

    xml->get_widget("EntryDuration", mEntryDuration);

    xml->get_widget("RadioSimulation", mRadioSimu);
    mRadioSimu->signal_pressed().connect(sigc::mem_fun
                                         (*this, &ExperimentBox::on_simu));
    xml->get_widget("RadioPlan", mRadioPlan);
    mRadioPlan->signal_pressed().connect(sigc::mem_fun
                                         (*this, &ExperimentBox::on_plan));

    if ((modeling->experiment().replicas().seed() == 0) && (modeling->experiment().replicas().seed() == 0))
        mRadioSimu->toggled();

    xml->get_widget("HBoxSimu", mHboxSimu);

    xml->get_widget("EntrySimuSeed", mEntrySimuSeed);
    xml->get_widget("ButtonSimuSeed", mButtonSimuSeed);
    mButtonSimuSeed->signal_clicked().connect(sigc::mem_fun
            (*this, &ExperimentBox::on_random_simu));

    xml->get_widget("HBoxPlan", mHboxPlan);

    xml->get_widget("HBoxCombi", mHboxCombi);
    mComboCombi = new Gtk::ComboBoxText();
    mComboCombi->append_text("linear");
    mComboCombi->append_text("total");
    mHboxCombi->pack_start(*mComboCombi);

    xml->get_widget("EntryPlanSeed", mEntryPlanSeed);
    xml->get_widget("ButtonPlanSeed", mButtonPlanSeed);
    if (mButtonPlanSeed)
        mButtonPlanSeed->signal_clicked().connect(sigc::mem_fun
                (*this, &ExperimentBox::on_random_plan));

    xml->get_widget("SpinButtonNumber", mButtonNumber);
    mButtonNumber->set_range(1, UINT_MAX);

    xml->get_widget("ButtonApply", mButtonApply);
    if (mButtonApply)
        mButtonApply->signal_clicked().connect(sigc::mem_fun
                                               (*this, &ExperimentBox::on_apply));

    xml->get_widget("ButtonCancel", mButtonCancel);
    if (mButtonCancel)
        mButtonCancel->signal_clicked().connect(sigc::mem_fun
                                                (*this, &ExperimentBox::on_cancel));
}

ExperimentBox::~ExperimentBox()
{
    delete mComboCombi;
}

void ExperimentBox::show()
{
    mEntryName->set_text(mModeling->experiment().name());
    mEntryDuration->set_text(utils::to_string(mModeling->experiment().duration()));
    mEntrySimuSeed->set_text(utils::to_string(mModeling->experiment().seed()));
    mComboCombi->set_active_text(mModeling->experiment().combination());
    mEntryPlanSeed->set_text(utils::to_string(mModeling->experiment().replicas().seed()));

    mDialog->show_all();
    if (mRadioSimu->get_active())
        on_simu();
    else
        on_plan();

    mDialog->run();
}

void ExperimentBox::on_simu()
{
    mHboxSimu->show_all();
    mHboxPlan->hide_all();
}

void ExperimentBox::on_plan()
{
    mHboxSimu->hide_all();
    mHboxPlan->show_all();
}

void ExperimentBox::on_random_simu()
{
    mEntrySimuSeed->set_text(utils::to_string(mRand.get_int()));
}

void ExperimentBox::on_random_plan()
{
    mEntryPlanSeed->set_text(utils::to_string(mRand.get_int()));
}

void ExperimentBox::on_apply()
{

    std::string error = "";
    if (mEntryName->get_text() == "")
        error += "Set a name to this experiment";

    if (mEntryDuration->get_text() == "")
        error += "Set a duration to this experiment\n";
    else {
        if ((!utils::is_double(mEntryDuration->get_text()))) {
            error += "Set a CORRECT duration to this experiment\n";
        } else if (utils::to_double(mEntryDuration->get_text()) <= 0) {
            error += "Set a CORRECT duration to this experiment\n";
        }
    }

    if (mRadioSimu->get_active()) {
        //Simulation
        bool is_uint = true;
        try {
            boost::lexical_cast<long>(mEntrySimuSeed->get_text());
        } catch (boost::bad_lexical_cast) {
            is_uint = false;
        }

        if (!is_uint)  {
            error += "Set a CORRECT seed to this experiment\n";
        } else if (utils::to_long(mEntrySimuSeed->get_text()) < 0) {
            error += "Set a CORRECT seed to this experiment\n";
        }
    } else {
        //Plan
        bool is_uint = true;

        if (mComboCombi->get_active_text() == "")
            error += "Set a Combination to this experiment\n";

        try {
            boost::lexical_cast<long>(mEntryPlanSeed->get_text());
        } catch (boost::bad_lexical_cast) {
            is_uint = false;
        }
        if (!is_uint)  {
            error += "Set a CORRECT seed to this experiment\n";
        } else if (utils::to_long(mEntryPlanSeed->get_text()) < 0) {
            error += "Set a CORRECT seed to this experiment\n";
        }
    }

    if (error != "") {
        WarningBox box(error);
        box.run();
    } else {
        vpz::Experiment& exp = mModeling->experiment();
        vpz::Replicas& rep = exp.replicas();

        exp.setName(mEntryName->get_text());
        exp.setDuration(utils::to_double(mEntryDuration->get_text()));
        if (mRadioSimu->get_active()) {
            //Simulation
            exp.setSeed(utils::to_int(mEntrySimuSeed->get_text()));
        } else {
            //Plan
            exp.setCombination(mComboCombi->get_active_text());
            rep.setSeed(utils::to_int(mEntryPlanSeed->get_text()));
            rep.setNumber(utils::to_int(mEntrySimuSeed->get_text()));
        }

        if (mDialog)
            mDialog->hide();
    }
}

void ExperimentBox::on_cancel()
{
    if (mDialog)
        mDialog->hide();
}

}
} // namespace vle gvle
