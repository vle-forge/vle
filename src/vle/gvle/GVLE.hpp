/**
 * @file vle/gvle/GVLE.hpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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
#include <vle/gvle/HelpBox.hpp>
#include <vle/gvle/LaunchSimulationBox.hpp>
#include <vle/gvle/ModelTreeBox.hpp>
#include <vle/gvle/ModelClassBox.hpp>
#include <vle/gvle/NewProjectBox.hpp>
#include <vle/gvle/ObserverPlugin.hpp>
#include <vle/gvle/OpenPackageBox.hpp>
#include <vle/gvle/OpenVpzBox.hpp>
#include <vle/gvle/ParameterExecutionBox.hpp>
#include <vle/gvle/Plugin.hpp>
#include <vle/gvle/SaveVpzBox.hpp>
#include <vle/gvle/ViewDrawingArea.hpp>
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


namespace vle
{
namespace gvle {

class GVLEMenu;
class Modeling;
class PluginTable;
class PreferencesBox;
class PackageBrowserWindow;
//class View;

/**
 * @brief Document class used within Gtk::Notebook
 */
class Document : public Gtk::ScrolledWindow {
public:

    Document();

    virtual inline bool isDrawingArea()
	{ return false; }
};

class DocumentText : public Document {
public:
    DocumentText(const std::string& filePath, bool newfile = false);
    ~DocumentText();

    void save();
    void saveAs(const std::string& filename);

    inline const std::string filename() const
    { return mFileName; }

    inline const std::string filepath() const
    { return mFilePath; }

    inline bool isNew() const
    { return mNew == true; }

private:
    Gtk::TextView  mView;
    std::string    mFilePath;
    std::string    mFileName;
    bool           mModified;
    bool           mNew;

    void init();
};

class DocumentDrawingArea : public Document {
public:
    DocumentDrawingArea(const std::string& filePath,
			View* view,
			graph::Model* model);
    ~DocumentDrawingArea();

     inline View* getView()
	{ return mView; }

    inline ViewDrawingArea* getDrawingArea()
	{ return mArea; }

    virtual inline bool isDrawingArea()
	{ return true; }


private:
    std::string         mFilePath;
    View*               mView;
    ViewDrawingArea*    mArea;
    graph::Model*       mModel;
};

/**
 * @brief GVLE is a Gtk::Window use to build the main window with all button
 * to control project.
 */
class GVLE : public Gtk::Window
{
public:
    typedef std::multimap < std::string, std::string > MapCategory;
    typedef std::multimap < std::string, std::string > MapObserverCategory;
    typedef std::map < std::string, Plugin * > MapPlugin;
    typedef std::map < std::string, ObserverPlugin * > MapObserverPlugin;
    typedef std::map < std::string, Document* > Documents;

    /** list off all available buttons. */
    enum ButtonType { POINTER, ADDMODEL, ADDLINK, ADDCOUPLED, DELETE, ZOOM,
                      PLUGINMODEL, GRID, QUESTION
                    };

