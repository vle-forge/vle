/**
 * @file vle/gvle/CoupledModelBox.cpp
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


#include <vle/gvle/CoupledModelBox.hpp>
#include <vle/gvle/PortDialog.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <vle/utils/Tools.hpp>
#include <gtkmm/stock.h>

using namespace vle;

namespace vle
{
namespace gvle {

CoupledModelBox::CoupledModelBox(Glib::RefPtr<Gnome::Glade::Xml> xml, Modeling* m):
        mXml(xml),
        mModeling(m),
        mModel(0)
{
    xml->get_widget("DialogCoupledModel", mDialog);

    xml->get_widget("CoupledModelName", mModelName);
    xml->get_widget("CoupledModelNbChildren", mModelNbChildren);

    xml->get_widget("CoupledModelTreeViewInput", mTreeViewInput);
    mRefListInput = Gtk::ListStore::create(mColumnsInput);
    mTreeViewInput->set_model(mRefListInput);
    mTreeViewInput->append_column("Input Ports", mColumnsInput.m_col_name);

    xml->get_widget("CoupledModelTreeViewOutput", mTreeViewOutput);
    mRefListOutput = Gtk::ListStore::create(mColumnsOutput);
    mTreeViewOutput->set_model(mRefListOutput);
    mTreeViewOutput->append_column("Output Ports", mColumnsOutput.m_col_name);

    xml->get_widget("CoupledModelAddInput", mButtonAddInput);
    mButtonAddInput->signal_clicked().connect(
        sigc::mem_fun(*this, &CoupledModelBox::add_input));

    xml->get_widget("CoupledModelDelInput", mButtonDelInput);
    mButtonDelInput->signal_clicked().connect(
        sigc::mem_fun(*this, &CoupledModelBox::del_input));

    xml->get_widget("CoupledModelAddOutput", mButtonAddOutput);
    mButtonAddOutput->signal_clicked().connect(
        sigc::mem_fun(*this, &CoupledModelBox::add_output));

    xml->get_widget("CoupledModelDelOutput", mButtonDelOutput);
    mButtonDelOutput->signal_clicked().connect(
        sigc::mem_fun(*this, &CoupledModelBox::del_output));

    xml->get_widget("ButtonCoupledValidate", mButtonValidate);
    mButtonValidate->signal_clicked().connect(
        sigc::mem_fun(*this, &CoupledModelBox::on_validate));

}

void CoupledModelBox::show(graph::CoupledModel* model)
{
    mModel = model;

    mModelName->set_text(model->getName());
    mModelNbChildren->set_text(utils::to_string(model->getModelList().size()));

    make_input();
    make_output();

    mDialog->show_all();
    mDialog->run();
}

void CoupledModelBox::make_input()
{
    using namespace graph;

    mRefListInput->clear();
    ConnectionList list = mModel->getInputPortList();
    ConnectionList::const_iterator it = list.begin();
    while (it != list.end()) {
        Gtk::TreeModel::Row row = *(mRefListInput->append());
        row[mColumnsInput.m_col_name] = it->first;

        ++it;
    }
}

void CoupledModelBox::make_output()
{
    using namespace graph;

    mRefListOutput->clear();
    ConnectionList list = mModel->getOutputPortList();
    ConnectionList::const_iterator it = list.begin();
    while (it != list.end()) {
        Gtk::TreeModel::Row row = *(mRefListOutput->append());
        row[mColumnsOutput.m_col_name] = it->first;

        ++it;
    }
}

void CoupledModelBox::add_input()
{
    PortDialog box(mModel, PortDialog::INPUT);
    if (box.run()) {
        make_input();
    }
}

void CoupledModelBox::del_input()
{
    Glib::RefPtr<Gtk::TreeSelection> selection = mTreeViewInput->get_selection();
    Gtk::TreeModel::iterator iter = selection->get_selected();
    if (iter) {
        Gtk::TreeModel::Row row = *iter;
        Glib::ustring name(row[mColumnsInput.m_col_name]);
        mModel->delInputPort(name);
        make_input();
    }
}

void CoupledModelBox::add_output()
{
    PortDialog box(mModel, PortDialog::OUTPUT);
    if (box.run()) {
        make_output();
    }
}

void CoupledModelBox::del_output()
{
    Glib::RefPtr<Gtk::TreeSelection> selection = mTreeViewOutput->get_selection();
    Gtk::TreeModel::iterator iter = selection->get_selected();
    if (iter) {
        Gtk::TreeModel::Row row = *iter;
        Glib::ustring name(row[mColumnsOutput.m_col_name]);
        mModel->delOutputPort(name);
        make_output();
    }
}

void CoupledModelBox::on_validate()
{
    mDialog->hide_all();
}

}
} // namespace vle gvle
