/**
 * @file vle/gvle/AtomicModelBox.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
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


#ifndef GUI_ATOMICMODELBOX_HPP
#define GUI_ATOMICMODELBOX_HPP

#include <gtkmm.h>
#include <vle/graph/AtomicModel.hpp>
#include <vle/gvle/DynamicBox.hpp>
#include <vle/gvle/Editor.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/gvle/ObsAndViewBox.hpp>
#include <vle/gvle/TreeViewConditions.hpp>

namespace vle { namespace gvle {

class Modeling;

class AtomicModelBox
{
public:
    AtomicModelBox(Glib::RefPtr<Gnome::Glade::Xml> xml, Modeling* m);

    virtual ~AtomicModelBox();

    void show(graph::AtomicModel* model,
	      const std::string& className = "");

    inline Modeling* getModeling()
    { return mModeling; }

protected:
    class ModelColumnsPort : public Gtk::TreeModel::ColumnRecord
    {
    public:

        ModelColumnsPort() {
            add(m_col_name);
        }

        Gtk::TreeModelColumn<Glib::ustring> m_col_name;
    };

    class ModelColumnsDyn : public Gtk::TreeModel::ColumnRecord
    {
    public:
        ModelColumnsDyn()
        {
            add(m_col_name);
            add(m_package);
	    add(m_dyn);
	    add(m_model);
        }

        Gtk::TreeModelColumn<Glib::ustring> m_col_name;
        Gtk::TreeModelColumn<Glib::ustring> m_package;
        Gtk::TreeModelColumn<Glib::ustring> m_dyn;
	Gtk::TreeModelColumn<Glib::ustring> m_model;
    };

    class ModelColumnsCond : public Gtk::TreeModel::ColumnRecord
    {
    public:

        ModelColumnsCond() {
            add(m_col_activ);
            add(m_col_name);
        }

        Gtk::TreeModelColumn<Glib::ustring> m_col_name;
        Gtk::TreeModelColumn<bool> m_col_activ;
    };

    class ModelColumnsObs : public Gtk::TreeModel::ColumnRecord
    {
    public:

        ModelColumnsObs() {
            add(m_col_name);
        }

        Gtk::TreeModelColumn<Glib::ustring> m_col_name;
    };

private:

    class InputPortTreeView : public Gtk::TreeView
    {
    public:
	InputPortTreeView(BaseObjectType* cobject,
			  const Glib::RefPtr<Gnome::Glade::Xml>& /*refGlade*/);
	virtual ~InputPortTreeView();

	void applyRenaming(graph::AtomicModel* model);
	void build();
	void clearRenaming()
	    { mRenameList.clear(); }
	void setModel(graph::AtomicModel* model)
	    { mModel = model; }

    protected:
	// Override Signal handler:
	// Alternatively, use signal_button_press_event().connect_notify()
	virtual bool on_button_press_event(GdkEventButton *ev);

	//Signal handler for popup menu items:
	virtual void onAdd();
	virtual void onRemove();
	virtual void onRename();

	// Signal handler for text area
	virtual void onEditionStarted(
	    Gtk::CellEditable* cellEditatble,
	    const Glib::ustring& path);
	virtual void onEdition(
	    const Glib::ustring& pathString,
	    const Glib::ustring& newText);

    private:
	typedef std::vector < std::pair < std::string,
					  std::string > > renameList;

	graph::AtomicModel* mModel;
	renameList mRenameList;
	Gtk::Menu mMenuPopup;
	ModelColumnsPort mColumnsInputPort;
	Glib::RefPtr<Gtk::ListStore> mRefTreeModelInputPort;

	int mColumnName;
	bool mValidateRetry;
	std::string mOldName;
	Gtk::CellRendererText* mCellrendererValidated;
	Glib::ustring mInvalidTextForRetry;
    };

    class OutputPortTreeView : public Gtk::TreeView
    {
    public:
	OutputPortTreeView(BaseObjectType* cobject,
			   const Glib::RefPtr<Gnome::Glade::Xml>& /*refGlade*/);
	virtual ~OutputPortTreeView();

	void applyRenaming(graph::AtomicModel* model);
	void build();
	void clearRenaming()
	    { mRenameList.clear(); }
	void setModel(graph::AtomicModel* model)
	    { mModel = model; }

    protected:
	// Override Signal handler:
	// Alternatively, use signal_button_press_event().connect_notify()
	virtual bool on_button_press_event(GdkEventButton *ev);

	//Signal handler for popup menu items:
	virtual void onAdd();
	virtual void onRemove();
	virtual void onRename();

	// Signal handler for text area
	virtual void onEditionStarted(
	    Gtk::CellEditable* cellEditatble,
	    const Glib::ustring& path);
	virtual void onEdition(
	    const Glib::ustring& pathString,
	    const Glib::ustring& newText);

    private:
	typedef std::vector < std::pair < std::string,
					  std::string > > renameList;

	graph::AtomicModel* mModel;
	renameList mRenameList;
	Gtk::Menu mMenuPopup;
	ModelColumnsPort mColumnsOutputPort;
	Glib::RefPtr<Gtk::ListStore> mRefTreeModelOutputPort;

	int mColumnName;
	bool mValidateRetry;
	std::string mOldName;
	Gtk::CellRendererText* mCellrendererValidated;
	Glib::ustring mInvalidTextForRetry;
    };

    class ConditionTreeView : public Gtk::TreeView
    {
    public:
	ConditionTreeView(BaseObjectType* cobject,
			  const Glib::RefPtr<Gnome::Glade::Xml>& /*refGlade*/);
	virtual ~ConditionTreeView();

	void applyRenaming();
	void build();
	void clearRenaming()
	    { mRenameList.clear(); }
	std::vector < std::string > getConditions();
	void setConditions(vpz::Conditions* conditions)
	    { mConditions = conditions; }
	void setModeling(Modeling* modeling)
	    { mModeling = modeling; }
	void setModel(vpz::AtomicModel* model)
	    { mModel = model; }
	void setLabel(Gtk::Label* label)
	    { mLabel = label; }

    protected:
	virtual void onRename();

	// Signal handler for text area
	virtual void onEditionStarted(
	    Gtk::CellEditable* cellEditatble,
	    const Glib::ustring& path);
	virtual void onEdition(
	    const Glib::ustring& pathString,
	    const Glib::ustring& newText);

    private:
	typedef std::vector < std::pair < std::string,
					  std::string > > renameList;

	void on_row_activated(const Gtk::TreeModel::Path& path,
			      Gtk::TreeViewColumn*  column);
	virtual bool on_button_press_event(GdkEventButton* event);

	// class members
	vpz::AtomicModel* mModel;
	vpz::Conditions* mConditions;
	renameList mRenameList;
	Modeling* mModeling;
	Gtk::Menu mMenuPopup;
	ModelColumnsCond mColumns;
	Glib::RefPtr < Gtk::ListStore > mRefTreeModel;
	//Label
	Gtk::Label* mLabel;

	//Cell
	int mColumnName;
	bool mValidateRetry;
	std::string mOldName;
	Gtk::CellRendererText* mCellrendererValidated;
	Glib::ustring mInvalidTextForRetry;
    };

    class DynamicTreeView : public Gtk::TreeView
    {
    public:
	DynamicTreeView(BaseObjectType* cobject,
			const Glib::RefPtr<Gnome::Glade::Xml>& /*refGlade*/);
	virtual ~DynamicTreeView();
	void applyRenaming();
	void build();
        void clearRenaming() { mRenameList.clear(); }
        void setDynamics(vpz::Dynamics* dynamics) { mDynamics = dynamics; }
        void setConditions(vpz::Conditions* conditions)
        { mConditions = conditions; }
        void setObservables(vpz::Observables* observables)
        { mObservables = observables; }
        void setModeling(Modeling* modeling) { mModeling = modeling; }
        void setModel(vpz::AtomicModel* model) { mModel = model; }
        void setGraphModel(graph::AtomicModel* atom) { mAtom = atom; }
        void setLabel(Gtk::Label* label) { mLabel = label; }
        void setParent(AtomicModelBox* parent) { mParent = parent; }

	std::string getDynamic();

    protected:
	// Override Signal handler:
	// Alternatively, use signal_button_press_event().connect_notify()
	virtual bool on_button_press_event(GdkEventButton *event);
	void onRowActivated(const Gtk::TreeModel::Path& path,
			    Gtk::TreeViewColumn* column);

	std::string pathFileSearch(const std::string& path,
				   const std::string& filename);

	//Signal handler for popup menu items:
	virtual void onAdd();
	virtual void onEdit();
	virtual void onRemove();
	virtual void onRename();

	// Signal handler for text area
	virtual void onEditionStarted(
	    Gtk::CellEditable* cellEditatble,
	    const Glib::ustring& path);
	virtual void onEdition(
	    const Glib::ustring& pathString,
	    const Glib::ustring& newName);

	void onClickColumn(int numColum);

    private:
	typedef std::vector < std::pair < std::string,
					  std::string > > renameList;
        Glib::RefPtr<Gnome::Glade::Xml> mXml;
	vpz::AtomicModel* mModel;
	graph::AtomicModel* mAtom;
	AtomicModelBox* mParent;
	vpz::Dynamics* mDynamics;
	vpz::Conditions* mConditions;
	vpz::Observables* mObservables;
	renameList mRenameList;
	Modeling* mModeling;
	Gtk::Menu mMenuPopup;
	ModelColumnsDyn mColumnsDyn;
	Glib::RefPtr<Gtk::ListStore> mRefListDyn;
	Glib::RefPtr<Gtk::TreeModelSort> mRefTreeModelDyn;

	//Label
	Gtk::Label* mLabel;

	 //Cell
	Gtk::CellRendererText*   mCellRenderer;
	std::string              mOldName;
	bool                     mValidateRetry;
	Glib::ustring            mInvalidTextForRetry;
	guint32                  mDelayTime;
	int                      mColumnName;
	int                      mColumnPackage;
	int                      mColumnDyn;
	int                      mColumnModel;
    };

    class ObservableTreeView : public Gtk::TreeView
    {
    public:
	ObservableTreeView(BaseObjectType* cobject,
			  const Glib::RefPtr<Gnome::Glade::Xml>& /*refGlade*/);
	virtual ~ObservableTreeView();
	void applyRenaming();
	void build();
	void clearRenaming()
	    { mRenameList.clear(); }
	void setObservables(vpz::Observables* observables)
	    { mObservables = observables; }
	void setModeling(Modeling* modeling)
	    { mModeling = modeling; }
	void setModel(vpz::AtomicModel* model)
	    { mModel = model; }
	void setLabel(Gtk::Label* label)
	    { mLabel = label; }
	std::string getObservable();

    protected:
	// Override Signal handler:
	// Alternatively, use signal_button_press_event().connect_notify()
	virtual bool on_button_press_event(GdkEventButton *event);

	//Signal handler for popup menu items:
	virtual void onAdd();
	virtual void onEdit();
	virtual void onRemove();
	virtual void onRename();

	// Signal handler for text area
	virtual void onEditionStarted(
	    Gtk::CellEditable* cellEditable,
	    const Glib::ustring& path);
	virtual void onEdition(
	    const Glib::ustring& pathString,
	    const Glib::ustring& newName);

    private:
	typedef std::vector < std::pair < std::string,
					  std::string > > renameList;

	vpz::AtomicModel* mModel;
	vpz::Observables* mObservables;
	renameList mRenameList;
	Modeling* mModeling;
	Gtk::Menu mMenuPopup;
	ModelColumnsObs mColumnsObs;
	Glib::RefPtr<Gtk::ListStore> mRefTreeModelObs;
	//Dialog Box
	ObsAndViewBox mObsAndViewBox;
	//Label
	Gtk::Label* mLabel;
	 //Cell
	int                      mColumnName;
	Gtk::CellRendererText*   mCellRenderer;
	std::string              mOldName;
	bool                     mValidateRetry;
	Glib::ustring            mInvalidTextForRetry;
	guint32                  mDelayTime;
    };

    Glib::RefPtr<Gnome::Glade::Xml>      mXml;
    Modeling*                            mModeling;

    Gtk::Dialog*                         mDialog;

    vpz::AtomicModel*                    mCurrentModel;
    graph::AtomicModel*                  mCurrentGraphModel;
    vpz::AtomicModel*                    mModel;
    graph::AtomicModel*                  mGraphModel;
    vpz::Conditions*                     mCond;
    vpz::Dynamics*                       mDyn;
    vpz::Observables*                    mObs;

    //Input Ports
    InputPortTreeView*                   mInputPorts;

    //Output Ports
    OutputPortTreeView*                  mOutputPorts;

    //Dynamics
    DynamicTreeView*                     mDynamics;
    Gtk::Label*                          mLabelDynamics;

    //Observables
    ObservableTreeView*                  mObservables;
    Gtk::Label*                          mLabelObservables;

    //Conditions
    ConditionTreeView*                   mConditions;
    Gtk::Label*                          mLabelConditions;

    //Buttons
    Gtk::Button*                         mButtonApply;
    Gtk::Button*                         mButtonCancel;

    Gtk::Menu m_Menu_Popup;

    void applyPorts();
    void applyConditions();
    void applyDynamics();
    void applyObservables();
    void on_apply();
    void on_cancel();
    void refresh();
};

} } // namespace vle gvle

#endif
