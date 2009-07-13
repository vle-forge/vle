/**
 * @file vle/gvle/Editor.cpp
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


#include <vle/gvle/Editor.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/gvle/GVLEMenuAndToolbar.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Path.hpp>
#include <vle/vpz/Vpz.hpp>
#include <boost/filesystem.hpp>

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
	mGVLE->getEditor()->setModifiedTab(mTitle, mFilePath);
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
    mView.get_buffer()->signal_changed().connect(
	sigc::mem_fun(this, &DocumentText::onChanged));
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
	mGVLE->getEditor()->setModifiedTab(mTitle, filepath());
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
	    Error(std::string("cannot open: " + filename()));
	}
    }
#ifdef VLE_HAVE_GTKSOURCEVIEWMM
    mView.set_source_buffer(buffer);
#endif
    applyEditingProperties();
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

void DocumentText::applyEditingProperties()
{
    Modeling* modeling = mGVLE->getModeling();

#ifdef VLE_HAVE_GTKSOURCEVIEWMM
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
#endif
    Pango::FontDescription font = Pango::FontDescription(
	modeling->getFontEditor());
    mView.modify_font(font);
}

void DocumentText::updateView()
{
    applyEditingProperties();
}

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

void DocumentText::onChanged()
{
    if (not mModified) {
	mModified = true;
	mTitle = "* "+ mTitle;
	mGVLE->getEditor()->setModifiedTab(mTitle, filepath());
    }
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

void DocumentDrawingArea::updateView()
{
    //maybe useful later
}

void DocumentDrawingArea::undo()
{
    //TODO
}

void DocumentDrawingArea::redo()
{
    //TODO
}

/*
 * Editor
 */

Editor::Editor(BaseObjectType* cobject,
		     const Glib::RefPtr<Gnome::Glade::Xml>& /*refGlade*/) :
    Gtk::Notebook(cobject)
{
    signal_switch_page().connect(
	sigc::mem_fun(this, &Editor::changeTab));
}

void Editor::focusTab(const std::string& filepath)
{
    int page = page_num(*(mDocuments.find(filepath)->second));
    set_current_page(page);
}

void Editor::openTab(const std::string& filepath)
{

    if(boost::filesystem::extension(filepath) != ".vpz") {
	try {
	    if (mDocuments.find(filepath) == mDocuments.end()) {
		DocumentText* doc = new DocumentText(mGVLE, filepath);
		mDocuments.insert(
		    std::make_pair <std::string, DocumentText*>(filepath, doc));
		int page = append_page(
		    *doc, *(addLabel(doc->getTitle(), filepath)));
		show_all_children();
		set_current_page(page);
	    } else {
		focusTab(filepath);
	    }
	} catch(utils::FileError& fe) {
	    Error(fe.what());
	} catch (std::exception& e) {
	    mGVLE->insertLog(e.what());
	}
	mGVLE->getMenu()->onFileMode();
    } else {
	mGVLE->getModeling()->parseXML(filepath);
    }
}

void Editor::openTabVpz(const std::string& filepath, graph::CoupledModel* model)
{
    Documents::iterator it = mDocuments.find(filepath);
    int page;
    if (it != mDocuments.end()) {
	if (dynamic_cast<DocumentDrawingArea*>(it->second)->getModel()
	    != model) {
	    focusTab(filepath);
	    page = get_current_page();
	    remove_page(page);
	    mDocuments.erase(filepath);
	    set_current_page(--page);

	    DocumentDrawingArea* doc = new DocumentDrawingArea(
		mGVLE,
		filepath,
		mGVLE->getModeling()->findView(model),
		model);
	    doc->setTitle(filepath, model, true);
	    mGVLE->setCurrentView(doc->getView());
	    mDocuments.insert(
		std::make_pair < std::string, DocumentDrawingArea* >(
		    filepath, doc));
	    append_page(*doc, *(addLabel(doc->getTitle(),
						    filepath)));

	    reorder_child(*doc, ++page);
	} else {
	    return;
	}
    } else {
	DocumentDrawingArea* doc = new DocumentDrawingArea(
	    mGVLE,
	    filepath,
	    mGVLE->getModeling()->findView(model),
	    model);
	mGVLE->setCurrentView(doc->getView());
	mDocuments.insert(
	  std::make_pair < std::string, DocumentDrawingArea* >(filepath, doc));
	page = append_page(*doc, *(addLabel(doc->getTitle(),
						       filepath)));
    }
    show_all_children();
    set_current_page(page);
    mGVLE->getMenu()->onViewMode();
}


