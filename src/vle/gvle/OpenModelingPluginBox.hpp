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


#ifndef GUI_OPENMODELINGPLUGINBOX_HPP
#define GUI_OPENMODELINGPLUGINBOX_HPP


#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/dialog.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treestore.h>

namespace vle {
    namespace graph {
        class AtomicModel;
    }

    namespace vpz {
        class AtomicModel;
        class Dynamic;
        class Observables;
        class Conditions;
    }
}

namespace vle { namespace gvle {

class GVLE;

/**
 * @brief A window to choice a package
 */
class OpenModelingPluginBox
{
public:
    OpenModelingPluginBox(const Glib::RefPtr < Gtk::Builder >& xml,
                          GVLE* gvle);

    virtual ~OpenModelingPluginBox();

    std::string pluginName() const
    { return mPluginName; }

    int run();

private:

    class ModelingPluginTreeColumn : public Gtk::TreeModel::ColumnRecord
    {
    public:
        ModelingPluginTreeColumn()
        {
            add(mName);
        }

        Gtk::TreeModelColumn <Glib::ustring> mName;
    };

    Glib::RefPtr < Gtk::Builder >   mXml;
    GVLE*                           mGVLE;

    Gtk::Dialog*                    mDialog;
    ModelingPluginTreeColumn        mColumns;
    Gtk::TreeView*                  mTreeView;
    Glib::RefPtr<Gtk::TreeStore>    mRefTreeModelingPlugin;
    std::string                     mPluginName;

    sigc::connection                mSignalTreeViewRowActivated;
    sigc::connection                mSignalApplyClicked;
    sigc::connection                mSignalCancelClicked;

    //Buttons
    Gtk::Button*                    mButtonApply;
    Gtk::Button*                    mButtonCancel;

    void build();
    void onApply();
    void onCancel();
    void onRowActivated(const Gtk::TreeModel::Path& path,
                        Gtk::TreeViewColumn *column);
};

}} // namespace vle gvle

#endif
