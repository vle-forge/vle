/**
 * @file vle/gvle/OpenModelingPluginBox.cpp
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


#include <vle/gvle/OpenModelingPluginBox.hpp>
#include <vle/gvle/ModelingPlugin.hpp>
#include <vle/gvle/GVLE.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/vpz/AtomicModels.hpp>
#include <vle/vpz/Dynamic.hpp>
#include <vle/vpz/Conditions.hpp>
#include <vle/vpz/Observables.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Path.hpp>

namespace vle { namespace gvle {

OpenModelingPluginBox::OpenModelingPluginBox(
    Glib::RefPtr<Gnome::Glade::Xml> xml, GVLE* gvle,
    graph::AtomicModel& atom,
    vpz::AtomicModel& vatom,
    vpz::Dynamic& dynamic,
    vpz::Conditions& conditions,
    vpz::Observables& observables)
    : mXml(xml), mGVLE(gvle), mAtom(atom), mVAtom(vatom), mDynamic(dynamic),
    mConditions(conditions), mObservables(observables)
{
    xml->get_widget("DialogModelingPlugin", mDialog);
    xml->get_widget("TreeViewModelingPlugin", mTreeView);
    xml->get_widget("ButtonApplyModelingPlugin", mButtonApply);
    xml->get_widget("ButtonCancelModelingPlugin", mButtonCancel);

    mRefTreeModelingPlugin = Gtk::TreeStore::create(mColumns);
    mTreeView->append_column(_("Name"), mColumns.mName);
    mTreeView->set_model(mRefTreeModelingPlugin);

    mTreeView->signal_row_activated().connect(
        sigc::mem_fun(*this, &OpenModelingPluginBox::onRowActivated));

    mButtonApply->signal_clicked().connect(
	sigc::mem_fun(*this, &OpenModelingPluginBox::onApply));

    mButtonCancel->signal_clicked().connect(
	sigc::mem_fun(*this, &OpenModelingPluginBox::onCancel));
}

OpenModelingPluginBox::~OpenModelingPluginBox()
{
    if (mTreeView) {
	mTreeView->remove_all_columns();
    }
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

    const PluginFactory& plf = mGVLE->pluginFactory();
    const ModelingPluginList& pll = plf.modelingPlugins();

    for (ModelingPluginList::const_iterator it = pll.begin();
	 it != pll.end(); ++it) {
	Gtk::TreeModel::Row row = *(mRefTreeModelingPlugin->append());
	row[mColumns.mName] = utils::Path::basename(it->first);
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
	    std::string name = row.get_value(mColumns.mName);

            SimpleTypeBox box("Name of the new C++ class", "");
            std::string result = box.run();
            if (box.valid()) {
                mDialog->response(execPlugin(name, result));
            } else {
                mDialog->response(Gtk::RESPONSE_CANCEL);
            }
	}
    }
    mDialog->hide_all();
}

void OpenModelingPluginBox::onCancel()
{
    mDialog->response(Gtk::RESPONSE_CANCEL);
    mDialog->hide_all();
}

void OpenModelingPluginBox::onRowActivated(
    const Gtk::TreeModel::Path& /*path*/,
    Gtk::TreeViewColumn* /*column*/)
{
    onApply();
}

int OpenModelingPluginBox::execPlugin(const std::string& pluginname,
                                      const std::string& classname)
{
    PluginFactory& plf = mGVLE->pluginFactory();
    ModelingPlugin& plugin = plf.getModeling(pluginname);

    if (plugin.create(mAtom, mVAtom, mDynamic, mConditions, mObservables,
                      classname)) {
        const std::string& buffer = plugin.source();
        std::string filename = utils::Path::path().getPackageSrcFile(classname);
        filename += ".cpp";

        try {
            std::ofstream f(filename.c_str());
            f.exceptions(std::ofstream::failbit | std::ofstream::badbit);
            f << buffer;
        } catch(const std::ios_base::failure& e) {
            throw utils::ArgError(fmt(
                    _("Cannot store buffer in file '%1%'")) % filename);
        }

        return Gtk::RESPONSE_OK;
    } else {
        return Gtk::RESPONSE_CANCEL;
    }
}

}} // namespace vle gvle