    static const int overview_max = 60;

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
        return m_currentButton;
    }

    /**
     * Create the hierarchy in the package browser
     */
    void show();

    /**
     * Assign a file to the window
     *
     * @param name the file name
     */
    void setFileName(std::string name);

    /**
     * Redraw the TreeViews
     */
    void redrawModelTreeBox();
    void redrawModelClassBox();

    /**
     * return the ConditionBox
     * @return ConditionBox instance of ConditionBox
     */
    inline ConditionsBox* getConditionsBox() {
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

    Gtk::RadioToolButton* getButtonRef(ButtonType button);

    /* methodes to manage tabs */
    void focusTab(const std::string& filepath);
    void openTab(const std::string& filepath);
    void openTabVpz(const std::string& filepath, graph::CoupledModel* model);
    void closeTab(const std::string& filepath);
    void closeAllTab();
    void changeTab(GtkNotebookPage* page, int num);

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
    private:
	void buildHierarchy(const Gtk::TreeModel::Row& parent,
			    const std::string& dirname);
	bool isDirectory(const std::string& dirname);
	void on_row_activated(const Gtk::TreeModel::Path& path,
			      Gtk::TreeViewColumn*  column);
	std::list<std::string>* projectFilePath(const Gtk::TreeRow& row);

	GVLE*                            mParent;
	FileModelColumns                 mColumns;
	std::string                      mPackage;
	Glib::RefPtr<Gtk::TreeStore>     mRefTreeModel;
	Glib::RefPtr<Gtk::TreeSelection> mRefTreeSelection;
	std::list<std::string>           mIgnoredFilesList;
    };

    /*********************************************************************
     *
     * PLUGIN
     *
     *********************************************************************/

    /**
     * recursive show plugin path and all file.
     *
     * @param path load plugin file from this directory
     */
    //void loadPlugins(const std::string& path);

    /**
     * recursive show observer plugin path and all file.
     *
     * @param path load observer plugin file from this directory
     */
    //void loadObserverPlugins(const std::string& path);

    /**
     * Open plugin, add icons and button into button group.
     *
     * @param name filename of plugin file to load
     */
    //Plugin* loadPlugin(const std::string& name);

    /**
     * Open observer plugin.
     *
     * @param name filename of observer plugin file to load
     */
    //ObserverPlugin* loadObserverPlugin(const std::string& name);

    /**
     * return a plugin from plugin list.
     *
     * @param name formalism name of plugin to find
     * @return a ptr to plugin, otherwise 0
     */
    //Plugin* getPlugin(const std::string& name);

    /**
     * return a observer plugin from observer plugin list.
     *
     * @param name observer name of plugin to find
     * @return a ptr to plugin, otherwise 0
     */
    //ObserverPlugin* getObserverPlugin(const std::string& name);

    //const MapObserverPlugin & getObserverPluginList() const
    //  { return m_observerPlugins; }

    /**
     * Delete all plugins and togglebutton loaded.
     *
     */
    //void delPlugins();

    /**
     * When select a plugin button, we change current selection.
     *
     * @param name formalism name (or plugin name) of selected plugin.
     */
    //void onPluginButton(const std::string& name);

    /**
     * Get current plugin name selected in GVLE plugin table.
     *
     * @return current plugin name.
     */
    //inline const std::string& getCurrentPluginName() const
    //{ return m_pluginSelected; }


private:
    /*********************************************************************
     *
     * MAKING GRAPHICS INTERFACE
     *
     *********************************************************************/

    /**
     * Buttons creating and connect events.
     *
     */
    void makeButtons();

    /**
     * Assign to this comboboxstring, all category read from plugins ;
     * Before call this function, comboboxstring default choice is 'All' ;
     * This function connect comboCategory to his function changed.
     */
    void makeComboCategory();

    /**
     * When user select a new category from ComboBoxString comboCategory.
     *
     */
    void on_combocategory_changed();


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


