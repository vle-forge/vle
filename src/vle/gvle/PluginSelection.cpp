/**
 * @file vle/gvle/PluginSelection.cpp
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


#include <vle/gvle/PluginSelection.hpp>

using std::vector;
using std::pair;
using std::string;

namespace vle
{
namespace gvle {

TreeModelPlugin::TreeModelPlugin()
{
    //We can't just call Gtk::TreeModel(m_Columns) in the initializer list
    //because m_Columns does not exist when the base class constructor runs.
    //And we can't have a static m_Columns instance, because that would be
    //instantiated before the gtkmm type system.
    //So, we use this method, which should only be used just after creation:
    set_column_types(m_Columns);
}

Glib::RefPtr<TreeModelPlugin> TreeModelPlugin::create()
{
    return Glib::RefPtr<TreeModelPlugin>(new TreeModelPlugin());
}

PluginSelection::PluginSelection(Gtk::Window &parent, std::vector < std::pair < std::string , std::string > > plugin_list)
        : Gtk::Dialog("Select plugin", parent, true, true), mpluginList(plugin_list)
{

    mScrolledWindow.add(mTreeViewPlugin);
    mScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    Gtk::VBox *vbox = get_vbox();
    vbox->pack_start(mScrolledWindow, true, true, 0);

    initTreeview();

    fillTreeview();

    Gtk::Button *btn_cancel;
    btn_cancel = add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    mOkButton = add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);


    mOkButton->signal_clicked().connect(sigc::mem_fun(*this, &PluginSelection::on_close));
    btn_cancel->signal_clicked().connect(sigc::mem_fun(*this, &PluginSelection::on_cancel));

    set_default_response(Gtk::RESPONSE_OK);
    show_all();
    hide();
}

void PluginSelection::on_close()
{
    updateSelected();
    eraseSelectedRow();
    hide();
}

void PluginSelection::on_cancel()
{
    hide();
}


PluginSelection::~PluginSelection()
{

}

void PluginSelection::initTreeview()
{
    int nbColonne;
    Gtk::TreeViewColumn *tvc;

    //Create the Tree model:
    //Use our derived model
    mRefTreeModelPlugin = TreeModelPlugin::create(); //The columns are declared in the overridden TreeModel.
    mTreeViewPlugin.set_model(mRefTreeModelPlugin);

    nbColonne = mTreeViewPlugin.append_column("Lib", mRefTreeModelPlugin->m_Columns.m_col_lib);
    tvc = mTreeViewPlugin.get_column(nbColonne-1);
    tvc->set_sort_column(0);
    nbColonne = mTreeViewPlugin.append_column("Output", mRefTreeModelPlugin->m_Columns.m_col_output);

    mRefTreeSelection = mTreeViewPlugin.get_selection();
    mRefTreeSelection->signal_changed().connect(sigc::mem_fun(*this, &PluginSelection::on_selection_changed));
}

void PluginSelection::fillTreeview()
{
    vector < pair < string , string > > :: const_iterator it;

    for (it = mpluginList.begin(); it != mpluginList.end(); ++it) {
        addRow((*it).first, (*it).second);
    }
}

void PluginSelection::addRow(std::string lib, std::string output)
{
    Gtk::TreeModel::Row row = getRow(lib, output);
    row[mRefTreeModelPlugin->m_Columns.m_col_lib] = lib;
    row[mRefTreeModelPlugin->m_Columns.m_col_output] = output;
}

void PluginSelection::addEntry(std::string lib, std::string output)
{
    addRow(lib, output);
}

void PluginSelection::eraseEntry(std::string lib, std::string output)
{
    setSelectedData(lib, output);
    eraseSelectedRow();
}


Gtk::TreeModel::Row PluginSelection::getRow(std::string const &lib, std::string const &output)
{
    Glib::RefPtr<Gtk::TreeModel> tm = mTreeViewPlugin.get_model();
    Gtk::TreePath tp("0");
    Gtk::TreeModel::iterator iter = tm->get_iter(tp);
    Gtk::TreeModel::Row row;
    bool exist = false;

    while (iter != 0 && !exist) {
        row = *iter;
        if (row[mRefTreeModelPlugin->m_Columns.m_col_lib] == lib
                && row[mRefTreeModelPlugin->m_Columns.m_col_output] == output) {
            exist = true;
        }
        ++iter;
    }


    if (!exist)
        row = *(mRefTreeModelPlugin->append());

    return row;
}

std::string PluginSelection::getSelectedLib()
{
    Gtk::TreeModel::iterator iter = mRefTreeSelection->get_selected();
    if (iter) { //If anything is selected
        Gtk::TreeModel::Row row = *iter;
        Glib::ustring select = row[mRefTreeModelPlugin->m_Columns.m_col_lib];
        mSelectedLib = select.raw();
        //Do something with the row.
    }
    return mSelectedLib;
}

std::string PluginSelection::getSelectedOutput()
{
    Gtk::TreeModel::iterator iter = mRefTreeSelection->get_selected();
    if (iter) { //If anything is selected
        Gtk::TreeModel::Row row = *iter;
        Glib::ustring select = row[mRefTreeModelPlugin->m_Columns.m_col_output];
        mSelectedOutput = select.raw();
        //Do something with the row.
    }

    return mSelectedOutput;
}


bool PluginSelection::existRow(std::string lib, std::string output, Gtk::TreeModel::iterator &ret_iter)
{
    Glib::RefPtr<Gtk::TreeModel> tm = mTreeViewPlugin.get_model();
    Gtk::TreePath tp("0");
    Gtk::TreeModel::iterator iter = tm->get_iter(tp);
    Gtk::TreeModel::Row row;
    bool exist = false;

    while (iter != 0 && !exist) {
        row = *iter;
        if (row[mRefTreeModelPlugin->m_Columns.m_col_lib] == lib
                && row[mRefTreeModelPlugin->m_Columns.m_col_output] == output) {
            exist = true;
            ret_iter = iter;
        }
        ++iter;
    }
    return exist;
}

void PluginSelection::setSelectedData(std::string lib, std::string output)
{
    Gtk::TreeModel::iterator iter;

    if (existRow(lib, output, iter)) {
        if (mRefTreeSelection)
            mRefTreeSelection->select(iter);
    }
}

void PluginSelection::updateSelected()
{
    if (mRefTreeSelection) {
        Gtk::TreeModel::iterator iter = mRefTreeSelection->get_selected();
        if (iter) { //If anything is selected
            Gtk::TreeModel::Row row = *iter;
            Glib::ustring select = row[mRefTreeModelPlugin->m_Columns.m_col_lib];
            mSelectedLib = select.raw();
            select = row[mRefTreeModelPlugin->m_Columns.m_col_output];
            mSelectedOutput = select.raw();
            //Do something with the row.
        }
    }
}

void PluginSelection::eraseSelectedRow()
{
    Gtk::TreeModel::iterator iter = mRefTreeSelection->get_selected();
    if (iter)
        mRefTreeModelPlugin->erase(iter);
}

int PluginSelection::run()
{
    if (mRefTreeSelection->get_selected() == 0)
        mOkButton->set_sensitive(false);
    return Gtk::Dialog::run();
}

int PluginSelection::getRowNumber()
{
    int row_number = 0;

    Glib::RefPtr<Gtk::TreeModel> tm = mTreeViewPlugin.get_model();
    Gtk::TreePath tp("0");
    Gtk::TreeModel::iterator iter = tm->get_iter(tp);

    while (iter != 0) {
        row_number++;
        ++iter;
    }
    return row_number;
}

void PluginSelection::on_selection_changed()
{
    if (mOkButton->sensitive() == false)
        mOkButton->set_sensitive(true);
}

void PluginSelection::setListPlugin(std::vector < std::pair < std::string , std::string > > plugin_list)
{
    mpluginList.clear();
    mpluginList = plugin_list;
    mRefTreeModelPlugin->clear();
    fillTreeview();
}

}
} // namespace vle gvle
