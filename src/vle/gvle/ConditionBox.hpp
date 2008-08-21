/**
 * @file vle/gvle/ConditionBox.hpp
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


#ifndef GUI_CONDITIONBOX_HPP
#define GUI_CONDITIONBOX_HPP

#include <gtkmm.h>
#include <vle/vpz/Condition.hpp>
#include <vle/gvle/TreeViewValue.hpp>
#include <vle/gvle/TreeViewPort.hpp>

namespace vle
{
namespace gvle {

class ConditionBox : public Gtk::Dialog
{
public:
    ConditionBox(vpz::Condition* cond);

    virtual ~ConditionBox();

    void run();

protected:
class ModelColumn : public Gtk::TreeModel::ColumnRecord
    {
    public:

        ModelColumn() {
            add(m_col_port);
        }

        Gtk::TreeModelColumn<Glib::ustring> m_col_port;
    };

private:
    vpz::Condition* mCond;
    vpz::Condition* backup;

    Gtk::HBox* mHb_top;

    //PortList
    TreeViewPort* mTreeViewPort;
    //Values
    TreeViewValue* mTreeviewValue;

    void on_port_clicked();
};

}
} // namespace vle gvle

#endif
