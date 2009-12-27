/**
 * @file vle/gvle/PortDialog.cpp
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


#include <vle/gvle/PortDialog.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/graph/Model.hpp>
#include <vle/utils/Debug.hpp>
#include <gtkmm/stock.h>

using namespace vle;

namespace vle
{
namespace gvle {

PortDialog::PortDialog(graph::Model* model, PortDialog::PortType type) :
        Gtk::Dialog(_("Port"), true, true),
        mLabelEntry(_("New port:")),
        mModel(model),
        mPortType(type)
{
    assert(model);

    add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    Gtk::Button* ok = add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);

    switch (mPortType) {
    case PortDialog::INPUT :
        mLabelDialogName.set_markup(_("<b>Add input port</b>"));
        break;
    case PortDialog::OUTPUT :
        mLabelDialogName.set_markup(_("<b>Add output port</b>"));
        break;
    }

    Glib::ustring name = _("Model name:") + model->getName();
    mLabelModelName.set_text(name);

    mListStore = Gtk::ListStore::create(mColumn);
    mTreeViewPorts.set_model(mListStore);
    mTreeViewPorts.append_column(_("Name"), mColumn.mName);
    mScrolledWindow.add(mTreeViewPorts);
    mScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

    mHBox.pack_start(mLabelEntry, false, true, 0);
    mHBox.pack_start(mEntryPortName, true, true, 0);

    get_vbox()->pack_start(mLabelDialogName, true, true, 10);
    get_vbox()->pack_start(mLabelModelName, false, true, 10);
    get_vbox()->pack_start(mHBox, false, true, 0);

    ok->set_flags(Gtk::CAN_DEFAULT);
    ok->grab_default();
    mEntryPortName.set_activates_default();

    set_border_width(3);
    show_all();
}

void PortDialog::fillTreeView()
{
    throw utils::NotYetImplemented(_("PortDialog::fillTreeView"));
    //const graph::MapStringPort& lst =
    //(mPortType == PortDialog::INPUT) ? mModel->getInputPortList() :
    //(mPortType == PortDialog::OUTPUT) ? mModel->getOutputPortList() :
    //(mPortType == PortDialog::INIT) ? mModel->getInitPortList() :
    //mModel->getStatePortList();
    //
    //graph::MapStringPort::const_iterator it = lst.begin();
    //while (it != lst.end()) {
    //Gtk::TreeModel::Row row = *(mListStore->append());
    //mSet.insert((*it).first);
    //row[mColumn.mName] = (*it).first;
    //++it;
    //}
}

bool PortDialog::run()
{
    for (;;) {
        int response = Gtk::Dialog::run();

        if (response == Gtk::RESPONSE_OK) {
            Glib::ustring name = mEntryPortName.get_text();
            if (name.empty()) {
                gvle::Error(_("Empty port name ?"));
            } else if (name.is_ascii() == false) {
                gvle::Error(
                    (fmt(_("'%1%' have no-ascii char")) % name).str());
            } else if (mSet.find(name) != mSet.end()) {
                gvle::Error(
                    (fmt(_("'%1%' already a port")) % name).str());
            } else {
                switch (mPortType) {
                case PortDialog::INPUT :
                    mModel->addInputPort(name);
                    break;
                case PortDialog::OUTPUT :
                    mModel->addOutputPort(name);
                    break;
                }
                hide();
                return true;
            }
        } else {
            hide();
            return false;
        }
    }
}

}
} // namespace vle gvle
