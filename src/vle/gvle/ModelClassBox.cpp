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
#include <boost/algorithm/string.hpp>
#include <vle/gvle/View.hpp>
#include <vle/gvle/InteractiveTypeBox.hpp>
#include <iostream>

namespace vle
{
namespace gvle {

ModelClassBox::ModelClassBox(Glib::RefPtr < Gnome::Glade::Xml > xml,
			     Modeling* m):
        mModeling(m),
	mNewModelBox(new NewModelClassBox(xml,m)),
        mClasses_backup(0)
{
    set_title(_("Model Class Tree"));
    set_default_size(200, 350);
    set_border_width(5);
    mScrolledWindow.add(mTreeView);
    mScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

    mRefTreeModel = Gtk::TreeStore::create(mColumns);
    mTreeView.set_model(mRefTreeModel);
    mTreeView.append_column(_("Name"), mColumns.mName);

    mTreeView.signal_row_activated().connect(
	sigc::mem_fun(*this, &ModelClassBox::row_activated));
    mTreeView.signal_button_release_event().connect(
	sigc::mem_fun(*this, &ModelClassBox::onButtonRealeaseModels));

    mTreeView.expand_all();
    mTreeView.set_rules_hint(true);
    add(mScrolledWindow);
    initMenuPopupModels();
}

void ModelClassBox::initMenuPopupModels()
{
    Gtk::Menu::MenuList& menulist(mMenuPopup.items());

    menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Add"),
		sigc::mem_fun(
		    *this,
		    &ModelClassBox::onAdd)));

	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Remove"),
		sigc::mem_fun(
		    *this,
		    &ModelClassBox::onRemove)));

	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Rename"),
		sigc::mem_fun(
		    *this,
		    &ModelClassBox::onRename)));

	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("_Export Class"),
		sigc::mem_fun(
		    *this,
		    &ModelClassBox::onExportVpz)));

	menulist.push_back(
	    Gtk::Menu_Helpers::MenuElem(
		_("Import Model as _Class"),
		sigc::mem_fun(
		    *this,
		    &ModelClassBox::onImportModelAsClass)));

    mMenuPopup.accelerate(mTreeView);
}

ModelClassBox::~ModelClassBox()
{
    hide_all();
    delete mNewModelBox;
}

void ModelClassBox::hide()
{
    hide_all();
}

bool ModelClassBox::onButtonRealeaseModels(GdkEventButton* event)
{
    if (event->button == 3) {
	mMenuPopup.popup(event->button, event->time);
    }
    return true;
}

void ModelClassBox::onAdd()
{
    mNewModelBox->run();
    parseClass();
}

void ModelClassBox::onRemove()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection =  mTreeView.get_selection();
    if (refSelection) {
        Gtk::TreeModel::iterator iter = refSelection->get_selected();
        if (iter and (mRefTreeModel->iter_depth(iter) == 0)) {
	    Gtk::TreeModel::Row row = *iter;

            vpz::Class& class_ = mModeling->vpz().project().classes().get(
		row.get_value(mColumns.mName));
            if (class_.model()->isCoupled()) {
                graph::CoupledModel* c_model = dynamic_cast<graph::CoupledModel*>(class_.model());
                if (mModeling->existView(c_model)) {
                    mModeling->delViewOnModel(c_model);
                }
            }
            mModeling->vpz().project().classes().del(row.get_value(mColumns.mName));
	    parseClass();
        }
    }
}

void ModelClassBox::onRename()
{
   SimpleTypeBox box(_("Name of the Class ?"));
   std::string name = boost::trim_copy(box.run());
   Glib::RefPtr<Gtk::TreeView::Selection> refSelection =  mTreeView.get_selection();
   if (refSelection and box.valid() and not name.empty()) {
       Gtk::TreeModel::iterator iter = refSelection->get_selected();
       if (iter and (mRefTreeModel->iter_depth(iter) == 0)) {
	   Gtk::TreeModel::Row row = *iter;
	   vpz::Class& oldClass = mModeling->vpz().project().classes().get(
	       row.get_value(mColumns.mName));
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
	   mModeling->vpz().project().classes().get(row.get_value(mColumns.mName)).setModel(0);
	   mModeling->vpz().project().classes().del(row.get_value(mColumns.mName));
	   parseClass();
       }
   }
}

