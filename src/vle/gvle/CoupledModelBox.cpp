/**
 * @file vle/gvle/CoupledModelBox.cpp
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


#include <vle/gvle/CoupledModelBox.hpp>
#include <vle/gvle/PortDialog.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <vle/utils/Tools.hpp>
#include <gtkmm/stock.h>

using namespace vle;

namespace vle
{
namespace gvle {

CoupledModelBox::InputPortTreeView::InputPortTreeView(
    BaseObjectType* cobject,
    const Glib::RefPtr<Gnome::Glade::Xml>&):
    Gtk::TreeView(cobject)
{
    mRefTreeModelInputPort = Gtk::ListStore::create(mColumnsInputPort);
    set_model(mRefTreeModelInputPort);
    append_column(_("Input Ports"), mColumnsInputPort.m_col_name);

    {
	Gtk::Menu::MenuList& menulist = mMenuPopup.items();

	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Add"),
		sigc::mem_fun(
		    *this,
		    &CoupledModelBox::InputPortTreeView::onAdd)));
	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Remove"),
		sigc::mem_fun(
		    *this,
		    &CoupledModelBox::InputPortTreeView::onRemove)));
	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Rename"),
		sigc::mem_fun(
		    *this,
		    &CoupledModelBox::InputPortTreeView::onRename)));
    }

    mMenuPopup.accelerate(*this);
}

CoupledModelBox::InputPortTreeView::~InputPortTreeView()
{
}

bool CoupledModelBox::InputPortTreeView::on_button_press_event(
    GdkEventButton* event)
{
    bool return_value = TreeView::on_button_press_event(event);
    if ( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) ) {
	mMenuPopup.popup(event->button, event->time);
    }

    return return_value;
}

void CoupledModelBox::InputPortTreeView::build()
{
    assert(mModel);
    using namespace graph;

    mRefTreeModelInputPort->clear();

    ConnectionList list = mModel->getInputPortList();
    ConnectionList::const_iterator it = list.begin();
    while (it != list.end()) {
        Gtk::TreeModel::Row row = *(mRefTreeModelInputPort->append());
        row[mColumnsInputPort.m_col_name] = it->first;

        ++it;
    }
}

void CoupledModelBox::InputPortTreeView::onAdd()
{
    PortDialog box(mModel, PortDialog::INPUT);
    if (box.run()) {
        build();
    }
}

void CoupledModelBox::InputPortTreeView::onRemove()
{
    Glib::RefPtr<Gtk::TreeSelection> selection = get_selection();
    Gtk::TreeModel::iterator iter = selection->get_selected();
    if (iter) {
        Gtk::TreeModel::Row row = *iter;
	std::string name = row.get_value(mColumnsInputPort.m_col_name);

	if(mModel->existInputPort(name)) {
	    mModel->delInputPort(name);
	}
	build();
    }
}

void CoupledModelBox::InputPortTreeView::onRename()
{
    SimpleTypeBox box(_("New name of the input port ?"));
    graph::ConnectionList& list = mModel->getInputPortList();
    Glib::ustring new_name = boost::trim_copy(box.run());

    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();

    if (refSelection and box.valid() and not new_name.empty()
	and list.find(new_name) == list.end()) {

	Gtk::TreeModel::iterator iter = refSelection->get_selected();

	if (iter) {
	    Gtk::TreeModel::Row row = *iter;
	    std::string old_name = row.get_value(mColumnsInputPort.m_col_name);
	    row.set_value(mColumnsInputPort.m_col_name, new_name);

	    if (mModel->existInputPort(old_name)) {
		mModel->renameInputPort(old_name, new_name);
	    }
	    build();
	}
    }
}


CoupledModelBox::OutputPortTreeView::OutputPortTreeView(
    BaseObjectType* cobject,
    const Glib::RefPtr<Gnome::Glade::Xml>& /*refGlade*/) :
    Gtk::TreeView(cobject)
{
    mRefTreeModelOutputPort = Gtk::ListStore::create(mColumnsOutputPort);
    set_model(mRefTreeModelOutputPort);
    append_column(_("Output Ports"), mColumnsOutputPort.m_col_name);

    //Fill popup menu:
    {
	Gtk::Menu::Menu::MenuList& menulist = mMenuPopup.items();

	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Add"),
		sigc::mem_fun(
		    *this,
		    &CoupledModelBox::OutputPortTreeView::onAdd)));
	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Remove"),
		sigc::mem_fun(
		    *this,
		    &CoupledModelBox::OutputPortTreeView::onRemove)));
	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Rename)"),
		sigc::mem_fun(
		    *this,
		    &CoupledModelBox::OutputPortTreeView::onRename)));
    }
    mMenuPopup.accelerate(*this);
}

