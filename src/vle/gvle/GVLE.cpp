/**
 * @file vle/gvle/GVLE.cpp
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


#include <vle/gvle/GVLE.hpp>
#include <vle/gvle/About.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/ExecutionBox.hpp>
#include <vle/gvle/ExperimentBox.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/gvle/ExperimentBox.hpp>
#include <vle/gvle/HostsBox.hpp>
#include <vle/gvle/GVLEMenuAndToolbar.hpp>
#include <vle/gvle/PreferencesBox.hpp>
#include <vle/gvle/ViewOutputBox.hpp>
#include <vle/gvle/View.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Path.hpp>
#include <vle/vpz/Vpz.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <boost/filesystem.hpp>
#include <gtkmm/filechooserdialog.h>
#include <glibmm/miscutils.h>
#include <gtkmm/stock.h>

namespace vle { namespace gvle {

Document::Document(GVLE* gvle, const std::string& filepath) :
    Gtk::ScrolledWindow(),
    mGVLE(gvle)
{
    mModified = false;
    mFilePath = filepath;
    mFileName = boost::filesystem::basename(filepath);
}

Document::~Document()
{
}

void Document::setTitle(std::string title, graph::Model* model,
			bool modified)
{
    if (boost::filesystem::extension(title) == ".vpz") {
	mTitle = boost::filesystem::basename(title) +
	    boost::filesystem::extension(title) + " - " +
	    model->getName();
	if (modified) {
	    mTitle = "* " + mTitle;
	    mModified = true;
	} else {
	    mModified = false;
	}
	mGVLE->setModifiedTab(mTitle, mFilePath);
    }
}

DocumentText::DocumentText(GVLE* gvle,
			   const std::string& filepath, bool newfile) :
    Document(gvle, filepath),
    mNew(newfile)
{
    mTitle = filename() + boost::filesystem::extension(filepath);
#ifdef VLE_HAVE_GTKSOURCEVIEWMM
    gtksourceview::init();
#endif
    init();
    signal_event().connect(
	sigc::mem_fun(this, &DocumentText::event));
}

DocumentText::~DocumentText()
{
}

void DocumentText::save()
{
    try {
	std::ofstream file(filepath().c_str());
	file << mView.get_buffer()->get_text();
	file.close();
	mNew = mModified = false;
	mTitle = filename() + boost::filesystem::extension(filepath());
	mGVLE->setModifiedTab(mTitle, filepath());
    } catch(std::exception& e) {
	throw _("Error while saving file.");
    }
}

void DocumentText::saveAs(const std::string& filename)
{
    setFilePath(filename);
    save();
}

void DocumentText::init()
{
#ifdef VLE_HAVE_GTKSOURCEVIEWMM
    Glib::RefPtr<gtksourceview::SourceLanguageManager> manager
	= gtksourceview::SourceLanguageManager::create();
    Glib::RefPtr<gtksourceview::SourceLanguage> language =
	manager->get_language(getIdLanguage());
    Glib::RefPtr<gtksourceview::SourceBuffer> buffer =
	gtksourceview::SourceBuffer::create(language);
#else
    Glib::RefPtr<Gtk::TextBuffer> buffer = mView.get_buffer();
#endif
    if (not mNew) {
	std::ifstream file(filepath().c_str());
	if (file) {
	    std::stringstream filecontent;
	    filecontent << file.rdbuf();
	    file.close();
	    Glib::ustring buffer_content = filecontent.str();
	    if (not buffer_content.validate()) {
		throw utils::FileError(_("This file isn't UTF-8 valid. Please"
					 " convert this file with your source"
					 " code editor."));
	    }
#ifdef VLE_HAVE_GTKSOURCEVIEWMM
	    buffer->begin_not_undoable_action();
	    buffer->insert(buffer->end(), buffer_content);
	    buffer->end_not_undoable_action();
#else
	    buffer->insert(buffer->end(), buffer_content);
#endif
	} else {
	    throw std::string("cannot open: " + filename());
	}
    }
#ifdef VLE_HAVE_GTKSOURCEVIEWMM
    mView.set_source_buffer(buffer);
    applyEditingProperties();
#endif
    add(mView);
}

std::string DocumentText::getIdLanguage()
{
    std::string ext(boost::filesystem::extension(filepath()));
    std::string idLang;

    if ((ext == ".cpp") or (ext == ".hpp") or (ext == ".cc"))
	idLang = "cpp";
    else if (ext == ".py")
	idLang = "python";
    else
	idLang = "cmake";

    return idLang;
}

#ifdef VLE_HAVE_GTKSOURCEVIEWMM
void DocumentText::applyEditingProperties()
{
    Modeling* modeling = mGVLE->getModeling();

    mView.get_source_buffer()->
	set_highlight_syntax(modeling->getHighlightSyntax());
    mView.get_source_buffer()->
	set_highlight_matching_brackets(modeling->getHighlightBrackets());
    mView.set_highlight_current_line(modeling->getHighlightLine());
    mView.set_show_line_numbers(modeling->getLineNumbers());
    mView.set_show_right_margin(modeling->getRightMargin());
    mView.set_auto_indent(modeling->getAutoIndent());
    mView.set_indent_on_tab(modeling->getIndentOnTab());
    mView.set_indent_width(modeling->getIndentSize());
    if (modeling->getSmartHomeEnd())
	mView.set_smart_home_end(gtksourceview::SOURCE_SMART_HOME_END_ALWAYS);

    Pango::FontDescription font = Pango::FontDescription(
	modeling->getFontEditor());
    mView.modify_font(font);
}
#endif

void DocumentText::undo()
{
#ifdef VLE_HAVE_GTKSOURCEVIEWMM
    mView.get_source_buffer()->undo();
#endif
}

void DocumentText::redo()
{
#ifdef VLE_HAVE_GTKSOURCEVIEWMM
    mView.get_source_buffer()->redo();
#endif
}

bool DocumentText::event(GdkEvent* event)
{
    if (event->type == GDK_KEY_RELEASE
	and event->key.state != GDK_CONTROL_MASK
	and mModified == false) {
	mModified = true;
	mTitle = "* "+ mTitle;
	mGVLE->setModifiedTab(mTitle, filepath());
	return true;

    }
    return false;
}

DocumentDrawingArea::DocumentDrawingArea(GVLE* gvle,
					 const std::string& filepath,
					 View* view, graph::Model* model) :
    Document(gvle, filepath),
    mView(view),
    mModel(model),
    mAdjustWidth(0,0,1),
    mAdjustHeight(0,0,1)
{
    mTitle = filename() + boost::filesystem::extension(filepath)
	+ " - " + model->getName();
    mArea = new ViewDrawingArea(mView);
    mViewport = new Gtk::Viewport(mAdjustWidth, mAdjustHeight);
    mTitle = filename() + boost::filesystem::extension(filepath)
	+ " - " + model->getName();

    mViewport->add(*mArea);
    mViewport->set_shadow_type(Gtk::SHADOW_NONE);
    mViewport->set_border_width(0);
    set_policy(Gtk::POLICY_ALWAYS, Gtk::POLICY_ALWAYS);
    set_flags(Gtk::CAN_FOCUS);
    add(*mViewport);
    set_shadow_type(Gtk::SHADOW_NONE);
    set_border_width(0);
}

DocumentDrawingArea::~DocumentDrawingArea()
{
    delete mModel;
    delete mArea;
}

void DocumentDrawingArea::setHadjustment(double h)
{
    mViewport->get_hadjustment()->set_lower(h);
    mViewport->get_hadjustment()->set_value(h);
}

void DocumentDrawingArea::setVadjustment(double v)
{
    mViewport->get_vadjustment()->set_lower(v);
    mViewport->get_vadjustment()->set_value(v);
}

void DocumentDrawingArea::undo()
{
    //TODO
}

void DocumentDrawingArea::redo()
{
    //TODO
}

GVLE::FileTreeView::FileTreeView(
    BaseObjectType* cobject,
    const Glib::RefPtr<Gnome::Glade::Xml>& /*refGlade*/) :
    Gtk::TreeView(cobject)
{

    mRefTreeModel = Gtk::TreeStore::create(mColumns);
    set_model(mRefTreeModel);
    append_column(_("Files"), mColumns.m_col_name);
    mRefTreeSelection = get_selection();
    mIgnoredFilesList.push_front("build");
}

