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


#ifndef GUI_GVLE_HH
#define GUI_GVLE_HH

#include <vle/gvle/DllDefines.hpp>
#include <vle/gvle/PluginFactory.hpp>
#include <vle/gvle/ConditionsBox.hpp>
#include <vle/gvle/ModelTreeBox.hpp>
#include <vle/gvle/ModelClassBox.hpp>
#include <vle/gvle/CutCopyPaste.hpp>
#include <vle/gvle/ObserverPlugin.hpp>
#include <vle/gvle/QuitBox.hpp>
#include <vle/gvle/SaveVpzBox.hpp>
#include <vle/gvle/SpawnPool.hpp>
#include <vle/utils/Package.hpp>
#include <vle/value/Value.hpp>
#include <gtkmm/window.h>
#include <gtkmm/textview.h>
#include <gtkmm/box.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/builder.h>
#include <string>
#include <vector>
#include <map>

#include <sigc++/sigc++.h>

namespace vle { namespace gvle {

class Document;
class Editor;
class FileTreeView;
class GVLEMenuAndToolbar;
class Modeling;
class AtomicModelBox;
class ImportModelBox;
class ImportClassesBox;
class CoupledModelBox;
class PreferencesBox;
class PackageBrowserWindow;
class CutCopyPaste;
class View;

typedef std::set < std::string > Depends;
typedef std::map < std::string, Depends > AllDepends;
typedef std::vector < std::pair < std::string,
                                  std::string > > renameList;
/** define vector of current view. */
typedef std::vector < View * > ListView;

/**
 * @brief GVLE is a Gtk::Window use to build the main window with all button
 * to control project.
 */
class GVLE_API GVLE : public Gtk::Window
{
public:
    /**
     * @attention With mingw32, gvle.exe and libgvle.dll, the Gtk::Window is
     * not displayed if the Gtk::Main loop is in gvle.exe and the GVLE is in
     * libgvle.dll. To fix the problem, we introduce the start function which
     * wraps the Gtk::Main loop into the libgvle.dll. The gvle.exe application
     * have only a call to this function.
     */
    static void start(int argc, char *argv[], const std::string &package,
            const std::string &filename);

    typedef std::multimap < std::string, std::string > MapCategory;
    typedef std::multimap < std::string, std::string > MapObserverCategory;
    typedef std::map < std::string, ObserverPlugin * > MapObserverPlugin;

    /** list off all available buttons. */
    enum ButtonType { VLE_GVLE_POINTER, VLE_GVLE_ADDMODEL, VLE_GVLE_ADDLINK,
        VLE_GVLE_ADDCOUPLED, VLE_GVLE_DELETE, VLE_GVLE_ZOOM,
        VLE_GVLE_PLUGINMODEL, VLE_GVLE_GRID, VLE_GVLE_QUESTION };

    static const int overview_max = 60;

    /**
     * @brief Return the version number of GVLE.
     * @return A string representation of the version number.
     */
    static std::string windowTitle();

    /**
     * Create a new window GVLE, if parameter filename is not empty, try to
     * load the vpz file else, new document is load.
     *
     */
    GVLE(BaseObjectType* cobject,
         const Glib::RefPtr < Gtk::Builder >& xml);

    /**
     * Delete all plugin and Modeling document class.
     *
     */
    virtual ~GVLE();

    /**
     * @brief update the view when Modeling mention that the vpz is
     * modified.
     */
    void onSignalModified();

    /**
     * @brief Used to erase the status bar.
     * @return a boolean
     */
    bool on_timeout();

    /**
     * @brief Get a complete source file name.
     * @return A string
     */
    std::string getPackageSrcFile(const std::string& file) const
    { return utils::Path::path().buildFilename(
            mCurrPackage.getDir(vle::utils::PKG_SOURCE), "src", file); }
    /**
     * @brief Get a complete source dir  name.
     * @return A string
     */
    std::string getPackageSrcDir() const
    { return utils::Path::path().buildDirname(
            mCurrPackage.getDir(vle::utils::PKG_SOURCE), "src"); }

