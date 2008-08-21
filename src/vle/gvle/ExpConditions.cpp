/**
 * @file vle/gvle/ExpConditions.cpp
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


#include <vle/graph/Model.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/gvle/ExpConditions.hpp>
#include <vle/gvle/Data.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/XML.hpp>

using std::string;
using namespace vle;
using namespace utils;

namespace vle
{
namespace gvle {

ExpConditions::ExpConditions(Modeling* mod, graph::Model* current) :
        mModeling(mod),
        mCurrent(current),
        mAddInitButton("Edit init value"),
        mFrameTreeView("Experimental conditions"),
        mFrameButtons("Show model"),
        mCurrentButton(mRadioButtonGroup, "current"),
        mBrotherButton(mRadioButtonGroup, "brother"),
        mAllButton(mRadioButtonGroup, "all")
{
    AssertI(mod != NULL);
    mTreeStoreInits = Gtk::TreeStore::create(mColumnsInits);
    mTreeViewInits.set_model(mTreeStoreInits);
    mTreeViewInits.append_column("Model", mColumnsInits.mName);
    mTreeViewInits.append_column("Port", mColumnsInits.mPortName);
    mTreeViewInits.append_column("Init", mColumnsInits.mValue);
    mScrolledWindow.add(mTreeViewInits);
    mScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    mVBoxTreeView.pack_start(mScrolledWindow, true, true, 0);
    mVBoxTreeView.pack_start(mAddInitButton, false, true, 0);
    mFrameTreeView.add(mVBoxTreeView);
    mFrameTreeView.set_border_width(3);
    pack_start(mFrameTreeView, true, true, 0);

    mFrameHBox.pack_start(mCurrentButton, true, true, 0);
    mFrameHBox.pack_start(mBrotherButton, true, true, 0);
    mFrameHBox.pack_start(mAllButton, true, true, 0);
    mFrameButtons.add(mFrameHBox);
    mFrameButtons.set_border_width(3);

    pack_start(mFrameButtons, false, true, 0);
    mCurrentButton.signal_clicked().connect(sigc::mem_fun(*this,
                                            &ExpConditions::on_current_button_clicked));
    mBrotherButton.signal_clicked().connect(sigc::mem_fun(*this,
                                            &ExpConditions::on_brother_button_clicked));
    mAllButton.signal_clicked().connect(sigc::mem_fun(*this,
                                        &ExpConditions::on_all_button_clicked));
    mAddInitButton.signal_clicked().connect(sigc::mem_fun(*this,
                                            &ExpConditions::on_add_init_button_clicked));
    set_border_width(3);

    if (current != NULL)
        setTreeCurrent(current);
    //TO FIX :
    //else
    //      setTreeAll(mModeling->getTopModel());
    mTreeViewInits.expand_all();

}

void ExpConditions::addInitPortModel(Gtk::TreeModel::Row /* row */,
                                     graph::AtomicModel* /* model */)
{
    /*
    const graph::MapStringPort& init_ports = model->getInitPort();
    graph::MapStringPort::const_iterator it = init_ports.begin();
    while (it != init_ports.end()) {
        Gtk::TreeModel::Row row_children;
        row_children = *mTreeStoreInits->append(row.children());
        row_children[mColumnsInits.mPortName] = (*it).first;

    string xml = model->getExperimentalCondition((*it).first);

    if (xml != "")
    {
     xmlpp::DomParser dom;
     dom.parse_memory(model->getExperimentalCondition((*it).first));
     xmlpp::Element* root = dom.get_document()->get_root_node();
     
     if (XML::exist_attribute(root,"VALUE"))
    row_children[mColumnsInits.mValue] = XML::get_attribute(root,"VALUE");
     else row_children[mColumnsInits.mValue] = root->get_name();
    }
    else row_children[mColumnsInits.mValue] = "???";
     
        ++it;
    }*/
}

void ExpConditions::setTreeCurrent(graph::Model* model)
{
    if (model) {
        string name(model->getName());
        Gtk::TreeModel::Row row = *mTreeStoreInits->append();
        row[mColumnsInits.mName] = name;
        if (model->isAtomic() and model->getInputPortNumber()) {
            addInitPortModel(row, (graph::AtomicModel*)(model));
        }
    }
}