public:

    /**
     * When click on new file menu
     *
     */
    void newFile();

    /**
     * When click on new menu.
     *
     */
    void onMenuNew();

    /**
     * When click on new project menu
     *
     */
    void onMenuNewProject();

    /**
     * When click on open file menu
     *
     */
    void openFile();

    /**
     * When click on open package menu
     *
     */
    void onMenuOpenPackage();

    /**
     * When clinck on open vpz menu
     *
     */
    void onMenuOpenVpz();

    /**
     * When click on load menu.
     *
     */
    void onMenuLoad();

    /**
     * When click on save file menu.
     *
     */
    void saveFile();

    /**
     * When click on save menu.
     *
     */
    void onMenuSave();

    /**
     * When click on save as menu
     *
     */
    void saveFileAs();

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
     * When click on close menu
     *
     */
    void closeFile();

    /**
     * Whien click on quit as menu
     *
     */
    void onMenuQuit();


    /********************************************************************
     *
     * MENU EDIT
     *
     ********************************************************************/

    /** Cut selected model list into CutCopyPaste. */
    void onCutModel();

    /** Copy selected model list into CutCopyPaste. */
    void onCopyModel();

    /**
     * Paste selected model list from CutCopyPaste into this GCoupledModel.
     */
    void onPasteModel();

    /********************************************************************
     *
     * MENU PACKAGE
     *
     ********************************************************************/

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
     * When click on Show Model TreeView menu.
     *
     */
    void onShowModelTreeView();


    /**
     * When click on Show deconnected models Treeview menu.
     *
     */
    void onShowModelClassView();

    /**
     * When click on Show PackageBrowser
     *
     */
    void onShowPackageBrowserWindow();

    /**
     * @brief Delete all gvle::View from the gvle::Modeling object.
     */
    void onCloseAllViews();

    /**
     * @brief Iconify all gvle::View from the gvle::Modeling object.
     */
    void onIconifyAllViews();

    /**
     * @brief Deiconify all gvle::View from the gvle::Modeling object.
     */
    void onDeiconifyAllViews();

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
     * Show the Package Browser Window
     */
    void showPackageBrowserWindow();

    /**
     * Hide the Package Browser Window
     */
    void hidePackageBrowserWindow();

    /**
     * toggle show/hide Package Browser Window
     */
    void togglePackageBrowserWindow();

    /**
     * Build the hierarchy in the Package Browser
     */
    void buildPackageHierarchy();

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
    void setTitle(const Glib::ustring& name);

    /**
     * @brief Append an asterisk to the Gtk::Window's title with an asterick.
     */
    void setModifiedTitle();

private:
    Gtk::VBox                       m_vbox;
    GVLEMenu*                       m_menu;
    Gtk::HSeparator                 m_separator;
    Gtk::Table                      m_buttons;
    //widgets::ComboBoxString         m_comboCategory;
    //Gtk::ScrolledWindow             m_scrolledPlugins;
    //PluginTable*                    m_pluginTable;
    //Gtk::Label                      m_labelName;
    Gtk::RadioButton::Group         m_buttonGroup;
    Gtk::RadioToolButton                m_arrow;
    Gtk::RadioToolButton                m_addModels;
    Gtk::RadioToolButton                m_addLinks;
    Gtk::RadioToolButton                m_addCoupled;
    Gtk::RadioToolButton                m_delete;
    Gtk::RadioToolButton                m_zoom;
    Gtk::RadioToolButton                m_question;
    Gtk::Tooltips                   m_tooltips;
    Gtk::Statusbar                  m_status;
    Modeling*                       m_modeling;

    MapCategory                     m_category;
    MapObserverCategory             m_observerCategory;
    MapPlugin                       m_plugins;
    MapObserverPlugin               m_observerPlugins;

    ButtonType                      m_currentButton;
    HelpBox*                        m_helpbox;

    //std::string                     m_pluginSelected;

    Glib::RefPtr < Gnome::Glade::Xml >  mRefXML;

    //Menu
    ConditionsBox*                  mConditionsBox;
    LaunchSimulationBox*            mSimulationBox;
    PreferencesBox*                 mPreferencesBox;
    OpenPackageBox*                 mOpenPackageBox;
    OpenVpzBox*                     mOpenVpzBox;
    PackageBrowserWindow*           mPackageBrowserWindow;
    NewProjectBox*                  mNewProjectBox;
    SaveVpzBox*                     mSaveVpzBox;

    Gtk::TextView*                  mLog;
    Gtk::Statusbar*                 mStatusbar;
    Gtk::Notebook*                  mNotebook;
    Gtk::Toolbar*                   mToolBar;
    FileTreeView*                   mFileTreeView;
    Documents                       mDocuments;
    ModelTreeBox*                   mModelTreeBox;
    ModelClassBox*                  mModelClassBox;
    std::string                     mPackage;
    View*                           mCurrentView;
    int                             mCurrentTab;

};

std::string valuetype_to_string(value::Value::type type);

}
} // namespace vle gvle

#endif