    /**
     * @brief Get a constant reference to the PluginFactory.
     * @return A constant reference.
     */
    const PluginFactory& pluginFactory() const
    { return mPluginFactory; }

    /**
     * @brief Get a reference to the PluginFactory.
     * @return A reference.
     */
    PluginFactory& pluginFactory()
    { return mPluginFactory; }

    bool on_delete_event(GdkEventAny* event);

    /**
     * return the current selectionned button.
     *
     * @return ButtonType of current selected button.
     */
    inline ButtonType getCurrentButton() const
    { return mCurrentButton; }

    /**
     * @brief Create the hierarchy in the package browser
     */
    void show();

    /**
     * @brief Provide a message to the StatusBar
     */
    void showMessage(const std::string& message);

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

    void modifications(std::string filepath, bool isModif);

    void focusRow(std::string filepath);

    /**
     * add a view to modeling for the GModel model.
     *
     * @param model a ptr to the GModel to show.
     */
    void addView(vpz::BaseModel* model);

    /**
     * find a view in list view that reference a GCoupledModel.
     * @param model reference to GCoupledModel to find.
     * @return int index in list view for founded view or first index with
     * null value
     */
    void addView(vpz::CoupledModel* model);

    /**
     * add a view to the current class
     *
     * @param model to ptr to the GModel assiociate to the class
     */
    void addViewClass(vpz::BaseModel* model, std::string name);

    /**
     * find a view in list view that reference a GCoupledModel.
     * @param model reference to GCoupledModel to find.
     */

    void addViewClass(vpz::CoupledModel* model, std::string name);

    /**
     * @brief Insert text into Log area
     * @param text Text to insert into the log area
     */
    void insertLog(const std::string& text);

    /**
     * @brief For the Gtk::TextView log widget, move the Gtk::TextView to the
     * last line of the Gtk::Buffer.
     */
    void scrollLogToLastLine();

     /**
     * @brief delete all models and views and restart with one view.
     *
     */
    void start();

    /**
     * @brief delete all models and views and restart with one view.
     *
     * @param path path where the vpz file is wanted to be stored
     * @param fileName vpz file name
     *
     */
    void start(const std::string& path, const std::string& fileName);

     /**
     * parse XML project file to get filename of XML files structures and
     * graphics.
     *
     * @param name project file name to parse.
     */
    void parseXML(const std::string& name);

    /**
     * Return True if a View of the parameter already exist.
     * @param model reference to GCoupledModel to find.
     * @return Return True if the View exist , False otherwise
     */
    bool existView(vpz::CoupledModel* model);

    /**
     * Return the View that reference the model.
     * @param model reference to GCoupledModel to find.
     * @return Return the View if exist , NULL otherwise
     */
    View* findView(vpz::CoupledModel* model);

    /**
     * delete a view from model view.
     *
     * @param index num of window to delete.
     */
    void delViewIndex(size_t index);

    /**
     * delete all view where coupled model is equal to cm.
     *
     * @param cm a ptr to vpz::CoupledModel to delete view.
     */
    void delViewOnModel(const vpz::CoupledModel* cm);

    /**
     * delete all view of modeling.
     *
     */
    void delViews();


    void refreshViews();

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
     * return the ConditionBox
     * @return ConditionBox instance of ConditionBox
     */
    inline ConditionsBox* getConditionsBox()
    { return mConditionsBox; }

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
     * return the FileTreeView
     * @return FileTreeView instance of FileTreeView
     */
    inline FileTreeView* getFileTreeView()
    { return mFileTreeView; }

    /**
     * return the Modeling instance
     * @return Modeling instance
     */
    inline Modeling* getModeling()
    { return mModeling; }

    /**
     * return the Modeling instance
     * @return Modeling instance
     */
    inline const Modeling* getModeling() const
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

    int getCurrentTab() const
    { return mCurrentTab; }

