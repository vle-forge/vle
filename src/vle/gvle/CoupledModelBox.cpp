/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <vle/gvle/CoupledModelBox.hpp>
#include <vle/gvle/GVLE.hpp>
#include <vle/gvle/PortDialog.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/vpz/CoupledModel.hpp>
#include <vle/utils/Tools.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace vle { namespace gvle {

CoupledModelBox::InputPortTreeView::InputPortTreeView(
    BaseObjectType* cobject,
    const Glib::RefPtr < Gtk::Builder >&):
    Gtk::TreeView(cobject)
{
    mRefTreeModelInputPort = Gtk::ListStore::create(mColumnsInputPort);
    set_model(mRefTreeModelInputPort);

    mColumnIn = append_column_editable(_("Input Ports"),
				   mColumnsInputPort.m_col_name);
    mCellRenderer = dynamic_cast<Gtk::CellRendererText*>(
	get_column_cell_renderer(mColumnIn - 1));
    mCellRenderer->property_editable() = true;

    mCellRenderer->signal_editing_started().connect(
	sigc::mem_fun(*this,
	&CoupledModelBox::InputPortTreeView::onEditionStarted));

    mCellRenderer->signal_edited().connect(
	sigc::mem_fun(*this,
		      &CoupledModelBox::InputPortTreeView::
		      onEdition));
    
    mPopupActionGroup = Gtk::ActionGroup::create("Coupled_InputPortTreeView");
    mPopupActionGroup->add(Gtk::Action::create("CoupledIPTV_ContextMenu", _("Context Menu")));
    
    mPopupActionGroup->add(Gtk::Action::create("CoupledIPTV_ContextAdd", _("_Add")),
	sigc::mem_fun(*this, &CoupledModelBox::InputPortTreeView::onAdd));

    mPopupActionGroup->add(Gtk::Action::create("CoupledIPTV_ContextRemove", _("_Remove")),
	sigc::mem_fun(*this, &CoupledModelBox::InputPortTreeView::onRemove));

    mPopupActionGroup->add(Gtk::Action::create("CoupledIPTV_ContextRename", _("_Rename")),
	sigc::mem_fun(*this, &CoupledModelBox::InputPortTreeView::onRename));

    mPopupUIManager = Gtk::UIManager::create();
    mPopupUIManager->insert_action_group(mPopupActionGroup);
    
    Glib::ustring ui_info =
	"<ui>"
	"  <popup name='CoupledIPTV_Popup'>"
        "      <menuitem action='CoupledIPTV_ContextAdd'/>"
	"      <menuitem action='CoupledIPTV_ContextRemove'/>"
	"      <menuitem action='CoupledIPTV_ContextRename'/>"
	"  </popup>"
	"</ui>";

    try {
	mPopupUIManager->add_ui_from_string(ui_info);
	mMenuPopup = (Gtk::Menu *) (
	mPopupUIManager->get_widget("/CoupledIPTV_Popup"));
    } catch(const Glib::Error& ex) {
	std::cerr << "building menus failed: CoupledIPTV_Popup \n" <<  ex.what();
    }
        
    if (!mMenuPopup)
        std::cerr << "menu not found : CoupledIPTV_Popup\n";
}

CoupledModelBox::InputPortTreeView::~InputPortTreeView()
{
}

void CoupledModelBox::InputPortTreeView::applyRenaming(
    vpz::CoupledModel* model)
{
    renameList::const_iterator it = mRenameList.begin();

    while (it != mRenameList.end()) {
        if (model->existInputPort(it->first)) {
            model->renameInputPort(it->first, it->second);
        }
	++it;
    }
    mRenameList.clear();
}

bool CoupledModelBox::InputPortTreeView::on_button_press_event(
    GdkEventButton* event)
{
    bool return_value = TreeView::on_button_press_event(event);
    if ( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) ) {
	mMenuPopup->popup(event->button, event->time);
    }

    return return_value;
}

