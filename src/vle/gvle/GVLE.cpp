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


#include <vle/gvle/GVLE.hpp>
#include <vle/gvle/About.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/AtomicModelBox.hpp>
#include <vle/gvle/ImportModelBox.hpp>
#include <vle/gvle/ImportClassesBox.hpp>
#include <vle/gvle/CoupledModelBox.hpp>
#include <vle/gvle/DynamicsBox.hpp>
#include <vle/gvle/Editor.hpp>
#include <vle/gvle/FileTreeView.hpp>
#include <vle/gvle/ExperimentBox.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/gvle/GVLEMenuAndToolbar.hpp>
#include <vle/gvle/PreferencesBox.hpp>
#include <vle/gvle/ViewDrawingArea.hpp>
#include <vle/gvle/ViewOutputBox.hpp>
#include <vle/gvle/View.hpp>
#include <vle/gvle/LaunchSimulationBox.hpp>
#include <vle/gvle/OpenPackageBox.hpp>
#include <vle/gvle/Settings.hpp>
#include <vle/utils/Package.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/version.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <glibmm/spawn.h>
#include <glibmm/miscutils.h>
#include <gtkmm/stock.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/main.h>
#include <glib/gstdio.h>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace vle { namespace gvle {

void GVLE::start(int argc, char *argv[],
        const std::string &packagename,
        const std::string &filename)
{
    Gtk::Main application(&argc, &argv);
    vle::gvle::GVLE* g = 0;

    Glib::RefPtr< Gtk::Builder > refBuilder = Gtk::Builder::create();

    refBuilder->add_from_file(vle::utils::Path::path().
            getGladeFile("gvle.glade").c_str());

    refBuilder->get_widget_derived("WindowPackageBrowser", g);

    if (packagename.empty() and not filename.empty())
        g->setFileName(filename);

    application.run(*g);

    delete g;
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

GVLE::GVLE(BaseObjectType* cobject,
           const Glib::RefPtr < Gtk::Builder >& xml):
    Gtk::Window(cobject),
    mModeling(new Modeling()),
    mCurrentButton(VLE_GVLE_POINTER),
    mCutCopyPaste(this),
    mCurrentTab(-1)
{
    mRefXML = xml;

    mModeling->signalModified().connect(
        sigc::mem_fun(*this, &GVLE::onSignalModified));

    Settings::settings().load();

    mConditionsBox = new ConditionsBox(mRefXML, this);
    mPreferencesBox = new PreferencesBox(mRefXML);
    mAtomicBox = new AtomicModelBox(mRefXML, mModeling, this);
    mImportModelBox = new ImportModelBox(mRefXML, mModeling);
    mCoupledBox = new CoupledModelBox(xml, mModeling, this);
    mImportClassesBox = new ImportClassesBox(xml, mModeling, this);
    mSaveVpzBox = new SaveVpzBox(mRefXML, mModeling, this);
    mQuitBox = new QuitBox(mRefXML, this);

    mRefXML->get_widget("MenuAndToolbarVbox", mMenuAndToolbarVbox);
    mRefXML->get_widget("StatusBarPackageBrowser", mStatusbar);
    mRefXML->get_widget("TextViewLogPackageBrowser", mLog);
    mRefXML->get_widget_derived("FileTreeViewPackageBrowser", mFileTreeView);
    mFileTreeView->setParent(this);
    mRefXML->get_widget_derived("NotebookPackageBrowser", mEditor);
    mEditor->setParent(this);
    mRefXML->get_widget_derived("TreeViewModel", mModelTreeBox);
    mModelTreeBox->setModelingGVLE(mModeling, this);
    mRefXML->get_widget_derived("TreeViewClass", mModelClassBox);
    mModelClassBox->createNewModelBox(mModeling, this);
    mModelTreeBox->set_sensitive(false);
    mModelClassBox->set_sensitive(false);
    mFileTreeView->set_sensitive(false);

    mMenuAndToolbar = new GVLEMenuAndToolbar(this);
    mMenuAndToolbarVbox->pack_start(*mMenuAndToolbar->getMenuBar(), Gtk::PACK_SHRINK);
    mMenuAndToolbarVbox->pack_start(*mMenuAndToolbar->getToolbar(), Gtk::PACK_SHRINK);
    mMenuAndToolbar->getToolbar()->set_toolbar_style(Gtk::TOOLBAR_BOTH);

    if (mModeling->vpz().project().model().model() != 0) {
        setTitle(mModeling->getFileName());
    }

    mCurrPackage.refreshPath(); /** Initialize the VLE
                                          * settings commands from the
                                          * configuration file `vle.conf'. */

    Settings::settings(); /** Initialize the GVLE settings singleton to read
                            colors, fonts, commands from the configuration file
                            `vle.conf'. */

    Glib::signal_timeout().connect(sigc::mem_fun(*this, &GVLE::on_timeout),
                                   1000 );


    mPluginFactory.update();

    utils::ModuleList lst;
    utils::ModuleList::iterator it;

    mPluginFactory.getGvleGlobalPlugins(&lst);
    GlobalPluginPtr plugin;

    for (it = lst.begin(); it != lst.end(); ++it) {

        std::string pluginName(it->package + "/" + it->library);
        GlobalPluginPtr plugin = mPluginFactory.getGlobalPlugin(pluginName, this);
        plugin->run();
    }

    set_title(windowTitle());
    resize(900, 550);
    show();
}

std::string GVLE::windowTitle()
{
    std::string result("GVLE ");
    result += VLE_VERSION;

    std::string extra(VLE_EXTRA_VERSION);
    if (not extra.empty()) {
        result += '-';
        result += VLE_EXTRA_VERSION;
    }

    return result;
}

GVLE::~GVLE()
{
    delViews();
    delete mModeling;
    delete mAtomicBox;
    delete mImportModelBox;
    delete mCoupledBox;
    delete mImportClassesBox;
    delete mConditionsBox;
    delete mPreferencesBox;
    delete mSaveVpzBox;
    delete mQuitBox;
    delete mMenuAndToolbar;
}

bool GVLE::on_timeout()
{
    mConnectionTimeout.disconnect();

    mStatusbar->push("");
    return false;
}

void GVLE::onSignalModified()
{
    if (mModeling->isModified()) {
        setModifiedTitle(mModeling->getFileName());
        getMenu()->showSave();
    } else {
        getMenu()->hideSave();
    }
}

void GVLE::show()
{
    buildPackageHierarchy();
    show_all();
}

void GVLE::showMessage(const std::string& message)
{
    mConnectionTimeout.disconnect();
    mConnectionTimeout = Glib::signal_timeout().
        connect(sigc::mem_fun(*this, &GVLE::on_timeout), 30000);

    mStatusbar->push(message);
}

void GVLE::setModifiedTitle(const std::string& name)
{
    if (not name.empty() and utils::Path::extension(name) == ".vpz") {
        Editor::Documents::const_iterator it =
            mEditor->getDocuments().find(name);
        if (it != mEditor->getDocuments().end()) {
            it->second->setTitle(name ,getModeling()->getTopModel(), true);
        }
    }
}

void GVLE::buildPackageHierarchy()
{
    mFileTreeView->clear();
    mFileTreeView->setPackage(mCurrPackage.name());
    mFileTreeView->build();
}

void GVLE::refreshPackageHierarchy()
{
    mFileTreeView->refresh();
}

void GVLE::refreshEditor(const std::string& oldName, const std::string& newName)
{
    if (newName.empty()) { // the file is removed
        mEditor->closeTab(Glib::build_filename(mCurrPackage.name(), oldName));
    } else {
        std::string filePath = utils::Path::buildFilename(mCurrPackage.name(),
                oldName);
        std::string newFilePath = utils::Path::buildFilename(
                mCurrPackage.name(), newName);
        mEditor->changeFile(filePath, newFilePath);
        mModeling->setFileName(newFilePath);
    }
}

void GVLE::setFileName(std::string name)
{
    if (not name.empty() and utils::Path::extension(name) == ".vpz") {
        parseXML(name);
        getEditor()->openTabVpz(mModeling->getFileName(),
                                mModeling->getTopModel());
        if (mModeling->getTopModel()) {
            mMenuAndToolbar->showEditMenu();
            mMenuAndToolbar->showSimulationMenu();
            redrawModelTreeBox();
            redrawModelClassBox();
        }
    }
    mModeling->setModified(false);
}

void GVLE::modifications(std::string filepath, bool isModif)
{
    if (utils::Path::extension(filepath) == ".vpz") {
        mMenuAndToolbar->onOpenVpz();
        mModelTreeBox->set_sensitive(true);
        mModelClassBox->set_sensitive(true);
    } else {
        mMenuAndToolbar->onOpenFile();
        mModelTreeBox->set_sensitive(false);
        mModelClassBox->set_sensitive(false);
    }
    if (isModif) {
        mMenuAndToolbar->showSave();
    } else {
        mMenuAndToolbar->hideSave();
    }
    setTitle(utils::Path::basename(filepath) +
                    utils::Path::extension(filepath));
}

void GVLE::focusRow(std::string filepath)
{
    mFileTreeView->showRow(filepath);
}

void GVLE::addView(vpz::BaseModel* model)
{
    if (model) {
        if (model->isCoupled()) {
            vpz::CoupledModel* m = vpz::BaseModel::toCoupled(model);
            addView(m);
        } else if (model->isAtomic()) {
            try {
                mAtomicBox->show((vpz::AtomicModel*)model);
            } catch (utils::SaxParserError& e) {
                gvle::Error((fmt(_("Error showing model, %1%")) %
                             e.what()).str(),false);
            }
        }
    }
    refreshViews();
}

void GVLE::addView(vpz::CoupledModel* model)
{
    const size_t szView = mListView.size();

    View* search = findView(model);
    if (search != NULL) {
        search->selectedWindow();
    } else {
        mListView.push_back(new View(mModeling, model, szView, this));
    }
    getEditor()->openTabVpz(mModeling->getFileName(), model);
}

void GVLE::addViewClass(vpz::BaseModel* model, std::string name)
{
    assert(model);
    if (model->isCoupled()) {
        vpz::CoupledModel* m = (vpz::CoupledModel*)(model);
        addViewClass(m, name);
    } else if (model->isAtomic()) {
        try {
            mAtomicBox->show((vpz::AtomicModel*)model);
        } catch (utils::SaxParserError& e) {
            gvle::Error((fmt(_("Error showing model, %1%")) %
                         e.what()).str(),false);
        }
    }
    refreshViews();
}

void GVLE::addViewClass(vpz::CoupledModel* model, std::string name)
{
    const size_t szView = mListView.size();

    View* search = findView(model);
    if (search != NULL) {
        search->selectedWindow();
    } else {
        View* v = new View(mModeling, model, szView, this);
        v->setCurrentClass(name);
        mListView.push_back(v);
    }
    getEditor()->openTabVpz(getModeling()->getFileName(), model);
}

void GVLE::insertLog(const std::string& text)
{
    if (not text.empty()) {
        Glib::RefPtr < Gtk::TextBuffer > ref = mLog->get_buffer();
        if (ref) {
            ref->insert(ref->end(), text);
        }
    }
}

void GVLE::scrollLogToLastLine()
{
    Glib::RefPtr < Gtk::TextBuffer > ref = mLog->get_buffer();
    if (ref) {
        Glib::RefPtr < Gtk::TextMark > mark = ref->get_mark("end");
        if (mark) {
            ref->move_mark(mark, ref->end());
        } else {
            mark = ref->create_mark("end", ref->end());
        }

        mLog->scroll_to(mark, 0.0, 0.0, 1.0);
    }
}

void GVLE::start()
{
    mModeling->clearModeling();
    mModeling->delNames();
    mModeling->setTopModel(mModeling->newCoupledModel(0,
                                                      "Top model", "", 0, 0));
    setTitle(mModeling->getFileName());
    mModeling->vpz().project().model().setModel(mModeling->getTopModel());
    redrawModelTreeBox();
    redrawModelClassBox();
    if (mCurrPackage.name().empty()) {
        mModeling->setFileName("noname.vpz");
    } else {
        mModeling->setFileName(Glib::build_filename(
                                 mCurrPackage.getExpDir(vle::utils::PKG_SOURCE),
                                 "noname.vpz"));
    }
    mModeling->setSaved(false);
    mModeling->setModified(true);
    addView(mModeling->getTopModel());
    getEditor()->openTabVpz(mModeling->getFileName(), mModeling->getTopModel());
    setModifiedTitle(mModeling->getFileName());
}

void GVLE::start(const std::string& path, const std::string& fileName)
{
    mModeling->clearModeling();
    mModeling->delNames();
    mModeling->setTopModel(mModeling->newCoupledModel(0,
                                                      "Top model", "", 0, 0));
    setTitle(mModeling->getFileName());
    mModeling->vpz().project().model().setModel(mModeling->getTopModel());
    redrawModelTreeBox();
    redrawModelClassBox();
    mModeling->setFileName(path + "/" + fileName);
    mModeling->setSaved(false);
    mModeling->setModified(true);
    addView(mModeling->getTopModel());
    getEditor()->openTabVpz(mModeling->getFileName(), mModeling->getTopModel());
    setModifiedTitle(mModeling->getFileName());
}

void GVLE::parseXML(const std::string& filename)
{
    getEditor()->closeVpzTab();
    mModeling->parseXML(filename);
    mModeling->setFileName(filename);
    delViews();
    addView(mModeling->getTopModel());
    mModeling->setSaved(true);
    mModeling->setModified(false);
    setTitle(mModeling->getFileName());
}

bool GVLE::existView(vpz::CoupledModel* model)
{
    assert(model);
    ListView::const_iterator it = mListView.begin();
    while (it != mListView.end()) {
        if ((*it) && (*it)->getGCoupledModel() == model)
            return true;
        ++it;
    }
    return false;
}

View* GVLE::findView(vpz::CoupledModel* model)
{
    assert(model);
    ListView::const_iterator it = mListView.begin();
    while (it != mListView.end()) {
        if ((*it) && (*it)->getGCoupledModel() == model) {
            return *it;
        }
        ++it;
    }
    return NULL;
}

void GVLE::delViewIndex(size_t index)
{
    assert(index < mListView.size());
    delete mListView[index];
    mListView[index] = NULL;
}

void GVLE::delViewOnModel(const vpz::CoupledModel* cm)
{
    assert(cm);
    const size_t sz = mListView.size();
    for (size_t i = 0; i < sz; ++i) {
        if (mListView[i] != 0 and mListView[i]->getGCoupledModel() == cm)
            delViewIndex(i);
    }
}

void GVLE::delViews()
{
    const size_t sz = mListView.size();
    for (size_t i = 0; i < sz; ++i)
        delViewIndex(i);
}

void GVLE::refreshViews()
{
    ListView::iterator it = mListView.begin();
    while (it != mListView.end()) {
        if (*it) {
            (*it)->redraw();
        }
        ++it;
    }
}

void GVLE::redrawModelTreeBox()
{
    assert(mModeling->getTopModel());
    mModelTreeBox->parseModel(mModeling->getTopModel());
}

void GVLE::redrawModelClassBox()
{
    mModelClassBox->parseClass();
}

void GVLE::clearModelTreeBox()
{
    mModelTreeBox->clear();
}

void GVLE::clearModelClassBox()
{
    mModelClassBox->clear();
}

bool GVLE::on_delete_event(GdkEventAny* event)
{
    if (event->type == GDK_DELETE) {
        onQuit();
        return true;
    }
    return false;
}

void GVLE::onArrow()
{
    mCurrentButton = VLE_GVLE_POINTER;
    mEditor->getDocumentDrawingArea()->updateCursor();
    showMessage(_("Selection"));
}

void GVLE::onAddModels()
{
    mCurrentButton = VLE_GVLE_ADDMODEL;
    mEditor->getDocumentDrawingArea()->updateCursor();
    showMessage(_("Add models"));
}

void GVLE::onAddLinks()
{
    mCurrentButton = VLE_GVLE_ADDLINK;
    mEditor->getDocumentDrawingArea()->updateCursor();
    showMessage(_("Add links"));
}

void GVLE::onDelete()
{
    mCurrentButton = VLE_GVLE_DELETE;
    mEditor->getDocumentDrawingArea()->updateCursor();
    showMessage(_("Delete object"));
}

void GVLE::onAddCoupled()
{
    mCurrentButton = VLE_GVLE_ADDCOUPLED;
    mEditor->getDocumentDrawingArea()->updateCursor();
    showMessage(_("Coupled Model"));
}

void GVLE::onZoom()
{
    mCurrentButton = VLE_GVLE_ZOOM;
    mEditor->getDocumentDrawingArea()->updateCursor();
    showMessage(_("Zoom"));
}

void GVLE::onQuestion()
{
    mCurrentButton = VLE_GVLE_QUESTION;
    mEditor->getDocumentDrawingArea()->updateCursor();
    showMessage(_("Question"));
}

void GVLE::onNewFile(const std::string& path, const std::string& fileName)
{
    mEditor->createBlankNewFile(path, fileName);
    mMenuAndToolbar->onOpenFile();
    mMenuAndToolbar->showSave();
}

void GVLE::onNewVpz()
{
    if (not mModeling->isModified() or mModeling->getFileName().empty() or
        (mModeling->isModified() and
         gvle::Question(_("Do you really want load a new Model ?\nCurrent "
                          "model will be destroy and not save")))) {
        getEditor()->closeVpzTab();
        start();
        redrawModelTreeBox();
        redrawModelClassBox();
        mMenuAndToolbar->onOpenVpz();
        mMenuAndToolbar->showSave();
        mEditor->getDocumentDrawingArea()->updateCursor();
        mModelTreeBox->set_sensitive(true);
        mModelClassBox->set_sensitive(true);
        if (mCurrentButton == VLE_GVLE_POINTER){
            showMessage(_("Selection"));
        }
    }
    onExperimentsBox();
}

void GVLE::onNewNamedVpz(const std::string& path, const std::string& filename)
{
    if (not mModeling->isModified() or mModeling->getFileName().empty() or
        (mModeling->isModified() and
         gvle::Question(_("Do you really want load a new Model ?\nCurrent "
                          "model will be destroy and not save")))) {
        getEditor()->closeVpzTab();
        start(path.c_str(), filename.c_str());
        redrawModelTreeBox();
        redrawModelClassBox();
        mMenuAndToolbar->onOpenVpz();
        mMenuAndToolbar->showSave();
        mEditor->getDocumentDrawingArea()->updateCursor();
        mModelTreeBox->set_sensitive(true);
        mModelClassBox->set_sensitive(true);
        if (mCurrentButton == VLE_GVLE_POINTER){
            showMessage(_("Selection"));
        }
    }
    onExperimentsBox();
}

void GVLE::onNewProject()
{
    Gtk::FileChooserDialog box(*this, "New project",
                               Gtk::FILE_CHOOSER_ACTION_SAVE);
    box.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    box.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_ACCEPT);
    box.set_response_sensitive(Gtk::RESPONSE_ACCEPT, true);
    box.set_default_response(Gtk::RESPONSE_ACCEPT);
    box.set_create_folders(true);

    int result = box.run();
    switch (result) {
    case Gtk::RESPONSE_ACCEPT:
        {
            std::string dirname = Glib::path_get_dirname(box.get_filename());
            std::string basename = Glib::path_get_basename(box.get_filename());
            if (not basename.empty()) {
                if (g_chdir(dirname.c_str()) == 0) {
                    getEditor()->closeAllTab();
                    mModeling->clearModeling();
                    setTitle(mModeling->getFileName());
                    currentPackage().select(basename);
                    pluginFactory().update();
                    currentPackage().create();
                    buildPackageHierarchy();
                    getMenu()->onOpenProject();
                }
            }
        }
        break;
    default:
        if (not mCurrPackage.existsSource()) {
            onCloseProject();
        }
        break;
    }
    mMenuAndToolbar->onOpenProject();
    clearModelTreeBox();
    clearModelClassBox();
    setTitle("");
    mFileTreeView->set_sensitive(true);
    onTrouble();
}

