/**
 * @file vle/gvle/modeling/fsa/TransitionDialog.cpp
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

#include <vle/gvle/modeling/fsa/TransitionDialog.hpp>
#include <vle/gvle/modeling/fsa/SourceDialog.hpp>
#include <vle/utils/i18n.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace vle { namespace gvle { namespace modeling { namespace fsa {

TransitionDialog::TransitionDialog(
    const Glib::RefPtr < Gnome::Glade::Xml >& xml,
    const Transition* transition, Statechart* statechart) : mXml(xml),
    mTransition(transition), mStatechart(statechart)
{
    xml->get_widget("TransitionDialog", mDialog);
    xml->get_widget("ComboVBox", mComboVBox);

    // buttons
    xml->get_widget("AfterSourceButton", mAfterButton);
    xml->get_widget("WhenSourceButton", mWhenButton);
    xml->get_widget("GuardSourceButton", mGuardButton);
    xml->get_widget("ActionSourceButton", mActionButton);
    xml->get_widget("SendSourceButton", mSendButton);

    xml->get_widget("TransitionOkButton", mOkButton);

    // event
    mEventEntry = Gtk::manage(new Gtk::ComboBoxEntryText);
    mComboVBox->pack_start(*mEventEntry, true, true);

    // after
    mAfterEntry = Gtk::manage(new Gtk::ComboBoxEntryText);
    mComboVBox->pack_start(*mAfterEntry, true, true);

    // when
    mWhenEntry = Gtk::manage(new Gtk::ComboBoxEntryText);
    mComboVBox->pack_start(*mWhenEntry, true, true);

    // guard
    mGuardEntry = Gtk::manage(new Gtk::ComboBoxEntryText);
    mComboVBox->pack_start(*mGuardEntry, true, true);

    // action
    mActionEntry = Gtk::manage(new Gtk::ComboBoxEntryText);
    mComboVBox->pack_start(*mActionEntry, true, true);

    // send
    mSendHBox = Gtk::manage(new Gtk::HBox);
    mOutputPortEntry = Gtk::manage(new Gtk::ComboBoxEntryText);
    mSendHBox->pack_start(*mOutputPortEntry, true, true);
    mSendEntry = Gtk::manage(new Gtk::ComboBoxEntryText);
    mSendHBox->pack_start(*mSendEntry, true, true);
    mComboVBox->pack_start(*mSendHBox, true, true);

    mComboVBox->show_all();

    mList.push_back(mActionButton->signal_clicked().connect(
                        sigc::mem_fun(*this,
                                      &TransitionDialog::onActionSource)));
    mList.push_back(mActionEntry->signal_changed().connect(
                        sigc::mem_fun(*this, &TransitionDialog::onAction)));
    mList.push_back(mAfterButton->signal_clicked().connect(
                        sigc::mem_fun(*this,
                                      &TransitionDialog::onAfterSource)));
    mList.push_back(mAfterEntry->signal_changed().connect(
                        sigc::mem_fun(*this, &TransitionDialog::onAfter)));
    mList.push_back(mGuardButton->signal_clicked().connect(
                        sigc::mem_fun(*this,
                                      &TransitionDialog::onGuardSource)));
    mList.push_back(mGuardEntry->signal_changed().connect(
                        sigc::mem_fun(*this, &TransitionDialog::onGuard)));
    mList.push_back(mSendButton->signal_clicked().connect(
                        sigc::mem_fun(*this,
                                      &TransitionDialog::onSendSource)));
    mList.push_back(mSendEntry->signal_changed().connect(
                        sigc::mem_fun(*this, &TransitionDialog::onSend)));
    mList.push_back(mWhenButton->signal_clicked().connect(
                        sigc::mem_fun(*this,
                                      &TransitionDialog::onWhenSource)));
    mList.push_back(mWhenEntry->signal_changed().connect(
                        sigc::mem_fun(*this, &TransitionDialog::onWhen)));
}

TransitionDialog::~TransitionDialog()
{
    mComboVBox->remove(*mEventEntry);
    mComboVBox->remove(*mAfterEntry);
    mComboVBox->remove(*mWhenEntry);
    mComboVBox->remove(*mGuardEntry);
    mComboVBox->remove(*mActionEntry);
    mComboVBox->remove(*mSendHBox);

    for (std::list < sigc::connection >::iterator it = mList.begin();
         it != mList.end(); ++it) {
        it->disconnect();
    }
}

void TransitionDialog::onAction()
{
    std::string entryAction = action();
    boost::trim(entryAction);
    if (entryAction.empty()) {
        mActionButton->set_sensitive(false);
        if (mActionEntry->get_entry()->get_text().empty()) {
            mOkButton->set_sensitive(true);
        } else {
            mOkButton->set_sensitive(false);
        }
    } else {
        if (isValidName(entryAction)) {
            mTrimAction = entryAction;
            mActionButton->set_sensitive(true);
            mOkButton->set_sensitive(true);
        } else {
            mActionButton->set_sensitive(false);
            mOkButton->set_sensitive(false);
        }
    }
}

void TransitionDialog::onActionSource()
{
    SourceDialog dialog(mXml);
    const std::string& name = mActionEntry->get_entry()->get_text();

    if (not mTransition->event().empty()) {
        const std::string& buffer = mStatechart->eventAction(name);

        if (buffer.empty()) {
            dialog.add(name,
                       (fmt(Statechart::EVENT_ACTION_DEFINITION) % name).str());
        } else {
            dialog.add(name, buffer);
        }
    } else {
        const std::string& buffer = mStatechart->action(name);

        if (buffer.empty()) {
            dialog.add(name,
                       (fmt(Statechart::ACTION_DEFINITION) % name).str());
        } else {
            dialog.add(name, buffer);
        }
    }

    if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
        if (not mTransition->event().empty()) {
            mStatechart->eventAction(name, dialog.buffer(name));
        } else {
            mStatechart->action(name, dialog.buffer(name));
        }
    }
}

void TransitionDialog::onAfter()
{
    std::string entryAfter = after();
    boost::trim(entryAfter);
    if (entryAfter.empty()) {
        mAfterButton->set_sensitive(false);
        if (mAfterEntry->get_entry()->get_text().empty()) {
            mOkButton->set_sensitive(true);
        } else {
            mOkButton->set_sensitive(false);
        }
    } else {
        if (isValidName(entryAfter)) {
            mTrimAfter = entryAfter;
            mAfterButton->set_sensitive(true);
            mOkButton->set_sensitive(true);
        } else {
            mAfterButton->set_sensitive(false);
            mOkButton->set_sensitive(false);
        }
    }
}

void TransitionDialog::onAfterSource()
{
    SourceDialog dialog(mXml);
    const std::string& name = mAfterEntry->get_entry()->get_text();
    const std::string& buffer = mStatechart->after(name);

    if (buffer.empty()) {
        dialog.add(name,
                   (fmt(Statechart::AFTER_DEFINITION) % name).str());
    } else {
        dialog.add(name, buffer);
    }
    if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
        mStatechart->after(name, dialog.buffer(name));
    }
}

void TransitionDialog::onGuard()
{
    std::string entryGuard = guard();
    boost::trim(entryGuard);
    if (entryGuard.empty()) {
        mGuardButton->set_sensitive(false);
        if (mGuardEntry->get_entry()->get_text().empty()) {
            mOkButton->set_sensitive(true);
        } else {
            mOkButton->set_sensitive(false);
        }
    } else {
        if (isValidName(entryGuard)) {
            mTrimGuard = entryGuard;
            mGuardButton->set_sensitive(true);
            mOkButton->set_sensitive(true);
        } else {
            mGuardButton->set_sensitive(false);
            mOkButton->set_sensitive(false);
        }
    }
}

void TransitionDialog::onGuardSource()
{
    SourceDialog dialog(mXml);
    const std::string& name = mGuardEntry->get_entry()->get_text();
    const std::string& buffer = mStatechart->guard(name);

    if (buffer.empty()) {
        dialog.add(name,
                   (fmt(Statechart::GUARD_DEFINITION) % name).str());
    } else {
        dialog.add(name, buffer);
    }
    if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
        mStatechart->guard(name, dialog.buffer(name));
    }
}

void TransitionDialog::onSend()
{
    std::string entrySend = send();
    boost::trim(entrySend);
    if (entrySend.empty() and std::find(mStatechart->outputPorts().begin(),
        mStatechart->outputPorts().end(), entrySend) !=
        mStatechart->outputPorts().end()) {

        mSendButton->set_sensitive(false);
        if (mSendEntry->get_entry()->get_text().empty()) {
            mOkButton->set_sensitive(true);
        } else {
            mOkButton->set_sensitive(false);
        }
    } else {
        if (isValidName(entrySend)) {
            mTrimSend = entrySend;
            mSendButton->set_sensitive(true);
            mOkButton->set_sensitive(true);
        } else {
            mSendButton->set_sensitive(false);
            mOkButton->set_sensitive(false);
        }
    }
}

void TransitionDialog::onSendSource()
{
    SourceDialog dialog(mXml);
    const std::string& name = mSendEntry->get_entry()->get_text();
    const std::string& buffer = mStatechart->send(name);

    if (buffer.empty()) {
        dialog.add(name,
                   (fmt(Statechart::SEND_DEFINITION) % name).str());
    } else {
        dialog.add(name, buffer);
    }
    if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
        mStatechart->send(name, dialog.buffer(name));
    }
}

void TransitionDialog::onWhen()
{
    std::string entryWhen = when();
    boost::trim(entryWhen);
    if (entryWhen.empty()) {
        mWhenButton->set_sensitive(false);
        if (mWhenEntry->get_entry()->get_text().empty()) {
            mOkButton->set_sensitive(true);
        } else {
            mOkButton->set_sensitive(false);
        }
    } else {
        if (isValidName(entryWhen)) {
            mTrimWhen = entryWhen;
            mWhenButton->set_sensitive(true);
            mOkButton->set_sensitive(true);
        } else {
            mWhenButton->set_sensitive(false);
            mOkButton->set_sensitive(false);
        }
    }
}

void TransitionDialog::onWhenSource()
{
    SourceDialog dialog(mXml);
    const std::string& name = mWhenEntry->get_entry()->get_text();
    const std::string& buffer = mStatechart->when(name);

    if (buffer.empty()) {
        dialog.add(name,
                   (fmt(Statechart::WHEN_DEFINITION) % name).str());
    } else {
        dialog.add(name, buffer);
    }
    if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
        mStatechart->when(name, dialog.buffer(name));
    }
}

int TransitionDialog::run()
{
    // event
    mEventEntry->clear_items();
    for (strings_t::const_iterator it = mStatechart->inputPorts().begin();
         it != mStatechart->inputPorts().end(); ++it) {
        mEventEntry->append_text(*it);
    }
    if (not mTransition->event().empty()) {
        mEventEntry->set_active_text(mTransition->event());
    }

    // action
    mActionEntry->clear_items();
    if (not mTransition->event().empty()) {
        for (buffers_t::const_iterator it = mStatechart->eventActions().begin();
             it != mStatechart->eventActions().end(); ++it) {
            mActionEntry->append_text(it->first);
        }
    } else {
        for (buffers_t::const_iterator it = mStatechart->actions().begin();
             it != mStatechart->actions().end(); ++it) {
            mActionEntry->append_text(it->first);
        }
    }
    if (not mTransition->action().empty()) {
        mActionEntry->set_active_text(mTransition->action());
    }
    mActionButton->set_sensitive(not mTransition->action().empty());

    // after
    mAfterEntry->clear_items();
    for (buffers_t::const_iterator it = mStatechart->afters().begin();
         it != mStatechart->afters().end(); ++it) {
        mAfterEntry->append_text(it->first);
    }
    if (not mTransition->after().empty()) {
        mAfterEntry->set_active_text(mTransition->after());
    }
    mAfterButton->set_sensitive(not mTransition->after().empty());

    // guard
    mGuardEntry->clear_items();
    for (buffers_t::const_iterator it = mStatechart->guards().begin();
         it != mStatechart->guards().end(); ++it) {
        mGuardEntry->append_text(it->first);
    }
    if (not mTransition->guard().empty()) {
        mGuardEntry->set_active_text(mTransition->guard());
    }
    mGuardButton->set_sensitive(not mTransition->guard().empty());

    // send
    mSendEntry->clear_items();
    for (strings_t::const_iterator it = mStatechart->outputPorts().begin();
         it != mStatechart->outputPorts().end(); ++it) {
        mOutputPortEntry->append_text(*it);
    }
    for (buffers_t::const_iterator it = mStatechart->sends().begin();
         it != mStatechart->sends().end(); ++it) {
        mSendEntry->append_text(it->first);
    }
    if (not mTransition->send().empty()) {
        if (std::find(mStatechart->outputPorts().begin(),
                      mStatechart->outputPorts().end(),
                      mTransition->send()) !=
            mStatechart->outputPorts().end()) {
            mOutputPortEntry->set_active_text(mTransition->send());
        } else {
            mSendEntry->set_active_text(mTransition->send());
        }
    }
    mSendButton->set_sensitive(not mSendEntry->get_entry()->get_text().empty());

    // when
    mWhenEntry->clear_items();
    for (buffers_t::const_iterator it = mStatechart->whens().begin();
         it != mStatechart->whens().end(); ++it) {
        mWhenEntry->append_text(it->first);
    }
    if (not mTransition->when().empty()) {
        mWhenEntry->set_active_text(mTransition->when());
    }
    mWhenButton->set_sensitive(not mTransition->when().empty());

    mDialog->set_default_response(Gtk::RESPONSE_ACCEPT);

    int response = mDialog->run();

    mDialog->hide();
    return response;
}

}}}} // namespace vle gvle modeling fsa
