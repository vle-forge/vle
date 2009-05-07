/**
 * @file vle/gvle/ModelClassBox.cpp
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


#include <vle/gvle/ModelClassBox.hpp>
#include <vle/gvle/Modeling.hpp>
#include <boost/algorithm/string/detail/trim.hpp>
#include <vle/gvle/View.hpp>

namespace vle
{
namespace gvle {

ModelClassBox::ClassTreeView::ClassTreeView(
    BaseObjectType* cobject,
    const Glib::RefPtr<Gnome::Glade::Xml>&):
    Gtk::TreeView(cobject)
{
    mRefTreeModel = Gtk::ListStore::create(mColumns);
    set_model(mRefTreeModel);
    append_column("Name", mColumns.m_col_name);

    {
	Gtk::Menu::MenuList& menulist = mMenuPopup.items();

	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		"_Add",
		sigc::mem_fun(
		    *this,
		    &ModelClassBox::ClassTreeView::onAdd)));

	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		"_Remove",
		sigc::mem_fun(
		    *this,
		    &ModelClassBox::ClassTreeView::onRemove)));

	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		"_Rename",
		sigc::mem_fun(
		    *this,
		    &ModelClassBox::ClassTreeView::onRename)));

	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		"_Export Class",
		sigc::mem_fun(
		    *this,
		    &ModelClassBox::ClassTreeView::onExportVpz)));
    }

    mMenuPopup.accelerate(*this);
}

ModelClassBox::ClassTreeView::~ClassTreeView()
{
}

bool ModelClassBox::ClassTreeView::on_button_press_event(
    GdkEventButton* event)
{
    bool return_value = TreeView::on_button_press_event(event);
    if ( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) ) {
	mMenuPopup.popup(event->button, event->time);
    }
    if ( (event->type == GDK_2BUTTON_PRESS) ) {
	 Glib::RefPtr<Gtk::TreeView::Selection> refSelection =  get_selection();
	 if (refSelection) {
	     Gtk::TreeModel::iterator iter = refSelection->get_selected();
	     if (iter) {
		 Gtk::TreeModel::Row row = *iter;
		 mModeling->addViewClass(
		     mModeling->getClassModel(row.get_value(mColumns.m_col_name)),
		     row.get_value(mColumns.m_col_name));
	     }
	 }
    }
    return return_value;
}

void ModelClassBox::ClassTreeView::build()
{
    using namespace vpz;
    mRefTreeModel->clear();

    ClassList& class_list = mModeling->vpz().project().classes().list();
    ClassList::iterator it = class_list.begin();
    while (it != class_list.end()) {
        Gtk::TreeModel::Row row = *(mRefTreeModel->append());
        row[mColumns.m_col_name] = it->first;

        ++it;
    }
}

void ModelClassBox::ClassTreeView::onAdd()
{
    mNewModelBox->run();
    build();
}

void ModelClassBox::ClassTreeView::onRemove()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection =  get_selection();
    if (refSelection) {
        Gtk::TreeModel::iterator iter = refSelection->get_selected();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;

            vpz::Class& class_ = mModeling->vpz().project().classes().get(
		row.get_value(mColumns.m_col_name));
            if (class_.model()->isCoupled()) {
                graph::CoupledModel* c_model = dynamic_cast<graph::CoupledModel*>(class_.model());
                if (mModeling->existView(c_model)) {
                    mModeling->delViewOnModel(c_model);
                }
            }
            mModeling->vpz().project().classes().del(row.get_value(mColumns.m_col_name));
            build();
        }
    }
}

void ModelClassBox::ClassTreeView::onRename()
{
    SimpleTypeBox box("Name of the Classe ?");
    std::string name = boost::trim_copy(box.run());
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection =  get_selection();
    if (refSelection and box.valid() and not name.empty()) {
        Gtk::TreeModel::iterator iter = refSelection->get_selected();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
	    vpz::Class& oldClass = mModeling->vpz().project().classes().get(
		row.get_value(mColumns.m_col_name));
	    graph::Model* model = oldClass.model();
	    vpz::AtomicModelList& atomicModel = oldClass.atomicModels();
	    mModeling->vpz().project().classes().add(name);
	    mModeling->vpz().project().classes().get(name).setModel(model);
	    mModeling->vpz().project().classes().get(name).setAtomicModel(atomicModel);

	    if (oldClass.model()->isCoupled()) {
                graph::CoupledModel* c_model = dynamic_cast<graph::CoupledModel*>(oldClass.model());
                if (mModeling->existView(c_model)) {
                    mModeling->delViewOnModel(c_model);
                }
            }
	    mModeling->vpz().project().classes().get(row.get_value(mColumns.m_col_name)).setModel(0);
            mModeling->vpz().project().classes().del(row.get_value(mColumns.m_col_name));
            build();
	}
    }
}

void ModelClassBox::ClassTreeView::onExportVpz()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    if (refSelection) {
	Gtk::TreeModel::iterator iter = refSelection->get_selected();
	if (iter) {
	    Gtk::TreeModel::Row row = *iter;
	    vpz::Class& currentClass = mModeling->vpz().project().classes().get(
		row.get_value(mColumns.m_col_name));
	    graph::Model* model = currentClass.model();

	    Gtk::FileChooserDialog file("VPZ file", Gtk::FILE_CHOOSER_ACTION_SAVE);
	    file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	    file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
	    Gtk::FileFilter filter;
	    filter.set_name("Vle Project gZipped");
	    filter.add_pattern("*.vpz");
	    file.add_filter(filter);

	    if (file.run() == Gtk::RESPONSE_OK) {
		std::string filename(file.get_filename());
		vpz::Vpz::fixExtension(filename);

		vpz::Vpz* save = new vpz::Vpz();
		if (utils::exist_file(filename)) {
		    save->clear();
		}
		save->setFilename(filename);

		mModeling->exportClass(model, currentClass, save);
		delete save;
	    }
	}
    }
}

ModelClassBox::ModelClassBox(Glib::RefPtr<Gnome::Glade::Xml> xml, Modeling* m):
        mXml(xml),
        mModeling(m),
        mNewModelBox(new NewModelClassBox(xml,m)),
        mClasses_backup(0)
{
    xml->get_widget("WindowModelClass", mWindow);

    xml->get_widget_derived("TreeViewModelClass", mClass);
}

ModelClassBox::~ModelClassBox()
{
    mWindow->hide_all();

    delete mNewModelBox;
}

void ModelClassBox::show()
{
    using namespace vpz;

    mClasses_backup = new vpz::ClassList(mModeling->vpz().project().classes().list());
    mClass->setModel(mModeling);
    mClass->setBox(mNewModelBox);
    mClass->build();

    mWindow->show_all();
}

void ModelClassBox::hide()
{
    mWindow->hide_all();
}

}
} // namespace vle gvle
