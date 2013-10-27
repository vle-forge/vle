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


#ifndef VLE_GVLE_OPENPACKAGEBOX_HPP
#define VLE_GVLE_OPENPACKAGEBOX_HPP 1

#include <gtkmm/treeview.h>
#include <gtkmm/dialog.h>
#include <gtkmm/treestore.h>
#include <gtkmm/builder.h>
#include <gtkmm/uimanager.h>
#include <vle/utils/Package.hpp>
#include <gtkmm/treepath.h>
#include <iostream>

namespace vle { namespace gvle {

/**
 * @brief A window to choose a package.
 *
 * This window also enables to remove a package
 */
class OpenPackageBox
{
public:
    OpenPackageBox(const Glib::RefPtr < Gtk::Builder >& xml,
            vle::utils::Package& curr_pack);

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

    Glib::RefPtr < Gtk::Builder >    mXml;

    Gtk::Dialog*                    mDialog;
    PackageTreeColumn               mColumns;
    Gtk::TreeView*                  mTreeView;
    Gtk::Menu*                      mMenuPopup;
    Glib::RefPtr <Gtk::UIManager>   mPopupUIManager;
    Glib::RefPtr <Gtk::ActionGroup> mPopupActionGroup;
    
    Glib::RefPtr < Gtk::TreeStore >     mTreeModel;
    Glib::RefPtr < Gtk::TreeSelection > mTreeSelection;

    std::string                     mName;
    vle::utils::Package&            mCurrentPackage;

    void build();
    void onCancel();

    /**
     * @brief Select the row(=the package)
     *
     * this method is called when the user either double clicks, or
     * press one of the keys Space, Shift+Space, Return or
     * Enter. Except that while a shortcuts as been configured on the
     * dialog, the Return can not launch this method. Nevertheless,
     * this method just propagate the activation of the row by
     * validating the dialog. This is what we want, there is only one
     * thing to choose.
     *
     */
    void onRowActivated(const Gtk::TreeModel::Path& path,
                        Gtk::TreeViewColumn *column);
    void onButtonPressEvent(GdkEventButton* event);

    void onRemove();

    void onRemoveCallBack(const Gtk::TreeModel::iterator& it);

    void onSelectionChanged();

    std::list < sigc::connection > mConnections;
};

}} // namespace vle gvle

#endif
