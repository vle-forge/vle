/**
 * @file vle/gvle/ModelClassBox.cpp
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


#include <vle/gvle/ModelClassBox.hpp>
#include <vle/gvle/Modeling.hpp>
#include <boost/algorithm/string/detail/trim.hpp>
#include <vle/gvle/View.hpp>

namespace vle
{
namespace gvle {

ModelClassBox::ModelClassBox(Glib::RefPtr<Gnome::Glade::Xml> xml, Modeling* m):
        mXml(xml),
        mModeling(m),
        mNewModelBox(new NewModelClassBox(xml,m)),
        mClasses_backup(0)
{
    xml->get_widget("WindowModelClass", mWindow);

    xml->get_widget("TreeViewModelClass", mTreeView);
    mRefTreeModel = Gtk::ListStore::create(mColumns);
    mTreeView->set_model(mRefTreeModel);
    mTreeView->append_column("Name", mColumns.m_col_name);

    xml->get_widget("ButtonAddModelClass", mButtonAdd);
    mButtonAdd->signal_clicked().connect(
        sigc::mem_fun(*this, &ModelClassBox::on_add));

    xml->get_widget("ButtonEditModelClass", mButtonEdit);
    mButtonEdit->signal_clicked().connect(
        sigc::mem_fun(*this, &ModelClassBox::on_edit));

    xml->get_widget("ButtonDelModelClass", mButtonDel);
    mButtonDel->signal_clicked().connect(
        sigc::mem_fun(*this, &ModelClassBox::on_del));

    xml->get_widget("ButtonApplyClassModel", mButtonValidate);
    mButtonValidate->signal_clicked().connect(
        sigc::mem_fun(*this, &ModelClassBox::on_validate));

    xml->get_widget("ButtonCancelClassModel", mButtonCancel);
    mButtonCancel->signal_clicked().connect(
        sigc::mem_fun(*this, &ModelClassBox::on_cancel));
}

ModelClassBox::~ModelClassBox()
{
    mWindow->hide_all();

    delete mNewModelBox;
}

void ModelClassBox::show()
{
    using namespace vpz;

    mClasses_backup = new vpz::ClassList(mModeling->vpz().project().classes().list());

    make_treeview();

    mWindow->show_all();
}

void ModelClassBox::hide()
{
    mWindow->hide_all();
}

void ModelClassBox::make_treeview()
{
    using namespace vpz;
    mRefTreeModel->clear();

    ClassList& class_list = mModeling->vpz().project().classes().list();
    ClassList::iterator it = class_list.begin();
    while (it != class_list.end()) {
        Gtk::TreeModel::Row row = *(mRefTreeModel->append());
        row[mColumns.m_col_name] = it->first;

        ++it;
    }
}

void ModelClassBox::on_add()
{
    mNewModelBox->run();
    make_treeview();
}

void ModelClassBox::on_edit()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection =  mTreeView->get_selection();
    if (refSelection) {
        Gtk::TreeModel::iterator iter = refSelection->get_selected();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            vpz::Classes classes = mModeling->vpz().project().classes();
            vpz::Class& c = classes.get(row.get_value(mColumns.m_col_name));
            mModeling->addView(c.model());
        }
    }
}

void ModelClassBox::on_del()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection =  mTreeView->get_selection();
    if (refSelection) {
        Gtk::TreeModel::iterator iter = refSelection->get_selected();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;

            vpz::Class& class_ = mModeling->vpz().project().classes().get(row.get_value(mColumns.m_col_name));
            if (class_.model()->isCoupled()) {
                graph::CoupledModel* c_model = dynamic_cast<graph::CoupledModel*>(class_.model());
                if (mModeling->existView(c_model)) {
                    mModeling->delViewOnModel(c_model);
                }
            }
            mModeling->vpz().project().classes().del(row.get_value(mColumns.m_col_name));
            make_treeview();
        }
    }
}

void ModelClassBox::on_validate()
{
    mWindow->hide_all();
    delete mClasses_backup;
}

void ModelClassBox::on_cancel()
{
    mWindow->hide_all();

    vpz::ClassList& class_list = mModeling->vpz().project().classes().list();
    vpz::ClassList::iterator it = mClasses_backup->begin();
    while (it != mClasses_backup->end()) {
        class_list.insert(*it);

        ++it;
    }

    delete mClasses_backup;
}

}
} // namespace vle gvle