void CoupledModelBox::InputPortTreeView::build()
{
    assert(mModel);

    mRefTreeModelInputPort->clear();

    vpz::ConnectionList list = mModel->getInputPortList();
    vpz::ConnectionList::const_iterator it = list.begin();
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
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();

    if (refSelection) {
	Gtk::TreeModel::iterator iter = refSelection->get_selected();

	if (iter) {
	    Gtk::TreeModel::Row row = *iter;
	    std::string old_name = row.get_value(mColumnsInputPort.m_col_name);
	    SimpleTypeBox box(_("New name of the input port ?"), old_name);
	    vpz::ConnectionList& list = mModel->getInputPortList();
	    Glib::ustring new_name = boost::trim_copy(box.run());

	    if (box.valid() and not new_name.empty()
		and list.find(new_name) == list.end()) {
		row.set_value(mColumnsInputPort.m_col_name, new_name);

		if (mModel->existInputPort(old_name)) {
		    mModel->renameInputPort(old_name, new_name);
		    mRenameList.push_back(std::make_pair(old_name, new_name));
		}
		build();
	    }
	}
    }
}

void CoupledModelBox::InputPortTreeView::onEditionStarted(
    Gtk::CellEditable* cellEditable, const Glib::ustring& /* path */)
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    Gtk::TreeModel::iterator iter = refSelection->get_selected();

    if (iter) {
	Gtk::TreeModel::Row row = *iter;
	mOldName = row.get_value(mColumnsInputPort.m_col_name);
    }

    if(mValidateRetry) {
	Gtk::CellEditable* celleditable_validated = cellEditable;
	Gtk::Entry* pEntry = dynamic_cast<Gtk::Entry*>(celleditable_validated);
	if(pEntry)
	{
	    pEntry->set_text(mInvalidTextForRetry);
	    mValidateRetry = false;
	    mInvalidTextForRetry.clear();
	}
    }
}

void CoupledModelBox::InputPortTreeView::onEdition(
        const Glib::ustring& pathString,
        const Glib::ustring& newName)
{
    Gtk::TreePath path(pathString);

    vpz::ConnectionList& list = mModel->getInputPortList();

    if (list.find(newName) == list.end() and newName != "") {
	Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
	Gtk::TreeModel::iterator iter = refSelection->get_selected();

	if (iter) {
	    Gtk::TreeModel::Row row = *iter;
	    row.set_value(mColumnsInputPort.m_col_name, newName);

	    if (mModel->existInputPort(mOldName)) {
		mModel->renameInputPort(mOldName, newName);
                mRenameList.push_back(std::make_pair(mOldName, newName));
	    }
	    mValidateRetry = true;
	}
    }
    build();
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

CoupledModelBox::OutputPortTreeView::OutputPortTreeView(
    BaseObjectType* cobject,
    const Glib::RefPtr < Gtk::Builder >& /*refGlade*/) :
    Gtk::TreeView(cobject)
{
    mRefTreeModelOutputPort = Gtk::ListStore::create(mColumnsOutputPort);
    set_model(mRefTreeModelOutputPort);

    mColumnOut = append_column_editable(_("Output Ports"),
				     mColumnsOutputPort.m_col_name);
    mCellRenderer = dynamic_cast<Gtk::CellRendererText*>(
	get_column_cell_renderer(mColumnOut - 1));
    mCellRenderer->property_editable() = true;
    mCellRenderer->signal_editing_started().connect(
	sigc::mem_fun(*this,
		      &CoupledModelBox::OutputPortTreeView::
		      onEditionStarted) );

    mCellRenderer->signal_edited().connect(
	sigc::mem_fun(*this,
		      &CoupledModelBox::OutputPortTreeView::
		      onEdition) );
    
    mPopupActionGroup = Gtk::ActionGroup::create("Coupled_OutputPortTreeView");
    mPopupActionGroup->add(Gtk::Action::create("CoupledOPTV_ContextMenu", _("Context Menu")));
    
    mPopupActionGroup->add(Gtk::Action::create("CoupledOPTV_ContextAdd", _("_Add")),
	sigc::mem_fun(*this, &CoupledModelBox::OutputPortTreeView::onAdd));

    mPopupActionGroup->add(Gtk::Action::create("CoupledOPTV_ContextRemove", _("_Remove")),
	sigc::mem_fun(*this, &CoupledModelBox::OutputPortTreeView::onRemove));

    mPopupActionGroup->add(Gtk::Action::create("CoupledOPTV_ContextRename", _("_Rename")),
	sigc::mem_fun(*this, &CoupledModelBox::OutputPortTreeView::onRename));

    mPopupUIManager = Gtk::UIManager::create();
    mPopupUIManager->insert_action_group(mPopupActionGroup);
    
    Glib::ustring ui_info =
	"<ui>"
	"  <popup name='CoupledOPTV_Popup'>"
        "      <menuitem action='CoupledOPTV_ContextAdd'/>"
	"      <menuitem action='CoupledOPTV_ContextRemove'/>"
	"      <menuitem action='CoupledOPTV_ContextRename'/>"
	"  </popup>"
	"</ui>";

    try {
	mPopupUIManager->add_ui_from_string(ui_info);
	mMenuPopup = (Gtk::Menu *) (
	mPopupUIManager->get_widget("/CoupledOPTV_Popup"));
    } catch(const Glib::Error& ex) {
	std::cerr << "building menus failed: CoupledOPTV_Popup \n" <<  ex.what();
    }
        
    if (!mMenuPopup)
        std::cerr << "menu not found : CoupledOPTV_Popup\n";
}

CoupledModelBox::OutputPortTreeView::~OutputPortTreeView()
{
}

void CoupledModelBox::OutputPortTreeView::applyRenaming(
    vpz::CoupledModel* model)
{
    renameList::const_iterator it = mRenameList.begin();

    while (it != mRenameList.end()) {
        if (model->existOutputPort(it->first)) {
            model->renameOutputPort(it->first, it->second);
        }
	++it;
    }
    mRenameList.clear();
}

bool CoupledModelBox::OutputPortTreeView::on_button_press_event(
    GdkEventButton* event)
{
    bool return_value = TreeView::on_button_press_event(event);

    if ( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) ) {
	mMenuPopup->popup(event->button, event->time);
    }

    return return_value;
}

