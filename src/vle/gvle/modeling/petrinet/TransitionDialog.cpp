/*
 * @file vle/gvle/modeling/petrinet/TransitionDialog.cpp
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


#include <vle/gvle/modeling/petrinet/TransitionDialog.hpp>
#include <vle/utils/i18n.hpp>
#include <boost/algorithm/string.hpp>
#include <gtkmm/stock.h>
#include <iostream>

namespace vle {
namespace gvle {
namespace modeling {
namespace petrinet {

TransitionDialog::TransitionDialog(
    const Glib::RefPtr < Gnome::Glade::Xml >& xml,
    const PetriNet& petrinet,
    const Transition* transition) : mPetriNet(petrinet),
    mTransition(transition)
{
    xml->get_widget("TransitionDialog", mDialog);
    xml->get_widget("TransitionNameEntry", mNameEntry);
    xml->get_widget("TransitionStatusImage", mStatusName);
    xml->get_widget("TransitionPriorityEntry", mPriorityEntry);
    //input
    xml->get_widget("TransitionInputHBox", mInputHBox);
    xml->get_widget("TransitionInputCheckbox", mInputCheckbox);
    mInputEntry = Gtk::manage(new Gtk::ComboBoxEntryText);
    mInputHBox->pack_start(*mInputEntry, true, true);
    mInputHBox->show_all();
    // output
    xml->get_widget("TransitionOutputHBox", mOutputHBox);
    xml->get_widget("TransitionOutputCheckbox", mOutputCheckbox);
    mOutputEntry = Gtk::manage(new Gtk::ComboBoxEntryText);
    mOutputHBox->pack_start(*mOutputEntry, true, true);
    mOutputHBox->show_all();
    // delay
    xml->get_widget("TransitionDelayCheckbox", mDelayCheckbox);
    xml->get_widget("TransitionDelayEntry", mDelayEntry);

    xml->get_widget("TransitionOkButton", mOkButton);
    mList.push_back(mNameEntry->signal_changed().connect(
            sigc::mem_fun(*this, &TransitionDialog::onChangeName)));
    mList.push_back(mInputCheckbox->signal_toggled().connect(
            sigc::mem_fun(*this, &TransitionDialog::onInput)));
    mList.push_back(mOutputCheckbox->signal_toggled().connect(
            sigc::mem_fun(*this, &TransitionDialog::onOutput)));
    mList.push_back(mDelayCheckbox->signal_toggled().connect(
            sigc::mem_fun(*this, &TransitionDialog::onDelay)));
}

TransitionDialog::~TransitionDialog()
{
    mInputHBox->remove(*mInputEntry);
    mOutputHBox->remove(*mOutputEntry);

    for (std::list < sigc::connection >::iterator it = mList.begin();
         it != mList.end(); ++it) {
        it->disconnect();
    }
}

void TransitionDialog::onChangeName()
{
    std::string entryName = name();

    if (entryName.empty()) {
        mOkButton->set_sensitive(false);
        mStatusName->set(Gtk::StockID(Gtk::Stock::NO),
                         Gtk::IconSize(1));
    } else {
        if (entryName != mInitialName){
            if (not(mPetriNet.existTransition(entryName))) {

                mStatusName->set(Gtk::StockID(Gtk::Stock::YES),
                                 Gtk::IconSize(1));
                mOkButton->set_sensitive(true);
            } else {
                mStatusName->set(Gtk::StockID(Gtk::Stock::NO),
                                 Gtk::IconSize(1));
                mOkButton->set_sensitive(false);
            }
        } else {
            mStatusName->set(Gtk::StockID(Gtk::Stock::YES),
                             Gtk::IconSize(1));
            mOkButton->set_sensitive(true);
        }
    }
}

void TransitionDialog::onDelay()
{
    mDelayEntry->set_sensitive(mDelayCheckbox->get_active());
}

void TransitionDialog::onInput()
{
    mInputEntry->set_sensitive(input());
    mPriorityEntry->set_sensitive(not input());
}

void TransitionDialog::onOutput()
{
    mOutputEntry->set_sensitive(output());
    mPriorityEntry->set_sensitive(not output());
}

int TransitionDialog::run()
{
    // input
    mInputEntry->clear_items();
    for (strings_t::const_iterator it = mPetriNet.inputPorts().begin();
         it != mPetriNet.inputPorts().end(); ++it) {
        mInputEntry->append_text(*it);
    }
    // output
    mOutputEntry->clear_items();
    for (strings_t::const_iterator it = mPetriNet.outputPorts().begin();
         it != mPetriNet.outputPorts().end(); ++it) {
        mOutputEntry->append_text(*it);
    }

    if (mTransition) {
        mNameEntry->set_text(mTransition->name());
        mInitialName = mTransition->name();
        mPriorityEntry->set_text((fmt("%1%") %
                                  mTransition->priority()).str());
        mInputCheckbox->set_active(mTransition->input());
        if (mTransition->input()) {
            if (find(mPetriNet.inputPorts().begin(),
                     mPetriNet.inputPorts().end(),
                     mTransition->inputPortName()) !=
                mPetriNet.inputPorts().end()) {
                mInputEntry->set_active_text(mTransition->inputPortName());
            } else {
                mInputEntry->get_entry()->set_text(mTransition->inputPortName());
            }
            mInputEntry->set_active_text(mTransition->inputPortName());
        } else {
            mInputEntry->set_sensitive(false);
        }
        mOutputCheckbox->set_active(mTransition->output());
        if (mTransition->output()) {
            if (find(mPetriNet.outputPorts().begin(),
                     mPetriNet.outputPorts().end(),
                     mTransition->outputPortName()) !=
                mPetriNet.outputPorts().end()) {

                mOutputEntry->set_active_text(mTransition->outputPortName());
            } else {
                mOutputEntry->get_entry()->set_text(mTransition->outputPortName());
            }
        } else {
            mOutputEntry->set_sensitive(false);
        }
        mDelayCheckbox->set_active(mTransition->delay());
        mDelayEntry->set_sensitive(mTransition->delay());
        if (mTransition->delay()) {
            mDelayEntry->set_text((fmt("%1%") %
                                   mTransition->delayValue()).str());
        }
        mPriorityEntry->set_sensitive(not mTransition->output() and
            not mTransition->input());
    } else {
        mInitialName = "";
        mPriorityEntry->set_text("0");
        mInputCheckbox->set_active(false);
        mInputEntry->set_sensitive(false);
        mOutputCheckbox->set_active(false);
        mOutputEntry->set_sensitive(false);
        mDelayCheckbox->set_active(false);
        mDelayEntry->set_sensitive(false);
    }

    mDialog->set_default_response(Gtk::RESPONSE_ACCEPT);

    int response = mDialog->run();

    mDialog->hide();
    return response;
}

}
}
}
}    // namespace vle gvle modeling petrinet
