/*
 * @file vle/gvle/OpenPackageBox.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
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


#ifndef VLE_GVLE_OPENPACKAGEBOX_HPP
#define VLE_GVLE_OPENPACKAGEBOX_HPP 1

#include <gtkmm/treeview.h>
#include <gtkmm/dialog.h>
#include <gtkmm/treestore.h>
#include <libglademm/xml.h>

namespace vle { namespace gvle {

/**
 * @brief A window to choice a package.
 */
class OpenPackageBox
{
public:
    OpenPackageBox(Glib::RefPtr<Gnome::Glade::Xml> xml);

    virtual ~OpenPackageBox();

    const std::string& name() const
    { return mName; }

    /**
     * @brief Show the dialog box and run it.
     *
     * @return true if the user correctly fill the entry, false otherwise.
     */
    bool run();

private:
    class PackageTreeColumn : public Gtk::TreeModel::ColumnRecord
    {
    public:
        PackageTreeColumn()
        {
            add(mName);
        }

        Gtk::TreeModelColumn <Glib::ustring> mName;
    };

    Glib::RefPtr<Gnome::Glade::Xml> mXml;

    Gtk::Dialog*                    mDialog;
    PackageTreeColumn               mColumns;
    Gtk::TreeView*                  mTreeView;
    Gtk::Menu                       mMenuPopup;

    Glib::RefPtr < Gtk::TreeStore >     mTreeModel;
    Glib::RefPtr < Gtk::TreeSelection > mTreeSelection;

    std::string                     mName;

    void build();
    void onCancel();
    void onRowActivated(const Gtk::TreeModel::Path& path,
                        Gtk::TreeViewColumn *column);
    bool onEvent(GdkEvent* event);

    void onRemove();
    void onRemoveCallBack(const Gtk::TreeModel::iterator& it);

    std::list < sigc::connection > mConnections;
};

}} // namespace vle gvle

#endif
