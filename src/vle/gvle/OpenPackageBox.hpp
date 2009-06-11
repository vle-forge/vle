/**
 * @file vle/gvle/OpenPackageBox.hpp
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

#ifndef GUI_OPENPACKAGEBOX_HPP
#define GUI_OPENPACKAGEBOX_HPP

#include <gtkmm.h>
#include <libglademm.h>
#include <vle/utils/Path.hpp>


namespace vle
{
namespace gvle {

class Modeling;

/**
 * @brief A window to choice a package
 */
class OpenPackageBox
{
public:

    OpenPackageBox(Glib::RefPtr<Gnome::Glade::Xml> xml, Modeling* m);
    ~OpenPackageBox();

    void show();
    void build();

protected:

    class PackageTreeColumn : public Gtk::TreeModel::ColumnRecord
    {
    public:
        PackageTreeColumn() {
            add(mName);
        }

        Gtk::TreeModelColumn <Glib::ustring> mName;
    };

    Glib::RefPtr<Gnome::Glade::Xml> mXml;

    Gtk::Dialog*                    mDialog;
    PackageTreeColumn               mColumns;
    Gtk::TreeView*                  mTreeView;
    Glib::RefPtr<Gtk::TreeStore>    mRefTreePackage;
    Modeling*                       mModeling;

    //Buttons
    Gtk::Button*                    mButtonApply;
    Gtk::Button*                    mButtonCancel;

    void onApply();
    void onCancel();
};

}
}

#endif
