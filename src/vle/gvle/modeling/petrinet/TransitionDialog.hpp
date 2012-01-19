/*
 * @file vle/gvle/modeling/petrinet/TransitionDialog.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2012 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2012 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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


#ifndef VLE_GVLE_MODELING_PETRINET_TRANSITION_DIALOG_HPP
#define VLE_GVLE_MODELING_PETRINET_TRANSITION_DIALOG_HPP

#include <vle/gvle/modeling/petrinet/PetriNet.hpp>
#include <gtkmm/checkbutton.h>
#include <gtkmm/comboboxentrytext.h>
#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <gtkmm/image.h>
#include <boost/algorithm/string.hpp>
#include <gtkmm/builder.h>

namespace vle {
namespace gvle {
namespace modeling {
namespace petrinet {

class TransitionDialog
{
public:
    TransitionDialog(const Glib::RefPtr < Gtk::Builder >& xml,
                     const PetriNet& petrinet,
                     const Transition* transition = 0);

    virtual ~TransitionDialog();

    bool delay() const
    {
        return mDelayCheckbox->get_active();
    }

    std::string delayValue() const
    {
        return mDelayEntry->get_text();
    }

    bool input() const
    {
        return mInputCheckbox->get_active();
    }

    std::string inputPortName() const
    {
        std::string inputPortName = mInputEntry->get_entry()->get_text();
        boost::trim(inputPortName);
        return inputPortName;
    }

    std::string name() const
    {
        std::string entryName = mNameEntry->get_text();
        boost::trim(entryName);
        return entryName;
    }

    bool output() const
    {
        return mOutputCheckbox->get_active();
    }

    std::string outputPortName() const
    {
        std::string outputPortName = mOutputEntry->get_entry()->get_text();
        boost::trim(outputPortName);
        return outputPortName;
    }

    std::string priority() const
    {
        return mPriorityEntry->get_text();
    }

    int run();

private:
    void onChangeName();
    void onDelay();
    void onInput();
    void onOutput();
    void onChangeOutput();
    void onChangeInput();

    Gtk::Dialog* mDialog;
    Gtk::Entry* mNameEntry;
    Gtk::Entry* mPriorityEntry;
    Gtk::Image* mStatusName;
    // input
    Gtk::HBox* mInputHBox;
    Gtk::CheckButton* mInputCheckbox;
    Gtk::ComboBoxEntryText* mInputEntry;
    // output
    Gtk::HBox* mOutputHBox;
    Gtk::CheckButton* mOutputCheckbox;
    Gtk::ComboBoxEntryText* mOutputEntry;
    // delay
    Gtk::CheckButton* mDelayCheckbox;
    Gtk::Entry* mDelayEntry;

    const PetriNet& mPetriNet;
    const Transition* mTransition;
    std::string mInitialName;

    Gtk::Button* mOkButton;

    std::list < sigc::connection > mList;
};

}
}
}
}    // namespace vle gvle modeling petrinet

#endif