GVLE::FileTreeView::~FileTreeView()
{
}

void GVLE::FileTreeView::buildHierarchyDirectory(
    const Gtk::TreeModel::Row& parent, const std::string& dirname)
{
    Glib::Dir dir(dirname);
    std::list<std::string> entries (dir.begin(), dir.end());
    entries.sort();
    std::list <std::string> ::iterator it;
    for (it = entries.begin(); it != entries.end(); ++it) {
	if (((*it)[0] != '.') //Don't show hidden files
	    and (std::find(mIgnoredFilesList.begin(), mIgnoredFilesList.end(), *it)
	         == mIgnoredFilesList.end())) {
	    std::string nextpath = Glib::build_filename(dirname, *it);
	    if (isDirectory(nextpath)) {
		Gtk::TreeModel::Row row = *(mRefTreeModel->append(parent.children()));
		row[mColumns.m_col_name] = *it;
		buildHierarchy(*row, nextpath);
	    }
	}
    }
}

void GVLE::FileTreeView::buildHierarchyFile(
    const Gtk::TreeModel::Row& parent, const std::string& dirname)
{
    Glib::Dir dir(dirname);
    std::list<std::string> entries (dir.begin(), dir.end());
    entries.sort();
    std::list<std::string>::iterator it;
    for (it = entries.begin(); it != entries.end(); ++it) {
	if (((*it)[0] != '.') //Don't show hidden files
	    and (std::find(mIgnoredFilesList.begin(), mIgnoredFilesList.end(), *it)
	         == mIgnoredFilesList.end())) {
	    std::string nextpath = Glib::build_filename(dirname, *it);
	    if (not isDirectory(nextpath)) {
		Gtk::TreeModel::Row row = *(mRefTreeModel->append(parent.children()));
		row[mColumns.m_col_name] = *it;
	    }
	}
    }
}


