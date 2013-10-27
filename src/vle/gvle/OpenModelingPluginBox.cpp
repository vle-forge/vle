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


#include <vle/gvle/OpenModelingPluginBox.hpp>
#include <vle/gvle/ModelingPlugin.hpp>
#include <vle/gvle/GVLE.hpp>

namespace vle { namespace gvle {

OpenModelingPluginBox::OpenModelingPluginBox(
    const Glib::RefPtr < Gtk::Builder >& xml, GVLE* gvle)
    : mXml(xml), mGVLE(gvle)
{
    xml->get_widget("DialogModelingPlugin", mDialog);
    xml->get_widget("TreeViewModelingPlugin", mTreeView);
    xml->get_widget("ButtonApplyModelingPlugin", mButtonApply);
    xml->get_widget("ButtonCancelModelingPlugin", mButtonCancel);

    mRefTreeModelingPlugin = Gtk::TreeStore::create(mColumns);
    mTreeView->append_column(_("Name"), mColumns.mName);
    mTreeView->set_model(mRefTreeModelingPlugin);

    mSignalTreeViewRowActivated = mTreeView->signal_row_activated().connect(
        sigc::mem_fun(*this, &OpenModelingPluginBox::onRowActivated));

    mSignalApplyClicked = mButtonApply->signal_clicked().connect(
	sigc::mem_fun(*this, &OpenModelingPluginBox::onApply));

    mSignalCancelClicked = mButtonCancel->signal_clicked().connect(
	sigc::mem_fun(*this, &OpenModelingPluginBox::onCancel));
}

OpenModelingPluginBox::~OpenModelingPluginBox()
{
    if (mTreeView) {
	mTreeView->remove_all_columns();
    }

    mSignalCancelClicked.disconnect();
    mSignalApplyClicked.disconnect();
    mSignalTreeViewRowActivated.disconnect();
}

int OpenModelingPluginBox::run()
{
    build();
    mDialog->set_title("Modeling plugin list");
    mDialog->show_all();
    return mDialog->run();
}

void OpenModelingPluginBox::build()
{
    mRefTreeModelingPlugin->clear();

    utils::ModuleList lst;
    utils::ModuleList::iterator it;

    mGVLE->pluginFactory().getGvleModelingPlugins(&lst);

    for (it = lst.begin(); it != lst.end(); ++it) {
	Gtk::TreeModel::Row row = *(mRefTreeModelingPlugin->append());
	row[mColumns.mName] = it->package + "/" + it->library;
    }
}

void OpenModelingPluginBox::onApply()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection
	= mTreeView->get_selection();

    if (refSelection) {
	Gtk::TreeModel::iterator iter = refSelection->get_selected();

	if (iter) {
	    Gtk::TreeModel::Row row = *iter;

	    mPluginName = row.get_value(mColumns.mName);
            mDialog->response(Gtk::RESPONSE_OK);
	}
    }
    // Gtk 3 no hide_all
    mDialog->hide();
}

void OpenModelingPluginBox::onCancel()
{
    mDialog->response(Gtk::RESPONSE_CANCEL);
    // Gtk 3 no hide_all
    mDialog->hide();
}

void OpenModelingPluginBox::onRowActivated(
    const Gtk::TreeModel::Path& /*path*/,
    Gtk::TreeViewColumn* /*column*/)
{
    onApply();
}

}} // namespace vle gvle