void CoupledModelBox::OutputPortTreeView::build()
{
    assert(mModel);

    mRefTreeModelOutputPort->clear();

    vpz::ConnectionList list = mModel->getOutputPortList();
    vpz::ConnectionList::const_iterator it = list.begin();
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
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();

    if (refSelection) {
	Gtk::TreeModel::iterator iter = refSelection->get_selected();

	if (iter) {
	    Gtk::TreeModel::Row row = *iter;
	    std::string old_name = row.get_value(mColumnsOutputPort.m_col_name);
	    SimpleTypeBox box(_("New name of the input port ?"), old_name);
	    vpz::ConnectionList& list = mModel->getOutputPortList();
	    Glib::ustring new_name = boost::trim_copy(box.run());

	    if (box.valid() and not new_name.empty()
		and list.find(new_name) == list.end()) {

		row.set_value(mColumnsOutputPort.m_col_name, new_name);

		if (mModel->existOutputPort(old_name)) {
		    mModel->renameOutputPort(old_name, new_name);
		    mRenameList.push_back(std::make_pair(old_name, new_name));
		}
		build();
	    }
	}
    }
}

void CoupledModelBox::OutputPortTreeView::onEditionStarted(
    Gtk::CellEditable* cellEditable, const Glib::ustring& /* path */)
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    Gtk::TreeModel::iterator iter = refSelection->get_selected();

    if (iter) {
	Gtk::TreeModel::Row row = *iter;
	mOldName = row.get_value(mColumnsOutputPort.m_col_name);
    }

    if(mValidateRetry) {
	Gtk::CellEditable* celleditable_validated = cellEditable;
	Gtk::Entry* pEntry = dynamic_cast<Gtk::Entry*>(celleditable_validated);
	if(pEntry)
	{
	    pEntry->set_text(mInvalidTextForRetry);
	    mValidateRetry = false;
	    mInvalidTextForRetry.clear();
	}
    }
}