void GVLE::onOpenProject()
{
    Gtk::FileChooserDialog box(*this, "Open project",
                               Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER);
    box.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    box.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_ACCEPT);
    box.set_response_sensitive(Gtk::RESPONSE_ACCEPT, true);
    box.set_default_response(Gtk::RESPONSE_ACCEPT);
    box.set_create_folders(true);

    int result = box.run();
    switch (result) {
    case Gtk::RESPONSE_ACCEPT:
        {
            std::string dirname = Glib::path_get_dirname(box.get_filename());
            std::string basename = Glib::path_get_basename(box.get_filename());

            if (not basename.empty()) {
                if (g_chdir(dirname.c_str()) == 0) {
                    onCloseProject();
                    mCurrPackage.select(basename);
                    mPluginFactory.update();
                    buildPackageHierarchy();
                    mMenuAndToolbar->onOpenProject();
                    setTitle("");
                    mFileTreeView->set_sensitive(true);
                    onTrouble();
                }
            }
        }
        break;
    default:
        if (not mCurrPackage.existsSource()) {
            onCloseProject();
        }
        break;
    }
}

void GVLE::onOpenVpz()
{
    if (not mModeling->isModified() or mModeling->getFileName().empty() or
        (mModeling->isModified() and
         gvle::Question(_("Do you really want load a new Model ?\nCurrent "
                          "model will be destroy and not save")))) {

        Gtk::FileChooserDialog file(_("VPZ file"), Gtk::FILE_CHOOSER_ACTION_OPEN);

        file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
        file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
        Glib::RefPtr<Gtk::FileFilter> filter = Gtk::FileFilter::create ();
        filter->set_name(_("Vle Project gZipped"));
        filter->add_pattern("*.vpz");
        file.add_filter(filter);
        std::string expDir = currentPackage().getExpDir(vle::utils::PKG_SOURCE);
        file.set_current_folder(expDir);

        int response = file.run();
        file.hide();

        if (response == Gtk::RESPONSE_OK) {
            std::string vpz_file = file.get_filename();

            parseXML(vpz_file);
            getEditor()->openTabVpz(mModeling->getFileName(),
                                    mModeling->getTopModel());

            redrawModelTreeBox();
            redrawModelClassBox();
            mMenuAndToolbar->onOpenVpz();
            mModelTreeBox->set_sensitive(true);
            mModelClassBox->set_sensitive(true);
            mEditor->getDocumentDrawingArea()->updateCursor();
            if (mCurrentButton == VLE_GVLE_POINTER){
                showMessage(_("Selection"));
            }
        }
    }
}