void GVLE::FileTreeView::buildHierarchy(
    const Gtk::TreeModel::Row& parent, const std::string& dirname)
{
    buildHierarchyDirectory(parent, dirname);
    buildHierarchyFile(parent, dirname);
}

void GVLE::FileTreeView::clear()
{
    mRefTreeModel->clear();
}

void GVLE::FileTreeView::build()
{
    if (not mPackage.empty()) {
	Gtk::TreeIter iter = mRefTreeModel->append();
	Gtk::TreeModel::Row row = *iter;
	row[mColumns.m_col_name] = boost::filesystem::basename(mPackage);
	buildHierarchy(*row, mPackage);
	expand_row(Gtk::TreePath(iter), false);
    }
}

bool GVLE::FileTreeView::isDirectory(const std::string& dirname)
{
    return Glib::file_test(dirname, Glib::FILE_TEST_IS_DIR);
}

void GVLE::FileTreeView::on_row_activated(
    const Gtk::TreeModel::Path&,
    Gtk::TreeViewColumn*)
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection	= get_selection();
    Gtk::TreeModel::const_iterator it = refSelection->get_selected();
    const Gtk::TreeModel::Row row = *it;
    const std::list<std::string>* lstpath = projectFilePath(row);

    std::string absolute_path =
	Glib::build_filename(mPackage, Glib::build_filename(*lstpath));
    if (not isDirectory(absolute_path)) {
	if (mParent->existTab(absolute_path)) {
	    mParent->focusTab(absolute_path);
	} else {
	    if (boost::filesystem::extension(absolute_path) == ".vpz") {
		mParent->closeVpzTab();
		if (not mParent->existVpzTab())
		    mParent->openTab(absolute_path);
	    } else {
		mParent->openTab(absolute_path);
	    }
	}
    }
    else {
	if (not row_expanded(Gtk::TreePath(it)))
	    expand_row(Gtk::TreePath(it), false);
	else
	    collapse_row(Gtk::TreePath(it));
    }
}

std::list<std::string>* GVLE::FileTreeView::projectFilePath(
    const Gtk::TreeRow& row)
{
    if (row.parent()) {
	std::list<std::string>* lst =
	    projectFilePath(*row.parent());
	lst->push_back(std::string(row.get_value(mColumns.m_col_name)));
	return lst;
    } else {
	return new std::list<std::string>();
    }
}

GVLE::GVLE(BaseObjectType* cobject,
	   const Glib::RefPtr<Gnome::Glade::Xml> xml):
    Gtk::Window(cobject),
    m_modeling(new Modeling(this)),
    m_currentButton(POINTER),
    mCurrentView(0),
    m_helpbox(0)
{
    mRefXML = xml;
    m_modeling->setGlade(mRefXML);

    mGlobalVpzPrevDirPath = "";

    mConditionsBox = new ConditionsBox(mRefXML, m_modeling);
    mSimulationBox = new LaunchSimulationBox(mRefXML, m_modeling);
    mPreferencesBox = new PreferencesBox(mRefXML, m_modeling);
    mOpenPackageBox = new OpenPackageBox(mRefXML, m_modeling);
    mOpenVpzBox = new OpenVpzBox(mRefXML, m_modeling);
    mNewProjectBox = new NewProjectBox(mRefXML, m_modeling);
    mSaveVpzBox = new SaveVpzBox(mRefXML, m_modeling);

    mRefXML->get_widget("MenuAndToolbarVbox", mMenuAndToolbarVbox);
    mRefXML->get_widget("StatusBarPackageBrowser", mStatusbar);
    mRefXML->get_widget("TextViewLogPackageBrowser", mLog);
    mRefXML->get_widget_derived("FileTreeViewPackageBrowser", mFileTreeView);
    mFileTreeView->setParent(this);
    mRefXML->get_widget("NotebookPackageBrowser", mNotebook);
    mRefXML->get_widget_derived("TreeViewModel", mModelTreeBox);
    mModelTreeBox->setModeling(m_modeling);
    mRefXML->get_widget_derived("TreeViewClass", mModelClassBox);
    mModelClassBox->createNewModelBox(m_modeling);

    mMenuAndToolbar = new GVLEMenuAndToolbar(this);
    mMenuAndToolbarVbox->pack_start(*mMenuAndToolbar->getMenuBar());
    mMenuAndToolbarVbox->pack_start(*mMenuAndToolbar->getToolbar());
    mMenuAndToolbar->getToolbar()->set_toolbar_style(Gtk::TOOLBAR_BOTH);

    mNotebook->signal_switch_page().connect(
	sigc::mem_fun(this, &GVLE::changeTab));

    m_modeling->setModified(false);
    resize(900, 550);
    show();
}

