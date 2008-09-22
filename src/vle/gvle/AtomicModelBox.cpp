/**
 * @file vle/gvle/AtomicModelBox.cpp
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


#include <iostream>
#include <vle/gvle/AtomicModelBox.hpp>
#include <vle/gvle/ConditionBox.hpp>
#include <vle/gvle/ObservableBox.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/gvle/WarningBox.hpp>
#include <vle/utils/Path.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

using namespace vle;

namespace vle
{
namespace gvle {

AtomicModelBox::AtomicModelBox(Glib::RefPtr<Gnome::Glade::Xml> xml, Modeling* m):
        mXml(xml),
        mModeling(m),
        mAtom(0),
        mDyn(0),
        mObs(0),
        mViews(0),
        mCond(0),
        mAtom_backup(0),
        mDyn_backup(0),
        mObs_backup(0),
        mViews_backup(0),
        mCond_backup(0),
        mOutputs_backup(0),
        mConnection_in_backup(0),
        mConnection_out_backup(0),
        mDynamicBox(xml),
        mObsAndViewBox(new ObsAndViewBox(xml))
{
    xml->get_widget("DialogAtomicModel", mDialog);

    //Input Ports
    xml->get_widget("TreeViewInputPorts", mInputPorts);
    mRefTreeModelInputPort = Gtk::ListStore::create(m_ColumnsInputPort);
    mInputPorts->set_model(mRefTreeModelInputPort);
    mInputPorts->append_column("Name", m_ColumnsInputPort.m_col_name);
    xml->get_widget("AtomicAddInputPort", mAddInputPort);
    mAddInputPort->signal_clicked().connect(
        sigc::mem_fun(*this, &AtomicModelBox::add_input_port));
    xml->get_widget("AtomicDelInputPort", mDelInputPort);
    mDelInputPort->signal_clicked().connect(
        sigc::mem_fun(*this, &AtomicModelBox::del_input_port));

    //Output Ports
    xml->get_widget("TreeViewOutputPorts", mOutputPorts);
    mRefTreeModelOutputPort = Gtk::ListStore::create(m_ColumnsOutputPort);
    mOutputPorts->set_model(mRefTreeModelOutputPort);
    mOutputPorts->append_column("Name", m_ColumnsOutputPort.m_col_name);
    xml->get_widget("AtomicAddOutputPort", mAddOutputPort);
    mAddOutputPort->signal_clicked().connect(
        sigc::mem_fun(*this, &AtomicModelBox::add_output_port));
    xml->get_widget("AtomicDelOutputPort", mDelOutputPort);
    mDelOutputPort->signal_clicked().connect(
        sigc::mem_fun(*this, &AtomicModelBox::del_output_port));

    //Dynamic
    xml->get_widget("ComboBoxDynamic", mComboDyn);
    mRefComboDyn = Gtk::ListStore::create(m_ColumnsDyn);
    mComboDyn->set_model(mRefComboDyn);
    mComboDyn->pack_start(m_ColumnsDyn.m_name);
    mComboDyn->pack_start(m_ColumnsDyn.m_dyn);
    xml->get_widget("AddDynamic", mAddDyn);
    mAddDyn->signal_clicked().connect(
        sigc::mem_fun(*this, &AtomicModelBox::add_dynamic));
    xml->get_widget("EditDynamic", mEditDyn);
    mEditDyn->signal_clicked().connect(
        sigc::mem_fun(*this, &AtomicModelBox::edit_dynamic));
    xml->get_widget("DeleteDynamic", mDelDyn);
    mDelDyn->signal_clicked().connect(
        sigc::mem_fun(*this, &AtomicModelBox::del_dynamic));

    //Observable
    xml->get_widget("ComboBoxObs", mComboObs);
    mRefComboObs = Gtk::ListStore::create(m_ColumnsObs);
    mComboObs->set_model(mRefComboObs);
    mComboObs->pack_start(m_ColumnsObs.m_col_name);
    xml->get_widget("AddObservable", mAddObs);
    mAddObs->signal_clicked().connect(
        sigc::mem_fun(*this, &AtomicModelBox::add_observable));
    xml->get_widget("EditObservable", mEditObs);
    mEditObs->signal_clicked().connect(
        sigc::mem_fun(*this, &AtomicModelBox::edit_observable));
    xml->get_widget("DeleteObservable", mDelObs);
    mDelObs->signal_clicked().connect(
        sigc::mem_fun(*this, &AtomicModelBox::del_observable));

    //Conditions
    xml->get_widget("TreeViewConditions", mTreeViewCond);
    mRefTreeModel = Gtk::ListStore::create(mColumnsCond);
    mTreeViewCond->set_model(mRefTreeModel);
    mTreeViewCond->append_column("Name", mColumnsCond.m_col_name);
    mTreeViewCond->append_column_editable("In", mColumnsCond.m_col_activ);
    mTreeViewCond->signal_row_activated().connect(sigc::mem_fun(*this,
            &AtomicModelBox::on_cond_activated));
    xml->get_widget("AtomicAddCondition", mAddCond);
    mAddCond->signal_clicked().connect(
        sigc::mem_fun(*this, &AtomicModelBox::add_condition));
    xml->get_widget("AtomicDelCondition", mDelCond);
    mDelCond->signal_clicked().connect(
        sigc::mem_fun(*this, &AtomicModelBox::del_condition));

    //Buttons
    xml->get_widget("ButtonAtomicApply", mButtonApply);
    mButtonApply->signal_clicked().connect(
        sigc::mem_fun(*this, &AtomicModelBox::on_apply));

    xml->get_widget("ButtonAtomicCancel", mButtonCancel);
    mButtonCancel->signal_clicked().connect(
        sigc::mem_fun(*this, &AtomicModelBox::on_cancel));
}

AtomicModelBox::~AtomicModelBox()
{
    mDialog->hide_all();

    if (mAtom_backup)
        delete mAtom_backup;
    if (mDyn_backup)
        delete mDyn_backup;
    if (mObs_backup)
        delete mObs_backup;
    if (mViews_backup)
        delete mViews_backup;
    if (mCond_backup)
        delete mCond_backup;

    delete mObsAndViewBox;
}

void AtomicModelBox::show(vpz::AtomicModel& atom,  graph::AtomicModel& model)
{
    mDialog->set_title("Atomic Model " + model.getName());

    //Data
    mAtom = &atom;
    mGraph_atom = &model;
    mDyn = &mModeling->dynamics();
    mObs = &mModeling->observables();
    mViews = &mModeling->measures();
    mCond = &mModeling->conditions();
    mOutputs = &mModeling->outputs();

    //Backup
    if (mAtom_backup)
        delete mAtom_backup;
    mAtom_backup = new vpz::AtomicModel(atom);

    if (mObs_backup)
        delete mObs_backup;
    mObs_backup = new vpz::Observables(*mObs);

    if (mDyn_backup)
        delete mDyn_backup;
    mDyn_backup = new vpz::Dynamics(*mDyn);

    if (mCond_backup)
        delete mCond_backup;
    mCond_backup = new vpz::Conditions(*mCond);

    if (mViews_backup)
        delete mViews_backup;
    mViews_backup = new vpz::Views(*mViews);

    if (mOutputs_backup)
        delete mOutputs_backup;
    mOutputs_backup = new vpz::Outputs(*mOutputs);

    if (mConnection_in_backup)
        delete mConnection_in_backup;
    mConnection_in_backup = new graph::ConnectionList(model.getInputPortList());

    if (mConnection_out_backup)
        delete mConnection_out_backup;
    mConnection_out_backup = new graph::ConnectionList(model.getOutputPortList());

    makeInputPorts();
    makeOutputPorts();
    makeDynamicsCombo();
    makeObsCombo();
    makeCondTreeview();

    mDialog->show_all();
    mDialog->run();
}

void AtomicModelBox::makeInputPorts()
{
    AssertI(mGraph_atom);
    using namespace graph;

    mRefTreeModelInputPort->clear();
    ConnectionList list = mGraph_atom->getInputPortList();
    ConnectionList::const_iterator it = list.begin();
    while (it != list.end()) {
        Gtk::TreeModel::Row row = *(mRefTreeModelInputPort->append());
        row[m_ColumnsInputPort.m_col_name] = it->first;

        ++it;
    }
}

void AtomicModelBox::makeOutputPorts()
{
    using namespace graph;

    mRefTreeModelOutputPort->clear();
    ConnectionList list = mGraph_atom->getOutputPortList();
    ConnectionList::const_iterator it = list.begin();
    while (it != list.end()) {
        Gtk::TreeModel::Row row = *(mRefTreeModelOutputPort->append());
        row[m_ColumnsOutputPort.m_col_name] = it->first;

        ++it;
    }
}

void AtomicModelBox::makeDynamicsCombo()
{
    mRefComboDyn->clear();

    const vpz::DynamicList& list = mDyn->dynamiclist();
    vpz::DynamicList::const_iterator it = list.begin();
    while (it != list.end())  {
        Gtk::TreeModel::Row row = *(mRefComboDyn->append());
        row[m_ColumnsDyn.m_name] = it->first;
        row[m_ColumnsDyn.m_dyn] = it->second.library();

        ++it;
    }

    const Gtk::TreeModel::Children& child = mRefComboDyn->children();
    Gtk::TreeModel::Children::const_iterator it_child = child.begin();
    while (it_child != child.end()) {
        if (it_child->get_value(m_ColumnsDyn.m_name) == mAtom->dynamics())
            mComboDyn->set_active(it_child);

        ++it_child;
    }
}

void AtomicModelBox::makeObsCombo()
{
    mRefComboObs->clear();

    vpz::ObservableList list = mObs->observablelist();
    vpz::ObservableList::iterator it = list.begin();

    Gtk::TreeModel::Row row = *(mRefComboObs->append());
    row[m_ColumnsObs.m_col_name] = "";

    while (it != list.end()) {
        Gtk::TreeModel::Row row = *(mRefComboObs->append());
        row[m_ColumnsObs.m_col_name] = it->first;

        it++;
    }
    const Gtk::TreeModel::Children& child = mRefComboObs->children();
    Gtk::TreeModel::Children::const_iterator it_child = child.begin();
    while (it_child != child.end()) {
        if (it_child->get_value(m_ColumnsObs.m_col_name) == mAtom->observables())
            mComboObs->set_active(it_child);

        ++it_child;
    }
}

void AtomicModelBox::makeCondTreeview()
{
    mRefTreeModel->clear();

    const vpz::Strings& cond = mAtom->conditions();
    vpz::Strings::const_iterator f;
    vpz::ConditionList list = mCond->conditionlist();
    vpz::ConditionList::iterator it = list.begin();

    while (it != list.end()) {
        Gtk::TreeModel::Row row = *(mRefTreeModel->append());
        row[mColumnsCond.m_col_name] = it->first;
        f = std::find(cond.begin(), cond.end(), it->first);
        if (f != cond.end())
            row[mColumnsCond.m_col_activ] = true;
        else
            row[mColumnsCond.m_col_activ] = false;

        it++;
    }
}

void AtomicModelBox::add_input_port()
{
    SimpleTypeBox box("name ?");
    graph::ConnectionList& list = mGraph_atom->getInputPortList();
    std::string name;
    do {
        name = box.run();
        boost::trim(name);
    } while (name == ""  || (list.find(name) != list.end()));
    mGraph_atom->addInputPort(name);
    makeInputPorts();
}

void AtomicModelBox::del_input_port()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection =  mInputPorts->get_selection();
    if (refSelection) {
        Gtk::TreeModel::iterator iter = refSelection->get_selected();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            if (mGraph_atom->existInputPort(row.get_value(m_ColumnsInputPort.m_col_name))) {
                mGraph_atom->delInputPort(row.get_value(m_ColumnsInputPort.m_col_name));
            }
            makeInputPorts();
        }
    }
}

void AtomicModelBox::add_output_port()
{
    SimpleTypeBox box("name ?");
    graph::ConnectionList& list = mGraph_atom->getOutputPortList();
    std::string name;
    do {
        name = box.run();
        boost::trim(name);
    } while (name == ""  || (list.find(name) != list.end()));
    mGraph_atom->addOutputPort(name);
    makeOutputPorts();
}

void AtomicModelBox::del_output_port()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection =  mOutputPorts->get_selection();
    if (refSelection) {
        Gtk::TreeModel::iterator iter = refSelection->get_selected();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            if (mGraph_atom->existOutputPort(row.get_value(m_ColumnsOutputPort.m_col_name))) {
                mGraph_atom->delOutputPort(row.get_value(m_ColumnsOutputPort.m_col_name));
            }
            makeOutputPorts();
        }
    }
}

void AtomicModelBox::add_dynamic()
{
    const vpz::DynamicList& list = mDyn->dynamiclist();
    std::string name;
    SimpleTypeBox box("name of the Dynamic ?");
    do {
        name = boost::trim_copy(box.run());
    } while (name == "" || (list.find(name) != list.end()));

    vpz::Dynamic* dyn = new vpz::Dynamic(name);
    mDynamicBox.show(dyn);
    mDyn->add(*dyn);
    makeDynamicsCombo();

    const Gtk::TreeModel::Children& child = mRefComboDyn->children();
    Gtk::TreeModel::Children::const_iterator it_child = child.begin();
    while (it_child != child.end()) {
        if (it_child->get_value(m_ColumnsDyn.m_name) == dyn->name()) {
            mComboDyn->set_active(it_child);
            break;
        }
        ++it_child;
    }
}

void AtomicModelBox::edit_dynamic()
{
    std::string dyn = mComboDyn->get_active()->get_value(m_ColumnsDyn.m_name);

    if (!dyn.empty()) {
        mDynamicBox.show(&(mDyn->get(dyn)));
        makeDynamicsCombo();

	const Gtk::TreeModel::Children& child = mRefComboDyn->children();
	Gtk::TreeModel::Children::const_iterator it_child = child.begin();
	while (it_child != child.end()) {
	    if (it_child->get_value(m_ColumnsDyn.m_name) == dyn) {
		mComboDyn->set_active(it_child);
		break;
	    }
	    ++it_child;
	}
    }
}

void AtomicModelBox::del_dynamic()
{
    std::string dyn = mComboDyn->get_active()->get_value(m_ColumnsDyn.m_name);

    if (dyn != "") {
        mDyn->del(dyn);
        makeDynamicsCombo();
    }
}

void AtomicModelBox::add_observable()
{
    SimpleTypeBox box("name ?");
    std::string name;
    do {
        name = box.run();
        boost::trim(name);
    } while (name=="" || mObs->exist(name));
    mObs->add(vpz::Observable(name));
    makeObsCombo();
}

void AtomicModelBox::edit_observable()
{
    std::string obs = mComboObs->get_active()
	->get_value(m_ColumnsObs.m_col_name);

    if (obs != "") {
        //ObservableBox box( mObs->get(obs), *mViews );
        //box.run();
        mObsAndViewBox->show(*mObs, obs, *mViews);
    }
}

void AtomicModelBox::del_observable()
{
    std::string name = mComboObs->get_active()->get_value(m_ColumnsObs.m_col_name);
    if (name != "") {
        mObs->del(name);
        makeObsCombo();
    }
}

void AtomicModelBox::add_condition()
{
    SimpleTypeBox box("name ?");
    std::string name;
    do {
        name = box.run();
        boost::trim(name);
    } while (name == ""  || mCond->exist(name));
    mCond->add(vpz::Condition(name));
    makeCondTreeview();
}

void AtomicModelBox::del_condition()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = mTreeViewCond->get_selection();
    if (refSelection) {
        Gtk::TreeModel::iterator iter = refSelection->get_selected();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            mCond->del(row.get_value(mColumnsCond.m_col_name));
            makeCondTreeview();
        }
    }
}

void AtomicModelBox::on_cond_activated(const Gtk::TreeModel::Path& path,
                                       Gtk::TreeViewColumn* /*column*/)
{
    Gtk::TreeModel::iterator iter = mRefTreeModel->get_iter(path);
    if (iter) {
        Gtk::TreeModel::Row row = *iter;
        Glib::ustring s = row[mColumnsCond.m_col_name];
        vpz::Condition& cond = mCond->get(s);
        ConditionBox cb(&cond);
        cb.run();
    }
}


