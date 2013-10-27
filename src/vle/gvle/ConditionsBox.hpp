/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef GUI_CONDITIONSBOX_HPP
#define GUI_CONDITIONSBOX_HPP

#include <gtkmm/builder.h>
#include <gtkmm/dialog.h>
#include <gtkmm/menu.h>
#include <gtkmm/uimanager.h>
#include <vle/gvle/ValuesTreeView.hpp>
#include <vle/vpz/Conditions.hpp>

namespace vle { namespace gvle {

typedef std::vector < std::pair < std::string,
                                  std::string > > renameList;

class GVLE;

class ConditionsBox
{
    class ModelColumnsConditions : public Gtk::TreeModel::ColumnRecord
    {
    public:

        ModelColumnsConditions() {
            add(m_col_name);
        }

        Gtk::TreeModelColumn<Glib::ustring> m_col_name;
    };

    class ConditionsTreeView : public Gtk::TreeView
    {
    public:
        ConditionsTreeView(BaseObjectType* cobject,
                           const Glib::RefPtr < Gtk::Builder >& /*refGlade*/);
        virtual ~ConditionsTreeView();

        void build();

        Glib::ustring getSelected()
        { return (*mRefTreeSelection->get_selected())[mColumns.m_col_name]; }

        void setConditions(vpz::Conditions* conditions)
        { mConditions = conditions; }

        void setParent(ConditionsBox* parent)
        { mParent = parent; }

	void makeMenus();

    protected:
	// Override Signal handler:
	// Alternatively, use signal_button_press_event().connect_notify()
	virtual bool on_button_press_event(GdkEventButton *ev);

	void on_select();
	//Signal handler for popup menu items:
	virtual void on_add();
	virtual void on_remove();
	virtual void onRename();
	virtual void onCopy();
	void onEdit(std::string pluginName);

	// Signal handler for text area
	virtual void onEditionStarted(
	    Gtk::CellEditable* cellEditatble,
	    const Glib::ustring& path);
	virtual void onEdition(
	    const Glib::ustring& pathString,
	    const Glib::ustring& newName);

        bool onQueryTooltip(int wx,int wy, bool keyboard_tooltip,
                            const Glib::RefPtr<Gtk::Tooltip>& tooltip);
	
	

    private:

	ConditionsBox* mParent;
	vpz::Conditions* mConditions;
//	Gtk::Menu* mMenuEdit;   Devenu inutile ! (Mikaël)
	Gtk::Menu* mMenuPopup;
	ModelColumnsConditions mColumns;
	Glib::RefPtr<Gtk::ListStore> mRefTreeModel;
	Glib::RefPtr<Gtk::TreeSelection> mRefTreeSelection;

	int mColumn;
	bool mValidateRetry;
	Glib::ustring mOldName;
	Gtk::CellRendererText* mCellRenderer;
	Glib::ustring mInvalidTextForRetry;
	
	Glib::RefPtr <Gtk::UIManager> mUIManager;
        Glib::RefPtr <Gtk::ActionGroup> mPopupActionGroup;
        Glib::RefPtr <Gtk::ActionGroup> mEditActionGroup;
    };

    class ModelColumnsPorts : public Gtk::TreeModel::ColumnRecord
    {
    public:

        ModelColumnsPorts() {
            add(m_col_name);
        }

        Gtk::TreeModelColumn<Glib::ustring> m_col_name;
    };

    class PortsTreeView : public Gtk::TreeView
    {
    public:
	PortsTreeView(BaseObjectType* cobject,
		      const Glib::RefPtr < Gtk::Builder >& /*refGlade*/);
	virtual ~PortsTreeView();

	void build();
	void clear();
	Glib::ustring getSelected()
	    { return (*mRefTreeSelection
		      ->get_selected())[mColumns.m_col_name]; }
	void setCondition(vpz::Condition* condition)
	    { mCondition = condition; }
	void setParent(ConditionsBox* parent)
	    { mParent = parent; }

	void makeMenus();

    protected:
	// Override Signal handler:
	// Alternatively, use signal_button_press_event().connect_notify()
	virtual bool on_button_press_event(GdkEventButton *ev);
	void on_select();
	//Signal handler for popup menu items:
	virtual void on_add();
	virtual void on_remove();
	virtual void onRename();
	virtual void onDuplicate();
	void onEdit(std::string pluginName);

	// Signal handler for text area
	virtual void onEditionStarted(
	    Gtk::CellEditable* cellEditatble,
	    const Glib::ustring& path);
	virtual void onEdition(
	    const Glib::ustring& pathString,
	    const Glib::ustring& newName);

    private:
	ConditionsBox* mParent;
	vpz::Condition* mCondition;
	Gtk::Menu* mMenuEdit;
	Gtk::Menu* mMenuPopup;
	ModelColumnsPorts mColumns;
	Glib::RefPtr<Gtk::ListStore> mRefTreeModel;
	Glib::RefPtr<Gtk::TreeSelection> mRefTreeSelection;

	int mColumn;
	bool mValidateRetry;
	Glib::ustring mOldName;
	Gtk::CellRendererText* mCellRenderer;
	Glib::ustring mInvalidTextForRetry;
	
	Glib::RefPtr <Gtk::UIManager> mUIManager;
        Glib::RefPtr <Gtk::ActionGroup> mPopupActionGroup;
    };

public:
    /**
     * Create a new ConditionsBox.Parameters modeling and current is to
     * intialise class with for condition information
     *
     * @throw Internal if modeling is bad
     * @param modeling to get information
     * @param current, current model to show. If current equal NULL then,
     * the modeling top GCoupledModel is used.
     */
    ConditionsBox(const Glib::RefPtr < Gtk::Builder >& xml, GVLE* gvle);

    ~ConditionsBox();

    int run(const vpz::Conditions& conditions);
    renameList apply(vpz::Conditions& conditions);

    inline GVLE* getGVLE()
    { return m_gvle; }

    inline void setRenameList(std::pair < std::string,
                                          std::string >  newPair)
    { mRenameList.push_back(newPair); }

private:
    renameList          mRenameList;

    GVLE*               m_gvle;
    vpz::Conditions*    mConditions;

    Gtk::Dialog*        mDialog;
    Gtk::Button*        mButtonCancel;
    Gtk::Button*        mButtonApply;

    Glib::RefPtr < Gtk::Builder >  m_xml;

    ConditionsTreeView*                 m_treeConditions;
    PortsTreeView*                      m_treePorts;
    ValuesTreeView*                     m_treeValues;

    void buildTreeConditions();
    void buildTreePorts(const std::string& conditionName);
    void buildTreeValues(const std::string& conditionName,
			 const std::string& port_name);

    void on_cancel();
};

} } // namespace vle gvle

#endif