GVLE::~GVLE()
{
    delete m_modeling;

    delete mConditionsBox;
    delete mSimulationBox;
    delete mPreferencesBox;
    delete mOpenPackageBox;
    delete mOpenVpzBox;
    delete mNewProjectBox;
    delete mSaveVpzBox;
}

void GVLE::show()
{
    buildPackageHierarchy();
    show_all();
}

void GVLE::buildPackageHierarchy()
{
    mModelTreeBox->clear();
    mModelClassBox->clear();
    mPackage = vle::utils::Path::path().getPackageDir();
    set_title(std::string(_("GVLE - package: ")).append(
		  boost::filesystem::basename(mPackage)));
    mFileTreeView->clear();
    mFileTreeView->setPackage(mPackage);
    mFileTreeView->build();
}

void GVLE::setFileName(std::string name)
{
    if (not name.empty() and
	boost::filesystem::extension(name) == ".vpz") {
	m_modeling->parseXML(name);
	mMenuAndToolbar->onViewMode();
    }
    m_modeling->setModified(false);
}

void GVLE::redrawModelTreeBox()
{
    assert(m_modeling->getTopModel());
    mModelTreeBox->parseModel(m_modeling->getTopModel());
}

void GVLE::redrawModelClassBox()
{
    mModelClassBox->parseClass();
}

void GVLE::showRowTreeBox(const std::string& name)
{
    mModelTreeBox->showRow(name);
}

void GVLE::showRowModelClassBox(const std::string& name)
{
    mModelClassBox->showRow(name);
}

bool GVLE::on_delete_event(GdkEventAny* event)
{
    if (event->type == GDK_DELETE) {
	onMenuQuit();
	return true;
    }
    return false;
}

void GVLE::onArrow()
{
    m_currentButton = POINTER;
    m_status.push(_("Selection"));
}

void GVLE::onAddModels()
{
    m_currentButton = ADDMODEL;
    m_status.push(_("Add models"));
}

void GVLE::onAddLinks()
{
    m_currentButton = ADDLINK;
    m_status.push(_("Add links"));
}

void GVLE::onDelete()
{
    m_currentButton = DELETE;
    m_status.push(_("Delete object"));
}

void GVLE::onAddCoupled()
{
    m_currentButton = ADDCOUPLED;
    m_status.push(_("Coupled Model"));
}

void GVLE::onZoom()
{
    m_currentButton = ZOOM;
    m_status.push(_("Zoom"));
}

void GVLE::onQuestion()
{
    m_currentButton = QUESTION;
    m_status.push(_("Question"));
}

void GVLE::newFile()
{
    try {
	DocumentText* doc = new DocumentText(this, _("untitled file"), true);
	mDocuments.insert(
	    std::make_pair < std::string, DocumentText* >(_("untitled file"), doc));
	mNotebook->append_page(*doc, *(addLabel(doc->filename(),
						_("untitled file"))));
    } catch (std::exception& e) {
	std::cout << e.what() << std::endl;
    }
    show_all_children();
}

void GVLE::onMenuNew()
{
    if (m_modeling->isModified() == false) {
        m_modeling->delNames();
        m_modeling->start();
        redrawModelTreeBox();
    } else if (gvle::Question(_("Do you really want destroy model ?"))) {
        m_modeling->delNames();
        m_modeling->start();
        redrawModelTreeBox();
    }
}

void GVLE::onMenuNewProject()
{
    mNewProjectBox->show();
}


void GVLE::openFile()
{
    Gtk::FileChooserDialog file(_("Choose a file"), Gtk::FILE_CHOOSER_ACTION_OPEN);
    file.set_transient_for(*this);
    file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

    if (file.run() == Gtk::RESPONSE_OK) {
	std::string selected_file = file.get_filename();
	openTab(selected_file);
    }
}
void GVLE::onMenuOpenPackage()
{
    closeAllTab();
    mOpenPackageBox->show();
    if (utils::Path::path().package() != "")
	mMenuAndToolbar->onPackageMode();
}

