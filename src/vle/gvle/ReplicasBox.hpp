/**
 * @file vle/gvle/ReplicasBox.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
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


#ifndef GUI_REPLICASBOX_HPP
#define GUI_REPLICASBOX_HPP

#include <sigc++/trackable.h>
#include <gtkmm/dialog.h>
#include <gtkmm/button.h>
#include <gtkmm/treeview.h>
#include <gtkmm/label.h>
#include <gtkmm/liststore.h>
#include <gtkmm/spinbutton.h>
#include <libglademm/xml.h>
#include <vle/utils/Rand.hpp>

namespace vle
{
namespace gvle {

/**
 * @brief A dialog box to get the replicas informations from user. Replicas
 * can be build automatically with increment or decrement the first value
 * of the treeview, or using random generator.
 *
 */
class ReplicasBox : public sigc::trackable
{
public:
    /**
     * Build the replicas box.
     *
     */
    ReplicasBox(Glib::RefPtr < Gnome::Glade::Xml > ref);

    /**
     * Nothing to delete.
     *
     *
     */
    ~ReplicasBox();

    /**
     * Start replicas box with empty treeview.
     *
     *
     * @return true if all is ok, false otherwise.
     */
    bool run();

    /**
     * Start replicas box with a specified list of value.
     *
     * @param lst a vector of value.
     *
     * @return true if all is ok, false otherwise.
     */
    bool run(const std::vector < guint32 >& lst);

private:
    /**
     * @brief A simple class to define a ColumnRecord of the ReplicasBox
     * treeview.
     *
     */
class Column : public Gtk::TreeModel::ColumnRecord
    {
    public:
        Column() {
            add(mValue);
        }
        Gtk::TreeModelColumn < guint32 > mValue;
    };

    void clear();

    void onIncrement();

    void onDecrement();

    void onRandom();

    void onSpinButtonNumberChange();

    int                                mNbReplicas;
    Gtk::TreeView*                     mTreeViewValue;
    Gtk::SpinButton*                   mSpinButtonNumber;
    Gtk::Button*                       mButtonRandom;
    Gtk::Button*                       mButtonDecrement;
    Gtk::Button*                       mButtonIncrement;
    Gtk::Dialog*                       mDialog;
    Glib::RefPtr < Gnome::Glade::Xml > mRefXML;
    Column                             mColumn;
    Glib::RefPtr < Gtk::ListStore >    mListStore;
    vle::utils::Rand* mRand;

};

}
} // namespace vle gvle

#endif
