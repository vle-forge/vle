/**
 * @file vle/gvle/SourceDialog.cpp
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

#include <vle/gvle/modeling/DifferenceEquation/SourceDialog.hpp>
#include <vle/gvle/Settings.hpp>

namespace vle { namespace gvle { namespace modeling {

Source::Source(const std::string& buffer)
{
#ifdef VLE_HAVE_GTKSOURCEVIEWMM
    gtksourceview::init();
#endif
    init(buffer);
}

std::string Source::getBuffer() const
{
#ifdef VLE_HAVE_GTKSOURCEVIEWMM
    return mView.get_source_buffer()->get_text();
#else
    return mView.get_buffer()->get_text();
#endif
}

void Source::init(const std::string& buffer)
{
#ifdef VLE_HAVE_GTKSOURCEVIEWMM
    Glib::RefPtr<gtksourceview::SourceLanguageManager> manager
	= gtksourceview::SourceLanguageManager::create();
    Glib::RefPtr<gtksourceview::SourceLanguage> language =
	manager->get_language("cpp");
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
    applyEditingProperties();
    add(mView);
}

void Source::applyEditingProperties()
{
#ifdef VLE_HAVE_GTKSOURCEVIEWMM
    mView.get_source_buffer()->
	set_highlight_syntax(Settings::settings().getHighlightSyntax());
    mView.get_source_buffer()->
	set_highlight_matching_brackets(
            Settings::settings().getHighlightBrackets());
    mView.set_highlight_current_line(Settings::settings().getHighlightLine());
    mView.set_show_line_numbers(Settings::settings().getLineNumbers());
    mView.set_show_right_margin(Settings::settings().getRightMargin());
    mView.set_auto_indent(Settings::settings().getAutoIndent());
    mView.set_indent_on_tab(Settings::settings().getIndentOnTab());
    mView.set_indent_width(Settings::settings().getIndentSize());
    if (Settings::settings().getSmartHomeEnd())
	mView.set_smart_home_end(gtksourceview::SOURCE_SMART_HOME_END_ALWAYS);
#endif
    Pango::FontDescription font = Pango::FontDescription(
	Settings::settings().getFontEditor());
    mView.modify_font(font);
}

void Source::undo()
{
#ifdef VLE_HAVE_GTKSOURCEVIEWMM
    mView.get_source_buffer()->undo();
#endif
}

void Source::redo()
{
#ifdef VLE_HAVE_GTKSOURCEVIEWMM
    mView.get_source_buffer()->redo();
#endif
}

void Source::paste()
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

void Source::copy()
{
#ifdef VLE_HAVE_GTKSOURCEVIEWMM
    Glib::RefPtr<gtksourceview::SourceBuffer> buffer(mView.get_source_buffer());
#else
    Glib::RefPtr<Gtk::TextBuffer> buffer(mView.get_buffer());
#endif

    Glib::RefPtr<Gtk::Clipboard> clipboard(Gtk::Clipboard::get());
    buffer->copy_clipboard(clipboard);
}

void Source::cut()
{
#ifdef VLE_HAVE_GTKSOURCEVIEWMM
    Glib::RefPtr<gtksourceview::SourceBuffer> buffer(mView.get_source_buffer());
#else
    Glib::RefPtr<Gtk::TextBuffer> buffer(mView.get_buffer());
#endif

    Glib::RefPtr<Gtk::Clipboard> clipboard(Gtk::Clipboard::get());
    buffer->cut_clipboard(clipboard);
}

SourceDialog::SourceDialog(Glib::RefPtr<Gnome::Glade::Xml> xml,
                           const std::string& computeFunction,
                           const std::string& initValueFunction,
                           const std::string& userFunctions) :
    mXml(xml)
{
    xml->get_widget("DialogSourceBox", mDialog);
    xml->get_widget("NotebookSource", mNotebook);

    mComputeFunction = Gtk::manage(new Source(computeFunction));
    mInitValueFunction = Gtk::manage(new Source(initValueFunction));
    mUserFunctions = Gtk::manage(new Source(userFunctions));

    mNotebook->append_page(*mInitValueFunction, std::string("InitValue"));
    mNotebook->append_page(*mComputeFunction, std::string("Compute"));
    mNotebook->append_page(*mUserFunctions, std::string("User"));
}

SourceDialog::~SourceDialog()
{
    mDialog->hide_all();
}

std::string SourceDialog::getComputeFunction() const
{
    return mComputeFunction->getBuffer();
}

std::string SourceDialog::getInitValueFunction() const
{
    return mInitValueFunction->getBuffer();
}

std::string SourceDialog::getUserFunctions() const
{
    return mUserFunctions->getBuffer();
}

int SourceDialog::run()
{
    mDialog->show_all();
    return mDialog->run();
}

}}} // namespace vle gvle modeling