void AtomicModelBox::on_apply()
{
    if (mComboDyn->get_active() == 0) {
        WarningBox box("You Have to assign a dynamic.");
        box.run();
    } else {
        mAtom->setDynamics(mComboDyn->get_active()->get_value(m_ColumnsDyn.m_name));
        mAtom->setObservables(mComboObs->get_active()->get_value(m_ColumnsObs.m_col_name));

        vpz::Strings vec;
        typedef Gtk::TreeModel::Children type_children;
        type_children children = mRefTreeModel->children();
        for (type_children::iterator iter = children.begin();
                iter != children.end(); ++iter) {
            Gtk::TreeModel::Row row = *iter;
            if (row.get_value(mColumnsCond.m_col_activ) == true) {
                vec.push_back(row.get_value(mColumnsCond.m_col_name));
            }
        }
        mAtom->setConditions(vec);
        mDialog->hide_all();
    }
}

void AtomicModelBox::on_cancel()
{
    using namespace vpz;

    mAtom->setConditions(mAtom_backup->conditions());
    mAtom->setDynamics(mAtom_backup->dynamics());
    mAtom->setObservables(mAtom_backup->observables());

    //Dynamics
    mDyn->clear();
    const DynamicList& list_dyn = mDyn_backup->dynamiclist();
    DynamicList::const_iterator it_dyn = list_dyn.begin();
    while (it_dyn != list_dyn.end()) {
        mDyn->add(it_dyn->second);
        ++it_dyn;
    }

    //Conditions
    mCond->clear();
    const ConditionList& list_cond = mCond_backup->conditionlist();
    ConditionList::const_iterator it_cond = list_cond.begin();
    while (it_cond != list_cond.end()) {
        mCond->add(it_cond->second);
        ++it_cond;
    }

    //Views
    mViews->clear();
    const ViewList& list_view = mViews_backup->viewlist();
    ViewList::const_iterator it_view = list_view.begin();
    while (it_view != list_view.end()) {
        mViews->add(it_view->second);
        ++it_view;
    }

    //Outputs
    mOutputs->clear();
    const OutputList& list_output = mOutputs_backup->outputlist();
    OutputList::const_iterator it_out = list_output.begin();
    while (it_out  != list_output.end()) {
        mOutputs->add(it_out->second);

        ++it_out;
    }

    //Observable
    mObs->clear();
    const ObservableList& list_obs = mObs_backup->observablelist();
    ObservableList::const_iterator it_obs = list_obs.begin();
    while (it_obs != list_obs.end()) {
        mObs->add(it_obs->second);
        ++it_obs;
    }

    //Ports
    graph::CoupledModel* parent = mGraph_atom->getParent();

    graph::ConnectionList& connec_in_list = mGraph_atom->getInputPortList();
    for (graph::ConnectionList::iterator it_connect_in = connec_in_list.begin();
            it_connect_in != connec_in_list.end();
            ++it_connect_in) {
        mGraph_atom->delInputPort(it_connect_in->first);
    }
    for (graph::ConnectionList::iterator it_in_connec =mConnection_in_backup->begin();
            it_in_connec != mConnection_in_backup->end();
            ++it_in_connec) {
        mGraph_atom->addInputPort(it_in_connec->first);
    }

    graph::ConnectionList& connec_out_list = mGraph_atom->getOutputPortList();
    for (graph::ConnectionList::iterator it_connect_out = connec_out_list.begin();
            it_connect_out != connec_out_list.end();
            ++it_connect_out) {
        mGraph_atom->delOutputPort(it_connect_out->first);
    }
    for (graph::ConnectionList::iterator it_out_connec =mConnection_out_backup->begin();
            it_out_connec != mConnection_out_backup->end();
            ++it_out_connec) {
        mGraph_atom->addOutputPort(it_out_connec->first);
    }

    //Input Ports
    for (graph::ConnectionList::iterator it_in_connec =mConnection_in_backup->begin();
            it_in_connec != mConnection_in_backup->end();
            ++it_in_connec) {
        graph::ModelPortList& in_port_list = it_in_connec->second;
        graph::ModelPortList::iterator it_port_list = in_port_list.begin();
        while (it_port_list != in_port_list.end()) {
            if (it_port_list->first == parent) {
                parent->addInputConnection(it_port_list->second, mGraph_atom, it_in_connec->first);
            } else {
                parent->addInternalConnection(it_port_list->first, it_port_list->second,
                                              mGraph_atom, it_in_connec->first);
            }

            ++it_port_list;
        }
    }

    //Output Ports
    for (graph::ConnectionList::iterator it_out_connec =mConnection_out_backup->begin();
            it_out_connec != mConnection_out_backup->end();
            ++it_out_connec) {
        graph::ModelPortList& out_port_list = it_out_connec->second;
        graph::ModelPortList::iterator it_port_list = out_port_list.begin();
        while (it_port_list != out_port_list.end()) {
            if (it_port_list->first == parent) {
                parent->addOutputConnection(mGraph_atom, it_out_connec->first, it_port_list->second);
            } else {
                parent->addInternalConnection(mGraph_atom, it_out_connec->first,
                                              it_port_list->first, it_port_list->second);
            }

            ++it_port_list;
        }
    }

    mDialog->hide_all();
}

}
} // namespace vle gvle
