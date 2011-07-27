/*
 * @file vle/gvle/modeling/petrinet/PlaceDialog.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
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


#include <vle/gvle/modeling/petrinet/PlaceDialog.hpp>
#include <vle/utils/i18n.hpp>
#include <gtkmm/stock.h>
#include <boost/algorithm/string.hpp>
#include <iostream>

namespace vle {
namespace gvle {
namespace modeling {
namespace petrinet {

PlaceDialog::PlaceDialog(const Glib::RefPtr < Gnome::Glade::Xml >& xml,
                         const PetriNet& petrinet,
                         const Place* place) : mPetriNet(petrinet),
    mPlace(place)
{
    xml->get_widget("PlaceDialog", mDialog);
    xml->get_widget("PlaceNameEntry", mNameEntry);
    xml->get_widget("PlaceStatusImage", mStatusName);
    // output
    xml->get_widget("PlaceOutputHBox", mOutputHBox);
    xml->get_widget("PlaceOutputCheckbox", mOutputCheckbox);
    mOutputEntry = Gtk::manage(new Gtk::ComboBoxEntryText);
    mOutputHBox->pack_start(*mOutputEntry, true, true);
    mOutputHBox->show_all();
    // marking
    xml->get_widget("PlaceMarkingEntry", mMarkingEntry);
    // delay
    xml->get_widget("PlaceDelayCheckbox", mDelayCheckbox);
    xml->get_widget("PlaceDelayEntry", mDelayEntry);

    xml->get_widget("PlaceOkButton", mOkButton);
    mList.push_back(mNameEntry->signal_changed().connect(
            sigc::mem_fun(*this, &PlaceDialog::onChangeName)));
    mList.push_back(mOutputCheckbox->signal_toggled().connect(
            sigc::mem_fun(*this, &PlaceDialog::onOutput)));
    mList.push_back(mDelayCheckbox->signal_toggled().connect(
            sigc::mem_fun(*this, &PlaceDialog::onDelay)));
}

PlaceDialog::~PlaceDialog()
{
    mOutputHBox->remove(*mOutputEntry);

    for (std::list < sigc::connection >::iterator it = mList.begin();
         it != mList.end(); ++it) {
        it->disconnect();
    }
}

void PlaceDialog::onChangeName()
{
    std::string entryName = name();

    if (entryName.empty()) {
        mOkButton->set_sensitive(false);
        mStatusName->set(Gtk::StockID(Gtk::Stock::NO),
                         Gtk::IconSize(1));
    } else {
        if (entryName != mInitialName) {
            if (not(mPetriNet.existPlace(entryName))) {

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

void PlaceDialog::onDelay()
{
    mDelayEntry->set_sensitive(mDelayCheckbox->get_active());
}

void PlaceDialog::onOutput()
{
    mOutputEntry->set_sensitive(output());
    if (output()) {
        mDelayEntry->set_sensitive(false);
        mDelayCheckbox->set_sensitive(false);
    } else {
        mOutputEntry->get_entry()->set_text("");
        mDelayCheckbox->set_sensitive(true);
        mDelayEntry->set_sensitive(mDelayCheckbox->get_active());
    }
}

int PlaceDialog::run()
{
    // output
    mOutputEntry->clear_items();
    for (strings_t::const_iterator it = mPetriNet.outputPorts().begin();
         it != mPetriNet.outputPorts().end(); ++it) {
        mOutputEntry->append_text(*it);
    }

    if (mPlace) {
        mNameEntry->set_text(mPlace->name());
        mInitialName = mPlace->name();
        mOutputCheckbox->set_active(mPlace->output());
        if (mPlace->output()) {
            if (find(mPetriNet.outputPorts().begin(),
                     mPetriNet.outputPorts().end(),
                     mPlace->outputPortName()) !=
                mPetriNet.outputPorts().end()) {

                mOutputEntry->set_active_text(mPlace->outputPortName());
            } else {
                mOutputEntry->get_entry()->set_text(mPlace->outputPortName());
            }
            mDelayEntry->set_sensitive(false);
            mDelayEntry->set_text("");
            mDelayCheckbox->set_sensitive(false);
            mDelayCheckbox->set_active(false);
        } else {
            mOutputEntry->set_sensitive(false);
            mDelayCheckbox->set_active(mPlace->delay());
            mDelayEntry->set_sensitive(mPlace->delay());
            if (mPlace->delay()) {
                mDelayEntry->set_text((fmt("%1%") %
                                       mPlace->delayValue()).str());
            } else {
                mDelayEntry->set_text("");
            }
        }
        mMarkingEntry->set_text((fmt(
                                     "%1%") %
                                 mPlace->initialMarking()).str());
    } else {
        mNameEntry->set_text("");
        mInitialName = "";
        mOutputCheckbox->set_active(false);
        mOutputEntry->set_sensitive(false);
        mMarkingEntry->set_text("0");
        mDelayCheckbox->set_active(false);
        mDelayEntry->set_sensitive(false);
    }
    onChangeName();
    mDialog->set_default_response(Gtk::RESPONSE_ACCEPT);

    int response = mDialog->run();

    mDialog->hide();
    return response;
}

}
}
}
}    // namespace vle gvle modeling petrinet
