/**
 * @file vle/gvle/AtomicModelBox.hpp
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


#ifndef GUI_ATOMICMODELBOX_HPP
#define GUI_ATOMICMODELBOX_HPP

#include <gtkmm.h>
#include <vle/graph/AtomicModel.hpp>
#include <vle/gvle/DynamicBox.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/gvle/ObsAndViewBox.hpp>
#include <vle/gvle/TreeViewConditions.hpp>
#include <vle/gvle/TreeViewModelPort.hpp>

namespace vle
{
namespace gvle {

class Modeling;

class AtomicModelBox
{
public:
    AtomicModelBox(Glib::RefPtr<Gnome::Glade::Xml> xml, Modeling* m);

    virtual ~AtomicModelBox();

    void show(vpz::AtomicModel& atom,  graph::AtomicModel& model);

protected:
class ModelColumnsPort : public Gtk::TreeModel::ColumnRecord
    {
    public:

        ModelColumnsPort() {
            add(m_col_name);
        }

        Gtk::TreeModelColumn<Glib::ustring> m_col_name;
    };

class ModelColumnsDyn : public Gtk::TreeModel::ColumnRecord
    {
    public:

        ModelColumnsDyn() {
            add(m_name);
            add(m_dyn);
        }

        Gtk::TreeModelColumn<Glib::ustring> m_name;
        Gtk::TreeModelColumn<Glib::ustring> m_dyn;
    };

class ModelColumnsCond : public Gtk::TreeModel::ColumnRecord
    {
    public:

        ModelColumnsCond() {
            add(m_col_name);
            add(m_col_activ);
        }

        Gtk::TreeModelColumn<Glib::ustring> m_col_name;
        Gtk::TreeModelColumn<bool> m_col_activ;
    };

private:
    Glib::RefPtr<Gnome::Glade::Xml>      mXml;
    Modeling*                            mModeling;

    Gtk::Dialog*                         mDialog;

    //Data
    vpz::AtomicModel*                    mAtom;
    graph::AtomicModel*                  mGraph_atom;
    vpz::Dynamics*                       mDyn;
    vpz::Observables*                    mObs;
    vpz::Views*                          mViews;
    vpz::Conditions*                     mCond;
    vpz::Outputs*                        mOutputs;

    //Backup
    vpz::AtomicModel*                    mAtom_backup;
    vpz::Dynamics*                       mDyn_backup;
    vpz::Observables*                    mObs_backup;
    vpz::Views*                          mViews_backup;
    vpz::Conditions*                     mCond_backup;
    vpz::Outputs*                        mOutputs_backup;
    graph::ConnectionList*               mConnection_in_backup;
    graph::ConnectionList*               mConnection_out_backup;

    //Input Ports
    Gtk::TreeView*                       mInputPorts;
    ModelColumnsPort                     m_ColumnsInputPort;
    Glib::RefPtr<Gtk::ListStore>         mRefTreeModelInputPort;
    Gtk::Button*                         mAddInputPort;
    Gtk::Button*                         mDelInputPort;

    //Output Ports
    Gtk::TreeView*                       mOutputPorts;
    ModelColumnsPort                     m_ColumnsOutputPort;
    Glib::RefPtr<Gtk::ListStore>         mRefTreeModelOutputPort;
    Gtk::Button*                         mAddOutputPort;
    Gtk::Button*                         mDelOutputPort;

    //Dynamics
    Gtk::ComboBox*                       mComboDyn;
    ModelColumnsDyn                      m_ColumnsDyn;
    Glib::RefPtr<Gtk::ListStore>         mRefComboDyn;
    Gtk::Button*                         mAddDyn;
    Gtk::Button*                         mEditDyn;
    Gtk::Button*                         mDelDyn;

    //Observables
    Gtk::ComboBox*                       mComboObs;
    ModelColumnsPort                     m_ColumnsObs;
    Glib::RefPtr<Gtk::ListStore>         mRefComboObs;
    Gtk::Button*                         mAddObs;
    Gtk::Button*                         mEditObs;
    Gtk::Button*                         mDelObs;

    //Conditions
    Gtk::TreeView*                       mTreeViewCond;
    ModelColumnsCond                     mColumnsCond;
    Glib::RefPtr<Gtk::ListStore>         mRefTreeModel;
    Gtk::Button*                         mAddCond;
    Gtk::Button*                         mDelCond;

    //Buttons
    Gtk::Button*                         mButtonApply;
    Gtk::Button*                         mButtonCancel;

    //Dialog Box
    DynamicBox                           mDynamicBox;
    ObsAndViewBox*                       mObsAndViewBox;

    void makeInputPorts();
    void makeOutputPorts();
    void makeDynamicsCombo();
    void makeObsCombo();
    void makeCondTreeview();

    void add_input_port();
    void del_input_port();

    void add_output_port();
    void del_output_port();

    void add_dynamic();
    void edit_dynamic();
    void del_dynamic();

    void add_observable();
    void edit_observable();
    void del_observable();

    void add_condition();
    void del_condition();

    void on_cond_activated(const Gtk::TreeModel::Path& path,
                           Gtk::TreeViewColumn* column);

    void on_apply();
    void on_cancel();
};

}
} // namespace vle gvle

#endif