void GVLE::onRefresh()
{
    mFileTreeView->refresh();
}

void GVLE::onShowCompleteView()
{
    DocumentDrawingArea* tab = dynamic_cast<DocumentDrawingArea*>(
        mEditor->get_nth_page(mCurrentTab));
    vpz::CoupledModel* currentModel;
    if (tab-> isComplete()) {
        currentModel = tab->getCompleteDrawingArea()->getModel();
    } else {
        currentModel = tab->getSimpleDrawingArea()->getModel();
    }
    mEditor->showCompleteView(mModeling->getFileName(),currentModel);
}

void GVLE::onShowSimpleView()
{
    DocumentDrawingArea* tab = dynamic_cast<DocumentDrawingArea*>(
        mEditor->get_nth_page(mCurrentTab));
    vpz::CoupledModel* currentModel;
    if (tab-> isComplete()) {
        currentModel = tab->getCompleteDrawingArea()->getModel();
    } else {
        currentModel = tab->getSimpleDrawingArea()->getModel();
    }
    mEditor->showSimpleView(mModeling->getFileName(), currentModel);
}

bool GVLE::checkVpz()
{
    std::vector<std::string> vec;
    mModeling->vpz_is_correct(vec);

    if (vec.size() != 0) {
        std::string error = _("Vpz incorrect :\n");
        std::vector<std::string>::const_iterator it = vec.begin();

        while (it != vec.end()) {
            error += *it + "\n";
            ++it;
        }
        Error(error);
        return false;
    }
    return true;
}

