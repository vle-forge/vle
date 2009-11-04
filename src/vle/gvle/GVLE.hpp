/**
 * @file vle/gvle/GVLE.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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


#ifndef GUI_GVLE_HH
#define GUI_GVLE_HH

#include <vle/gvle/ConditionsBox.hpp>
#include <vle/gvle/ModelTreeBox.hpp>
#include <vle/gvle/ModelClassBox.hpp>
#include <vle/gvle/NewProjectBox.hpp>
#include <vle/gvle/ObserverPlugin.hpp>
#include <vle/gvle/OpenPackageBox.hpp>
#include <vle/gvle/OpenVpzBox.hpp>
#include <vle/gvle/ParameterExecutionBox.hpp>
#include <vle/gvle/QuitBox.hpp>
#include <vle/gvle/SaveVpzBox.hpp>
#include <vle/value/Value.hpp>
#include <vle/gvle/ComboboxString.hpp>
#include <vle/utils/Path.hpp>
#include <gtkmm/window.h>
#include <gtkmm/box.h>
#include <gtkmm/separator.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/table.h>
#include <gtkmm/tooltips.h>
#include <libglademm/xml.h>
#include <string>
#include <list>
#include <map>

namespace vle { namespace gvle {

class Editor;
class GVLEMenuAndToolbar;
class Modeling;
class PreferencesBox;
class PackageBrowserWindow;

/**
 * @brief GVLE is a Gtk::Window use to build the main window with all button
 * to control project.
 */
class GVLE : public Gtk::Window
{
public:
    typedef std::multimap < std::string, std::string > MapCategory;
    typedef std::multimap < std::string, std::string > MapObserverCategory;
    typedef std::map < std::string, ObserverPlugin * > MapObserverPlugin;

    /** list off all available buttons. */
    enum ButtonType { POINTER, ADDMODEL, ADDLINK, ADDCOUPLED, DELETE, ZOOM,
                      PLUGINMODEL, GRID, QUESTION
                    };

    static const int overview_max = 60;
    static const std::string WINDOW_TITLE;

    /**
     * Create a new window GVLE, if parameter filename is not empty, try to
     * load the vpz file else, new document is load.
     *
     */
    GVLE(BaseObjectType* cobject,
		 const Glib::RefPtr<Gnome::Glade::Xml> xml);

    /**
     * Delete all plugin and Modeling document class.
     *
     */
    virtual ~GVLE();

    bool on_delete_event(GdkEventAny* event);

    /**
     * return the current selectionned button.
     *
     * @return ButtonType of current selected button.
     */
    inline ButtonType getCurrentButton() const {
        return mCurrentButton;
    }

    /**
     * Create the hierarchy in the package browser
     */
    void show();

    /**
     * @brief Append an asterisk to the Gtk::Window's title with an asterick.
     */
    void setModifiedTitle(const std::string& name);

    /**
     * Assign a file to the window
     *
     * @param name the file name
     */
    void setFileName(std::string name);

    /**
     * @brief Insert text into Log area
     * @param text Text to insert into the log area
     */
    void insertLog(const std::string& text);

    /**
     * Redraw the VPZ view
     */
    void redrawView();

    /**
     * Redraw the TreeViews
     */
    void redrawModelTreeBox();
    void redrawModelClassBox();

    /**
     * Clear the TreeViews
     */
    void clearModelTreeBox();
    void clearModelClassBox();

    /**
     * active a row into TreeBox for a particular string, all activated row
     * area hide.
     *
     * @param name model name to activate.
     */
    void showRowTreeBox(const std::string& name);

    /**
     * active a row into ModelClassBox for a particular string, all activated
     * row area hide.
     *
     * @param name class name to activate.
     */
    void showRowModelClassBox(const std::string& name);

    /**
     * return the ConditionBox
     * @return ConditionBox instance of ConditionBox
     */
    inline ConditionsBox* getConditionsBox()
    {
	return mConditionsBox;
    }

    /**
     * return the ModelTreeBox
     * @return ModelTreeBox instance of ModelTreeBox
     */
    inline ModelTreeBox* getModelTreeBox()
    { return mModelTreeBox; }

    /**
     * return the ModelTreeBox
     * @return ModelTreeBox instance of ModelTreeBox
     */
    inline ModelClassBox* getModelClassBox()
    { return mModelClassBox; }

    /**
     * return the Modeling instance
     * @return Modeling instance
     */
    inline Modeling* getModeling()
    { return mModeling; }

    /**
     * @brief return the MenuAndToolbar instance
     * @return MenuAndToolbar instance
     */
    inline GVLEMenuAndToolbar* getMenu()
    { return mMenuAndToolbar; }

    /**
     * @brief return the Editor
     * @return Editor instance
     */
    inline Editor* getEditor()
    { return mEditor; }

