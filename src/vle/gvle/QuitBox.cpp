/**
 * @file vle/gvle/QuitBox.cpp
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

#include <vle/gvle/OpenPackageBox.hpp>
#include <vle/gvle/Modeling.hpp>
#include <boost/filesystem.hpp>
#include <vle/gvle/Editor.hpp>

namespace vle
{
namespace gvle {

QuitBox::QuitBox(Glib::RefPtr<Gnome::Glade::Xml> xml, Modeling* m):
    mXml(xml),
    mModeling(m)
{
    xml->get_widget("DialogQuit", mDialog);

    xml->get_widget("FileNotSavedTreeView", mTreeView);
    mRefTreeFile = Gtk::TreeStore::create(mColumns);
    mTreeView->append_column(_("Name"), mColumns.mName);
    mTreeView->set_model(mRefTreeFile);

    xml->get_widget("LabelQuit", mMessage);

    xml->get_widget("ButtonQuitSaved", mButtonSave);
    mButtonSave->signal_clicked().connect(
	sigc::mem_fun(*this, &QuitBox::onSave));

    xml->get_widget("ButtonQuitCancel", mButtonCancel);
    mButtonCancel->signal_clicked().connect(
	sigc::mem_fun(*this, &QuitBox::onCancel));

    xml->get_widget("ButtonQuitNotSaved", mButtonClose);
    mButtonClose->signal_clicked().connect(
	sigc::mem_fun(*this, &QuitBox::onClose));
}

QuitBox::~QuitBox()
{
}

void QuitBox::show()
{
    build();
    if (mFileModified.size() != 0) {
	mMessage->set_label(boost::lexical_cast<std::string>
			   (mFileModified.size()) +
			   _(" documents are not saved\n")
			   + _("Do you want to saved before close ?"));

	mDialog->set_title("Open Package");
	mDialog->show_all();
	mDialog->run();
    } else {
	mModeling->getGVLE()->hide();
    }
}

void QuitBox::build()
{
    mRefTreeFile->clear();
    mFileModified.clear();
    Editor::Documents documents = mModeling->getGVLE()->getEditor()
	->getDocumentsList();
    Editor::Documents::iterator it = documents.begin();
    while (it != documents.end()) {
	int page = mModeling->getGVLE()->getEditor()->page_num(*it->second);
	Document* tab = dynamic_cast<Document*>(
	    mModeling->getGVLE()->getEditor()->get_nth_page(page));
	if (tab->isModified()) {
	    Gtk::TreeModel::Row row = *(mRefTreeFile->append());
	    row[mColumns.mName] = boost::filesystem::basename(tab->filepath())
		+ boost::filesystem::extension(tab->filepath());
	    mFileModified.push_back(tab->filepath());
	}
	++it;
    }
}

void QuitBox::onSave()
{
    Editor* editor = mModeling->getGVLE()->getEditor();
    std::list < std::string >::iterator it = mFileModified.begin();
    while (it != mFileModified.end()) {
	editor->focusTab(*it);
	if (boost::filesystem::extension(*it) == ".vpz")
	    mModeling->getGVLE()->onMenuSave();
	else
	    mModeling->getGVLE()->saveFile();
	++it;
    }
    mModeling->getGVLE()->hide();
}

void QuitBox::onCancel()
{
    mDialog->hide_all();
}

void QuitBox::onClose()
{
    mModeling->getGVLE()->hide();
}


}
} // namespace vle gvle