void GVLE::onSave()
{
    int page = mEditor->get_current_page();

    if (page != -1) {
        if (dynamic_cast < Document* >(mEditor->get_nth_page(page))
            ->isDrawingArea()) {
            saveVpz();
        } else {
            DocumentText* doc = dynamic_cast < DocumentText* >(
                mEditor->get_nth_page(page));

            if (not doc->isNew() || doc->hasFullName()) {
                doc->save();
            } else {
                Gtk::FileChooserDialog file(_("Text file"),
                                            Gtk::FILE_CHOOSER_ACTION_SAVE);

                file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
                file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
                file.set_current_folder(mCurrPackage.getSrcDir(
                        vle::utils::PKG_SOURCE));
                if (file.run() == Gtk::RESPONSE_OK) {
                    std::string filename(file.get_filename());

                    doc->saveAs(filename);
                    mModeling->setFileName(filename);
                }
            }
            refreshPackageHierarchy();
        }
    }
}

void GVLE::onSaveAs()
{
    int page = mEditor->get_current_page();

    Glib::ustring title;

    bool isVPZ = dynamic_cast < Document* >(mEditor->get_nth_page(page))
        ->isDrawingArea();

    if (page != -1) {
        if (!checkVpz()) {
            return;
        }
        if (isVPZ) {
            title = _("VPZ file");
        } else {
            title = _("Text file");
        }

        Gtk::FileChooserDialog file(title,
                                    Gtk::FILE_CHOOSER_ACTION_SAVE);

        file.set_transient_for(*this);
        file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
        file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

        if (isVPZ) {
            Glib::RefPtr<Gtk::FileFilter> filter = Gtk::FileFilter::create ();
            filter->set_name(_("Vle Project gZipped"));
            filter->add_pattern("*.vpz");
            file.add_filter(filter);
        }

        // to provide a default filename
        // but also a default location
        file.set_filename(dynamic_cast < Document* >(mEditor->
                                                     get_nth_page(page))
                          ->filepath());

        if (file.run() == Gtk::RESPONSE_OK) {
            std::string filename(file.get_filename());

            if (isVPZ) {
                vpz::Vpz::fixExtension(filename);

                Editor::Documents::const_iterator it =
                    mEditor->getDocuments().find(mModeling->getFileName());

                mModeling->saveXML(filename);
                setTitle(mModeling->getFileName());

                if (it != mEditor->getDocuments().end()) {
                    it->second->setTitle(filename,
                                         mModeling->getTopModel(), false);
                }
            } else {
                DocumentText* doc = dynamic_cast < DocumentText* >(
                    mEditor->get_nth_page(page));

                doc->saveAs(filename);
            }
            refreshPackageHierarchy();
        }
    }
}

void GVLE::fixSave()
{
    if (not mEditor->getDocuments().empty()) {
        int page = mEditor->get_current_page();
        Gtk::Widget* tab = mEditor->get_nth_page(page);
        bool found = false;
        Editor::Documents::const_iterator it =
            mEditor->getDocuments().begin();

        while (not found and it != mEditor->getDocuments().end()) {
            if (it->second == tab) {
                found = true;
            } else {
                ++it;
            }
        }
        if (it->second->isModified()) {
            mMenuAndToolbar->showSave();
        } else {
            mMenuAndToolbar->hideSave();
        }
    }
}

bool GVLE::closeTab(const std::string& filepath)
{
    bool vpz = false;
    bool close = false;
    Editor::Documents::const_iterator it =
        mEditor->getDocuments().find(filepath);

    if (it != mEditor->getDocuments().end()) {
        if (not it->second->isModified() or
            gvle::Question(_("The current tab is not saved\n"
                             "Do you really want to close this file ?"))) {
            if (it->second->isDrawingArea()) {
                clearModelTreeBox();
                mEditor->closeVpzTab();
                mModeling->clearModeling();
                setTitle(mModeling->getFileName());
                clearModelClassBox();
                mModelTreeBox->set_sensitive(false);
                mModelClassBox->set_sensitive(false);
                vpz = true;
                close = true;
            } else {
                mEditor->closeTab(it->first);
            }
            mMenuAndToolbar->onCloseTab(vpz, mEditor->getDocuments().empty());
            fixSave();
            updateTitle();
        }
    }
    return close;
}

void GVLE::onCloseTab()
{
    bool vpz = false;
    int page = mEditor->get_current_page();
    Gtk::Widget* tab = mEditor->get_nth_page(page);
    bool found = false;
    Editor::Documents::const_iterator it = mEditor->getDocuments().begin();

    while (not found and it != mEditor->getDocuments().end()) {
        if (it->second == tab) {
            found = true;
        } else {
            ++it;
        }
    }
    if (found) {
        if (not it->second->isModified() or
            gvle::Question(_("The current tab is not saved\n"
                             "Do you really want to close this file ?"))) {
            if (it->second->isDrawingArea()) {
                clearModelTreeBox();
                mModeling->clearModeling();
                getEditor()->closeVpzTab();
                setTitle(mModeling->getFileName());
                clearModelClassBox();
                mModelTreeBox->set_sensitive(false);
                mModelClassBox->set_sensitive(false);
                vpz = true;
            }
            mEditor->closeTab(it->first);
            mMenuAndToolbar->onCloseTab(vpz, mEditor->getDocuments().empty());
            fixSave();
            updateTitle();
        }
    }
}

void GVLE::onCloseProject()
{
    clearModelTreeBox();
    mEditor->closeAllTab();
    mModeling->clearModeling();
    setTitle(mModeling->getFileName());
    clearModelClassBox();
    mModelTreeBox->set_sensitive(false);
    mModelClassBox->set_sensitive(false);
    mCurrPackage.select("");
    mPluginFactory.update();
    buildPackageHierarchy();
    mMenuAndToolbar->showMinimalMenu();
    setTitle("");
    mFileTreeView->set_sensitive(false);

}

void GVLE::onQuit()
{
    mQuitBox->show();
}

void GVLE::onPreferences()
{
    if (mPreferencesBox->run() == Gtk::RESPONSE_OK) {
        refreshViews();
        mEditor->refreshViews();
    }
}

void GVLE::onSimulationBox()
{
    if (mModeling->isSaved()) {
        LaunchSimulationBox box(mRefXML, ((const Modeling*)mModeling)->vpz(),
                mCurrPackage);
        box.run();
        refreshPackageHierarchy();
    } else {
        gvle::Error(_("Save or load a project before simulation"));
    }
}

void GVLE::onExperimentsBox()
{
    ExperimentBox box(mRefXML, mModeling);
    box.run();
}