    void setCurrentTab(int n)
    { mCurrentTab = n; }

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
     * @brief When asking for, from the project treeview, and after
     * the name has been choosen with a dialog.
     *
     */
    void onNewFile(const std::string& path, const std::string& fileName);

    /**
     * When click on new menu.
     *
     */
    void onNewVpz();

    /**
     * @brief When asking for, from the project treeview, and after
     * the name has been choosen with a dialog.
     *
     */
    void onNewNamedVpz(const std::string& path, const std::string& fileName);

    /**
     * When click on new project menu
     *
     */
    void onNewProject();

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

    void EditCoupledModel(vpz::CoupledModel* model);

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

    void importClasses(vpz::Vpz* src);

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

     /** Select all models in current VPZ */
    void onSelectAll();

    /********************************************************************
     *
     * CUT COPY AND PASTE
     *
     ********************************************************************/


    /**
     * clone the ListGModel if has no connection with external model.
     *
     * @param lst list of selected GModel.
     * @param gc parent of selected GModel.
     */
    void cut(vpz::ModelList& lst, vpz::CoupledModel* gc,
             std::string className);

    /**
     * detach the list of GModel of GCoupledModel parent.
     *
     * @param lst list of selected GModel.
     * @param gc parent of selected GModel.
     */
    void copy(vpz::ModelList& lst, vpz::CoupledModel* gc,
              std::string className);

    /**
     * paste the current list GModel into GCoupledModel ; rename
     * models to elimiante duplicated name.
     *
     * @param gc paste selected GModel under this GCoupledModel.
     */
    void paste(vpz::CoupledModel* gc, std::string className);

    /**
     * select all models in the current list GModel.
     *
     * @param lst list of selected GModel.
     */
    void selectAll(vpz::ModelList& lst, vpz::CoupledModel* gc);

    bool paste_is_empty();

    /**
     * Set document modification.
     *
     * @param modified true if document is modified, otherwise false.
     */
    void setModified(bool modified);

    /********************************************************************
     *
     * MENU PACKAGE
     *
     *
     ********************************************************************/

    /**
     * @brief log the output of the current operation an manage the
     * launch of the next one
     */
    bool packageAllTimer();
    /**
     * @brief log the output of the current operation an manage the
     * launch of the next one
     */
    bool packageConfigureAllTimer();
    /**
     * @brief log the output of the current operation an manage the
     * launch of the next one
     */
    bool packageBuildAllTimer();
    /**
     * @brief log the output of the current operation an manage the
     * launch of the next one
     */
    bool packageTimer();
    /**
     * @brief log the output of the current operation an manage the
     * launch of the next one
     */
    bool packageBuildTimer();

    /**
     * @brief only configure the current project and connect a timer
     *
     */
    void configureProject();

    /**
     * @brief build the current project and connect a timer
     *
     * The launched timer used can manage the next operation to be
     * done on the package: install. The Install operation is expected.
     */
    void buildAllProject();

    /**
     * @brief When click on build project menu
     *
     */
    void buildProject();

    /**
     * @brief get all the dependencies generated from each experiment
     *
     * @warning This code is duplicated from the main of the CLI application.
     */
    AllDepends depends();

    /**
     * @brief display the the dependencies of the current project
     *
     * For each simulator of the current package, a list of package
     * is displayed. This mean that a simulator uses a component
     * available inside a package of the list.
     *
     * @warning This code is close from showDepends() of the CLI application.
     */
    void displayDependencies();

    /**
     * @brief install the current project and connect a timer
     *
     * The launched timer used can manage the next operation to be
     * done on the next package: configure. The build configure
     * operation on the next package is expected.
     */
    void installAllProject();

    /**
     * @brief When click on install project menu.
     */
    void installProject();
    /**
     * @brief When click on test project menu.
     */
    void testProject();

    /**
     * When click on clean project menu
     *
     */
    void cleanProject();

    /**
     * When click on remove project menu
     *
     */
    void removeProject();

