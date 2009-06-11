/**
 * @file vle/gvle/OpenPackageBox.cpp
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
#include <vle/utils/Debug.hpp>
#include <vle/utils/Package.hpp>
#include <boost/filesystem.hpp>

namespace vle
{
namespace gvle {

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

void OpenPackageBox::show()
{
    build();
    mDialog->set_title("Open Package");
    mDialog->show_all();
    mDialog->run();
}


void OpenPackageBox::build()
{
    using namespace utils;

    mRefTreePackage->clear();

    PathList list = CMakePackage::getInstalledPackages();
    PathList::const_iterator it = list.begin();
    while (it != list.end()) {
	Gtk::TreeModel::Row row = *(mRefTreePackage->append());
	row[mColumns.mName] = boost::filesystem::basename(*it);
	++it;
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
	    std::string name = row.get_value(mColumns.mName);
	    utils::Path::path().setPackage(name);
	}
    }
    mDialog->hide_all();
}

void OpenPackageBox::onCancel()
{
    mDialog->hide_all();
}

}
} // namespace vle gvle