void ExpConditions::setTreeBrother(graph::Model* /*model*/)
{
    Throw(utils::NotYetImplemented, "ExpConditions::setTreeBrother");
    //if (model) {
    //graph::CoupledModel* parent = model->getParent();
    //
    //AssertI(parent);
    //
    //graph::VectorModel& models = parent->getModelList();
    //graph::VectorModel::iterator it = models.begin();
    //while (it != models.end()) {
    //const std::string& name = (*it)->getName();
    //Gtk::TreeModel::Row row = *mTreeStoreInits->append();
    //row[mColumnsInits.mName] = name;
    //
    //if ((*it)->isAtomic() and (*it)->getInitPortNumber()) {
    //addInitPortModel(row, (graph::AtomicModel*)(*it));
    //}
    //++it;
    //}
    //}
}

void ExpConditions::setTreeAll(Gtk::TreeModel::Row /*tree*/,
                               graph::CoupledModel* /*model*/)
{
    Throw(utils::NotYetImplemented, "ExpConditions::setTreeAll");
    //Gtk::TreeModel::Row row = *(mTreeStoreInits->append(tree.children()));
    //row[mColumnsInits.mName] = model->getName();
    //
    //graph::VectorModel& models = model->getModelList();
    //graph::VectorModel::iterator it = models.begin();
    //while (it != models.end()) {
    //if ((*it)->isCoupled()) {
    //setTreeAll(row, (graph::CoupledModel*)(*it));
    //} else {
    //if ((*it)->getInitPortNumber()) {
    //Gtk::TreeModel::Row row2;
    //row2 = *mTreeStoreInits->append(row.children());
    //row2[mColumnsInits.mName] = (*it)->getName();
    //addInitPortModel(row2, (graph::AtomicModel*)(*it));
    //}
    //}
    //++it;
    //}
}

void ExpConditions::setTreeAll(graph::CoupledModel* /*model*/)
{
    Throw(utils::NotYetImplemented, "ExpConditions::setTreeAll");
    //Gtk::TreeModel::Row row = *(mTreeStoreInits->append());
    //row[mColumnsInits.mName] = model->getName();
    //
    //graph::VectorModel& models = model->getModelList();
    //graph::VectorModel::iterator it = models.begin();
    //while (it != models.end()) {
    //if ((*it)->isCoupled()) {
    //setTreeAll(row, (graph::CoupledModel*)(*it));
    //} else {
    //if ((*it)->getInitPortNumber()) {
    //Gtk::TreeModel::Row row2;
    //row2 = *mTreeStoreInits->append(row.children());
    //row2[mColumnsInits.mName] = (*it)->getName();
    //addInitPortModel(row2, (graph::AtomicModel*)(*it));
    //}
    //}
    //++it;
    //}
}

void ExpConditions::on_current_button_clicked()
{
    mTreeStoreInits->clear();
    if (mCurrent != NULL) {
        setTreeCurrent(mCurrent);
    } else {
        setTreeCurrent(mModeling->getTopModel());
    }
    mTreeViewInits.expand_all();
}

void ExpConditions::on_brother_button_clicked()
{
    mTreeStoreInits->clear();
    if (mCurrent != NULL) {
        setTreeBrother(mCurrent);
    } else {
        setTreeCurrent(mModeling->getTopModel());
    }
    mTreeViewInits.expand_all();
}

void ExpConditions::on_all_button_clicked()
{
    mTreeStoreInits->clear();
    //TO FIX:
    //setTreeAll(mModeling->getTopModel());
    mTreeViewInits.expand_all();
}

void ExpConditions::on_add_init_button_clicked()
{
    Glib::RefPtr < Gtk::TreeSelection > select;
    select = mTreeViewInits.get_selection();

    Gtk::TreeModel::iterator iter = select->get_selected();
    if (iter) {
        Gtk::TreeModel::Row row = *iter;
        Glib::ustring name = row[mColumnsInits.mName];
        if (name == "") {
            Gtk::TreeModel::Row rowparent = *row.parent();
            name = rowparent[mColumnsInits.mName];
            graph::Model* mdl = mModeling->getTopModel()->findModel(name);

            if (mdl != NULL) {
                Glib::ustring portname = row[mColumnsInits.mPortName];
                /*                Data data((graph::AtomicModel*)(mdl)
                                          ->getExperimentalCondition(portname));

                                if (data.run() == Gtk::RESPONSE_OK) {
                      xmlpp::DomParser dom;
                      dom.parse_memory(data.getDataXML());
                      xmlpp::Element* root = dom.get_document()->get_root_node();

                      if (XML::exist_attribute(root,"VALUE"))
                   row[mColumnsInits.mValue] = XML::get_attribute(root,"VALUE");
                      else row[mColumnsInits.mValue] = root->get_name();

                                    ((graph::AtomicModel*)(mdl))->addExperimentalCondition(
                                            portname, data.getDataXML());
                                } */
            }
        }
    }
}

}
} // namespace vle gvle