void GVLE::onConditionsBox()
{
    const Modeling* modeling = (const Modeling*)mModeling;

    if (runConditionsBox(modeling->conditions()) == 1) {

        renameList tmpRename= applyConditionsBox(mModeling->conditions());

        renameList::const_iterator it = tmpRename.begin();

        while (it != tmpRename.end()) {

            mModeling->vpz().project().model().updateConditions(it->first,
                                                                it->second);
            mModeling->vpz().project().classes().updateConditions(it->first,
                                                                  it->second);
            ++it;
        }

        applyRemoved();
    }
}

void GVLE::applyRemoved()
{
    std::set < std::string > conditions = mModeling->conditions().getKeys();

    mModeling->vpz().project().model().purgeConditions(conditions);
    mModeling->vpz().project().classes().purgeConditions(conditions);
}

int GVLE::runConditionsBox(const vpz::Conditions& conditions)
{
    return mConditionsBox->run(conditions);
}

renameList GVLE::applyConditionsBox(vpz::Conditions& conditions)
{
    return mConditionsBox->apply(conditions);
}

void GVLE::onViewOutputBox()
{
    const Modeling* modeling((const Modeling*)mModeling);
    vpz::Views views(modeling->views());
    ViewOutputBox box(*mModeling, this, mRefXML, views);
    box.run();
}

void GVLE::onDynamicsBox()
{
    const Modeling* modeling((const Modeling*)mModeling);
    vpz::Dynamics dynamics(modeling->dynamics());
    DynamicsBox box(*mModeling, mPluginFactory, mRefXML, dynamics);
    box.run();
}

void GVLE::onShowAbout()
{
    About box(mRefXML);
    box.run();
}

void GVLE::saveVpz()
{
    if (!checkVpz()){
        return;
    }
    if (mModeling->isSaved()) {
        Editor::Documents::const_iterator it =
            mEditor->getDocuments().find(mModeling->getFileName());

        mModeling->saveXML(mModeling->getFileName());
        mModeling->setModified(false);
        setTitle(mModeling->getFileName());
        if (it != mEditor->getDocuments().end()) {
            it->second->setTitle(mModeling->getFileName(),
                                 mModeling->getTopModel(), false);
        }
    } else {
        saveFirstVpz();
        refreshPackageHierarchy();
    }
}

void GVLE::saveFirstVpz()
{
    if (not mCurrPackage.name().empty()) {
        mSaveVpzBox->show();
    } else {
        Gtk::FileChooserDialog file(_("VPZ file"),
                                    Gtk::FILE_CHOOSER_ACTION_SAVE);
        file.set_transient_for(*this);
        file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
        file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
        Glib::RefPtr<Gtk::FileFilter> filter = Gtk::FileFilter::create ();
        filter->set_name(_("Vle Project gZipped"));
        filter->add_pattern("*.vpz");
        file.add_filter(filter);

        if (file.run() == Gtk::RESPONSE_OK) {
            std::string filename(file.get_filename());
            vpz::Vpz::fixExtension(filename);
            Editor::Documents::const_iterator it =
                mEditor->getDocuments().find(mModeling->getFileName());
            mModeling->saveXML(filename);
            setTitle(mModeling->getFileName());
            if (it != mEditor->getDocuments().end()) {
                it->second->setTitle(filename,
                                     mModeling->getTopModel(), false);
            }
        }
    }
}

void GVLE::setTitle(const Glib::ustring& name)
{
    Glib::ustring title(windowTitle());

    if (not mCurrPackage.name().empty()) {
        title += " - " + mCurrPackage.name();
    }

    if (not name.empty()) {
        title += " - " + Glib::path_get_basename(name);
    }
    set_title(title);
}

void GVLE::updateTitle()
{
    if (not mEditor->getDocuments().empty()) {
        int page = mEditor->get_current_page();
        Gtk::Widget* tab = mEditor->get_nth_page(page);
        bool found = false;
        Editor::Documents::const_iterator it =
            mEditor->getDocuments().begin();

        while (not found and it != mEditor->getDocuments().end()) {
            if (it->second == tab) { found = true; } else { ++it; }
        }
        if (found) {
            setTitle(utils::Path::basename(it->second->filename()) +
                     utils::Path::extension(it->second->filename()));
        }
    }
}

std::string valuetype_to_string(value::Value::type type)
{
    switch (type) {
    case(value::Value::BOOLEAN):
        return "boolean";
        break;
    case(value::Value::INTEGER):
        return "integer";
        break;
    case(value::Value::DOUBLE):
        return "double";
        break;
    case(value::Value::STRING):
        return "string";
        break;
    case(value::Value::SET):
        return "set";
        break;
    case(value::Value::MAP):
        return "map";
        break;
    case(value::Value::TUPLE):
        return "tuple";
        break;
    case(value::Value::TABLE):
        return "table";
        break;
    case(value::Value::XMLTYPE):
        return "xml";
        break;
    case(value::Value::NIL):
        return "null";
        break;
    case(value::Value::MATRIX):
        return "matrix";
        break;
    default:
        return "(no value)";
        break;
    }
}

bool GVLE::packageAllTimer()
{
    std::string o, e;

    if (mCurrPackage.get(&o, &e)) {
        insertLog(o);
        insertLog(e);
        scrollLogToLastLine();
    }

    if (mCurrPackage.isFinish()) {
        ++itDependencies;

        if (itDependencies != mDependencies.end()) {
            mCurrPackage.select(*itDependencies);
            buildAllProject();
        } else {
            insertLog("package " +
                      mCurrPackage.name() +
                      " & first level dependencies built\n");
            getMenu()->showProjectMenu();
        }
        scrollLogToLastLine();
        return false;
    } else {
        return true;
    }
}

bool GVLE::packageTimer()
{
    std::string o, e;

    if (mCurrPackage.get(&o, &e)) {
        insertLog(o);
        insertLog(e);
        scrollLogToLastLine();
    }

    if (mCurrPackage.isFinish()) {
        getMenu()->showProjectMenu();
        return false;
    } else {
        return true;
    }
}

bool GVLE::packageConfigureAllTimer()
{
    std::string o, e;

    if (mCurrPackage.get(&o, &e)) {
        insertLog(o);
        insertLog(e);
        scrollLogToLastLine();
    }

    if (mCurrPackage.isFinish()) {
        if (mCurrPackage.isSuccess()) {
            buildAllProject();
        } else {
            getMenu()->showProjectMenu();
        }
        scrollLogToLastLine();
        return false;
    } else {
        return true;
    }
}

bool GVLE::packageBuildAllTimer()
{
    std::string o, e;

    if (mCurrPackage.get(&o, &e)) {
        insertLog(o);
        insertLog(e);
        scrollLogToLastLine();
    }

    if (mCurrPackage.isFinish()) {
        if (mCurrPackage.isSuccess()) {
            installAllProject();
        } else {
            getMenu()->showProjectMenu();
        }
        scrollLogToLastLine();
        return false;
    } else {
        return true;
    }
}

bool GVLE::packageBuildTimer()
{
    std::string o, e;

    if (mCurrPackage.get(&o, &e)) {
        insertLog(o);
        insertLog(e);
        scrollLogToLastLine();
    }

    if (mCurrPackage.isFinish()) {
        if (mCurrPackage.get(&o, &e)) {
            insertLog(o);
            insertLog(e);
            scrollLogToLastLine();
        }

        if (mCurrPackage.isSuccess()) {
            installProject();
        } else {
            getMenu()->showProjectMenu();
        }
        scrollLogToLastLine();
        return false;
    } else {
        return true;
    }
}

