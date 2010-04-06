/**
 * @file vle/gvle/modeling/fsa/StateDialog.cpp
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

#include <vle/gvle/modeling/fsa/StateDialog.hpp>
#include <vle/gvle/modeling/fsa/SourceDialog.hpp>
#include <vle/utils/i18n.hpp>

namespace vle { namespace gvle { namespace modeling { namespace fsa {

NewStateDialog::NewStateDialog(const Glib::RefPtr < Gnome::Glade::Xml >& xml)
{
    xml->get_widget("NewStateDialog", mDialog);
    xml->get_widget("NewStateNameEntry", mNameEntry);
    xml->get_widget("NewStateInitialCheckbox", mInitialCheckbox);
}

int NewStateDialog::run()
{
    mNameEntry->set_text("");
    mInitialCheckbox->set_active(false);
    mDialog->set_default_response(Gtk::RESPONSE_ACCEPT);

    int response = mDialog->run();

    mDialog->hide();
    return response;
}

EventInStateDialog::EventInStateDialog(
    const Glib::RefPtr < Gnome::Glade::Xml >& xml,
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
    mEventEntry->set_text(event);

    mActionEntry->clear();
    for (buffers_t::const_iterator it = mStatechart->eventActions().begin();
         it != mStatechart->eventActions().end(); ++it) {
        mActionEntry->append_text(it->first);
    }
    if (not action.empty()) {
        mActionEntry->set_active_text(action);
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

StateDialog::StateDialog(const Glib::RefPtr < Gnome::Glade::Xml >& xml,
                         Statechart* statechart, const State* state)  :
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

    xml->get_widget_derived("EventInStateTreeView", mEventInStateTreeView);
    mEventInStateTreeView->setStatechart(mStatechart, &mEventInStates);

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

void StateDialog::onActivity()
{
    mActivityButton->set_sensitive(
        not mActivityEntry->get_entry()->get_text().empty());
}

void StateDialog::onActivitySource()
{
    SourceDialog dialog(mXml);
    const std::string& name = mActivityEntry->get_entry()->get_text();
    const std::string& buffer = mStatechart->activity(name);

    if (buffer.empty()) {
        dialog.add(name,
                   (fmt(Statechart::ACTIVITY_DEFINITION) % name).str());
    } else {
        dialog.add(name, buffer);
    }
    if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
        mStatechart->activity(name, dialog.buffer(name));
    }
}

void StateDialog::onInAction()
{
    mInActionButton->set_sensitive(
        not mInActionEntry->get_entry()->get_text().empty());
}

void StateDialog::onInActionSource()
{
    SourceDialog dialog(mXml);
    const std::string& name = mInActionEntry->get_entry()->get_text();
    const std::string& buffer = mStatechart->eventAction(name);

    if (buffer.empty()) {
        dialog.add(name,
                   (fmt(Statechart::ACTION_DEFINITION) % name).str());
    } else {
        dialog.add(name, buffer);
    }
    if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
        mStatechart->action(name, dialog.buffer(name));
    }
}

void StateDialog::onOutAction()
{
    mOutActionButton->set_sensitive(
        not mOutActionEntry->get_entry()->get_text().empty());
}

void StateDialog::onOutActionSource()
{
    SourceDialog dialog(mXml);
    const std::string& name = mOutActionEntry->get_entry()->get_text();
    const std::string& buffer = mStatechart->eventAction(name);

    if (buffer.empty()) {
        dialog.add(name,
                   (fmt(Statechart::ACTION_DEFINITION) % name).str());
    } else {
        dialog.add(name, buffer);
    }
    if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
        mStatechart->action(name, dialog.buffer(name));
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
        mInActionEntry->set_active_text(mState->inAction());
    }
    mInActionButton->set_sensitive(not mState->inAction().empty());
    if (not mState->outAction().empty()) {
        mOutActionEntry->set_active_text(mState->outAction());
    }
    mOutActionButton->set_sensitive(not mState->outAction().empty());

    // activity
    mActivityEntry->clear();
    for (buffers_t::const_iterator it = mStatechart->activities().begin();
         it != mStatechart->activities().end(); ++it) {
        mActivityEntry->append_text(it->first);
    }
    if (not mState->activity().empty()) {
        mActivityEntry->set_active_text(mState->activity());
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

}}}} // namespace vle gvle modeling fsa
