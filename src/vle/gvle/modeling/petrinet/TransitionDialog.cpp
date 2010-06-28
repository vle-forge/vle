/**
 * @file vle/gvle/modeling/petrinet/TransitionDialog.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2010 ULCO http://www.univ-littoral.fr
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
#include <gtkmm/stock.h>

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
    setStatus();
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
            mInputEntry->set_active_text(mTransition->inputPortName());
        } else {
            mInputEntry->set_sensitive(false);
        }
        mOutputCheckbox->set_active(mTransition->output());
        if (mTransition->output()) {
            mOutputEntry->set_active_text(mTransition->outputPortName());
        } else {
            mOutputEntry->set_sensitive(false);
        }
        mDelayCheckbox->set_active(mTransition->delay());
        mDelayEntry->set_sensitive(mTransition->delay());
        if (mTransition->delay()) {
            mDelayEntry->set_text((fmt("%1%") %
                                   mTransition->delayValue()).str());
        } else {
            mDelayEntry->set_text("");
        }
        mPriorityEntry->set_sensitive(not mTransition->output() and
            not mTransition->input());
    } else {
        mNameEntry->set_text("");
        mInitialName = "";
        mPriorityEntry->set_text("0");
        mInputCheckbox->set_active(false);
        mInputEntry->set_sensitive(false);
        mOutputCheckbox->set_active(false);
        mOutputEntry->set_sensitive(false);
        mDelayCheckbox->set_active(false);
        mDelayEntry->set_sensitive(false);
    }
    setStatus();
    mDialog->set_default_response(Gtk::RESPONSE_ACCEPT);

    int response = mDialog->run();

    mDialog->hide();
    return response;
}

void TransitionDialog::setStatus()
{
    if (valid()) {
        mStatusName->set(Gtk::StockID(Gtk::Stock::YES), Gtk::IconSize(1));
    } else {
        mStatusName->set(Gtk::StockID(Gtk::Stock::NO), Gtk::IconSize(1));
    }
}

}
}
}
}    // namespace vle gvle modeling petrinet
