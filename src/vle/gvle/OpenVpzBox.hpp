/**
 * @file vle/gvle/OpenVpzBox.hpp
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


#ifndef GUI_OPENVPZBOX_HPP
#define GUI_OPENVPZBOX_HPP

#include <gtkmm.h>
#include <libglademm.h>
#include <vle/utils/Path.hpp>

class Modeling;

namespace vle { namespace gvle {

class Modeling;

/**
 * @brief A window to choice a vpz into a package
 */
class OpenVpzBox
{
public:

   OpenVpzBox(Glib::RefPtr<Gnome::Glade::Xml> xml, Modeling* m);
    virtual ~OpenVpzBox();

    int run();

private:

    class VpzTreeColumn : public Gtk::TreeModel::ColumnRecord
    {
    public:
        VpzTreeColumn()
        {
            add(mName);
        }

        Gtk::TreeModelColumn <Glib::ustring> mName;
    };

    Glib::RefPtr<Gnome::Glade::Xml> mXml;
    Gtk::Dialog*                    mDialog;
    Modeling*                       mModeling;
    VpzTreeColumn                   mColumns;
    Gtk::TreeView*                  mTreeView;
    Glib::RefPtr<Gtk::TreeStore>    mRefTreeVpz;

    // Buttons
    Gtk::Button*                    mButtonApply;
    Gtk::Button*                    mButtonCancel;

    void build();
    void onApply();
    void onCancel();
    void onRowActivated(const Gtk::TreeModel::Path& path,
                        Gtk::TreeViewColumn* column);
};

}} // namespace vle gvle

#endif
