/*
 * @file vle/gvle/modeling/fsa/StateDialog.cpp
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


#include <vle/gvle/modeling/fsa/StateDialog.hpp>
#include <vle/gvle/modeling/fsa/SourceDialog.hpp>
#include <vle/utils/i18n.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>

namespace vle {
namespace gvle {
namespace modeling {
namespace fsa {

NewStateDialog::NewStateDialog(
    const Glib::RefPtr < Gtk::Builder >& xml,
    const Statechart& statechart) : mStatechart(statechart)
{
    xml->get_widget("NewStateDialog", mDialog);
    xml->get_widget("NewStateNameEntry", mNameEntry);
    xml->get_widget("PlaceStatusImage", mStatusName);
    xml->get_widget("NewStateInitialCheckbox", mInitialCheckbox);
    xml->get_widget("StateNameOkButton", mOkButton);
    mOkButton->set_sensitive(false);
    mList.push_back(mNameEntry->signal_changed().connect(
            sigc::mem_fun(*this, &NewStateDialog::onChangeName)));
}

void NewStateDialog::onChangeName()
{
    std::string entryName = name();

    if (entryName.empty()) {
        mStatusName->set(Gtk::StockID(Gtk::Stock::NO),
                         Gtk::IconSize(1));
        mOkButton->set_sensitive(false);
    } else {
        if (not(mStatechart.existState(entryName)) &&
            entryName != "" &&
            isValidName(entryName)) {

            mStatusName->set(Gtk::StockID(Gtk::Stock::YES),
                             Gtk::IconSize(1));
            mOkButton->set_sensitive(true);
        } else {
            mStatusName->set(Gtk::StockID(Gtk::Stock::NO),
                             Gtk::IconSize(1));
            mOkButton->set_sensitive(false);
        }
    }
}

int NewStateDialog::run()
{
    mInitialCheckbox->set_active(false);
    mDialog->set_default_response(Gtk::RESPONSE_ACCEPT);

    int response = mDialog->run();

    mDialog->hide();
    return response;
}

EventInStateDialog::EventInStateDialog(
    const Glib::RefPtr < Gtk::Builder >& xml,
    Statechart* statechart, const eventInStates_t& eventInStates) :
    mXml(xml), mStatechart(statechart), mEventInStates(eventInStates)
{
    xml->get_widget("EventInStateDialog", mDialog);
    xml->get_widget("EventInStateNameEntry", mEventEntry);
    xml->get_widget("EventInStateVBox", mEventInStateVBox);
    mActionEntry = Gtk::manage(new Gtk::ComboBoxEntryText);
    mEventInStateVBox->pack_start(*mActionEntry, true, true);
    mActionEntry->show();
    xml->get_widget("EventInStateButton", mActionButton);

    mList.push_back(mActionEntry->signal_changed().connect(
            sigc::mem_fun(*this, &EventInStateDialog::onAction)));
    mList.push_back(mActionButton->signal_clicked().connect(
            sigc::mem_fun(*this,
                &EventInStateDialog::onEventActionSource
                )));
}

void EventInStateDialog::onAction()
{
    mActionButton->set_sensitive(
        not mActionEntry->get_entry()->get_text().empty());
}

void EventInStateDialog::onEventActionSource()
{
    SourceDialog dialog(mXml);
    const std::string& name = mActionEntry->get_entry()->get_text();
    const std::string& buffer = mStatechart->eventAction(name);

    if (buffer.empty()) {
        dialog.add(name,
            (fmt(Statechart::EVENT_ACTION_DEFINITION) % name).str());
    } else {
        dialog.add(name, buffer);
    }
    if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
        mStatechart->eventAction(name, dialog.buffer(name));
    }
}

int EventInStateDialog::run(const std::string& event,
                            const std::string& action)
{
    mEventEntry->get_buffer()->set_text(event);

    mActionEntry->clear();
    for (buffers_t::const_iterator it = mStatechart->eventActions().begin();
         it != mStatechart->eventActions().end(); ++it) {
        mActionEntry->append_text(it->first);
    }
    if (not action.empty()) {
        const std::string& buffer = mStatechart->eventAction(action);
        if (buffer.empty()) {
            mActionEntry->get_entry()->set_text(action);
        } else {
            mActionEntry->set_active_text(action);
        }
    }
    mActionButton->set_sensitive(not action.empty());
    mDialog->set_default_response(Gtk::RESPONSE_ACCEPT);

    int response = mDialog->run();

    mDialog->hide();
    return response;
}

void StateDialog::EventInStateTreeView::onAdd()
{
    EventInStateDialog dialog(mXml, mStatechart, *mEventInStates);

    if (dialog.run("", "") == Gtk::RESPONSE_ACCEPT) {
        (*mEventInStates)[dialog.event()] = dialog.action();
        add(dialog.event(), dialog.action());
    }
}

void StateDialog::EventInStateTreeView::onEdit()
{
    Glib::RefPtr < Gtk::TreeView::Selection > selection = get_selection();

    if (selection) {
        Gtk::TreeModel::iterator iter = selection->get_selected();

        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            std::string event = row.get_value(mColumns.mEvent);
            std::string action = row.get_value(mColumns.mAction);
            EventInStateDialog dialog(mXml, mStatechart, *mEventInStates);

            if (dialog.run(event, action) == Gtk::RESPONSE_ACCEPT) {
                mTreeModel->erase(iter);
                (*mEventInStates)[dialog.event()] = dialog.action();
                add(dialog.event(), dialog.action());
            }
        }
    }
}

void StateDialog::EventInStateTreeView::onRemove()
{
    Glib::RefPtr < Gtk::TreeView::Selection > selection = get_selection();

    if (selection) {
        Gtk::TreeModel::iterator iter = selection->get_selected();

        mTreeModel->erase(iter);
    }
}

StateDialog::StateDialog(const Glib::RefPtr < Gtk::Builder >& xml,
                         Statechart* statechart, const State* state) :
    mXml(xml), mStatechart(statechart), mState(state)
{
    xml->get_widget("StateDialog", mDialog);
    xml->get_widget("StateNameEntry", mNameEntry);
    xml->get_widget("StateInitialCheckbox", mInitialCheckbox);

    xml->get_widget("StateVBox", mStateVBox);
    mInActionEntry = Gtk::manage(new Gtk::ComboBoxEntryText);
    mStateVBox->pack_start(*mInActionEntry, true, true);
    mInActionEntry->show();
    mOutActionEntry = Gtk::manage(new Gtk::ComboBoxEntryText);
    mStateVBox->pack_start(*mOutActionEntry, true, true);
    mOutActionEntry->show();
    mActivityEntry = Gtk::manage(new Gtk::ComboBoxEntryText);
    mStateVBox->pack_start(*mActivityEntry, true, true);
    mActivityEntry->show();

    xml->get_widget("InActionSourceButton", mInActionButton);
    xml->get_widget("OutActionSourceButton", mOutActionButton);
    xml->get_widget("ActivitySourceButton", mActivityButton);
    xml->get_widget("StateOkButton", mOkButton);

    xml->get_widget_derived("EventInStateTreeView", mEventInStateTreeView);
    mEventInStateTreeView->setStatechart(mStatechart, &mEventInStates);

    mList.push_back(mNameEntry->signal_changed().connect(
            sigc::mem_fun(*this, &StateDialog::onName)));
    mList.push_back(mInActionButton->signal_clicked().connect(
            sigc::mem_fun(*this,
                &StateDialog::onInActionSource)));
    mList.push_back(mInActionEntry->signal_changed().connect(
            sigc::mem_fun(*this, &StateDialog::onInAction)));
    mList.push_back(mOutActionButton->signal_clicked().connect(
            sigc::mem_fun(*this,
                &StateDialog::onOutActionSource)));
    mList.push_back(mOutActionEntry->signal_changed().connect(
            sigc::mem_fun(*this, &StateDialog::onOutAction)));
    mList.push_back(mActivityButton->signal_clicked().connect(
            sigc::mem_fun(*this,
                &StateDialog::onActivitySource)));
    mList.push_back(mActivityEntry->signal_changed().connect(
            sigc::mem_fun(*this, &StateDialog::onActivity)));
}

void StateDialog::onName()
{
    std::string entryName = name();

    if (entryName.empty()) {
        mOkButton->set_sensitive(false);
    } else {
        if (mState->name() != entryName) {
            if (not(mStatechart->existState(entryName)) &&
                isValidName(entryName)) {

                mOkButton->set_sensitive(true);
            } else {
                mOkButton->set_sensitive(false);
            }
        } else {
            mOkButton->set_sensitive(true);
        }
    }
}

void StateDialog::onActivity()
{
    std::string entryActivity = activity();
    boost::trim(entryActivity);
    if (entryActivity.empty()) {
        mActivityButton->set_sensitive(false);
        if (mActivityEntry->get_entry()->get_text().empty()) {
            mOkButton->set_sensitive(true);
        } else {
            mOkButton->set_sensitive(false);
        }
    } else {
        if (isValidName(entryActivity)) {
            mTrimActivity = entryActivity;
            mActivityButton->set_sensitive(true);
            mOkButton->set_sensitive(true);
        } else {
            mActivityButton->set_sensitive(false);
            mOkButton->set_sensitive(false);
        }
    }
}

void StateDialog::onActivitySource()
{
    SourceDialog dialog(mXml);
    const std::string& buffer = mStatechart->activity(mTrimActivity);

    if (buffer.empty()) {
        dialog.add(mTrimActivity,
            (fmt(Statechart::ACTIVITY_DEFINITION) % mTrimActivity).str());
    } else {
        dialog.add(mTrimActivity, buffer);
    }
    if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
        mStatechart->activity(mTrimActivity, dialog.buffer(mTrimActivity));
    }
}

void StateDialog::onInAction()
{
    std::string entryInAction = inAction();
    boost::trim(entryInAction);
    if (entryInAction.empty()) {
        mInActionButton->set_sensitive(false);
        if (mInActionEntry->get_entry()->get_text().empty()) {
            mOkButton->set_sensitive(true);
        } else {
            mOkButton->set_sensitive(false);
        }
    } else {
        if (isValidName(entryInAction)) {
            mTrimInAction = entryInAction;
            mInActionButton->set_sensitive(true);
            mOkButton->set_sensitive(true);
        } else {
            mInActionButton->set_sensitive(false);
            mOkButton->set_sensitive(false);
        }
    }
}

void StateDialog::onInActionSource()
{
    SourceDialog dialog(mXml);
    const std::string& buffer = mStatechart->action(mTrimInAction);

    if (buffer.empty()) {
        dialog.add(mTrimInAction,
                   (fmt(Statechart::ACTION_DEFINITION) % mTrimInAction).str());
    } else {
        dialog.add(mTrimInAction, buffer);
    }
    if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
        mStatechart->action(mTrimInAction, dialog.buffer(mTrimInAction));
    }
}

void StateDialog::onOutAction()
{
    std::string entryOutAction = outAction();
    boost::trim(entryOutAction);
    if (entryOutAction.empty()) {
        mOutActionButton->set_sensitive(false);
        if (mOutActionEntry->get_entry()->get_text().empty()) {
            mOkButton->set_sensitive(true);
        } else {
            mOkButton->set_sensitive(false);
        }
    } else {
        if (isValidName(entryOutAction)) {
            mTrimOutAction = entryOutAction;
            mOutActionButton->set_sensitive(true);
            mOkButton->set_sensitive(true);
        } else {
            mOutActionButton->set_sensitive(false);
            mOkButton->set_sensitive(false);
        }
    }
}

void StateDialog::onOutActionSource()
{
    SourceDialog dialog(mXml);
    const std::string& buffer = mStatechart->action(mTrimOutAction);

    if (buffer.empty()) {
        dialog.add(mTrimOutAction,
            (fmt(Statechart::ACTION_DEFINITION) % mTrimOutAction).str());
    } else {
        dialog.add(mTrimOutAction, buffer);
    }
    if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
        mStatechart->action(mTrimOutAction, dialog.buffer(mTrimOutAction));
    }
}

int StateDialog::run()
{
    mNameEntry->set_text(mState->name());
    mInitialCheckbox->set_active(mState->initial());

    // inAction & outAction
    mInActionEntry->clear();
    mOutActionEntry->clear();
    for (buffers_t::const_iterator it = mStatechart->actions().begin();
         it != mStatechart->actions().end(); ++it) {
        mInActionEntry->append_text(it->first);
        mOutActionEntry->append_text(it->first);
    }
    if (not mState->inAction().empty()) {
        const std::string action = mState->inAction();
        const std::string& buffer = mStatechart->action(action);
        if (buffer.empty()) {
            mInActionEntry->get_entry()->set_text(action);
        } else {
            mInActionEntry->set_active_text(action);
        }
    }
    mInActionButton->set_sensitive(not mState->inAction().empty());

    if (not mState->outAction().empty()) {
        const std::string action = mState->outAction();
        const std::string& buffer = mStatechart->action(action);
        if (buffer.empty()) {
            mOutActionEntry->get_entry()->set_text(action);
        } else {
            mOutActionEntry->set_active_text(action);
        }
    }
    mOutActionButton->set_sensitive(not mState->outAction().empty());

    // activity
    mActivityEntry->clear();
    for (buffers_t::const_iterator it = mStatechart->activities().begin();
         it != mStatechart->activities().end(); ++it) {
        mActivityEntry->append_text(it->first);
    }
    if (not mState->activity().empty()) {
        const std::string action = mState->activity();
        const std::string& buffer = mStatechart->action(action);
        if (buffer.empty()) {
            mActivityEntry->get_entry()->set_text(action);
        } else {
            mActivityEntry->set_active_text(action);
          }
    }
    mActivityButton->set_sensitive(not mState->activity().empty());

    // events in state
    mEventInStateTreeView->clear();
    for (buffers_t::const_iterator it = mState->eventInStates().begin();
         it != mState->eventInStates().end(); ++it) {
        mEventInStateTreeView->add(it->first, it->second);
    }
    mDialog->set_default_response(Gtk::RESPONSE_ACCEPT);

    int response = mDialog->run();

    mDialog->hide();
    return response;
}

}
}
}
}    // namespace vle gvle modeling fsa
