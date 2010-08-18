/**
 * @file vle/gvle/OpenPackageBox.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
 * Copyright (C) 2003-2010 ULCO http://www.univ-littoral.fr
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
#include <vle/utils/Debug.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Path.hpp>

namespace vle { namespace gvle {

OpenPackageBox::OpenPackageBox(Glib::RefPtr<Gnome::Glade::Xml> xml,
			       Modeling* m) :
    mXml(xml),
    mModeling(m)
{
    xml->get_widget("DialogPackage", mDialog);

    xml->get_widget("TreeViewPackage", mTreeView);
    mRefTreePackage = Gtk::TreeStore::create(mColumns);
    mTreeView->append_column(_("Name"), mColumns.mName);
    mTreeView->set_model(mRefTreePackage);
    mTreeView->signal_row_activated().connect(
        sigc::mem_fun(*this, &OpenPackageBox::onRowActivated));

    xml->get_widget("ButtonPackageApply", mButtonApply);
    mButtonApply->signal_clicked().connect(
	sigc::mem_fun(*this, &OpenPackageBox::onApply));

    xml->get_widget("ButtonPackageCancel", mButtonCancel);
    mButtonCancel->signal_clicked().connect(
	sigc::mem_fun(*this, &OpenPackageBox::onCancel));
}

OpenPackageBox::~OpenPackageBox()
{
    if (mTreeView) {
	mTreeView->remove_all_columns();
    }
    delete mButtonApply;
    delete mButtonCancel;
    delete mTreeView;
    delete mDialog;
}

int OpenPackageBox::run()
{
    build();
    mDialog->set_title("Open Project");
    mDialog->show_all();
    return mDialog->run();
}

void OpenPackageBox::build()
{
    mRefTreePackage->clear();

    utils::PathList list = utils::Path::path().getInstalledPackages();

    std::sort(list.begin(), list.end());
    for (utils::PathList::const_iterator it = list.begin();
	 it != list.end(); ++it) {
	Gtk::TreeModel::Row row = *(mRefTreePackage->append());
        row[mColumns.mName] = utils::Path::filename(*it);
    }
}

void OpenPackageBox::onApply()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection
	= mTreeView->get_selection();

    if (refSelection) {
	Gtk::TreeModel::iterator iter = refSelection->get_selected();

	if (iter) {
	    Gtk::TreeModel::Row row = *iter;

            mName = row.get_value(mColumns.mName);
            mDialog->response(Gtk::RESPONSE_OK);
	}
    }
    mDialog->hide_all();
}

void OpenPackageBox::onCancel()
{
    mDialog->response(Gtk::RESPONSE_CANCEL);
    mDialog->hide_all();
}

void OpenPackageBox::onRowActivated(const Gtk::TreeModel::Path& /* path */,
                                    Gtk::TreeViewColumn* /* column */)
{
    onApply();
}

}} // namespace vle gvle