void GVLE::configureToSimulate()
{
    if (Settings::settings().getAutoBuild()) {
        insertLog("configure package\n");
        getMenu()->hideProjectMenu();
        getMenu()->hideSimulationMenu();

        try {
            mCurrPackage.configure();
        } catch (const std::exception& e) {
            getMenu()->showProjectMenu();
            getMenu()->showSimulationMenu();
            gvle::Error(e.what());
            return;
        } catch (const Glib::Exception& e) {
            getMenu()->showProjectMenu();
            getMenu()->showSimulationMenu();
            gvle::Error(e.what());
            return;
        }
        Glib::signal_timeout().connect(
            sigc::mem_fun(*this, &GVLE::configureToSimulateTimer), 250);
    } else {
        onSimulationBox();
    }
}

bool GVLE::configureToSimulateTimer()
{
    std::string o, e;

    if (mCurrPackage.get(&o, &e)) {
        insertLog(o);
        insertLog(e);
        scrollLogToLastLine();
    }

    if (mCurrPackage.isFinish()) {
        if (mCurrPackage.get(&o, &e)) {
            insertLog(o);
            insertLog(e);
            scrollLogToLastLine();
        }

        if (mCurrPackage.isSuccess()) {
            buildToSimulate();
        } else {
            getMenu()->showProjectMenu();
            getMenu()->showSimulationMenu();
        }
        scrollLogToLastLine();
        return false;
    } else {
        return true;
    }
}

void GVLE::buildToSimulate()
{
    insertLog("build package\n");
    getMenu()->hideProjectMenu();
    try {
        mCurrPackage.build();
    } catch (const std::exception& e) {
        getMenu()->showProjectMenu();
        getMenu()->showSimulationMenu();
        gvle::Error(e.what());
        return;
    } catch (const Glib::Exception& e) {
        getMenu()->showProjectMenu();
        getMenu()->showSimulationMenu();
        gvle::Error(e.what());
        return;
    }
    Glib::signal_timeout().connect(
        sigc::mem_fun(*this, &GVLE::buildToSimulateTimer), 250);
}

bool GVLE::buildToSimulateTimer()
{
    std::string o, e;

    if (mCurrPackage.get(&o, &e)) {
        insertLog(o);
        insertLog(e);
        scrollLogToLastLine();
    }

    if (mCurrPackage.isFinish()) {
        if (mCurrPackage.get(&o, &e)) {
            insertLog(o);
            insertLog(e);
            scrollLogToLastLine();
        }

        if (mCurrPackage.isSuccess()) {
            installtoSimulate();
        } else {
            getMenu()->showProjectMenu();
            getMenu()->showSimulationMenu();
        }
        scrollLogToLastLine();
        return false;
    } else {
        return true;
    }
}
void GVLE::installtoSimulate()
{
    insertLog("install package\n");
    getMenu()->hideProjectMenu();
    try {
        mCurrPackage.install();
    } catch (const std::exception& e) {
        getMenu()->showProjectMenu();
        getMenu()->showSimulationMenu();
        gvle::Error(e.what());
        return;
    } catch (const Glib::Exception& e) {
        getMenu()->showProjectMenu();
        getMenu()->showSimulationMenu();
        gvle::Error(e.what());
        return;
    }
    Glib::signal_timeout().connect(
        sigc::mem_fun(*this, &GVLE::installtoSimulateTimer), 250);
}

bool GVLE::installtoSimulateTimer()
{
    std::string o, e;

    if (mCurrPackage.get(&o, &e)) {
        insertLog(o);
        insertLog(e);
        scrollLogToLastLine();
    }

    if (mCurrPackage.isFinish()) {
        onSimulationBox();
        getMenu()->showProjectMenu();
        getMenu()->showSimulationMenu();
        return false;
    } else {
        return true;
    }
}

void GVLE::configureProject()
{
    insertLog("configure package\n");
    getMenu()->hideProjectMenu();
    try {
        mCurrPackage.configure();
    } catch (const std::exception& e) {
        getMenu()->showProjectMenu();
        gvle::Error(e.what());
        return;
    } catch (const Glib::Exception& e) {
        getMenu()->showProjectMenu();
        gvle::Error(e.what());
        return;
    }
    Glib::signal_timeout().connect(
        sigc::mem_fun(*this, &GVLE::packageTimer), 250);
}

void GVLE::buildAllProject()
{
    insertLog("build package " + mCurrPackage.name() + "\n");
    try {
        mCurrPackage.build();
    } catch (const std::exception& e) {
        getMenu()->showProjectMenu();
        gvle::Error(e.what());
        return;
    } catch (const Glib::Exception& e) {
        getMenu()->showProjectMenu();
        gvle::Error(e.what());
        return;
    }
    Glib::signal_timeout().connect(
        sigc::mem_fun(*this, &GVLE::packageBuildAllTimer), 250);
}

void GVLE::buildProject()
{
    insertLog("build package\n");
    getMenu()->hideProjectMenu();
    try {
        mCurrPackage.build();
    } catch (const std::exception& e) {
        getMenu()->showProjectMenu();
        gvle::Error(e.what());
        return;
    } catch (const Glib::Exception& e) {
        getMenu()->showProjectMenu();
        gvle::Error(e.what());
        return;
    }
    Glib::signal_timeout().connect(
        sigc::mem_fun(*this, &GVLE::packageBuildTimer), 250);
}

std::map < std::string, Depends > GVLE::depends()
{
    std::map < std::string, Depends > result;

    utils::PathList vpz(mCurrPackage.getExperiments(vle::utils::PKG_BINARY));
    std::sort(vpz.begin(), vpz.end());

    for (utils::PathList::iterator it = vpz.begin(); it != vpz.end(); ++it) {
        std::set < std::string > depends;
        try {
            vpz::Vpz vpz(*it);
            depends = vpz.depends();
        } catch (const std::exception& /*e*/) {
        }
        result[*it] = depends;
    }

    return result;
}

void GVLE::displayDependencies()
{
    std::string dependsbuffer;

    AllDepends deps = depends();

    for (AllDepends::const_iterator it = deps.begin(); it != deps.end(); ++it) {
        if (it->second.empty()) {
            dependsbuffer += "<b>" + utils::Path::basename(it->first) +
                "</b> : -\n";
        } else {
            dependsbuffer += "<b>" + utils::Path::basename(it->first) +
                "</b> : ";

            Depends::const_iterator jt = it->second.begin();
            while (jt != it->second.end()) {
                Depends::const_iterator kt = jt++;
                dependsbuffer += *kt;
                if (jt != it->second.end()) {
                    dependsbuffer += ", ";
                } else {
                    dependsbuffer += '\n';
                }
            }
        }
    }

    const std::string title =
        utils::Path::filename(mCurrPackage.name()) +
        _(" - Package Dependencies");

    Gtk::MessageDialog* box;

    box = new Gtk::MessageDialog(dependsbuffer, true, Gtk::MESSAGE_INFO,
                                 Gtk::BUTTONS_OK, true);
    box->set_title(title);

    box->run();
    delete box;
}

void GVLE::testProject()
{
    insertLog("test package\n");
    getMenu()->hideProjectMenu();
    try {
        mCurrPackage.test();
    } catch (const std::exception& e) {
        getMenu()->showProjectMenu();
        gvle::Error(e.what());
        return;
    } catch (const Glib::Exception& e) {
        getMenu()->showProjectMenu();
        gvle::Error(e.what());
        return;
    }
    Glib::signal_timeout().connect(
        sigc::mem_fun(*this, &GVLE::packageTimer), 250);
}


void GVLE::installAllProject()
{
    insertLog("install package : " +
            mCurrPackage.name() +
              "\n");
    try {
        mCurrPackage.install();
    } catch (const std::exception& e) {
        getMenu()->showProjectMenu();
        gvle::Error(e.what());
        return;
    } catch (const Glib::Exception& e) {
        getMenu()->showProjectMenu();
        gvle::Error(e.what());
        return;
    }
    Glib::signal_timeout().connect(
        sigc::mem_fun(*this, &GVLE::packageAllTimer), 250);
}