CoupledModelBox::OutputPortTreeView::~OutputPortTreeView()
{
}

bool CoupledModelBox::OutputPortTreeView::on_button_press_event(
    GdkEventButton* event)
{
    bool return_value = TreeView::on_button_press_event(event);

    if ( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) ) {
	mMenuPopup.popup(event->button, event->time);
    }

    return return_value;
}

void CoupledModelBox::OutputPortTreeView::build()
{
    assert(mModel);
    using namespace graph;

    mRefTreeModelOutputPort->clear();

    ConnectionList list = mModel->getOutputPortList();
    ConnectionList::const_iterator it = list.begin();
    while (it != list.end()) {
	Gtk::TreeModel::Row row = *(mRefTreeModelOutputPort->append());
	row[mColumnsOutputPort.m_col_name] = it->first;

	++it;
    }
}

void CoupledModelBox::OutputPortTreeView::onAdd()
{
    PortDialog box(mModel, PortDialog::OUTPUT);
    if (box.run()) {
        build();
    }
}

void CoupledModelBox::OutputPortTreeView::onRemove()
{
    Glib::RefPtr<Gtk::TreeSelection> selection = get_selection();
    Gtk::TreeModel::iterator iter = selection->get_selected();
    if (iter) {
        Gtk::TreeModel::Row row = *iter;
	std::string name = row.get_value(mColumnsOutputPort.m_col_name);

	if(mModel->existOutputPort(name)) {
	    mModel->delOutputPort(name);
	}
	build();
    }
}

void CoupledModelBox::OutputPortTreeView::onRename()
{
    SimpleTypeBox box(_("New name of the input port ?"));
    graph::ConnectionList& list = mModel->getOutputPortList();
    Glib::ustring new_name = boost::trim_copy(box.run());

    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();

    if (refSelection and box.valid() and not new_name.empty()
	and list.find(new_name) == list.end()) {

	Gtk::TreeModel::iterator iter = refSelection->get_selected();

	if (iter) {
	    Gtk::TreeModel::Row row = *iter;
	    std::string old_name = row.get_value(mColumnsOutputPort.m_col_name);
	    row.set_value(mColumnsOutputPort.m_col_name, new_name);

	    if (mModel->existOutputPort(old_name)) {
		mModel->renameOutputPort(old_name, new_name);
	    }
	    build();
	}
    }
}

CoupledModelBox::CoupledModelBox(Glib::RefPtr<Gnome::Glade::Xml> xml, Modeling* m):
        mXml(xml),
        mModeling(m),
        mModel(0)
{
    xml->get_widget("DialogCoupledModel", mDialog);

    xml->get_widget("CoupledModelName", mModelName);
    xml->get_widget("CoupledModelNbChildren", mModelNbChildren);

    xml->get_widget_derived("CoupledModelTreeViewInput", mInputPorts);

    xml->get_widget_derived("CoupledModelTreeViewOutput", mOutputPorts);


    xml->get_widget("ButtonCoupledValidate", mButtonValidate);
    mButtonValidate->signal_clicked().connect(
        sigc::mem_fun(*this, &CoupledModelBox::on_validate));
}

void CoupledModelBox::show(graph::CoupledModel* model)
{
    mModel = model;

    mModelName->set_text(model->getName());
    mModelNbChildren->set_text(utils::to_string(model->getModelList().size()));

    mInputPorts->setModel(mModel);
    mInputPorts->build();
    mOutputPorts->setModel(mModel);
    mOutputPorts->build();

    mDialog->show_all();
    mDialog->run();
}

void CoupledModelBox::on_validate()
{
    mDialog->hide_all();
}

}
} // namespace vle gvle
