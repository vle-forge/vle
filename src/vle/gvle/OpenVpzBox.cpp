/**
 * @file vle/gvle/OpenVpzBox.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
 * Copyright (C) 2003-2009 ULCO http://www.univ-littoral.fr
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


#include <vle/gvle/OpenVpzBox.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Path.hpp>

namespace vle { namespace gvle {

OpenVpzBox::OpenVpzBox(Glib::RefPtr<Gnome::Glade::Xml> xml, Modeling* m) :
    mXml(xml),
    mModeling(m)
{
    xml->get_widget("DialogVpz", mDialog);

    xml->get_widget("TreeViewVpz", mTreeView);
    mRefTreeVpz = Gtk::TreeStore::create(mColumns);
    mTreeView->append_column(_("Name"), mColumns.mName);
    mTreeView->set_model(mRefTreeVpz);
    mTreeView->signal_row_activated().connect(
        sigc::mem_fun(*this, &OpenVpzBox::onRowActivated));

    xml->get_widget("ButtonVpzApply", mButtonApply);
    mButtonApply->signal_clicked().connect(
	sigc::mem_fun(*this, &OpenVpzBox::onApply));

    xml->get_widget("ButtonVpzCancel", mButtonCancel);
    mButtonCancel->signal_clicked().connect(
    sigc::mem_fun(*this, &OpenVpzBox::onCancel));

}

int OpenVpzBox::run()
{
    build();
    mDialog->set_title("Open VPZ");
    mDialog->show_all();
    return mDialog->run();
}

void OpenVpzBox::build()
{
    mRefTreeVpz->clear();

    utils::PathList list = utils::Path::path().getInstalledExperiments();
    utils::PathList::const_iterator it = list.begin();
    while (it != list.end()) {
	Gtk::TreeModel::Row row = *(mRefTreeVpz->append());

	row[mColumns.mName] = utils::Path::basename(*it);
	++it;
    }
}

OpenVpzBox::~OpenVpzBox()
{
    if (mTreeView) {
	mTreeView->remove_all_columns();
    }
    delete mButtonApply;
    delete mButtonCancel;
    delete mTreeView;
    delete mDialog;
}

void OpenVpzBox::onApply()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection
	= mTreeView->get_selection();

    if (refSelection) {
	Gtk::TreeModel::iterator iter = refSelection->get_selected();

	if (iter) {
	    Gtk::TreeModel::Row row = *iter;
	    std::string name = row.get_value(mColumns.mName);
	    std::string pathFile = Glib::build_filename(
		utils::Path::path().getPackageExpDir(), name);

	    pathFile += ".vpz";
	    mModeling->parseXML(pathFile);
            mDialog->response(Gtk::RESPONSE_OK);
	}
    }
    mDialog->hide_all();
}

void OpenVpzBox::onCancel()
{
    mDialog->response(Gtk::RESPONSE_CANCEL);
    mDialog->hide_all();
}

void OpenVpzBox::onRowActivated(const Gtk::TreeModel::Path& /* path */,
                                Gtk::TreeViewColumn* /* column */)
{
    onApply();
}

}} //namespace vle gvle