void GVLE::installProject()
{
    insertLog("install package\n");
    getMenu()->hideProjectMenu();
    try {
        mCurrPackage.install();
    } catch (const std::exception& e) {
        getMenu()->showProjectMenu();
        gvle::Error(e.what());
        return;
    } catch (const Glib::Exception& e) {
        getMenu()->showProjectMenu();
        gvle::Error(e.what());
        return;
    }
    Glib::signal_timeout().connect(
        sigc::mem_fun(*this, &GVLE::packageTimer), 250);
}

void GVLE::cleanProject()
{
    insertLog("clean package\n");
    getMenu()->hideProjectMenu();
    try {
        mCurrPackage.clean();
    } catch (const std::exception& e) {
        getMenu()->showProjectMenu();
        gvle::Error(e.what());
        return;
    } catch (const Glib::Exception& e) {
        getMenu()->showProjectMenu();
        gvle::Error(e.what());
        return;
    }
    Glib::signal_timeout().connect(
        sigc::mem_fun(*this, &GVLE::packageTimer), 250);
}
void GVLE::removeProject()
{
    insertLog("remove the binary package\n");
    getMenu()->hideProjectMenu();
    try {
        mCurrPackage.rclean();
    } catch (const std::exception& e) {
        getMenu()->showProjectMenu();
        gvle::Error(e.what());
        return;
    } catch (const Glib::Exception& e) {
        getMenu()->showProjectMenu();
        gvle::Error(e.what());
        return;
    }
    Glib::signal_timeout().connect(
        sigc::mem_fun(*this, &GVLE::packageTimer), 250);
}

void GVLE::packageProject()
{
    insertLog("make source and binary packages\n");
    getMenu()->hideProjectMenu();
    try {
        mCurrPackage.pack();
    } catch (const std::exception& e) {
        getMenu()->showProjectMenu();
        gvle::Error(e.what());
        return;
    } catch (const Glib::Exception& e) {
        getMenu()->showProjectMenu();
        gvle::Error(e.what());
        return;
    }
    Glib::signal_timeout().connect(
        sigc::mem_fun(*this, &GVLE::packageTimer), 250);
}

void GVLE::onCutModel()
{
    if (mCurrentTab >= 0) {
        if (dynamic_cast<Document*>(mEditor->get_nth_page(mCurrentTab))
            ->isDrawingArea()) {
            View* currentView = dynamic_cast<DocumentDrawingArea*>(
                mEditor->get_nth_page(mCurrentTab))->getView();

            if (currentView) {
                currentView->onCutModel();
                mMenuAndToolbar->showPaste();
            }
        } else {
            DocumentText* doc = dynamic_cast<DocumentText*>(
                mEditor->get_nth_page(mCurrentTab));

            if (doc) {
                doc->cut();
            }
            mMenuAndToolbar->showPaste();
        }
    }
}

void GVLE::onCopyModel()
{
    if (mCurrentTab >= 0) {
        if (dynamic_cast<Document*>(mEditor->get_nth_page(mCurrentTab))
            ->isDrawingArea()) {
            View* currentView = dynamic_cast<DocumentDrawingArea*>(
                mEditor->get_nth_page(mCurrentTab))->getView();

            if (currentView) {
                currentView->onCopyModel();
                mMenuAndToolbar->showPaste();
            }
        } else {
            DocumentText* doc = dynamic_cast<DocumentText*>(
                mEditor->get_nth_page(mCurrentTab));

            if (doc) {
                doc->copy();
            }
            mMenuAndToolbar->showPaste();
        }
    }
}

void GVLE::onPasteModel()
{
    if (mCurrentTab >= 0) {
        if (dynamic_cast<Document*>(mEditor->get_nth_page(mCurrentTab))
            ->isDrawingArea()) {
            View* currentView = dynamic_cast<DocumentDrawingArea*>(
                mEditor->get_nth_page(mCurrentTab))->getView();

            if (currentView) {
                currentView->onPasteModel();
            }
        } else {
            DocumentText* doc = dynamic_cast<DocumentText*>(
                mEditor->get_nth_page(mCurrentTab));

            if (doc) {
                doc->paste();
            }
        }
    }
}

void GVLE::onSelectAll()
{
    if (mCurrentTab >= 0) {
        if (dynamic_cast<Document*>(mEditor->get_nth_page(mCurrentTab))
            ->isDrawingArea()) {
            View* currentView = dynamic_cast<DocumentDrawingArea*>(
                mEditor->get_nth_page(mCurrentTab))->getView();

            if (currentView) {
                vpz::CoupledModel* cModel = currentView->getGCoupledModel();
                currentView->onSelectAll(cModel);
                mMenuAndToolbar->showCopyCut();
            }
        } else {
            DocumentText* doc = dynamic_cast<DocumentText*>(
                mEditor->get_nth_page(mCurrentTab));

            if (doc) {
                doc->selectAll();
                mMenuAndToolbar->showCopyCut();
            }
        }
    }
}

void GVLE::cut(vpz::ModelList& lst, vpz::CoupledModel* gc,
               std::string className)
{
    if (className.empty()) {
        mCutCopyPaste.cut(lst, gc);
    } else {
        mCutCopyPaste.cut(lst, gc);
    }
}

void GVLE::copy(vpz::ModelList& lst, vpz::CoupledModel* gc,
                std::string className)
{
    // the current view is not a class
    if (className.empty()) {
        // no model is selected in current view and a class is selected
        // -> class instantiation
        if (lst.empty() and not mModeling->getSelectedClass().empty()) {
            vpz::Class& currentClass = mModeling->vpz().project().classes()
                .get(mModeling->getSelectedClass());
            vpz::BaseModel* model = currentClass.model();
            vpz::ModelList lst2;

            lst2[model->getName()] = model;
            mCutCopyPaste.copy(lst2, gc, true);
        } else {
            mCutCopyPaste.copy(lst, gc, false);
        }
    } else {
        mCutCopyPaste.copy(lst, gc, false);
    }
}

void GVLE::paste(vpz::CoupledModel* gc, std::string className)
{
    if (className.empty()) {
        mCutCopyPaste.paste(gc);
    } else {
        mCutCopyPaste.paste(gc);
    }
}

void GVLE::selectAll(vpz::ModelList& lst, vpz::CoupledModel* gc)
{
    lst = gc->getModelList();
}

bool GVLE::paste_is_empty() {
    return mCutCopyPaste.paste_is_empty();
}

void GVLE::setModified(bool modified)
{
    mModeling->setModified(modified);
}

void GVLE::clearCurrentModel()
{
    if (mCurrentTab >= 0) {
        View* currentView = dynamic_cast<DocumentDrawingArea*>(
            mEditor->get_nth_page(mCurrentTab))->getView();

        if (currentView) {
            currentView->clearCurrentModel();
        }
    }
}

void GVLE::importModel()
{
    Gtk::FileChooserDialog file(_("VPZ file"), Gtk::FILE_CHOOSER_ACTION_OPEN);
    file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
    Glib::RefPtr<Gtk::FileFilter> filter = Gtk::FileFilter::create ();
    filter->set_name(_("Vle Project gZipped"));
    filter->add_pattern("*.vpz");
    file.add_filter(filter);

    int response = file.run();
    file.hide();

    if (response == Gtk::RESPONSE_OK) {
        std::string project_file = file.get_filename();
        try {
            using namespace vpz;

            View* currentView = dynamic_cast<DocumentDrawingArea*>(
                mEditor->get_nth_page(mCurrentTab))->getView();
            vpz::Vpz* src = new vpz::Vpz(project_file);

            assert(currentView->getGCoupledModel());
            assert(src);

            if (mImportModelBox) {
                mImportModelBox->setGCoupled(currentView->getGCoupledModel());
                if (mImportModelBox->show(src)) {
                    vpz::BaseModel* import = src->project().model().model();
                    currentView->getGCoupledModel()->addModel(import);
                    mModeling->importModel(src);
                    redrawModelTreeBox();
                    refreshViews();
                }
            }
            delete src;
        } catch (std::exception& E) {
            Error(E.what());
        }
    }
}

