/**
 * @file vle/gvle/modeling/fsa/StateDialog.hpp
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


#ifndef VLE_GVLE_MODELING_FSA_STATE_DIALOG_HPP
#define VLE_GVLE_MODELING_FSA_STATE_DIALOG_HPP

#include <vle/gvle/modeling/fsa/Statechart.hpp>
#include <vle/utils/i18n.hpp>
#include <gtkmm/comboboxentrytext.h>
#include <gtkmm/dialog.h>
#include <gtkmm/liststore.h>
#include <gtkmm/menu.h>
#include <gtkmm/treeview.h>
#include <libglademm.h>

namespace vle { namespace gvle { namespace modeling {

class NewStateDialog
{
public:
    NewStateDialog(const Glib::RefPtr < Gnome::Glade::Xml >& xml);

    virtual ~NewStateDialog()
    { }

    bool initial() const
    { return mInitialCheckbox->get_active(); }

    std::string name() const
    { return mNameEntry->get_text(); }

    int run();

private:
    Gtk::Dialog*      mDialog;
    Gtk::Entry*       mNameEntry;
    Gtk::CheckButton* mInitialCheckbox;
};

class EventInStateDialog
{
public:
    EventInStateDialog(const Glib::RefPtr < Gnome::Glade::Xml >& xml,
                       Statechart* statechart,
                       const eventInStates_t& eventInStates);

    virtual ~EventInStateDialog()
    {
        mEventInStateVBox->remove(*mActionEntry);

        for (std::list < sigc::connection >::iterator it = mList.begin();
             it != mList.end(); ++it) {
            it->disconnect();
        }
    }

    std::string action() const
    { return mActionEntry->get_entry()->get_text(); }

    std::string event() const
    { return mEventEntry->get_text(); }

    int run(const std::string& event, const std::string& action);

private:
    void onAction();
    void onEventActionSource();

    Glib::RefPtr < Gnome::Glade::Xml > mXml;
    Statechart*                        mStatechart;
    const eventInStates_t&             mEventInStates;
    Gtk::Dialog*                       mDialog;
    Gtk::Entry*                        mEventEntry;
    Gtk::VBox*                         mEventInStateVBox;
    Gtk::ComboBoxEntryText*            mActionEntry;
    Gtk::Button*                       mActionButton;
    std::list < sigc::connection >     mList;
};

class StateDialog
{
    class EventInStateColumns : public Gtk::TreeModel::ColumnRecord
    {
    public:
        EventInStateColumns()
        { add(mEvent); add(mAction); }

        Gtk::TreeModelColumn < Glib::ustring > mEvent;
        Gtk::TreeModelColumn < Glib::ustring > mAction;
    };

    class EventInStateTreeView : public Gtk::TreeView
    {
    public:
        EventInStateTreeView(BaseObjectType* cobject,
                             const Glib::RefPtr < Gnome::Glade::Xml >& xml) :
            Gtk::TreeView(cobject), mXml(xml), mStatechart(0), mEventInStates(0)
        {
            mTreeModel = Gtk::ListStore::create(mColumns);
            set_model(mTreeModel);
            append_column(_("Event"), mColumns.mEvent);
            append_column(_("Action"), mColumns.mAction);

            Gtk::Menu::MenuList& items(mMenu.items());

            items.push_back(Gtk::Menu_Helpers::MenuElem(
                                _("Add"), sigc::mem_fun(
                                    *this, &EventInStateTreeView::onAdd)));
            items.push_back(Gtk::Menu_Helpers::MenuElem(
                                _("Edit"), sigc::mem_fun(
                                    *this, &EventInStateTreeView::onEdit)));
            items.push_back(Gtk::Menu_Helpers::MenuElem(
                                _("Remove"), sigc::mem_fun(
                                    *this, &EventInStateTreeView::onRemove)));
            mMenu.show_all();
        }

        virtual ~EventInStateTreeView()
        { }

        void add(const std::string& event, const std::string& action)
        {
            Gtk::TreeModel::Row row = *mTreeModel->append();
            row[mColumns.mEvent] = event;
            row[mColumns.mAction] = action;
        }

        void clear()
        { mTreeModel->clear(); }

        void setStatechart(Statechart* statechart,
                           eventInStates_t* eventInStates)
        { mStatechart = statechart; mEventInStates = eventInStates; }

        bool on_button_press_event(GdkEventButton* event)
        {
            if (event->type == GDK_BUTTON_PRESS and event->button == 3) {
                mMenu.popup(event->button, event->time);
            }
            return Gtk::TreeView::on_button_press_event(event);
        }

    private:
        void onAdd();
        void onEdit();
        void onRemove();

        Glib::RefPtr < Gnome::Glade::Xml > mXml;
        Statechart*                        mStatechart;
        eventInStates_t*                   mEventInStates;
        EventInStateColumns                mColumns;
        Glib::RefPtr < Gtk::ListStore >    mTreeModel;
        Gtk::Menu                          mMenu;
    };

public:
    StateDialog(const Glib::RefPtr < Gnome::Glade::Xml >& xml,
                Statechart* statechart, const State* state);

    virtual ~StateDialog()
    {
        mStateVBox->remove(*mInActionEntry);
        mStateVBox->remove(*mOutActionEntry);
        mStateVBox->remove(*mActivityEntry);

        for (std::list < sigc::connection >::iterator it = mList.begin();
             it != mList.end(); ++it) {
            it->disconnect();
        }
    }

    std::string activity() const
    { return mActivityEntry->get_entry()->get_text(); }

    eventInStates_t eventInStates() const
    { return mEventInStates; }

    std::string inAction() const
    { return mInActionEntry->get_entry()->get_text(); }

    bool initial() const
    { return mInitialCheckbox->get_active(); }

    std::string name() const
    { return mNameEntry->get_text(); }

    std::string outAction() const
    { return mOutActionEntry->get_entry()->get_text(); }

    int run();

private:
    void onActivity();
    void onActivitySource();
    void onInAction();
    void onInActionSource();
    void onOutAction();
    void onOutActionSource();

    Glib::RefPtr < Gnome::Glade::Xml > mXml;
    Statechart*                        mStatechart;
    const State*                       mState;

    Gtk::Dialog*            mDialog;
    Gtk::Entry*             mNameEntry;
    Gtk::CheckButton*       mInitialCheckbox;
    Gtk::VBox*              mStateVBox;
    Gtk::ComboBoxEntryText* mInActionEntry;
    Gtk::Button*            mInActionButton;
    Gtk::ComboBoxEntryText* mOutActionEntry;
    Gtk::Button*            mOutActionButton;
    Gtk::ComboBoxEntryText* mActivityEntry;
    Gtk::Button*            mActivityButton;
    EventInStateTreeView*   mEventInStateTreeView;
    eventInStates_t         mEventInStates;

    std::list < sigc::connection > mList;
};

}}} // namespace vle gvle modeling

#endif
