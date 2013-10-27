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
#include <vle/gvle/Editor.hpp>
#include <vle/utils/Path.hpp>
#include <boost/lexical_cast.hpp>

namespace vle { namespace gvle {

QuitBox::QuitBox(const Glib::RefPtr < Gtk::Builder >& xml,
                 GVLE* app) :
    mXml(xml),
    mApp(app)
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
    xml->get_widget("ButtonQuitNotSaved", mButtonClose);
    mButtonClose->signal_clicked().connect(
	sigc::mem_fun(*this, &QuitBox::onClose));
}

void QuitBox::show()
{
    build();
    if (not mFileModified.empty()) {
	if (mFileModified.size() == 1) {
	    mMessage->set_label(_("One document is not saved.\n" \
				  "Do you want to save before closing ?"));
	} else {
	    mMessage->set_label(boost::lexical_cast<std::string>
				(mFileModified.size()) +
				_(" documents are not saved.\n")
				+ _("Do you want to save before closing ?"));
	}
	mDialog->set_title("Open Package");
	mDialog->show_all();
        mDialog->run();
        mDialog->hide();
    } else {
	mApp->hide();
    }
}

void QuitBox::build()
{
    const Editor::Documents& documents(mApp->getEditor()->getDocuments());
    Editor::Documents::const_iterator it = documents.begin();

    mRefTreeFile->clear();
    mFileModified.clear();
    while (it != documents.end()) {
	const Document* tab = it->second;

	if (tab->isModified()) {
	    Gtk::TreeModel::Row row = *(mRefTreeFile->append());

	    row[mColumns.mName] = utils::Path::basename(tab->filepath())
		+ utils::Path::extension(tab->filepath());
	    mFileModified.push_back(tab->filepath());
	}
	++it;
    }
}

void QuitBox::onSave()
{
    Editor* editor = mApp->getEditor();
    std::list < std::string >::iterator it = mFileModified.begin();

    while (it != mFileModified.end()) {
	editor->focusTab(*it);
        mApp->onSave();
	++it;
    }
    
    // no hide_all in GTK 3
    mDialog->hide();
    mApp->hide();
}

void QuitBox::onClose()
{
    mApp->hide();
}

}} // namespace vle gvle
