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


#include <vle/gvle/ConditionsBox.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/gvle/PluginFactory.hpp>
#include <vle/gvle/GVLE.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

namespace vle { namespace gvle {

////
//  ConditionsTreeView
////

ConditionsBox::ConditionsTreeView::ConditionsTreeView(
    BaseObjectType* cobject,
    const Glib::RefPtr < Gtk::Builder >& /*refGlade*/) :
    Gtk::TreeView(cobject),
    mConditions(0),
    mValidateRetry(false)
{
    mRefTreeModel = Gtk::ListStore::create(mColumns);
    set_model(mRefTreeModel);
    
    mColumn = append_column_editable(_("Initial conditions"),
				     mColumns.m_col_name);
    mCellRenderer = dynamic_cast<Gtk::CellRendererText*>(
	get_column_cell_renderer(mColumn - 1));
    mCellRenderer->property_editable() = true;

    mCellRenderer->signal_editing_started().connect(
	sigc::mem_fun(*this,
		      &ConditionsBox::ConditionsTreeView::
		      onEditionStarted));
    mCellRenderer->signal_edited().connect(
	sigc::mem_fun(*this,
		      &ConditionsBox::ConditionsTreeView::
		      onEdition));

    mRefTreeSelection = get_selection();
    mRefTreeSelection->signal_changed().connect(
        sigc::mem_fun(*this,
                      &ConditionsBox::ConditionsTreeView::on_select));

    set_has_tooltip();
    signal_query_tooltip().connect(
        sigc::mem_fun(*this, &ConditionsBox::ConditionsTreeView::onQueryTooltip));
    
}

void ConditionsBox::ConditionsTreeView::makeMenus () {
    mPopupActionGroup = Gtk::ActionGroup::create("ConditionsTreeView");
    mPopupActionGroup->add(Gtk::Action::create("CTV_ContextMenu", _("Context Menu")));
    
    mPopupActionGroup->add(Gtk::Action::create("CTV_ContextAdd", _("_Add")),
        sigc::mem_fun(*this, &ConditionsBox::ConditionsTreeView::on_add));
    
    mPopupActionGroup->add(Gtk::Action::create("CTV_ContextRemove", _("_Remove")),
        sigc::mem_fun(*this, &ConditionsBox::ConditionsTreeView::on_remove));
    
    mPopupActionGroup->add(Gtk::Action::create("CTV_ContextRename", _("_Rename")),
        sigc::mem_fun(*this, &ConditionsBox::ConditionsTreeView::onRename));
    
    mPopupActionGroup->add(Gtk::Action::create("CTV_ContextCopy", _("_Copy")),
        sigc::mem_fun(*this, &ConditionsBox::ConditionsTreeView::onCopy));
    
    mUIManager = Gtk::UIManager::create();
    mUIManager->insert_action_group(mPopupActionGroup);
    
    Glib::ustring name_item = "";
    Glib::ustring ui_info =
          "<ui>"
          "  <popup name='CTView_Popup'>"
          "    <menuitem action='CTV_ContextAdd'/>"
          "    <menuitem action='CTV_ContextRemove'/>"
          "    <menuitem action='CTV_ContextRename'/>"
          "    <menuitem action='CTV_ContextCopy'/>"
          "    <menu action='CTV_ContextEdit'>";

    //// Partie importer depuis ex ConditionsBox::ConditionsTreeView::makeMenuEdit
    utils::ModuleList lst;
    utils::ModuleList::iterator it;
    mParent->getGVLE()->pluginFactory().getGvleModelingPlugins(&lst);
    
    mPopupActionGroup->add(Gtk::Action::create("CTV_ContextEdit", _("_Edit")));
    for (it = lst.begin(); it != lst.end(); ++it) {
        name_item ="CTV_Edit_" + it->package + "_" + it->library; 
         
        mPopupActionGroup->add(Gtk::Action::create(name_item, it->package + "/" + it->library),
            sigc::bind<std::string>(sigc::mem_fun(
                    *this, &ConditionsBox::ConditionsTreeView::onEdit),
                    it->package + "/" + it->library));
        
        ui_info = ui_info + ""
	  "      <menuitem action='" + name_item + "'/>";
    }
    //// Fin Partie importer depuis ex ConditionsBox::ConditionsTreeView::makeMenuEdit
    
    ui_info = ui_info + ""
          "    </menu>"
          "  </popup>"
          "</ui>";

    try {
        mUIManager->add_ui_from_string(ui_info);
        mMenuPopup = (Gtk::Menu *) (
		mUIManager->get_widget("/CTView_Popup"));
    } catch(const Glib::Error& ex) {
        std::cerr << "building menus failed: CTView_Popup " <<  ex.what();
    }
    
    if (!mMenuPopup)
        std::cerr << "not a menu : CTView_Popup\n" ;
}

ConditionsBox::ConditionsTreeView::~ConditionsTreeView()
{
}

bool ConditionsBox::ConditionsTreeView::on_button_press_event(
    GdkEventButton* event)
{
    //Call base class, to allow normal handling,
    //such as allowing the row to be selected by the right-click:
    bool return_value = TreeView::on_button_press_event(event);

    //Then do our custom stuff:
    if (mConditions and event->type == GDK_BUTTON_PRESS and event->button == 3)
    {
	mMenuPopup->popup(event->button, event->time);
    }

    return return_value;
}

void ConditionsBox::ConditionsTreeView::build()
{
    assert(mConditions);

    mRefTreeModel->clear();

    vpz::ConditionList& conditionList = mConditions->conditionlist();
    vpz::ConditionList::const_iterator it = conditionList.begin();

    for(it = conditionList.begin(); it != conditionList.end(); ++it) {
	Gtk::TreeModel::Row row = *(mRefTreeModel->append());

	row[mColumns.m_col_name] = it->first;
    }
}

void ConditionsBox::ConditionsTreeView::on_select()
{
    Gtk::TreeModel::iterator it = mRefTreeSelection->get_selected();

    if (it) {
        Glib::ustring name = (*it)[mColumns.m_col_name];

	mParent->buildTreePorts(name);
    }
}

void ConditionsBox::ConditionsTreeView::on_add()
{
    SimpleTypeBox box(_("Name of the condition ?"), "");
    vpz::ConditionList& list = mConditions->conditionlist();
    std::string name = boost::trim_copy(box.run());

    if (box.valid() and not name.empty() and list.find(name) == list.end()) {
        mConditions->add(vpz::Condition(name));
        mParent->buildTreeConditions();
    }
}

void ConditionsBox::ConditionsTreeView::on_remove()
{
    Gtk::TreeModel::iterator it = mRefTreeSelection->get_selected();

    if (it) {
        Glib::ustring name = (*it)[mColumns.m_col_name];

	mConditions->del(name);
	mParent->buildTreeConditions();
    }
}

void ConditionsBox::ConditionsTreeView::onRename()
{
    Gtk::TreeModel::iterator it = mRefTreeSelection->get_selected();

    if (it) {
        Glib::ustring oldname = (*it)[mColumns.m_col_name];
        SimpleTypeBox box(_("New name of the condition ?"), oldname);
        std::string newname = boost::trim_copy(box.run());
	vpz::ConditionList& list = mConditions->conditionlist();

	if (box.valid() and not newname.empty()
	   and list.find(newname) == list.end()) {
	    mConditions->rename(oldname, newname);
	    mParent->buildTreeConditions();
            mParent->setRenameList(std::make_pair(oldname, newname));
	}
    }
}

void ConditionsBox::ConditionsTreeView::onCopy()
{
    vpz::ConditionList& list = mConditions->conditionlist();
    Gtk::TreeModel::iterator it = mRefTreeSelection->get_selected();
    int number = 1;
    std::string copy;

    if (it) {
	Glib::ustring name = (*it)[mColumns.m_col_name];
	do {
	    copy = name + "_" + boost::lexical_cast < std::string >(number);
	    ++number;
	}while (list.find(copy) != list.end());
	mConditions->copy(name, copy);
        mParent->buildTreeConditions();
    }

}

void ConditionsBox::ConditionsTreeView::onEdit(std::string pluginName)
{
    Gtk::TreeModel::iterator it = mRefTreeSelection->get_selected();

    if (it) {
        std::string conditionName = Glib::ustring((*it)[mColumns.m_col_name]);

        try {
            PluginFactory& plf = mParent->getGVLE()->pluginFactory();
            ModelingPluginPtr plugin = plf.getModelingPlugin(pluginName,
                    mParent->getGVLE()->currentPackage().name());
            vpz::Condition& cond = mConditions->get(conditionName);
            plugin->start(cond);

            mParent->buildTreePorts(conditionName);

        } catch(const std::exception& e) {
            // Error(e.what());
        }
    }
}

void ConditionsBox::ConditionsTreeView::onEditionStarted(
    Gtk::CellEditable* cellEditable, const Glib::ustring& /* path */)
{
    Gtk::TreeModel::iterator it = mRefTreeSelection->get_selected();

    if (it) {
	mOldName = (*it)[mColumns.m_col_name];
    }

    if(mValidateRetry)
    {
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

void ConditionsBox::ConditionsTreeView::onEdition(
    const Glib::ustring& pathString,
    const Glib::ustring& newName)
{
    Gtk::TreePath path(pathString);

    vpz::ConditionList& list = mConditions->conditionlist();

    if (list.find(newName) == list.end() and not newName.empty()) {
	mConditions->rename(mOldName, newName);
	mParent->buildTreeConditions();
        mParent->setRenameList(std::make_pair(mOldName, newName));
    }
    build();
}

bool ConditionsBox::ConditionsTreeView::onQueryTooltip(
    int wx,int wy, bool keyboard_tooltip,
    const Glib::RefPtr<Gtk::Tooltip>& tooltip)
{
    Gtk::TreeModel::iterator iter;
    Glib::ustring card;

    if (get_tooltip_context_iter(wx, wy, keyboard_tooltip, iter)) {
        Glib::ustring cond = (*iter).get_value(mColumns.m_col_name);
        card = mParent->getGVLE()->getModeling()->getInfoCard(cond);
        tooltip->set_markup(card);
        set_tooltip_row(tooltip, Gtk::TreePath(iter));
        return true;
    } else {
        return false;
    }
}

////
//  PortsTreeView
////

ConditionsBox::PortsTreeView::PortsTreeView(
    BaseObjectType* cobject,
    const Glib::RefPtr < Gtk::Builder >& /*refGlade*/) :
    Gtk::TreeView(cobject),
    mCondition(0),
    mValidateRetry(false)
{
    mRefTreeModel = Gtk::ListStore::create(mColumns);
    set_model(mRefTreeModel);

    mColumn = append_column_editable(_("Parameters"),
			   mColumns.m_col_name);
    mCellRenderer = dynamic_cast<Gtk::CellRendererText*>(
	get_column_cell_renderer(mColumn - 1));
    mCellRenderer->property_editable() = true;

    mCellRenderer->signal_editing_started().connect(
	sigc::mem_fun(*this,
		      &ConditionsBox::PortsTreeView::
		      onEditionStarted));

    mCellRenderer->signal_edited().connect(
	sigc::mem_fun(*this,
		      &ConditionsBox::PortsTreeView::
		      onEdition));

    mRefTreeSelection = get_selection();
    mRefTreeSelection->signal_changed().connect(
        sigc::mem_fun(*this,
                      &ConditionsBox::PortsTreeView::on_select));
}

void ConditionsBox::PortsTreeView::makeMenus () {
    mPopupActionGroup = Gtk::ActionGroup::create("PortsTreeView");
    mPopupActionGroup->add(Gtk::Action::create("PTV_ContextMenu", _("Context Menu")));
    
    mPopupActionGroup->add(Gtk::Action::create("PTV_ContextAdd", _("_Add")),
        sigc::mem_fun(*this, &ConditionsBox::PortsTreeView::on_add));
    
    mPopupActionGroup->add(Gtk::Action::create("PTV_ContextRemove", _("_Remove")),
        sigc::mem_fun(*this, &ConditionsBox::PortsTreeView::on_remove));
    
    mPopupActionGroup->add(Gtk::Action::create("PTV_ContextRename", _("_Rename")),
        sigc::mem_fun(*this, &ConditionsBox::PortsTreeView::onRename));
    
    mPopupActionGroup->add(Gtk::Action::create("PTV_ContextDuplicate", _("_Duplicate")),
        sigc::mem_fun(*this, &ConditionsBox::PortsTreeView::onDuplicate));
    
    mUIManager = Gtk::UIManager::create();
    mUIManager->insert_action_group(mPopupActionGroup);
    
    Glib::ustring name_item = "";
    Glib::ustring ui_info =
          "<ui>"
          "  <popup name='PTView_Popup'>"
          "    <menuitem action='PTV_ContextAdd'/>"
          "    <menuitem action='PTV_ContextRemove'/>"
          "    <menuitem action='PTV_ContextRename'/>"
          "    <menuitem action='PTV_ContextDuplicate'/>"
          "    <menu action='PTV_ContextEdit'>";
    
    //// Partie importer depuis ex ConditionsBox::PortsTreeView::makeMenuEdit
    utils::ModuleList lst;
    utils::ModuleList::iterator it;
    mParent->getGVLE()->pluginFactory().getGvleModelingPlugins(&lst);
    
    mPopupActionGroup->add(Gtk::Action::create("PTV_ContextEdit", _("_Edit")));
    for (it = lst.begin(); it != lst.end(); ++it) {
        name_item ="PTV_Edit_" + it->package + "_" + it->library;
        mPopupActionGroup->add(Gtk::Action::create(name_item, it->package + "/" + it->library),
            sigc::bind<std::string>(sigc::mem_fun(
                    *this, &ConditionsBox::PortsTreeView::onEdit),
                    it->package + "/" + it->library));
	       
        ui_info = ui_info + ""
	  "      <menuitem action='" + name_item + "'/>";
    }
    //// Fin Partie importer depuis ex ConditionsBox::PortsTreeView::makeMenuEdit
    
    ui_info = ui_info + ""
          "    </menu>"
          "  </popup>"
          "</ui>";

    try {
        mUIManager->add_ui_from_string(ui_info);
        mMenuPopup = (Gtk::Menu *) (
            mUIManager->get_widget("/PTView_Popup"));
    } catch(const Glib::Error& ex) {
        std::cerr << "building menus failed: PTView_Popup" <<  ex.what();
    }
    
    if (!mMenuPopup)
        std::cerr << "menu not found : PTView_Popup\n";
}

ConditionsBox::PortsTreeView::~PortsTreeView()
{
}

bool ConditionsBox::PortsTreeView::on_button_press_event(
    GdkEventButton* event)
{
    //Call base class, to allow normal handling,
    //such as allowing the row to be selected by the right-click:
    bool return_value = TreeView::on_button_press_event(event);

    //Then do our custom stuff:
    if (mCondition and event->type == GDK_BUTTON_PRESS and event->button == 3)
    {
	mMenuPopup->popup(event->button, event->time);
    }

    return return_value;
}

void ConditionsBox::PortsTreeView::clear()
{
    mRefTreeModel->clear();
}

void ConditionsBox::PortsTreeView::build()
{
    if (mCondition) {
	mRefTreeModel->clear();

	const vpz::ConditionValues& conditionValues =
	    mCondition->conditionvalues();
	vpz::ConditionValues::const_iterator it = conditionValues.begin();

	for (it = conditionValues.begin(); it != conditionValues.end(); ++it) {
	    Gtk::TreeModel::Row row = *(mRefTreeModel->append());

	    row[mColumns.m_col_name] = it->first;
	}
    }
}

void ConditionsBox::PortsTreeView::on_select()
{
    Gtk::TreeModel::iterator it = mRefTreeSelection->get_selected();

    if (it) {
        Glib::ustring name = (*it)[mColumns.m_col_name];

	mParent->buildTreeValues(mCondition->name(), name);
    }
}

void ConditionsBox::PortsTreeView::on_add()
{
    SimpleTypeBox box(_("Name of the parameter ?"), "");
    std::list < std::string > list;

    mCondition->portnames(list);

    std::string name = boost::trim_copy(box.run());
    std::list < std::string >::const_iterator it =
	std::find(list.begin(), list.end(), name);

    if (box.valid() and not name.empty() and it == list.end()) {
        mCondition->add(name);
        mParent->buildTreePorts(mCondition->name());
    }
}

void ConditionsBox::PortsTreeView::on_remove()
{
    Gtk::TreeModel::iterator it = mRefTreeSelection->get_selected();

    if (it) {
        Glib::ustring name = (*it)[mColumns.m_col_name];
        mCondition->del(name);
        mParent->buildTreePorts(mCondition->name());
    }
}

void ConditionsBox::PortsTreeView::onRename()
{
    Gtk::TreeModel::iterator it_select = mRefTreeSelection->get_selected();
    std::list < std::string > list;
    mCondition->portnames(list);
    if (it_select) {
	Glib::ustring oldname = (*it_select)[mColumns.m_col_name];
        SimpleTypeBox box(_("Name of the condition ?"), oldname);
        std::string newname = boost::trim_copy(box.run());
        std::list < std::string >::const_iterator it_find =
            std::find(list.begin(), list.end(), newname);
        if (box.valid() and not newname.empty() and it_find == list.end()){
            mCondition->rename(oldname, newname);
            mParent->buildTreePorts(mCondition->name());
        }
    }
}

void ConditionsBox::PortsTreeView::onDuplicate()
{
    Gtk::TreeModel::iterator it_select = mRefTreeSelection->get_selected();
    std::list < std::string > list;
    mCondition->portnames(list);
    if (it_select) {
	Glib::ustring name = (*it_select)[mColumns.m_col_name];
        int number = 1;
        std::string copy;
        std::list < std::string >::const_iterator it_find;

        do {
            copy = name
                + "_"
                + boost::lexical_cast< std::string >(number);
            ++number;
            it_find =
                std::find(list.begin(), list.end(), copy);
        } while (it_find != list.end());

        mCondition->add(copy);

        value::Set& set = mCondition->getSetValues(name);
        value::VectorValue& vector = set.value();

        value::VectorValue::iterator it = vector.begin();
        while (it != vector.end()) {
            mCondition->addValueToPort(copy, (*it)->clone());
            it++;
        }

        mParent->buildTreePorts(mCondition->name());
    }
}

void ConditionsBox::PortsTreeView::onEdit(std::string pluginName)
{

    Gtk::TreeModel::iterator it = mRefTreeSelection->get_selected();

    if(it) {

	std::string name = Glib::ustring((*it)[mColumns.m_col_name]);

	PluginFactory& plf = mParent->getGVLE()->pluginFactory();

	try {
	    ModelingPluginPtr plugin = plf.getModelingPlugin(pluginName,
	            mParent->getGVLE()->currentPackage().name());
	    plugin->start(*mCondition, name);
	    mParent->buildTreeValues(mCondition->name(), name);
	} catch(const std::exception& /*e*/) {
	}
    }
}

void ConditionsBox::PortsTreeView::onEditionStarted(
    Gtk::CellEditable* cellEditable, const Glib::ustring& /* path */)
{
    Gtk::TreeModel::iterator it_select = mRefTreeSelection->get_selected();
    if (it_select) {
	mOldName = (*it_select)[mColumns.m_col_name];
    }

    if(mValidateRetry)
    {
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

void ConditionsBox::PortsTreeView::onEdition(
        const Glib::ustring& pathString,
        const Glib::ustring& newName)
{
    Gtk::TreePath path(pathString);

    Gtk::TreeModel::iterator it_select = mRefTreeSelection->get_selected();
    std::list < std::string > list;
    mCondition->portnames(list);
    if (it_select) {
        std::list < std::string >::const_iterator it_find =
            std::find(list.begin(), list.end(), newName);
        if (it_find == list.end() and not newName.empty()){
            mCondition->rename(mOldName, newName);
            mParent->buildTreePorts(mCondition->name());
        }
    }
    build();
}



////
// ConditionsBox
////

ConditionsBox::ConditionsBox(const Glib::RefPtr < Gtk::Builder >& xml,
			     GVLE* gvle) :
    m_gvle(gvle),
    mConditions(0),
    mDialog(0),
    mButtonCancel(0),
    m_xml(xml)
{
    xml->get_widget("DialogConditions", mDialog);

    m_xml->get_widget_derived("simuTreeConditions", m_treeConditions);
    m_treeConditions->setParent(this);
    m_treeConditions->makeMenus ();

    m_xml->get_widget_derived("simuTreePorts", m_treePorts);
    m_treePorts->setParent(this);
    m_treePorts->makeMenus ();
    
    m_xml->get_widget_derived("simuTreeValues", m_treeValues);

    xml->get_widget("ButtonConditionsCancel", mButtonCancel);
    if (mButtonCancel)
        mButtonCancel->signal_clicked().connect(
	    sigc::mem_fun
	    (*this, &ConditionsBox::on_cancel));
}

ConditionsBox::~ConditionsBox()
{
}

void ConditionsBox::buildTreeConditions()
{
    m_treePorts->clear();
    m_treeValues->clear();
    m_treeConditions->setConditions(mConditions);
    m_treePorts->setCondition(0);
    m_treeValues->setCondition(0);
    m_treeConditions->build();
}

void ConditionsBox::buildTreePorts(const std::string& conditionName)
{
    m_treeValues->clear();
    m_treePorts->setCondition(&mConditions->get(conditionName));
    m_treeValues->setCondition(0);
    m_treePorts->build();
}

void ConditionsBox::buildTreeValues(const std::string& conditionName,
				    const std::string& portName)
{
    value::Set& values(mConditions->get(conditionName).getSetValues(portName));

    m_treeValues->setCondition(&mConditions->get(conditionName));
    m_treeValues->makeTreeView(values);
}

int ConditionsBox::run(const vpz::Conditions& conditions)
{
    mRenameList.clear();
    mDialog->hide();

    mConditions = new vpz::Conditions(conditions);
    buildTreeConditions();
    mDialog->show_all();
    int response = mDialog->run();
    mDialog->hide();
    return response;
}

renameList ConditionsBox::apply(vpz::Conditions& conditions)
{
    conditions.clear();
    const vpz::ConditionList& list = mConditions->conditionlist();
    vpz::ConditionList::const_iterator it = list.begin();

    while (it != list.end()) {
        conditions.add(it->second);

        ++it;
    }

    delete mConditions;
    mConditions = 0;

    return mRenameList;
}

void ConditionsBox::on_cancel()
{
    mRenameList.clear();
    delete mConditions;
    mConditions = 0;
}

} } // namespace vle gvle