    inline const Editor& getEditor() const
    { return *mEditor; }

    /**
     * To update the adjustment.
     */
    void updateAdjustment(double h, double v);

    void setCurrentTab(int n)
    { mCurrentTab = n; }

    /**
     * @brief Tree model columns used in FileTreeView
     */
    class FileModelColumns : public Gtk::TreeModel::ColumnRecord
    {
    public:
	FileModelColumns()
	    { add(m_col_name); }

	Gtk::TreeModelColumn<Glib::ustring> m_col_name;
    };

    /**
     * @brief FileTreeView used for display file hierarchy
     */
    class FileTreeView : public Gtk::TreeView
    {
    public:
	FileTreeView(BaseObjectType* cobject,
		     const Glib::RefPtr<Gnome::Glade::Xml>& /*refGlade*/);
        virtual ~FileTreeView();

        void build();

	void clear();

	inline void setPackage(const std::string& package)
        { mPackage = package; }

	void setParent(GVLE* parent)
        { mParent = parent; }

        Glib::ustring getSelected()
        { return (*mRefTreeSelection->get_selected())[mColumns.m_col_name]; }

    protected:
	bool on_button_press_event(GdkEventButton* event);
	void onOpen();
	void onNewFile();
	void onNewDirectory();
	void onRemove();
	void onRename();

    private:
	/**
	 * Create the hierarchy with the directories
	 *
	 * @param parent the parent's of the current row
	 * @param dirname the name of the parent directory
	 *
	 */
	void buildHierarchyDirectory(const Gtk::TreeModel::Row* parent,
				     const std::string& dirname);

	/**
	 * Create the hierarchy with the files
	 *
	 * @param parent the parent's of the current row
	 * @param dirname the name of the parent directory
	 *
	 */
	void buildHierarchyFile(const Gtk::TreeModel::Row* parent,
				const std::string& dirname);

	void buildHierarchy(const Gtk::TreeModel::Row* parent,
			    const std::string& dirname);
	bool isDirectory(const std::string& dirname);
	void on_row_activated(const Gtk::TreeModel::Path& path,
			    Gtk::TreeViewColumn*  column);
	void projectFilePath(const Gtk::TreeRow& row,
			     std::list<std::string>& lst);

	GVLE*                            mParent;
	Gtk::Menu                        mMenuPopup;
	FileModelColumns                 mColumns;
	std::string                      mPackage;
	Glib::RefPtr<Gtk::TreeStore>     mRefTreeModel;
	Glib::RefPtr<Gtk::TreeSelection> mRefTreeSelection;
	std::list<std::string>           mIgnoredFilesList;

	//Cell
	int                              mColumnName;
	bool                             mValidateRetry;
	Gtk::CellRendererText*           mCellrenderer;
	Glib::ustring                    mInvalidTextForRetry;
	guint32                          mDelayTime;
    };

public:
    /*********************************************************************
     *
     * WHEN SELECT A MENU FROM GVLEMenu
     *
     *********************************************************************/


    /**
     * When select arrow button.
     *
     */
    void onArrow();

    /**
     * When select atomic models button.
     *
     */
    void onAddModels();

    /**
     * When select connection button.
     *
     */
    void onAddLinks();

    /**
     * When select coupled button.
     *
     */
    void onAddCoupled();

    /**
     * When select delete button.
     *
     */
    void onDelete();

    /**
     * When select zoom button.
     *
     */
    void onZoom();

    /**
     * When select question button.
     *
     */
    void onQuestion();


    /*********************************************************************
     *
     * MENU FILE
     *
     *********************************************************************/


    /**
     * When click on new file menu
     *
     */
    void onNewFile();

    /**
     * When click on new menu.
     *
     */
    void onNewVpz();

    /**
     * When click on new project menu
     *
     */
    void onNewProject();

    /**
     * When click on open file menu
     *
     */
    void onOpenFile();

    /**
     * When click on open project menu
     *
     */
    void onOpenProject();

    /**
     * When click on open vpz menu
     *
     */
    void onOpenVpz();

    /**
     * When click on open global vpz menu.
     *
     */
    void onOpenGlobalVpz();

    /**
     * When click on save menu.
     *
     */
    void onSave();

    /**
     * When click on save as menu
     *
     */
    void onSaveAs();

    /**
     * When click on clear as menu
     *
     */
    void clearCurrentModel();

    /**
     * When click on export as menu
     *
     */
    void exportCurrentModel();

    /**
     * When click on export graphic as menu
     *
     */
    void exportGraphic();

    /**
     * When click on import model as menu
     *
     */
    void importModel();

    /**
     * When click on close tab menu
     *
     */
    void onCloseTab();
    bool closeTab(const std::string& filepath = "");

