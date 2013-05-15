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


#include <vle/gvle/OpenVpzBox.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/gvle/GVLE.hpp>
#include <vle/gvle/Editor.hpp>
#include <vle/utils/Path.hpp>

namespace vle { namespace gvle {

OpenVpzBox::OpenVpzBox(const Glib::RefPtr < Gtk::Builder >& xml,
                       Modeling* m, GVLE* gvle) :
    mXml(xml),
    mModeling(m),
    mGVLE(gvle)
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

    utils::PathList list = mGVLE->currentPackage().getExperiments(
            vle::utils::PKG_SOURCE);
    utils::PathList::const_iterator it = list.begin();
    while (it != list.end()) {
        Gtk::TreeModel::Row row(*mRefTreeVpz->append());
        size_t begin = mGVLE->currentPackage().getDir(
                vle::utils::PKG_SOURCE).size() + 1;
        std::string name(*it, begin, it->size() - begin - 4);
        row[mColumns.mName] = name;
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
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection =
            mTreeView->get_selection();

    if (refSelection) {
        Gtk::TreeModel::iterator iter = refSelection->get_selected();

        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            std::string name = row.get_value(mColumns.mName);
            std::string pathFile = Glib::build_filename(
                    mGVLE->currentPackage().getDir(
                            vle::utils::PKG_SOURCE), name);

            pathFile += ".vpz";

            mGVLE->parseXML(pathFile);
            mGVLE->getEditor()->openTabVpz(mModeling->getFileName(),
                                           mModeling->getTopModel());
            if (mModeling->getTopModel()) {
                mDialog->response(Gtk::RESPONSE_OK);
            }
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