void GVLE::onMenuOpenVpz()
{
    if (m_modeling->isModified() == false or
	gvle::Question(_("Do you really want load a new Model ?\nCurrent"
			 "model will be destroy and not save"))) {
	try {
	    mOpenVpzBox->show();
	    mMenuAndToolbar->onViewMode();
	} catch(utils::InternalError) {
	    Error(_("No experiments in the package ") +
		    utils::Path::path().package());
	}
    }
}

void GVLE::onMenuLoad()
{
    if (m_modeling->isModified() == false or
            gvle::Question(_("Do you really want load a new Model ?\nCurrent"
                             "model will be destroy and not save"))) {
        Gtk::FileChooserDialog file("VPZ file", Gtk::FILE_CHOOSER_ACTION_OPEN);
        file.set_transient_for(*this);
        file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
        file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
        Gtk::FileFilter filter;
        filter.set_name(_("Vle Project gZipped"));
        filter.add_pattern("*.vpz");
        file.add_filter(filter);
	if (mGlobalVpzPrevDirPath != "") {
	    file.set_current_folder(mGlobalVpzPrevDirPath);
        }

        if (file.run() == Gtk::RESPONSE_OK) {
	    mGlobalVpzPrevDirPath = file.get_current_folder();
	    closeAllTab();
            m_modeling->parseXML(file.get_filename());
	    utils::Path::path().setPackage("");
	    mMenuAndToolbar->onGlobalMode();
	    mMenuAndToolbar->onViewMode();
	    mFileTreeView->clear();
        }
    }
}

void GVLE::saveFile()
{
    int page = mNotebook->get_current_page();
    if (page != -1) {
	DocumentText* doc = dynamic_cast < DocumentText* >(
	    mNotebook->get_nth_page(page));
	if (not doc->isNew()) {
	    doc->save();
	} else {
	    Gtk::FileChooserDialog file(_("VPZ file"), Gtk::FILE_CHOOSER_ACTION_SAVE);
	    file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	    file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

	    if (file.run() == Gtk::RESPONSE_OK) {
		std::string filename(file.get_filename());
		doc->saveAs(filename);
	    }
	}
	buildPackageHierarchy();
    }
}

void GVLE::onMenuSave()
{
    std::vector<std::string> vec;
    m_modeling->vpz_is_correct(vec);

    if (vec.size() != 0) {
        //vpz is correct
        std::string error = _("Vpz incorrect :\n");
        std::vector<std::string>::const_iterator it = vec.begin();
        while (it != vec.end()) {
            error += *it + "\n";

            ++it;
        }
        Error(error);
        return;
    }

    if (m_modeling->isSaved()) {
	m_modeling->saveXML(m_modeling->getFileName());
	Documents::iterator it = mDocuments.find(m_modeling->getFileName());
	if (it != mDocuments.end())
	    it->second->setTitle(m_modeling->getFileName(),
		      m_modeling->getTopModel(), false);
    } else if (utils::Path::path().package() != "") {
	mSaveVpzBox->show();
    } else {
	Gtk::FileChooserDialog file(_("VPZ file"), Gtk::FILE_CHOOSER_ACTION_SAVE);
	file.set_transient_for(*this);
	file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
	Gtk::FileFilter filter;
	filter.set_name(_("Vle Project gZipped"));
	filter.add_pattern("*.vpz");
	file.add_filter(filter);

	if (file.run() == Gtk::RESPONSE_OK) {
	    std::string filename(file.get_filename());
	    vpz::Vpz::fixExtension(filename);
	    m_modeling->saveXML(filename);
	    Documents::iterator it = mDocuments.find(filename);
	    if (it != mDocuments.end())
	    it->second->setTitle(filename,
		      m_modeling->getTopModel(), false);
	}
    }
}

void GVLE::saveFileAs()
{
    int page = mNotebook->get_current_page();
    if (page != -1) {
	DocumentText* doc = dynamic_cast < DocumentText* >(
	    mNotebook->get_nth_page(page));
	Gtk::FileChooserDialog file(_("VPZ file"), Gtk::FILE_CHOOSER_ACTION_SAVE);
	file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

	if (file.run() == Gtk::RESPONSE_OK) {
	    std::string filename(file.get_filename());
	    doc->saveAs(filename);
	}
	buildPackageHierarchy();
    }
}

void GVLE::closeFile()
{
    int page = mNotebook->get_current_page();
    if (page != -1) {
	Gtk::Widget* tab = mNotebook->get_nth_page(page);
	Documents::iterator it = mDocuments.begin();
	while (it != mDocuments.end()) {
	    if (it->second == tab) {
		closeTab(it->first);
		break;
	    }
	    ++it;
	}
    }
}

