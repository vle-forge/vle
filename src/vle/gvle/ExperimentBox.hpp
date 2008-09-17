/**
 * @file vle/gvle/ExperimentBox.hpp
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


#ifndef GUI_EXPERIMENTBOX_HPP
#define GUI_EXPERIMENTBOX_HPP

#include <vle/utils/Rand.hpp>
#include <gtkmm.h>
#include <libglademm.h>
#include <iostream>

namespace vle
{
namespace gvle {

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
    ExperimentBox(Glib::RefPtr<Gnome::Glade::Xml> xml, Modeling* modeling);

    ~ExperimentBox();

    void show();

private:
    Modeling*           mModeling;
    Gtk::Dialog*        mDialog;

    Gtk::Entry*         mEntryName;

    Gtk::SpinButton*    mSpinDuration;

    Gtk::RadioButton*   mRadioSimu;
    Gtk::HBox*          mHboxSimu;

    Gtk::SpinButton*    mSpinSimuSeed;
    Gtk::Button*        mButtonSimuSeed;

    Gtk::RadioButton*   mRadioPlan;
    Gtk::HBox*          mHboxPlan;

    Gtk::HBox*          mHboxCombi;
    Gtk::ComboBoxText*  mComboCombi;

    Gtk::SpinButton*    mSpinPlanSeed;
    Gtk::Button*        mButtonPlanSeed;

    Gtk::SpinButton*    mButtonNumber;

    Gtk::Button*        mButtonApply;
    Gtk::Button*        mButtonCancel;

    utils::Rand         mRand;

    void on_simu();
    void on_plan();

    void on_random_simu();
    void on_random_plan();

    void on_apply();
    void on_cancel();
};

}
} // namespace vle gvle

#endif