void Editor::closeTab(const std::string& filepath)
{
    if (boost::filesystem::extension(filepath) == ".vpz")
	closeVpzTab();
    else {
	Documents::iterator it = mDocuments.find(filepath);
	if (it != mDocuments.end()) {
	    if (not it->second->isModified() or
		gvle::Question(_("The current tab is not saved\n"
				 "Do you really want to close this file ?"))) {
		int page = page_num(*it->second);
		if (page != -1) {
		    remove_page(page);
		    mDocuments.erase(filepath);

		    set_current_page(--page);
		}
	    }
	}
    }

    if (getDocumentsList().size() == 0)
	mGVLE->tabClosed();
}

void Editor::closeVpzTab()
{
    int page;
    Documents::iterator it = mDocuments.begin();
    while (it != mDocuments.end()) {
        if (boost::filesystem::extension(it->first) == ".vpz") {
            if (not it->second->isModified() or
                gvle::Question(_("The current tab is not saved\n"
                                 "Do you really want to close this file ?"))) {
                mGVLE->clearModelTreeBox();
                mGVLE->clearModelClassBox();
                page = page_num(
                    *(dynamic_cast<DocumentDrawingArea*>(it->second)));
                remove_page(page);
                mDocuments.erase(it->first);

                set_current_page(--page);
            }
        }
	++it;
    }
}

bool Editor::existVpzTab()
{
    Documents::iterator it = mDocuments.begin();
    while (it != mDocuments.end()) {
	if (boost::filesystem::extension(it->first) == ".vpz")
	    return true;
	++it;
    }
    return false;
}

void Editor::closeAllTab()
{
    Documents::iterator it = mDocuments.begin();
    while (it != mDocuments.end()) {
	int page = page_num(*it->second);
	remove_page(page);
	mDocuments.erase(it->first);

	set_current_page(--page);
	++it;
    }
}

void Editor::changeTab(GtkNotebookPage* /*page*/, int num)
{
    Gtk::Widget* tab = get_nth_page(num);
    Documents::iterator it = mDocuments.begin();
    while (it != mDocuments.end()) {
	if (it->second == tab) {
	    if (boost::filesystem::extension(it->first) == ".vpz") {
		DocumentDrawingArea* area =
		    dynamic_cast< DocumentDrawingArea *>(it->second);
		mGVLE->setCurrentView(area->getView());
		mGVLE->setCurrentTab(num);
		mGVLE->getMenu()->onViewMode();
	    } else{
		mGVLE->setCurrentView(0);
		mGVLE->setCurrentTab(num);
		mGVLE->getMenu()->onFileMode();
	    }
	    mGVLE->setTitle(boost::filesystem::basename(it->first) +
			    boost::filesystem::extension(it->first));
	    break;
	}
	++it;
    }
}

void Editor::setModifiedTab(const std::string title, const std::string filepath)
{
    if (mDocuments.find(filepath) != mDocuments.end()) {
	int page = get_current_page();
	Gtk::Widget* tab = get_nth_page(page);

	set_tab_label(*tab, *(addLabel(title, filepath)));
    }
}

void Editor::createBlankNewFile()
{
    try {
	DocumentText* doc = new DocumentText(mGVLE, _("untitled file"), true);
	mDocuments.insert(
	    std::make_pair < std::string, DocumentText* >(_("untitled file"), doc));
	append_page(*doc, *(addLabel(doc->filename(),
				     _("untitled file"))));
    } catch (std::exception& e) {
	std::cout << e.what() << std::endl;
    }
    show_all_children();
}

void Editor::closeFile()
{
    int page = get_current_page();
    if (page != -1) {
	Gtk::Widget* tab = get_nth_page(page);
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


void Editor::refreshViews()
{
    for (Documents::iterator it = mDocuments.begin();
	 it != mDocuments.end(); ++it) {
	it->second->updateView();
    }
}

void Editor::onUndo()
{
    int page = get_current_page();
    if (page != -1) {
	Document* doc = dynamic_cast < Document* >(
	    get_nth_page(page));
	doc->undo();
    }
}

void Editor::onRedo()
{
    int page = get_current_page();
    if (page != -1) {
	Document* doc = dynamic_cast < Document* >(
	    get_nth_page(page));
	doc->redo();
    }
}

Gtk::HBox* Editor::addLabel(const std::string& title,
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
	sigc::bind(sigc::mem_fun(*this, &Editor::closeTab), filepath));

    tabLabel->pack_start(*closeButton, false, false, 0);
    tabLabel->show_all();
    return tabLabel;
}

}}