void GVLE::onMenuQuit()
{
    if (m_modeling->isModified() == true and
	gvle::Question(_("Changes have been made,\n"
                         "Do you want the model to be saved?"))) {
	std::vector<std::string> vec;
	m_modeling->vpz_is_correct(vec);
	if (vec.size() != 0) {
	    //vpz is correct
	    std::string error = _("incorrect VPZ:\n");
	    std::vector<std::string>::const_iterator it = vec.begin();
	    while (it != vec.end()) {
		error += *it + "\n";

		++it;
	    }
	    Error(error);
	    return;
	} else {
	    onMenuSave();
	}
    }
    hide();
}


void GVLE::onPreferences()
{
    mPreferencesBox->show();
}

void GVLE::onSimulationBox()
{
    if (m_modeling->isSaved()) {
        mSimulationBox->show();
    } else {
        gvle::Error(_("Save or load a project before simulation"));
    }
}

void GVLE::onParameterExecutionBox()
{
    ParameterExecutionBox* box = new ParameterExecutionBox(m_modeling);
    box->run();
    delete box;
}

void GVLE::onExperimentsBox()
{
    ExperimentBox box(mRefXML, m_modeling);
    box.run();
}

void GVLE::onConditionsBox()
{
    mConditionsBox->show();
}

void GVLE::onHostsBox()
{
    HostsBox* box = new HostsBox(mRefXML);
    box->run();
    delete box;
}

void GVLE::onHelpBox()
{
    if (m_helpbox == 0)
        m_helpbox = new HelpBox;

    m_helpbox->show_all();
}

void GVLE::onViewOutputBox()
{
    ViewOutputBox box(*m_modeling, mRefXML, m_modeling->views());
    box.run();
}

void GVLE::onShowAbout()
{
    About box(mRefXML);
    box.run();
}

void GVLE::setTitle(const Glib::ustring& name)
{
    Glib::ustring title("gvle");
    if (not name.empty()) {
        title += " - ";
        title += Glib::path_get_basename(name);
    }
    set_title(title);
}