void CoupledModelBox::OutputPortTreeView::onEdition(
        const Glib::ustring& pathString,
        const Glib::ustring& newName)
{
    Gtk::TreePath path(pathString);

    vpz::ConnectionList& list = mModel->getOutputPortList();

    if (list.find(newName) == list.end() and newName != "") {
	Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
	Gtk::TreeModel::iterator iter = refSelection->get_selected();

	if (iter) {
	    Gtk::TreeModel::Row row = *iter;
	    row.set_value(mColumnsOutputPort.m_col_name, newName);

	    if (mModel->existOutputPort(mOldName)) {
		mModel->renameOutputPort(mOldName, newName);
                mRenameList.push_back(std::make_pair(mOldName, newName));
	    }
	    mValidateRetry = true;
	}
    }
    build();
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

CoupledModelBox::CoupledModelBox(const Glib::RefPtr < Gtk::Builder >& xml,
                                 Modeling* m, GVLE* gvle):
    mXml(xml),
    mModeling(m),
    mGVLE(gvle),
    mGraphModel(0),
    mCurrentGraphModel(0)
{
    xml->get_widget("DialogCoupledModel", mDialog);

    xml->get_widget("CoupledModelName", mModelName);
    xml->get_widget("CoupledModelNbChildren", mModelNbChildren);

    xml->get_widget_derived("CoupledModelTreeViewInput", mInputPorts);

    xml->get_widget_derived("CoupledModelTreeViewOutput", mOutputPorts);


    xml->get_widget("CoupledModel-okbutton", mOkButton);
    mOkButton->signal_clicked().connect(
        sigc::mem_fun(*this, &CoupledModelBox::on_validate));
    xml->get_widget("CoupledModel-cancelbutton", mCancelButton);
    mCancelButton->signal_clicked().connect(
        sigc::mem_fun(*this, &CoupledModelBox::on_cancel));
}

void CoupledModelBox::show(vpz::CoupledModel* model)
{
    mCurrentGraphModel = model;
    mGraphModel = new vpz::CoupledModel(*model);

    mModelName->set_text(mGraphModel->getName());
    mModelNbChildren->set_text(
        utils::to < int32_t >(mGraphModel->getModelList().size()));

    mInputPorts->setModel(mGraphModel);
    mInputPorts->build();
    mOutputPorts->setModel(mGraphModel);
    mOutputPorts->build();

    mDialog->show_all();
    mDialog->run();
}

void CoupledModelBox::applyPorts()
{
    mInputPorts->applyRenaming(mCurrentGraphModel);
    mOutputPorts->applyRenaming(mCurrentGraphModel);

    // Apply input ports
    {
	vpz::ConnectionList connec_in_list =
	    mCurrentGraphModel->getInputPortList();
	vpz::ConnectionList::iterator it = connec_in_list.begin();

	while (it != connec_in_list.end()) {
	    if (not mGraphModel->existInputPort(it->first)) {
		mCurrentGraphModel->delInputPort(it->first);
	    }
	    ++it;
	}
    }
    {
	vpz::ConnectionList connec_in_list =
     	    mGraphModel->getInputPortList();
	vpz::ConnectionList::iterator it = connec_in_list.begin();

     	while (it != connec_in_list.end()) {
	    if (not mCurrentGraphModel->existInputPort(it->first)) {
		mCurrentGraphModel->addInputPort(it->first);
	    }
     	    ++it;
     	}
     }

    // Apply output ports
    {
	vpz::ConnectionList connec_out_list =
	    mCurrentGraphModel->getOutputPortList();
	vpz::ConnectionList::iterator it = connec_out_list.begin();

	while (it != connec_out_list.end()) {
	    if (not mGraphModel->existOutputPort(it->first)) {
		mCurrentGraphModel->delOutputPort(it->first);
	    }
	    ++it;
	}
    }
    {
	vpz::ConnectionList& connec_out_list =
     	    mGraphModel->getOutputPortList();
	vpz::ConnectionList::iterator it = connec_out_list.begin();

     	while (it != connec_out_list.end()) {
	    if (not mCurrentGraphModel->existOutputPort(it->first)) {
		mCurrentGraphModel->addOutputPort(it->first);
	    }
     	    ++it;
     	}
    }
}

void CoupledModelBox::on_validate()
{
    applyPorts();
    mDialog->hide();
    delete mGraphModel;
    mGVLE->setModified(true);
}

void CoupledModelBox::on_cancel()
{
    mDialog->hide();
    delete mGraphModel;
}

} } // namespace vle gvle