void ModelClassBox::onExportVpz()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = mTreeView.get_selection();
    if (refSelection) {
	Gtk::TreeModel::iterator iter = refSelection->get_selected();
	if (iter and (mRefTreeModel->iter_depth(iter) == 0)) {
	    Gtk::TreeModel::Row row = *iter;
	    vpz::Class& currentClass = mModeling->vpz().project().classes().get(
		row.get_value(mColumns.mName));
	    graph::Model* model = currentClass.model();

	    Gtk::FileChooserDialog file(_("VPZ file"), Gtk::FILE_CHOOSER_ACTION_SAVE);
	    file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	    file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
	    Gtk::FileFilter filter;
	    filter.set_name(_("Vle Project gZipped"));
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

void ModelClassBox::onImportModelAsClass()
{
    Gtk::FileChooserDialog file(_("VPZ file"), Gtk::FILE_CHOOSER_ACTION_OPEN);
    file.set_transient_for(*this);
    file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
    Gtk::FileFilter filter;
    filter.set_name(_("Vle Project gZipped"));
    filter.add_pattern("*.vpz");
    file.add_filter(filter);

    if (file.run() == Gtk::RESPONSE_OK) {
	std::string project_file = file.get_filename();
        try {
            vpz::Vpz* import = new vpz::Vpz(project_file);
	    InteractiveTypeBox box(_("Name of the Class ?"),
				   &mModeling->vpz().project().classes());
	    std::string name = boost::trim_copy(box.run());
            if (not name.empty())
		mModeling->importModelToClass(import, name);
            delete import;
        } catch (std::exception& E) {
            std::cout << "Exception :\n" << E.what() << "\n";
        }
    }
}

void ModelClassBox::parseClass()
{
    mRefTreeModel->clear();

    vpz::Classes::iterator iter = mModeling->vpz().project().classes().begin();
    while (iter != mModeling->vpz().project().classes().end()) {
	Gtk::TreeModel::Row row = addClass(mModeling->vpz().project().classes().get(
					       iter->second.name()));
	graph::Model* top = mModeling->vpz().project().classes().get(iter->second.name()).model();
	Gtk::TreeModel::Row row2 = addSubModel(row, top);
	if (top->isCoupled()) {
	    parseModel(row2,  (graph::CoupledModel*)top);
	}
	++iter;
    }
    mTreeView.expand_all();
}

Gtk::TreeModel::Row
ModelClassBox::addClass(vpz::Class& classe)
{
    Gtk::TreeModel::Row row = *(mRefTreeModel->append());
    row[mColumns.mName] = classe.name();
    return row;
}

Gtk::TreeModel::Row
ModelClassBox::addSubModel(Gtk::TreeModel::Row tree, graph::Model* model)
{
    Gtk::TreeModel::Row row = *(mRefTreeModel->append(tree.children()));
    row[mColumns.mName] = model->getName();
    row[mColumns.mModel] = model;
    return row;
}

void ModelClassBox::parseModel(Gtk::TreeModel::Row row,
                              const graph::CoupledModel* top)
{
    const graph::ModelList& list = top->getModelList();
    graph::ModelList::const_iterator it = list.begin();

    while (it != list.end()) {
        Gtk::TreeModel::Row row2 = addSubModel(row, it->second);
        if (it->second->isCoupled() == true) {
            parseModel(row2, (graph::CoupledModel*)it->second);
        }
        ++it;
    }
}

bool ModelClassBox::on_key_release_event(GdkEventKey* event)
{
    if (((event->state & GDK_CONTROL_MASK) and event->keyval == GDK_w) or
            (event->keyval == GDK_Escape)) {
        mModeling->hideModelClassBox();
    }
    return true;
}

void ModelClassBox::showRow(const std::string& model_name)
{
    mSearch.assign(model_name);
    mRefTreeModel->foreach(sigc::mem_fun(*this, &ModelClassBox::on_foreach));
}

void ModelClassBox::row_activated(const Gtk::TreeModel::Path& path,
                                 Gtk::TreeViewColumn* column)
{
    if (column) {
        Gtk::TreeIter iter = mRefTreeModel->get_iter(path);
        Gtk::TreeRow row = (*iter);
	if (mRefTreeModel->iter_depth(iter) == 0) {
	    mModeling->addViewClass(mModeling->getClassModel(row.get_value(mColumns.mName)),
				    row.get_value(mColumns.mName));
	} else {
	    std::vector <std::string> Vec;
	    Glib::ustring str = path.to_string();
	    boost::split(Vec, str, boost::is_any_of(":"));
	    Gtk::TreeIter iterClass = mRefTreeModel->get_iter(Vec[0]);
	    Gtk::TreeRow rowClass = (*iterClass);
	    mModeling->addViewClass(row.get_value(mColumns.mModel),
				    rowClass.get_value(mColumns.mName));
	}
    }
}


bool ModelClassBox::on_foreach(const Gtk::TreeModel::Path&,
                              const Gtk::TreeModel::iterator& iter)
{
    if ((*iter).get_value(mColumns.mName) == mSearch) {
        mTreeView.set_cursor(mRefTreeModel->get_path(iter));
        return true;
    }
    return false;
}

}
} // namespace vle gvle
