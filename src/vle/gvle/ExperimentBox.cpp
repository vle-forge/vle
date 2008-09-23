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
#include <limits>

using namespace vle;

namespace vle
{
namespace gvle {

ExperimentBox::ExperimentBox(Glib::RefPtr<Gnome::Glade::Xml> xml,
                             Modeling* modeling) :
    mModeling(modeling),
    mDialog(0),
    mButtonApply(0),
    mButtonCancel(0)
{
    xml->get_widget("DialogExperiment", mDialog);

    xml->get_widget("EntryName", mEntryName);

    xml->get_widget("SpinButtonDuration", mSpinDuration);
    mSpinDuration->set_range(
        std::numeric_limits < double >::epsilon(),
        std::numeric_limits < double >::max());

    xml->get_widget("RadioSimulation", mRadioSimu);
    xml->get_widget("RadioPlan", mRadioPlan);
    if ((modeling->experiment().replicas().seed() == 0) &&
        (modeling->experiment().replicas().seed() == 0))
        mRadioSimu->toggled();

    xml->get_widget("frameExperimentSimulation", mHboxSimu);

    xml->get_widget("SpinSimuSeed", mSpinSimuSeed);
    mSpinSimuSeed->set_range(0, std::numeric_limits < guint32 >::max());
    on_random_simu();

    xml->get_widget("ButtonSimuSeed", mButtonSimuSeed);
    mButtonSimuSeed->signal_clicked().connect(sigc::mem_fun
            (*this, &ExperimentBox::on_random_simu));

    xml->get_widget("frameExperimentPlan", mHboxPlan);

    xml->get_widget("HBoxCombi", mHboxCombi);
    mComboCombi = new Gtk::ComboBoxText();
    mComboCombi->append_text("linear");
    mComboCombi->append_text("total");
    mHboxCombi->pack_start(*mComboCombi, Gtk::PACK_SHRINK, 5);

    xml->get_widget("SpinPlanSeed", mSpinPlanSeed);
    mSpinPlanSeed->set_range(0, std::numeric_limits < guint32 >::max());
    on_random_plan();

    xml->get_widget("ButtonPlanSeed", mButtonPlanSeed);
    if (mButtonPlanSeed)
        mButtonPlanSeed->signal_clicked().connect(sigc::mem_fun
                (*this, &ExperimentBox::on_random_plan));

    xml->get_widget("SpinButtonNumber", mButtonNumber);
    mButtonNumber->set_range(1, std::numeric_limits < guint32 >::max());

    xml->get_widget("ButtonApply", mButtonApply);
    if (mButtonApply)
        mButtonApply->signal_clicked().connect(sigc::mem_fun
                                               (*this, &ExperimentBox::on_apply));

    xml->get_widget("ButtonCancel", mButtonCancel);
    if (mButtonCancel) {
        mButtonCancel->signal_clicked().connect(
            sigc::mem_fun (*this, &ExperimentBox::on_cancel));
    }

    mRadioPlan->signal_toggled().connect(
        sigc::mem_fun (*this, &ExperimentBox::on_plan));
    mRadioSimu->signal_toggled().connect(
        sigc::mem_fun (*this, &ExperimentBox::on_simu));
}

ExperimentBox::~ExperimentBox()
{
    delete mComboCombi;
}

void ExperimentBox::show()
{
    mEntryName->set_text(mModeling->experiment().name());
    mSpinDuration->set_value(mModeling->experiment().duration());
    mSpinSimuSeed->set_value(mModeling->experiment().seed());
    mComboCombi->set_active_text(mModeling->experiment().combination());
    mSpinPlanSeed->set_value(mModeling->experiment().replicas().seed());

    mDialog->show_all();
    if (mRadioSimu->get_active())
        on_simu();
    else
        on_plan();

    mDialog->run();
}

void ExperimentBox::on_simu()
{
    if (mRadioSimu->get_active()) {
        mHboxSimu->show_all();
        mHboxPlan->hide_all();
    }
}

void ExperimentBox::on_plan()
{
    if (mRadioPlan->get_active()) {
        mHboxSimu->hide_all();
        mHboxPlan->show_all();
    }
}

void ExperimentBox::on_random_simu()
{
    mSpinSimuSeed->set_value(mRand.get_int());
}

void ExperimentBox::on_random_plan()
{
    mSpinPlanSeed->set_value(mRand.get_int());
}

void ExperimentBox::on_apply()
{
    std::string error = "";
    if (mEntryName->get_text() == "") {
        error += "Set a name to this experiment";
    }

    if (!mRadioSimu->get_active()) {
        if (mComboCombi->get_active_text() == "")
            error += "Set a Combination to this experiment\n";
    }

    if (error != "") {
        Error(error);
    } else {
        vpz::Experiment& exp = mModeling->experiment();
        vpz::Replicas& rep = exp.replicas();

        exp.setName(mEntryName->get_text());
        exp.setDuration(mSpinDuration->get_value());
        if (mRadioSimu->get_active()) {
            //Simulation
            exp.setSeed(mSpinSimuSeed->get_value());
        } else {
            //Plan
            exp.setCombination(mComboCombi->get_active_text());
            rep.setSeed(mSpinPlanSeed->get_value());
            rep.setNumber(mButtonNumber->get_value());
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