    /**
     * When click on create project menu
     *
     */
    void packageProject();

    inline vle::utils::Package& currentPackage()
    {
        return mCurrPackage;
    }



    /********************************************************************
     *
     * MENU VIEW
     *
     ********************************************************************/

    /**
     * @brief Order the models
     */
    void onOrder();

    /**
     * @brief Refresh the package tree view
     */
    void onRefresh();

    /**
     * @brief Show the complete view with all details
     */
    void onShowCompleteView();

    /**
     * @brief Show the simple view
     */
    void onShowSimpleView();

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
     * Refresh the hierarchy in the Package Browser
     */
    void refreshPackageHierarchy();

    /**
     * Refresh the editor
     */
    void refreshEditor(const std::string& name, const std::string& path);

    /*********************************************************************
     *
     * MENU EXECUTION
     *
     *********************************************************************/

    void configureToSimulate();
    bool configureToSimulateTimer();
    void buildToSimulate();
    bool buildToSimulateTimer();
    void installtoSimulate();
    bool installtoSimulateTimer();

    /**
     * When click on Execution menu.
     *
     */
    void onSimulationBox();

    /**
     * When click on conditions box to define initial conditions
     * of experiments
     *
     */
    void onConditionsBox();
    void applyRemoved();

    int runConditionsBox(const vpz::Conditions& conditions);
    renameList applyConditionsBox(vpz::Conditions& conditions);

    /**
     * When click on experiment box to define init, state etc.
     *
     */
    void onExperimentsBox();

    /**
     * @brief When user select the gvle::ViewOutputBox.
     */
    void onViewOutputBox();

    /**
     * @brief When user select the gvle::DynamicsBox.
     */
    void onDynamicsBox();

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

    void updateTitle();

    /*********************************************************************
     *
     * SPAWNPOOL ACCES
     *
     *********************************************************************/

    SpawnPool& spawnPool()
    { return mSpawnPool; }

private:

    bool checkVpz();
    void fixSave();
    void saveVpz();
    void saveFirstVpz();

    /**
     * @brief Start the trouble system.
     */
    void onTrouble();

    /**
     * @brief Trouble timer callback.
     *
     * @return true or false to close or continue the callback.
     */
    bool signalTroubleTimer();

    Glib::RefPtr < Gtk::Builder >   mRefXML;

    sigc::connection                mConnectionTimeout;
    sigc::connection                mTroubleTimemout;


    /* Widgets */
    Gtk::Box*                      mMenuAndToolbarVbox;
    GVLEMenuAndToolbar*             mMenuAndToolbar;
    Gtk::TextView*                  mLog;
    Gtk::Statusbar*                 mStatusbar;
    Editor*                         mEditor;
    FileTreeView*                   mFileTreeView;

    /* class members */
    Modeling*                       mModeling;
    std::string                     mCurrentClass;
    ListView                        mListView;
    ButtonType                      mCurrentButton;
    CutCopyPaste                    mCutCopyPaste;
    int                             mCurrentTab;

    /* Dialog boxes */
    AtomicModelBox*                 mAtomicBox;
    ImportModelBox*                 mImportModelBox;
    CoupledModelBox*                mCoupledBox;
    ImportClassesBox*               mImportClassesBox;
    ConditionsBox*                  mConditionsBox;
    PreferencesBox*                 mPreferencesBox;
    PackageBrowserWindow*           mPackageBrowserWindow;
    SaveVpzBox*                     mSaveVpzBox;
    ModelTreeBox*                   mModelTreeBox;
    ModelClassBox*                  mModelClassBox;
    QuitBox*                        mQuitBox;
    utils::Package                  mCurrPackage;

    PluginFactory                   mPluginFactory;

    /* Project managment */
    Depends mDependencies;
    Depends::const_iterator itDependencies;

    /* Spawn managment */
    SpawnPool mSpawnPool;
};

std::string valuetype_to_string(value::Value::type type);

}} // namespace vle gvle

#endif