    /**
     * When click on close project menu
     *
     */
    void onCloseProject();

    /**
     * Whien click on quit as menu
     *
     */
    void onQuit();


    /********************************************************************
     *
     * MENU EDIT
     *
     ********************************************************************/

    /** Cut selected model list into CutCopyPaste. */
    void onCutModel();

    /** Copy selected model list into CutCopyPaste. */
    void onCopyModel();

    /** Paste selected model list from CutCopyPaste into this GCoupledModel.  */
    void onPasteModel();

    /********************************************************************
     *
     * MENU PACKAGE
     *
     *
     ********************************************************************/

    bool packageTimer();
    bool packageBuildTimer();

    /**
     * When click on configure project menu
     *
     */
    void configureProject();

    /**
     * When click on build project menu
     *
     */
    void buildProject();

    /**
     * @brief When click on install project menu.
     */
    void installProject();

    /**
     * When click on clean project menu
     *
     */
    void cleanProject();

    /**
     * When click on create project menu
     *
     */
    void packageProject();



    /********************************************************************
     *
     * MENU SHOW
     *
     ********************************************************************/

    /**
     * @brief Order the models
     */
    void onRandomOrder();

    /**
     * @brief Show the PreferencesBox
     */
    void onPreferences();

    /********************************************************************
     *
     * PACKAGE BROWSER
     *
     ********************************************************************/

    /**
     * Build the hierarchy in the Package Browser
     */
    void buildPackageHierarchy();

    /**
     * Refresh the editor
     */
    void refreshEditor(const std::string& name, const std::string& path);

    /*********************************************************************
     *
     * MENU EXECUTION
     *
     *********************************************************************/


    /**
     * When click on Execution menu.
     *
     */
    void onSimulationBox();

    /**
     * When click on parameter execution menu box.
     *
     */
    void onParameterExecutionBox();

    /**
     * When click on conditions box to define initial conditions
     * of experiments
     *
     */
    void onConditionsBox();

    int runConditionsBox(const vpz::Conditions& conditions);
    void applyConditionsBox(vpz::Conditions& conditions);

    /**
     * When click on experiment box to define init, state etc.
     *
     */
    void onExperimentsBox();

    /**
     * When click on hosts box to define simulation hosts.
     *
     */
    void onHostsBox();

    /**
     * @brief When user select the gvle::ViewOutputBox.
     */
    void onViewOutputBox();

    /*********************************************************************
     *
     * MENU ZOOM
     *
     *********************************************************************/

    /**
     * To zoom more
     *
     */
    void addCoefZoom();

    /**
     * To zoom less
     *
     */
    void delCoefZoom();

    /**
     * To set a coefficient to the zoom
     *
     * @param coef the coefficient
     */
    void setCoefZoom(double coef);


    /*********************************************************************
       *
       * MENU HELP
       *
       *********************************************************************/


    /**
     * on menu HelpBox click.
     *
     */
    void onHelpBox();

    /**
     * on menu About click.
     *
     */
    void onShowAbout();

    /**
     * @brief Assign a new title to Gtk::Window managed by the GVLE
     * class.
     * @param name The name to assign.
     */
    void setTitle(const Glib::ustring& name = "");

private:
    void fixSave();
    void saveVpz();
    void saveFirstVpz();

    Glib::RefPtr < Gnome::Glade::Xml >  mRefXML;

    /* Widgets */
    Gtk::VBox*                      mMenuAndToolbarVbox;
    GVLEMenuAndToolbar*             mMenuAndToolbar;
    Gtk::Statusbar                  mStatusBar;
    Gtk::TextView*                  mLog;
    Gtk::Statusbar*                 mStatusbar;
    Editor*                         mEditor;
    FileTreeView*                   mFileTreeView;

    /* class members */
    Modeling*                       mModeling;
    ButtonType                      mCurrentButton;
    std::string                     mPackage;
    int                             mCurrentTab;

    /* File chooser */
    Glib::ustring                   mGlobalVpzPrevDirPath; /* the previous
                                    directory where a vpz was chosen
                                    in global mode */

    /* Dialog boxes */
    ConditionsBox*                  mConditionsBox;
    PreferencesBox*                 mPreferencesBox;
    OpenPackageBox*                 mOpenPackageBox;
    OpenVpzBox*                     mOpenVpzBox;
    PackageBrowserWindow*           mPackageBrowserWindow;
    NewProjectBox*                  mNewProjectBox;
    SaveVpzBox*                     mSaveVpzBox;
    ModelTreeBox*                   mModelTreeBox;
    ModelClassBox*                  mModelClassBox;
    QuitBox*                        mQuitBox;
};

std::string valuetype_to_string(value::Value::type type);

}} // namespace vle gvle

#endif
