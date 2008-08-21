/**
 * @file vle/gvle/CoupledModelBox.hpp
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


#ifndef GUI_COUPLEDMODELBOX_HPP
#define GUI_COUPLEDMODELBOX_HPP

#include <gtkmm.h>
#include <libglademm.h>
#include <vle/graph/CoupledModel.hpp>

namespace vle
{
namespace graph {

class CoupledModel;

}
} // namespace vle graph

namespace vle
{
namespace gvle {

class Modeling;

class CoupledModelBox
{
public:
    CoupledModelBox(Glib::RefPtr<Gnome::Glade::Xml> xml, Modeling* m);

    void show(graph::CoupledModel* model);

protected:
class ModelColumns : public Gtk::TreeModelColumnRecord
    {
    public:

        ModelColumns() {
            add(m_col_name);
        }

        Gtk::TreeModelColumn<Glib::ustring> m_col_name;
    };
private:
    void make_input();
    void make_output();

    void add_input();
    void del_input();

    void add_output();
    void del_output();

    void on_validate();

    Glib::RefPtr<Gnome::Glade::Xml>        mXml;
    Modeling*                              mModeling;
    graph::CoupledModel*                   mModel;

    Gtk::Dialog*                           mDialog;
    Gtk::Label*                            mModelName;
    Gtk::Label*                            mModelNbChildren;

    Gtk::TreeView*                         mTreeViewInput;
    ModelColumns                           mColumnsInput;
    Glib::RefPtr<Gtk::ListStore>           mRefListInput;

    Gtk::TreeView*                         mTreeViewOutput;
    ModelColumns                           mColumnsOutput;
    Glib::RefPtr<Gtk::ListStore>           mRefListOutput;

    Gtk::Button*                           mButtonAddInput;
    Gtk::Button*                           mButtonDelInput;

    Gtk::Button*                           mButtonAddOutput;
    Gtk::Button*                           mButtonDelOutput;

    Gtk::Button*                           mButtonValidate;
};

}
} // namespace vle gvle

#endif
