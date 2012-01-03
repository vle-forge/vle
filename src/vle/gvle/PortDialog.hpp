/*
 * @file vle/gvle/PortDialog.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2012 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2012 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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


#ifndef GUI_PORTDIALOG_HPP
#define GUI_PORTDIALOG_HPP

#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <gtkmm/treeview.h>
#include <gtkmm/liststore.h>
#include <gtkmm/scrolledwindow.h>
#include <set>

namespace vle
{
namespace graph {

class Model;

}
} // namespace vle graph

namespace vle
{
namespace gvle {

/**
 * @brief Manage a port for graphics model.
 */
class PortDialog : public Gtk::Dialog
{
public:
    enum PortType { INPUT, OUTPUT };

    /**
     * @brief Create a new port dialog to insert a new port in a
     * graph::Model automatically.
     *
     * @param model graph::Model to insert a new port.
     * @param type dialog box string to put in label.
     */
    PortDialog(graph::Model* model, PortDialog::PortType type);

    /**
     * @brief Nothing to delete
     *
     */
    virtual ~PortDialog() {}

    /**
     * @brief Run dialog and verify inputs of user and affect
     * graph::Model if good inputs
     *
     * @return true if successfull modification, otherwise false
     */
    bool run();

private:
    /**
     * @brief Get all inputs or outputs port from model and show it
     *
     */
    void fillTreeView();

class Column : public Gtk::TreeModel::ColumnRecord
    {
    public:
        Column() {
            add(mName);
        }

        Gtk::TreeModelColumn<Glib::ustring> mName;
    };

    Gtk::Label                      mLabelDialogName;
    Gtk::Label                      mLabelModelName;
    Gtk::HBox                       mHBox;
    Gtk::Label                      mLabelEntry;
    Gtk::Entry                      mEntryPortName;
    Gtk::ScrolledWindow             mScrolledWindow;
    Gtk::TreeView                   mTreeViewPorts;
    Glib::RefPtr < Gtk::ListStore > mListStore;
    Column                          mColumn;
    graph::Model*              mModel;
    std::set < std::string >        mSet;
    PortDialog::PortType            mPortType;
};

}
} // namespace vle gvle

#endif