void GVLE::importClasses(vpz::Vpz* src)
{
    using namespace vpz;
    assert(src);

    if (mImportClassesBox) {
        mImportClassesBox->show(src);
    }
}

void GVLE::exportCurrentModel()
{
    View* currentView = dynamic_cast<DocumentDrawingArea*>(
        mEditor->get_nth_page(mCurrentTab))->getView();
    currentView->exportCurrentModel();
}

void GVLE::EditCoupledModel(vpz::CoupledModel* model)
{
    assert(model);
    mCoupledBox->show(model);
}

void GVLE::exportGraphic()
{
    ViewDrawingArea* tab;
    if ( dynamic_cast<DocumentDrawingArea*>(
            mEditor->get_nth_page(mCurrentTab))->isComplete()) {
        tab = dynamic_cast<DocumentDrawingArea*>(
            mEditor->get_nth_page(mCurrentTab))->getCompleteDrawingArea();
    } else {
        tab = dynamic_cast<DocumentDrawingArea*>(
            mEditor->get_nth_page(mCurrentTab))->getSimpleDrawingArea();
    }


    const vpz::Experiment& experiment = ((const Modeling*)mModeling)
        ->vpz().project().experiment();
    if (experiment.name().empty() || experiment.duration() == 0) {
        Error(_("Fix a Value to the name and the duration "	\
                "of the experiment before exportation."));
        return;
    }

    Gtk::FileChooserDialog file(_("Image file"), Gtk::FILE_CHOOSER_ACTION_SAVE);
    file.set_transient_for(*this);
    file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
    Glib::RefPtr<Gtk::FileFilter> filterAuto = Gtk::FileFilter::create ();
    Glib::RefPtr<Gtk::FileFilter> filterPng = Gtk::FileFilter::create ();
    Glib::RefPtr<Gtk::FileFilter> filterPdf = Gtk::FileFilter::create ();
    Glib::RefPtr<Gtk::FileFilter> filterSvg = Gtk::FileFilter::create ();
    filterAuto->set_name(_("Guess type from file name"));
    filterAuto->add_pattern("*");
    filterPng->set_name(_("Portable Newtork Graphics (.png)"));
    filterPng->add_pattern("*.png");
    filterPdf->set_name(_("Portable Format Document (.pdf)"));
    filterPdf->add_pattern("*.pdf");
    filterSvg->set_name(_("Scalable Vector Graphics (.svg)"));
    filterSvg->add_pattern("*.svg");
    file.add_filter(filterAuto);
    file.add_filter(filterPng);
    file.add_filter(filterPdf);
    file.add_filter(filterSvg);


    if (file.run() == Gtk::RESPONSE_OK) {
        std::string filename(file.get_filename());
        std::string extension(file.get_filter()->get_name());

        if (extension == _("Guess type from file name")) {
            size_t ext_pos = filename.find_last_of('.');
            if (ext_pos != std::string::npos) {
                std::string type(filename, ext_pos+1);
                filename.resize(ext_pos);
                if (type == "png")
                    tab->exportPng(filename);
                else if (type == "pdf")
                    tab->exportPdf(filename);
                else if (type == "svg")
                    tab->exportSvg(filename);
                else
                    Error(_("Unsupported file format"));
            }
        }
        else if (extension == _("Portable Newtork Graphics (.png)"))
            tab->exportPng(filename);
        else if (extension == _("Portable Format Document (.pdf)"))
            tab->exportPdf(filename);
        else if (extension == _("Scalable Vector Graphics (.svg)"))
            tab->exportSvg(filename);
    }
}

void GVLE::addCoefZoom()
{
    ViewDrawingArea* tab;
    if ( dynamic_cast<DocumentDrawingArea*>(
            mEditor->get_nth_page(mCurrentTab))->isComplete()) {
        tab = dynamic_cast<DocumentDrawingArea*>(
            mEditor->get_nth_page(mCurrentTab))->getCompleteDrawingArea();
    } else {
        tab = dynamic_cast<DocumentDrawingArea*>(
            mEditor->get_nth_page(mCurrentTab))->getSimpleDrawingArea();
    }
    tab->addCoefZoom();
}

void GVLE::delCoefZoom()
{
    ViewDrawingArea* tab;
    if ( dynamic_cast<DocumentDrawingArea*>(
            mEditor->get_nth_page(mCurrentTab))->isComplete()) {
        tab = dynamic_cast<DocumentDrawingArea*>(
            mEditor->get_nth_page(mCurrentTab))->getCompleteDrawingArea();
    } else {
        tab = dynamic_cast<DocumentDrawingArea*>(
            mEditor->get_nth_page(mCurrentTab))->getSimpleDrawingArea();
    }
    tab->delCoefZoom();
}

void GVLE::setCoefZoom(double coef)
{
    ViewDrawingArea* tab;
    if ( dynamic_cast<DocumentDrawingArea*>(
            mEditor->get_nth_page(mCurrentTab))->isComplete()) {
        tab = dynamic_cast<DocumentDrawingArea*>(
            mEditor->get_nth_page(mCurrentTab))->getCompleteDrawingArea();
    } else {
        tab = dynamic_cast<DocumentDrawingArea*>(
            mEditor->get_nth_page(mCurrentTab))->getSimpleDrawingArea();
    }
    tab->setCoefZoom(coef);
}

void  GVLE::updateAdjustment(double h, double v)
{
    DocumentDrawingArea* tab = dynamic_cast<DocumentDrawingArea*>(
        mEditor->get_nth_page(mCurrentTab));
    tab->setHadjustment(h);
    tab->setVadjustment(v);
}

void GVLE::onOrder()
{
    ViewDrawingArea* tab;
    if ( dynamic_cast<DocumentDrawingArea*>(
            mEditor->get_nth_page(mCurrentTab))->isComplete()) {
        tab = dynamic_cast<DocumentDrawingArea*>(
            mEditor->get_nth_page(mCurrentTab))->getCompleteDrawingArea();
    } else {
        tab = dynamic_cast<DocumentDrawingArea*>(
            mEditor->get_nth_page(mCurrentTab))->getSimpleDrawingArea();
    }
    tab->onOrder();
    mModeling->setModified(true);
}

void GVLE::onTrouble()
{
    if (not mCurrPackage.name().empty()) {
        std::string package = mCurrPackage.name();
        boost::algorithm::to_lower(package);

        if (package == "canabis" and not mTroubleTimemout.connected()) {
            mTroubleTimemout = Glib::signal_timeout().connect(
                sigc::mem_fun(*this, &GVLE::signalTroubleTimer), 50);
        }
    }
}

bool GVLE::signalTroubleTimer()
{
    static int nb = 0;

    if (mCurrPackage.name().empty()) {
        return false;
    }

    std::string package = mCurrPackage.name();
    if (package.empty()) {
        return false;
    }

    boost::algorithm::to_lower(package);
    if (package == "canabis") {
        int x, y;

        get_position(x, y);

        switch (nb % 4) {
        case 0:
            x = x - 5;
            break;
        case 1:
            y = y + 5;
            break;
        case 2:
            x = x + 5;
            break;
        default:
            y = y - 5;
            break;
        }

        move(x, y);
        nb++;
        return true;
    }
    return false;
}

}} // namespace vle gvle
