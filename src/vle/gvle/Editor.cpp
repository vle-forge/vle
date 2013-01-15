/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2013 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2013 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2013 INRA http://www.inra.fr
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


#include <vle/gvle/Editor.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/gvle/GVLE.hpp>
#include <vle/gvle/GVLEMenuAndToolbar.hpp>
#include <vle/gvle/Settings.hpp>
#include <vle/gvle/View.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Path.hpp>
#include <gtkmm/stock.h>
#ifdef VLE_HAVE_GTKSOURCEVIEWMM
#include <gtkmm/clipboard.h>
#endif
#include <gdkmm/cursor.h>
#include <boost/lexical_cast.hpp>
#include <fstream>


namespace vle { namespace gvle {

Document::Document(GVLE* gvle, const std::string& filepath) :
    Gtk::ScrolledWindow(),
    mGVLE(gvle)
{
    setModified(false);
    mFilePath = filepath;
    mFileName = utils::Path::basename(filepath);
}

Document::Document() :
    Gtk::ScrolledWindow(), mGVLE(0)
{
    setModified(false);
}

void Document::setTitle(const std::string& filePath,
			vpz::BaseModel* model,
			bool modified)
{
    if (mGVLE) {
        if (utils::Path::extension(filePath) == ".vpz") {
            mTitle = utils::Path::basename(filePath) +
                utils::Path::extension(filePath) + " - " +
                model->getName();
        } else {
            mTitle = utils::Path::basename(filePath) +
                utils::Path::extension(filePath);
        }
        if (modified) {
            mTitle = "* " + mTitle;
            setModified(true);
        } else {
            setModified(false);
        }
        mGVLE->getEditor()->setModifiedTab(mTitle, filePath, mFilePath);
        mFilePath = filePath;
    }
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

DocumentText::DocumentText(GVLE* gvle,
			   const std::string& filepath,
			   bool newfile, bool hasFullName) :
    Document(gvle, filepath),
    mNew(newfile),
    mHasFullName(hasFullName)
{
    mTitle = filename() + utils::Path::extension(filepath);

    mIdLang = guessIdLanguage();

#ifdef VLE_HAVE_GTKSOURCEVIEWMM
    gtksourceview::init();
#endif

    if (not mNew) {
	std::ifstream file(filepath.c_str());
	if (file) {
	    std::stringstream filecontent;
	    filecontent << file.rdbuf();
	    file.close();
	    Glib::ustring buffer_content = filecontent.str();
	    if (not buffer_content.validate()) {
                try {
                    buffer_content = Glib::locale_to_utf8(buffer_content);
                }
                catch(const Glib::ConvertError& e){
                    throw utils::FileError(_("This file isn't UTF-8 valid,"
                                             " and convert from locale"
                                             " encoding failed."));
                }
	    }
            init(buffer_content);
        } else {
	    Error(std::string("cannot open: " + filename()));
	}
    }

    applyEditingProperties();

    add(mView);

}

DocumentText::DocumentText(const std::string& buffer)
    : Document()
{
    mIdLang = "cpp";

#ifdef VLE_HAVE_GTKSOURCEVIEWMM
    gtksourceview::init();
#endif
    init(buffer);

    add(mView);

    applyEditingProperties();
}

void DocumentText::save()
{
    try {
	std::ofstream file(filepath().c_str());
	file << mView.get_buffer()->get_text();
	file.close();
	mNew = false;
	setModified(false);
	mTitle = filename() + utils::Path::extension(filepath());
        if (mGVLE) {
            mGVLE->getEditor()->setModifiedTab(mTitle, filepath(), filepath());
            mGVLE->getMenu()->hideSave();
        }
    } catch(std::exception& e) {
	throw _("Error while saving file.");
    }
}

void DocumentText::saveAs(const std::string& newFilePath)
{
    try {
	std::string oldFilePath(filepath().c_str());
	setFilePath(newFilePath);
	setFileName(utils::Path::basename(newFilePath));
	std::ofstream file(filepath().c_str());
	file << mView.get_buffer()->get_text();
	file.close();
	mNew = false;
	setModified(false);
	mTitle = filename() + utils::Path::extension(filepath());
        if (mGVLE) {
            mGVLE->getEditor()->setModifiedTab(mTitle, newFilePath,
                                               oldFilePath);
            mGVLE->getMenu()->hideSave();
        }
    } catch (const std::exception& /*e*/) {
	throw _("Error while saving file.");
    }
}

void DocumentText::init(const std::string& buffer)
{
#ifdef VLE_HAVE_GTKSOURCEVIEWMM
    Glib::RefPtr<gtksourceview::SourceLanguageManager> manager
	= gtksourceview::SourceLanguageManager::create();
    Glib::RefPtr<gtksourceview::SourceLanguage> language =
	manager->get_language(mIdLang);
    Glib::RefPtr<gtksourceview::SourceBuffer> buffer_ =
	gtksourceview::SourceBuffer::create(language);
#else
    Glib::RefPtr<Gtk::TextBuffer> buffer_ = mView.get_buffer();
#endif

#ifdef VLE_HAVE_GTKSOURCEVIEWMM
    buffer_->begin_not_undoable_action();
    buffer_->insert(buffer_->end(), buffer);
    buffer_->end_not_undoable_action();
#else
    buffer_->insert(buffer_->end(), buffer);
#endif

#ifdef VLE_HAVE_GTKSOURCEVIEWMM
    mView.set_source_buffer(buffer_);
#endif

    mView.get_buffer()->signal_changed().connect(
	sigc::mem_fun(this, &DocumentText::onChanged));


}

std::string DocumentText::guessIdLanguage()
{
    std::string ext(utils::Path::extension(filepath()));
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
#ifdef VLE_HAVE_GTKSOURCEVIEWMM
    mView.get_source_buffer()->set_highlight_syntax(
        Settings::settings().getHighlightSyntax());
    mView.get_source_buffer()->set_highlight_matching_brackets(
        Settings::settings().getHighlightBrackets());
    mView.set_highlight_current_line(
        Settings::settings().getHighlightLine());
    mView.set_show_line_numbers(
        Settings::settings().getLineNumbers());
    mView.set_show_right_margin(
        Settings::settings().getRightMargin());
    mView.set_auto_indent(
        Settings::settings().getAutoIndent());
    mView.set_indent_on_tab(
        Settings::settings().getIndentOnTab());
    mView.set_indent_width(
        Settings::settings().getIndentSize());
    if (Settings::settings().getSmartHomeEnd()) {
        mView.set_smart_home_end(
            gtksourceview::SOURCE_SMART_HOME_END_ALWAYS);
    }
#endif

    Pango::FontDescription font =
        Pango::FontDescription(Settings::settings().getFontEditor());

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

void DocumentText::paste()
{
#ifdef VLE_HAVE_GTKSOURCEVIEWMM
    Glib::RefPtr<gtksourceview::SourceBuffer> buffer(mView.get_source_buffer());
#else
    Glib::RefPtr<Gtk::TextBuffer> buffer(mView.get_buffer());
#endif

    Glib::RefPtr<Gtk::Clipboard> clipboard(Gtk::Clipboard::get());
    buffer->paste_clipboard(clipboard, true);
    mView.scroll_to_mark(buffer->get_insert(), 0.02);
}

void DocumentText::copy()
{
#ifdef VLE_HAVE_GTKSOURCEVIEWMM
    Glib::RefPtr<gtksourceview::SourceBuffer> buffer(mView.get_source_buffer());
#else
    Glib::RefPtr<Gtk::TextBuffer> buffer(mView.get_buffer());
#endif

    Glib::RefPtr<Gtk::Clipboard> clipboard(Gtk::Clipboard::get());
    buffer->copy_clipboard(clipboard);
}

void DocumentText::cut()
{
#ifdef VLE_HAVE_GTKSOURCEVIEWMM
    Glib::RefPtr<gtksourceview::SourceBuffer> buffer(mView.get_source_buffer());
#else
    Glib::RefPtr<Gtk::TextBuffer> buffer(mView.get_buffer());
#endif

    Glib::RefPtr<Gtk::Clipboard> clipboard(Gtk::Clipboard::get());
    buffer->cut_clipboard(clipboard);
}

void DocumentText::selectAll()
{
#ifdef VLE_HAVE_GTKSOURCEVIEWMM
    Glib::RefPtr<gtksourceview::SourceBuffer> buffer(mView.get_source_buffer());
#else
    Glib::RefPtr<Gtk::TextBuffer> buffer(mView.get_buffer());
#endif
    buffer->select_range(buffer->begin(), buffer->end());
}

void DocumentText::onChanged()
{
    if (not isModified()) {
        setModified(true);

        if (mGVLE) { /* We need to protect the following methods since the
                        DocumentText class may be used into ModelingPlugin
                        without any reference to the global context GVLE
                        (closes #3405385). */
            mGVLE->getMenu()->showSave();
            mTitle = "* "+ mTitle;
            mGVLE->getEditor()->setModifiedTab(mTitle, filepath(), filepath());
        }
    }
}

std::string DocumentText::getBuffer()
{
#ifdef VLE_HAVE_GTKSOURCEVIEWMM
    return mView.get_source_buffer()->get_text();
#else
    return mView.get_buffer()->get_text();
#endif
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

DocumentDrawingArea::DocumentDrawingArea(GVLE* gvle,
                                         const std::string& filepath,
                                         View* view, vpz::BaseModel* model) :
    Document(gvle, filepath),
    mView(view),
    mModel(model),
    mAdjustWidth(0,0,1),
    mAdjustHeight(0,0,1)
{
}

DocumentDrawingArea::~DocumentDrawingArea()
{

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
    mCompleteArea->queueRedraw();
}

void DocumentDrawingArea::updateCursor()
{
    Glib::RefPtr< Gdk::Window > daw;

    if (isComplete()) {
        daw = mCompleteArea->get_window();
    } else {
        daw = mSimpleArea->get_window();
    }

    switch(Document::mGVLE->getCurrentButton()) {
        case GVLE::VLE_GVLE_POINTER :
        daw->set_cursor(Gdk::Cursor(Gdk::HAND2));
        break;
        case GVLE::VLE_GVLE_ADDMODEL :
        daw->set_cursor(Gdk::Cursor(Gdk::CROSS));
        break;
        case GVLE::VLE_GVLE_ADDLINK :
        daw->set_cursor(Gdk::Cursor(Gdk::PENCIL));
        break;
        case GVLE::VLE_GVLE_DELETE :
        daw->set_cursor(Gdk::Cursor(Gdk::X_CURSOR));
        break;
        case GVLE::VLE_GVLE_ADDCOUPLED :
        daw->set_cursor(Gdk::Cursor(Gdk::DRAPED_BOX));
        break;
        case GVLE::VLE_GVLE_ZOOM :
        daw->set_cursor(Gdk::Cursor(Gdk::SIZING));
        break;
        case GVLE::VLE_GVLE_QUESTION :
        daw->set_cursor(Gdk::Cursor(Gdk::QUESTION_ARROW));
        break;
    default :
        daw->set_cursor(Gdk::Cursor(Gdk::ARROW));
    }
}

void DocumentDrawingArea::undo()
{
    //TODO
}

void DocumentDrawingArea::redo()
{
    //TODO
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

DocumentCompleteDrawingArea::DocumentCompleteDrawingArea(
    GVLE* gvle,
    const std::string& filepath,
    View* view, vpz::BaseModel* model) :
    DocumentDrawingArea(gvle, filepath, view, model)
{
    mTitle = filename() + utils::Path::extension(filepath)
        + " - " + model->getName();
    mCompleteArea = mView->getCompleteArea();
    mViewport = new Gtk::Viewport(mAdjustWidth, mAdjustHeight);
    mViewport->add(*mCompleteArea);
    mViewport->set_shadow_type(Gtk::SHADOW_NONE);
    mViewport->set_border_width(0);

    set_policy(Gtk::POLICY_ALWAYS, Gtk::POLICY_ALWAYS);
    set_flags(Gtk::CAN_FOCUS);
    add(*mViewport);
    set_shadow_type(Gtk::SHADOW_NONE);
    set_border_width(0);
}

DocumentCompleteDrawingArea::~DocumentCompleteDrawingArea()
{
    remove();
    delete mViewport;
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

DocumentSimpleDrawingArea::DocumentSimpleDrawingArea(GVLE* gvle,
                                                     const std::string& filepath,
                                                     View* view, vpz::BaseModel* model) :
    DocumentDrawingArea(gvle, filepath, view, model)
{
    mTitle = filename() + utils::Path::extension(filepath)+
        " - " + model->getName();
    mSimpleArea = mView->getSimpleArea();
    mViewport = new Gtk::Viewport(mAdjustWidth, mAdjustHeight);
    mViewport->add(*mSimpleArea);
    mViewport->set_shadow_type(Gtk::SHADOW_NONE);
    mViewport->set_border_width(0);

    set_policy(Gtk::POLICY_ALWAYS, Gtk::POLICY_ALWAYS);
    set_flags(Gtk::CAN_FOCUS);
    add(*mViewport);
    set_shadow_type(Gtk::SHADOW_NONE);
    set_border_width(0);
}

DocumentSimpleDrawingArea::~DocumentSimpleDrawingArea()
{
    remove();
    delete mViewport;
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

Editor::Editor(BaseObjectType* cobject,
               const Glib::RefPtr < Gtk::Builder >& /*refGlade*/) :
    Gtk::Notebook(cobject)
{
    tabSignal = signal_switch_page().connect(sigc::mem_fun(this, &Editor::changeTab));
}

Editor::~Editor()
{
    closeAllTab();
}

void Editor::onCloseTab(const std::string& filepath)
{
    mGVLE->closeTab(filepath);
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
        sigc::bind(sigc::mem_fun(this, &Editor::onCloseTab), filepath));

    tabLabel->pack_start(*closeButton, false, false, 0);
    tabLabel->show_all();
    return tabLabel;
}

void Editor::changeTab(GtkNotebookPage* /*page*/, int num)
{
    if (mGVLE->getCurrentTab() != num) {
        Gtk::Widget* tab = get_nth_page(num);
        Documents::iterator it = mDocuments.begin();

        while (it != mDocuments.end()) {
            if (it->second == tab) {
                mGVLE->setCurrentTab(num);
                mGVLE->focusRow(it->first);
                mGVLE->modifications(it->first, it->second->isModified());
                break;
            }
            ++it;
        }
    }
}

void Editor::changeAndSignal(GtkNotebookPage* /*page*/, int num)
{
    if (mGVLE->getCurrentTab() != num) {
        Gtk::Widget* tab = get_nth_page(num);
        Documents::iterator it = mDocuments.begin();

        while (it != mDocuments.end()) {
            if (it->second == tab) {
                mGVLE->setCurrentTab(num);
                mGVLE->modifications(it->first, it->second->isModified());
                break;
            }
            ++it;
        }
    }

    tabSignal.disconnect();
    tabSignal = signal_switch_page().connect(sigc::mem_fun(this,
                                                   &Editor::changeTab));
}

void Editor::refreshTab()
{
    Documents::iterator it = mDocuments.begin();

    while (it != mDocuments.end()) {
        if (it->second == get_nth_page(mGVLE->getCurrentTab()) and
            utils::Path::extension(it->first) != ".vpz")
        {
            mGVLE->getMenu()->onOpenFile();
        }
        ++it;
    }
}

void Editor::changeFile(const std::string& oldName,
                        const std::string& newName)
{
    Documents::iterator it = mDocuments.find(oldName);

    if (it != mDocuments.end()) {
        vpz::BaseModel* model = 0;

        if (utils::Path::extension(oldName) == ".vpz") {
            model = dynamic_cast < DocumentDrawingArea* >(
                it->second)->getModel();
        }
        it->second->setTitle(newName, model, it->second->isModified());
    }
}

void Editor::closeAllTab()
{
    Documents::iterator it = mDocuments.begin();

    while (it != mDocuments.end()) {
        int page = page_num(*it->second);
        remove_page(page);
        delete it->second;
        mDocuments.erase(it->first);
        it = mDocuments.begin();
    }
}

void Editor::closeTab(const std::string& filepath)
{
    Documents::iterator it = mDocuments.find(filepath);

    if (it != mDocuments.end()) {
        int page = page_num(*it->second);

        if (page != -1) {
            remove_page(page);
            delete it->second;
            mDocuments.erase(it->first);
        }
    }
}

void Editor::createBlankNewFile(const std::string& path,
                                const std::string& fileName)
{
    try {
        DocumentText* doc =
            new DocumentText(mGVLE,  path + "/" + fileName, true, true);
        mDocuments.insert(
            std::make_pair < std::string, DocumentText* >
            (path + "/" + fileName, doc));
        append_page(*doc, *(addLabel(fileName,
                                     path + "/" + fileName)));
    } catch (std::exception& e) {
        Error(e.what());
    }
    show_all_children();
}

void Editor::createBlankNewFile()
{
    std::string name = _("untitled file");
    std::string nameTmp;
    int compteur = 0;

    do {
        if (compteur != 0) {
            nameTmp = name + boost::lexical_cast < std::string >(compteur);
        } else {
            nameTmp = name;
        }
        ++compteur;
    } while (existTab(nameTmp));
    name = nameTmp;
    try {
        DocumentText* doc = new DocumentText(mGVLE, nameTmp, true);
        mDocuments.insert(
            std::make_pair < std::string, DocumentText* >(nameTmp, doc));
        append_page(*doc, *(addLabel(doc->filename(),
                                     nameTmp)));
    } catch (std::exception& e) {
        Error(e.what());
    }
    show_all_children();
}

void Editor::closeVpzTab()
{
    Documents::iterator it = mDocuments.begin();

    while (it != mDocuments.end()) {
        if (utils::Path::extension(it->first) == ".vpz") {
            closeTab(it->first);
            break;
        }
        ++it;
    }
}

bool Editor::existVpzTab()
{
    Documents::iterator it = mDocuments.begin();

    while (it != mDocuments.end()) {
        if (utils::Path::extension(it->first) == ".vpz")
            return true;
        ++it;
    }
    return false;
}

void Editor::focusTab(const std::string& filepath)
{
    int page = page_num(*(mDocuments.find(filepath)->second));

    set_current_page(page);
}

void Editor::focusAndSignal(const std::string& filepath)
{
    tabSignal.disconnect();
    tabSignal = signal_switch_page().connect(sigc::mem_fun(this,
                                                  &Editor::changeAndSignal));

    focusTab(filepath);
}

void Editor::onUndo()
{
    int page = get_current_page();

    if (page != -1) {
        Document* doc = dynamic_cast < Document* >(get_nth_page(page));
        doc->undo();
    }
}

void Editor::onRedo()
{
    int page = get_current_page();

    if (page != -1) {
        Document* doc = dynamic_cast < Document* >(get_nth_page(page));
        doc->redo();
    }
}

void Editor::openTab(const std::string& filepath)
{
    if (utils::Path::extension(filepath) != ".vpz") {
try {
    if (mDocuments.find(filepath) == mDocuments.end()) {
        DocumentText* doc = new DocumentText(mGVLE, filepath);
        int page = append_page(*doc,
                               *(addLabel(doc->getTitle(), filepath)));

        mDocuments.insert(std::pair <std::string,
                          DocumentText*>(filepath, doc));
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
    } else {
        mGVLE->parseXML(filepath);
        openTabVpz(mGVLE->getModeling()->getFileName(),
                   mGVLE->getModeling()->getTopModel());
    }
}

void Editor::openTabVpz(const std::string& filepath,
                        vpz::CoupledModel* model)
{
    Documents::iterator it = mDocuments.find(filepath);
    int page;

    if (it != mDocuments.end()) {
        if (dynamic_cast<DocumentDrawingArea*>(it->second)->getModel()
            != model) {
            focusTab(filepath);
            page = get_current_page();
            DocumentDrawingArea* currentTab =
                dynamic_cast<DocumentDrawingArea*>(get_nth_page(page));
            if (currentTab->isComplete()) {
                showCompleteView(filepath, model);
            } else {
                showSimpleView(filepath, model);
            }
        } else {
            return;
        }
    } else {
        showCompleteView(filepath, model);
    }
    getDocumentDrawingArea()->updateCursor();
}

void Editor::showCompleteView(const std::string& filepath,
                              vpz::CoupledModel* model)
{
    Documents::iterator it = mDocuments.find(filepath);
    int page;

    if (it != mDocuments.end()) {
        focusTab(filepath);
        page = get_current_page();
        remove_page(page);
        delete it->second;
        mDocuments.erase(it->first);
        DocumentCompleteDrawingArea* doc = new DocumentCompleteDrawingArea(
            mGVLE,
            filepath,
            mGVLE->findView(model),
            model);
        doc->setTitle(filepath, model, mGVLE->getModeling()->isModified());
        mDocuments.insert(
            std::pair < std::string, DocumentDrawingArea* > (filepath,doc));
        append_page(*doc, *(addLabel(doc->getTitle(),
                                     filepath)));
        reorder_child(*doc, page);
    } else {
        DocumentCompleteDrawingArea* doc = new DocumentCompleteDrawingArea(
            mGVLE,
            filepath,
            mGVLE->findView(model),
            model);
        doc->setTitle(filepath, model, mGVLE->getModeling()->isModified());
        mDocuments.insert(
            std::pair < std::string, DocumentDrawingArea* >(filepath, doc));
        page = append_page(*doc, *(addLabel(doc->getTitle(),
                                            filepath)));
    }

    show_all_children();
    set_current_page(page);
    getDocumentDrawingArea()->updateCursor();
}

void Editor::showSimpleView(const std::string& filepath,
                            vpz::CoupledModel* model)
{
    Documents::iterator it = mDocuments.find(filepath);
    int page;

    if (it != mDocuments.end()) {
        focusTab(filepath);
        page = get_current_page();
        remove_page(page);
        delete it->second;
        mDocuments.erase(it->first);

        DocumentSimpleDrawingArea* doc = new DocumentSimpleDrawingArea(
            mGVLE,
            filepath,
            mGVLE->findView(model),
            model);
        doc->setTitle(filepath, model, mGVLE->getModeling()->isModified());
        mDocuments.insert(
            std::pair < std::string, DocumentDrawingArea* > (filepath, doc));
        append_page(*doc, *(addLabel(doc->getTitle(),
                                     filepath)));

        reorder_child(*doc, page);

    } else {
        return;
    }
    show_all_children();
    set_current_page(page);
    getDocumentDrawingArea()->updateCursor();
}

void Editor::refreshViews()
{
    for (Documents::iterator it = mDocuments.begin();
         it != mDocuments.end(); ++it) {
        it->second->updateView();
    }
}

void Editor::setModifiedTab(const std::string& title,
                            const std::string& newFilePath,
                            const std::string& oldFilePath)
{
    Documents::iterator it = mDocuments.find(oldFilePath);

    if (it != mDocuments.end()) {
        Document* doc = it->second;
        int page = page_num(*doc);
        Gtk::Widget* tab = get_nth_page(page);

        set_tab_label(*tab, *(addLabel(title, newFilePath)));
        if (utils::Path::basename(newFilePath) !=
            utils::Path::basename(oldFilePath)) {
            mDocuments.erase(it);
            mDocuments[newFilePath] = doc;
        }
        mGVLE->setTitle(utils::Path::basename(newFilePath) +
                        utils::Path::extension(newFilePath));
    }
}

}} // namespace vle gvle