void GVLE::setModifiedTitle(const std::string& name)
{
    Glib::ustring current("* ");
    current += get_title();

    set_title(current);

    if (not name.empty() and
	boost::filesystem::extension(name) == ".vpz") {
	Documents::iterator it = mDocuments.find(name);
	if (it != mDocuments.end())
	    it->second->setTitle(name,
				 m_modeling->getTopModel(),
				 true);
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

void GVLE::focusTab(const std::string& filepath)
{
    int page = mNotebook->page_num(*(mDocuments.find(filepath)->second));
    mNotebook->set_current_page(page);
}

Gtk::HBox* GVLE::addLabel(const std::string& title,
			const std::string& filepath)
{
    Gtk::HBox* tabLabel = new Gtk::HBox(false, 0);
    Gtk::Label* label = new Gtk::Label(title);
    tabLabel->pack_start(*label, true, true, 0);
    Gtk::Button* closeButton = new Gtk::Button();
    Gtk::Image* imgButton = new Gtk::Image(Gtk::Stock::CLOSE,
					   Gtk::IconSize(Gtk::ICON_SIZE_MENU));
    closeButton->set_image(*imgButton);
    closeButton->set_focus_on_click(false);
    closeButton->set_relief(Gtk::RELIEF_NONE);
    closeButton->set_tooltip_text(_("Close Tab"));
    closeButton->signal_clicked().connect(
	sigc::bind(sigc::mem_fun(*this, &GVLE::closeTab), filepath));

    tabLabel->pack_start(*closeButton, false, false, 0);
    tabLabel->show_all();
    return tabLabel;
}

void GVLE::openTab(const std::string& filepath)
{

    if(boost::filesystem::extension(filepath) != ".vpz") {
	try {
	    if (mDocuments.find(filepath) == mDocuments.end()) {
		DocumentText* doc = new DocumentText(this, filepath);
		mDocuments.insert(
		    std::make_pair < std::string, DocumentText* >(filepath, doc));
		int page = mNotebook->append_page(*doc,
						  *(addLabel(doc->getTitle(),
							     filepath)));
		show_all_children();
		mNotebook->set_current_page(page);
	    } else {
		focusTab(filepath);
	    }
	} catch(utils::FileError& fe) {
	    Error(fe.what());
	} catch (std::exception& e) {
	    mLog->get_buffer()->insert(
		mLog->get_buffer()->end(), e.what());
	}
	mMenuAndToolbar->onFileMode();
    } else {
	m_modeling->parseXML(filepath);
    }
}

void GVLE::openTabVpz(const std::string& filepath, graph::CoupledModel* model)
{
    Documents::iterator it = mDocuments.find(filepath);
    int page;
    if (it != mDocuments.end()) {
	if (dynamic_cast<DocumentDrawingArea*>(it->second)->getModel()
	    != model) {
	    focusTab(filepath);
	    page = mNotebook->get_current_page();
	    mNotebook->remove_page(page);
	    mDocuments.erase(filepath);
	    mNotebook->set_current_page(--page);

	    DocumentDrawingArea* doc = new DocumentDrawingArea(
		this,
		filepath,
		m_modeling->findView(model),
		model);
	    doc->setTitle(filepath, model, true);
	    mCurrentView = doc->getView();
	    mDocuments.insert(
		std::make_pair < std::string, DocumentDrawingArea* >(
		    filepath, doc));
	    mNotebook->append_page(*doc, *(addLabel(doc->getTitle(),
						    filepath)));

	    mNotebook->reorder_child(*doc, ++page);
	} else {
	    return;
	}
    } else {
	DocumentDrawingArea* doc = new DocumentDrawingArea(
	    this,
	    filepath,
	    m_modeling->findView(model),
	    model);
	mCurrentView = doc->getView();
	mDocuments.insert(
	  std::make_pair < std::string, DocumentDrawingArea* >(filepath, doc));
	page = mNotebook->append_page(*doc, *(addLabel(doc->getTitle(),
						       filepath)));
    }
    show_all_children();
    mNotebook->set_current_page(page);
    mMenuAndToolbar->onViewMode();
}


void GVLE::closeTab(const std::string& filepath)
{
    if (boost::filesystem::extension(filepath) == ".vpz")
	closeVpzTab();
    else {
	Documents::iterator it = mDocuments.find(filepath);
	if (it != mDocuments.end()) {
	    if (not it->second->isModified() or
		gvle::Question(_("The current tab is not saved\n"
				 "Do you really want to close this file ?"))) {
		int page = mNotebook->page_num(*it->second);
		if (page != -1) {
		    mNotebook->remove_page(page);
		    mDocuments.erase(filepath);

		    mNotebook->set_current_page(--page);
		}
	    }
	}
    }
}

void GVLE::closeVpzTab()
{
    int page;
    Documents::iterator it = mDocuments.begin();
    while (it != mDocuments.end()) {
        if (boost::filesystem::extension(it->first) == ".vpz") {
            if (not it->second->isModified() or
                gvle::Question(_("The current tab is not saved\n"
                                 "Do you really want to close this file ?"))) {
                mModelTreeBox->clear();
                mModelClassBox->clear();
                page = mNotebook->page_num(
                    *(dynamic_cast<DocumentDrawingArea*>(it->second)));
                mNotebook->remove_page(page);
                mDocuments.erase(it->first);

                mNotebook->set_current_page(--page);
            }
        }
	++it;
    }
}

bool GVLE::existVpzTab()
{
    Documents::iterator it = mDocuments.begin();
    while (it != mDocuments.end()) {
	if (boost::filesystem::extension(it->first) == ".vpz")
	    return true;
	++it;
    }
    return false;
}

void GVLE::closeAllTab()
{
    Documents::iterator it = mDocuments.begin();
    while (it != mDocuments.end()) {
	int page = mNotebook->page_num(*it->second);
	mNotebook->remove_page(page);
	mDocuments.erase(it->first);

	mNotebook->set_current_page(--page);
	++it;
    }
}

void GVLE::changeTab(GtkNotebookPage* /*page*/, int num)
{
    Gtk::Widget* tab = mNotebook->get_nth_page(num);
    Documents::iterator it = mDocuments.begin();
    while (it != mDocuments.end()) {
	if (it->second == tab) {
	    if (boost::filesystem::extension(it->first) == ".vpz") {
		DocumentDrawingArea* area =
		    dynamic_cast< DocumentDrawingArea *>(it->second);
		mCurrentTab = num;
		mCurrentView = area->getView();
		mMenuAndToolbar->onViewMode();
	    } else{
		mCurrentTab = num;
		mCurrentView = 0;
		mMenuAndToolbar->onFileMode();
	    }
	    break;
	}
	++it;
    }
}

void GVLE::setModifiedTab(const std::string title, const std::string filepath)
{
    if (mDocuments.find(filepath) != mDocuments.end()) {
	int page = mNotebook->get_current_page();
	Gtk::Widget* tab = mNotebook->get_nth_page(page);

	mNotebook->set_tab_label(*tab, *(addLabel(title, filepath)));
    }
}

void GVLE::configureProject()
{
    std::string out, err;
    utils::CMakePackage::configure(out, err);
    mLog->get_buffer()->insert(mLog->get_buffer()->end(), out);
    if (not err.empty())
	mLog->get_buffer()->insert(mLog->get_buffer()->end(), err);
}

void GVLE::buildProject()
{
    std::string out, err;
    utils::CMakePackage::build(out, err);
    mLog->get_buffer()->insert(mLog->get_buffer()->end(), out);
    if (not err.empty())
	mLog->get_buffer()->insert(mLog->get_buffer()->end(), err);
}

void GVLE::cleanProject()
{
    std::string out, err;
    utils::CMakePackage::clean(out, err);
    mLog->get_buffer()->insert(mLog->get_buffer()->end(), out);
    if (not err.empty())
	mLog->get_buffer()->insert(mLog->get_buffer()->end(), err);
}

void GVLE::packageProject()
{
    std::string out, err;
    utils::CMakePackage::package(out, err);
    mLog->get_buffer()->insert(mLog->get_buffer()->end(), out);
    if (not err.empty())
	mLog->get_buffer()->insert(mLog->get_buffer()->end(), err);
}

void GVLE::onUndo()
{
    int page = mNotebook->get_current_page();
    if (page != -1) {
	Document* doc = dynamic_cast < Document* >(
	    mNotebook->get_nth_page(page));
	doc->undo();
    }
}

void GVLE::onRedo()
{
    int page = mNotebook->get_current_page();
    if (page != -1) {
	Document* doc = dynamic_cast < Document* >(
	    mNotebook->get_nth_page(page));
	doc->redo();
    }
}

void GVLE::onCutModel()
{
    mCurrentView->onCutModel();
}

void GVLE::onCopyModel()
{
    mCurrentView->onCopyModel();
}

void GVLE::onPasteModel()
{
    mCurrentView->onPasteModel();
}

void GVLE::clearCurrentModel()
{
    mCurrentView->clearCurrentModel();
}

void GVLE::importModel()
{
    mCurrentView->importModel();
}

void GVLE::exportCurrentModel()
{
    mCurrentView->exportCurrentModel();
}

void GVLE::exportGraphic()
{
    ViewDrawingArea* tab = dynamic_cast<DocumentDrawingArea*>(
	mNotebook->get_nth_page(mCurrentTab))->getDrawingArea();
    vpz::Experiment& experiment = m_modeling->vpz().project().experiment();
    if (experiment.name().empty() || experiment.duration() == 0) {
        Error(_("Fix a Value to the name and the duration of the experiment before exportation."));
        return;
    }

    Gtk::FileChooserDialog file(_("Image file"), Gtk::FILE_CHOOSER_ACTION_SAVE);
    file.set_transient_for(*this);
    file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
    Gtk::FileFilter filterPng;
    Gtk::FileFilter filterPdf;
    Gtk::FileFilter filterSvg;
    filterPng.set_name(_("Portable Newtork Graphics (.png)"));
    filterPng.add_pattern("*.png");
    filterPdf.set_name(_("Portable Format Document (.pdf)"));
    filterPdf.add_pattern("*.pdf");
    filterSvg.set_name(_("Scalable Vector Graphics (.svg)"));
    filterSvg.add_pattern("*.svg");
    file.add_filter(filterPng);
    file.add_filter(filterPdf);
    file.add_filter(filterSvg);


    if (file.run() == Gtk::RESPONSE_OK) {
        std::string filename(file.get_filename());
	std::string extension(file.get_filter()->get_name());

	if (extension == _("Portable Newtork Graphics (.png)"))
	    tab->exportPng(filename);
	else if (extension == _("Portable Format Document (.pdf)"))
	    tab->exportPdf(filename);
	else if (extension == _("Scalable Vector Graphics (.svg)"))
	    tab->exportSvg(filename);
    }
}

void GVLE::addCoefZoom()
{
    ViewDrawingArea* tab = dynamic_cast<DocumentDrawingArea*>(
	mNotebook->get_nth_page(mCurrentTab))->getDrawingArea();
    tab->addCoefZoom();
}

void GVLE::delCoefZoom()
{
    ViewDrawingArea* tab = dynamic_cast<DocumentDrawingArea*>(
	mNotebook->get_nth_page(mCurrentTab))->getDrawingArea();
    tab->delCoefZoom();
}

void GVLE::setCoefZoom(double coef)
{
    ViewDrawingArea* tab = dynamic_cast<DocumentDrawingArea*>(
	mNotebook->get_nth_page(mCurrentTab))->getDrawingArea();
    tab->setCoefZoom(coef);
}

void  GVLE::updateAdjustment(double h, double v)
{
    DocumentDrawingArea* tab = dynamic_cast<DocumentDrawingArea*>(
	mNotebook->get_nth_page(mCurrentTab));
    tab->setHadjustment(h);
    tab->setVadjustment(v);
}

void GVLE::onRandomOrder()
{
    ViewDrawingArea* tab = dynamic_cast<DocumentDrawingArea*>(
	mNotebook->get_nth_page(mCurrentTab))->getDrawingArea();
    tab->onRandomOrder();
}

}} // namespace vle gvle
