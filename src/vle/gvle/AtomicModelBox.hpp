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


#ifndef GUI_ATOMICMODELBOX_HPP
#define GUI_ATOMICMODELBOX_HPP

#include <gtkmm/builder.h>
#include <gtkmm/liststore.h>
#include <gtkmm/menu.h>
#include <gtkmm/uimanager.h>
#include <vle/gvle/DynamicBox.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/gvle/ObsAndViewBox.hpp>

namespace vle { namespace gvle {

class Modeling;
class GVLE;

class AtomicModelBox
{
public:
    AtomicModelBox(const Glib::RefPtr < Gtk::Builder >& xml,
                   Modeling* m, GVLE* gvle);

    virtual ~AtomicModelBox();

    void show(vpz::AtomicModel* model);

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
            add(m_sel);
        }

        Gtk::TreeModelColumn<Glib::ustring> m_col_name;
        Gtk::TreeModelColumn<Glib::ustring> m_package;
        Gtk::TreeModelColumn<Glib::ustring> m_dyn;
        Gtk::TreeModelColumn<bool> m_sel;
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
            add(m_col_sel);
        }

        Gtk::TreeModelColumn<Glib::ustring> m_col_name;
        Gtk::TreeModelColumn<bool> m_col_sel;
    };

private:
    class InputPortTreeView : public Gtk::TreeView
    {
    public:
	InputPortTreeView(BaseObjectType* cobject,
			  const Glib::RefPtr < Gtk::Builder >& /*refGlade*/);
	virtual ~InputPortTreeView();

	void applyRenaming(vpz::AtomicModel* model);
	void build();
	void clearRenaming()
	    { mRenameList.clear(); }
	void setModel(vpz::AtomicModel* model)
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

	vpz::AtomicModel* mModel;
	renameList mRenameList;

        Glib::RefPtr <Gtk::UIManager> mPopupUIManager;
        Glib::RefPtr <Gtk::ActionGroup> mPopupActionGroup;
	Gtk::Menu *mMenuPopup;

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
			   const Glib::RefPtr < Gtk::Builder >& /*refGlade*/);
	virtual ~OutputPortTreeView();

	void applyRenaming(vpz::AtomicModel* model);
	void build();
	void clearRenaming()
	    { mRenameList.clear(); }
	void setModel(vpz::AtomicModel* model)
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

	vpz::AtomicModel* mModel;
	renameList mRenameList;
        Glib::RefPtr <Gtk::UIManager> mPopupUIManager;
        Glib::RefPtr <Gtk::ActionGroup> mPopupActionGroup;
	Gtk::Menu *mMenuPopup;
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
			  const Glib::RefPtr < Gtk::Builder >& /*refGlade*/);
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
        void setGVLE(GVLE* gvle)
        { mGVLE = gvle; }
        void setModel(vpz::AtomicModel* atom)
        { mAtom = atom; }
	void setLabel(Gtk::Label* label)
        { mLabel = label; }

        bool onQueryTooltip(int wx,int wy, bool keyboard_tooltip,
                            const Glib::RefPtr<Gtk::Tooltip>& tooltip);

    protected:
	virtual void onConditionsEditor();
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
	vpz::AtomicModel* mAtom;
	vpz::Conditions* mConditions;
	renameList mRenameList;
	Modeling* mModeling;
        GVLE* mGVLE;
        Glib::RefPtr <Gtk::UIManager> mPopupUIManager;
        Glib::RefPtr <Gtk::ActionGroup> mPopupActionGroup;
	Gtk::Menu *mMenuPopup;
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
			const Glib::RefPtr < Gtk::Builder >& /*refGlade*/);
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
        void setGVLE(GVLE* gvle) { mGVLE = gvle; }
        void setModel(vpz::AtomicModel* atom) { mAtom = atom; }
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
	virtual void onNewLibrary();
	virtual void onRemove();
	virtual void onRename();
        int execPlugin(vpz::Dynamic& dynamic,
                       const std::string& pluginname,
                       const std::string& classname,
                       const std::string& namespace_);


	// Signal handler for text area
	virtual void onEditionStarted(
	    Gtk::CellEditable* cellEditatble,
	    const Glib::ustring& path);
	virtual void onEdition(
	    const Glib::ustring& path,
	    const Glib::ustring& newName);

        void onToggled(const Glib::ustring&  path);

        Gtk::TreeModel::iterator getSelectedRadio();

    private:
	typedef std::vector < std::pair < std::string,
					  std::string > > renameList;
        Glib::RefPtr < Gtk::Builder >   mXml;
	vpz::AtomicModel*               mAtom;
	AtomicModelBox*                 mParent;
	vpz::Dynamics*                  mDynamics;
	vpz::Conditions*                mConditions;
	vpz::Observables*               mObservables;
	renameList                      mRenameList;
	Modeling*                       mModeling;
        GVLE*                           mGVLE;
        Glib::RefPtr <Gtk::UIManager> mPopupUIManager;
        Glib::RefPtr <Gtk::ActionGroup> mPopupActionGroup;
	Gtk::Menu *mMenuPopup;
	ModelColumnsDyn                 mColumnsDyn;
	Glib::RefPtr<Gtk::ListStore>    mRefListDyn;

	//Label
	Gtk::Label*              mLabel;

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
        int                      mColumnSel;
    };

    class ObservableTreeView : public Gtk::TreeView
    {
    public:
	ObservableTreeView(BaseObjectType* cobject,
			  const Glib::RefPtr < Gtk::Builder >& /*refGlade*/);
	virtual ~ObservableTreeView();
	void applyRenaming();
	void build();
	void clearRenaming()
        { mRenameList.clear(); }
	void setObservables(vpz::Observables* observables)
        { mObservables = observables; }
	void setModeling(Modeling* modeling)
        { mModeling = modeling; }
        void setModel(vpz::AtomicModel* atom)
        { mAtom = atom; }
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
	    const Glib::ustring& path,
	    const Glib::ustring& newName);

        void onToggled(const Glib::ustring&  path);

        Gtk::TreeModel::iterator getSelectedRadio();

    private:
	typedef std::vector < std::pair < std::string,
					  std::string > > renameList;

        vpz::AtomicModel*          mAtom;
	vpz::Observables* mObservables;
	renameList mRenameList;
	Modeling* mModeling;
        Glib::RefPtr <Gtk::UIManager> mPopupUIManager;
        Glib::RefPtr <Gtk::ActionGroup> mPopupActionGroup;
	Gtk::Menu *mMenuPopup;
	ModelColumnsObs mColumnsObs;
	Glib::RefPtr<Gtk::ListStore> mRefTreeModelObs;
	//Dialog Box
	ObsAndViewBox mObsAndViewBox;
	//Label
	Gtk::Label* mLabel;
        //Cell
	int                      mColumnName;
        int                      mColumnSel;
	Gtk::CellRendererText*   mCellRenderer;
	std::string              mOldName;
	bool                     mValidateRetry;
	Glib::ustring            mInvalidTextForRetry;
	guint32                  mDelayTime;
    };

    Glib::RefPtr < Gtk::Builder >        mXml;
    Modeling*                            mModeling;
    GVLE*                                mGVLE;

    Gtk::Dialog*                         mDialog;

    vpz::AtomicModel*                  mCurrentModel;
    vpz::AtomicModel*                  mModel;
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
