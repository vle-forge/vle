/**
 * @file vle/gvle/ImportClassesBox.hpp
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

#include <vle/gvle/ImportClassesBox.hpp>
#include <vle/gvle/InteractiveTypeBox.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/vpz/Classes.hpp>
#include <vle/vpz/Vpz.hpp>

namespace vle { namespace gvle {

/**
 * ClassesTreeView
 */
ImportClassesBox::ClassesTreeView::ClassesTreeView(
    BaseObjectType* cobject,
    const Glib::RefPtr<Gnome::Glade::Xml>& /*refGlade*/) :
    Gtk::TreeView(cobject),
    mClasses(0)
{
    mRefTreeModel = Gtk::ListStore::create(mColumns);
    set_model(mRefTreeModel);
    append_column(_("Class name"), mColumns.m_col_name);
    mRefTreeSelection = get_selection();
}

ImportClassesBox::ClassesTreeView::~ClassesTreeView()
{
}

void ImportClassesBox::ClassesTreeView::build(vpz::Classes* classes)
{
    mClasses = classes;
    clear();

    vpz::Classes::const_iterator it;
    for (it = classes->begin(); it != classes->end(); ++it) {
	Gtk::TreeModel::Row row = *(mRefTreeModel->append());
	row[mColumns.m_col_name] = it->first;
    }
}

void ImportClassesBox::ClassesTreeView::clear()
{
    mRefTreeModel->clear();
}

void ImportClassesBox::ClassesTreeView::on_drag_data_get(
    const Glib::RefPtr<Gdk::DragContext>& ,
    Gtk::SelectionData& selection_data, guint, guint)
{
    Glib::RefPtr<Gtk::TreeSelection> srcSelect(get_selection());
    if (srcSelect) {
	Gtk::TreeModel::iterator iter = srcSelect->get_selected();
	if (iter) {
	    Gtk::TreeModel::Row row = *iter;
	    std::string className = row.get_value(mColumns.m_col_name);
	    selection_data.set(selection_data.get_target(), className);
	}
    }
}

void ImportClassesBox::ClassesTreeView::on_drag_data_received(
    const Glib::RefPtr<Gdk::DragContext>& context, int, int,
    const Gtk::SelectionData& selection_data, guint, guint time)
{
    std::string className = selection_data.get_data_as_string();

    if (mClasses->exist(className)) {
	const std::string title(_("Choose a new class name"));
	InteractiveTypeBox box(title, mClasses, className);
	std::string newClassName = box.run();
	if (not newClassName.empty())
	    mParent->importClass(className, newClassName);
    } else {
	mParent->importClass(className);
    }
    build(mClasses);

    if (context->get_action() == Gdk::ACTION_MOVE)
	context->drag_finish(true, false, time);
}

/**
 * ImportClassesBox
 */
ImportClassesBox::ImportClassesBox(Glib::RefPtr<Gnome::Glade::Xml> xml,
				   Modeling* modeling) :
    mXml(xml),
    mModeling(modeling)
{
    xml->get_widget("DialogImportClassesBox", mDialog);

    //TreeViews
    xml->get_widget_derived("TreeViewProjectClasses", mProjectClasses);
    mProjectClasses->setParent(this);

    xml->get_widget_derived("TreeViewVpzClasses", mVpzClasses);
    mVpzClasses->setParent(this);

    //Buttons
    xml->get_widget("ButtonImportClassesClose", mButtonClose);
    mButtonClose->signal_clicked().connect(
        sigc::mem_fun(*this, &ImportClassesBox::onClose));
}

void ImportClassesBox::show(vpz::Vpz* src)
{
    mSrc = src;

    mProjectClasses->build(&mModeling->vpz().project().classes());
    mProjectClasses->enable_model_drag_dest();
    mVpzClasses->build(&src->project().classes());
    mVpzClasses->enable_model_drag_source();

    mDialog->show_all();
    mDialog->run();
}

void ImportClassesBox::importClass(std::string& className,
				   std::string newClassName)
{
    vpz::Class& srcClass = mSrc->project().classes().get(className);

    if (newClassName.empty()) {
	newClassName = className;
    }
    vpz::Class& destClass =
	mModeling->vpz().project().classes().add(newClassName);

    graph::Model* import = srcClass.model();
    if (import->isCoupled()) {
	graph::CoupledModel* model = graph::Model::toCoupled(import->clone());
	destClass.setModel(model);
    } else {
	graph::AtomicModel* model = graph::Model::toAtomic(import->clone());
	destClass.setModel(model);
    }
    std::for_each(srcClass.atomicModels().begin(),
		  srcClass.atomicModels().end(),
		  vpz::CopyAtomicModel(destClass.atomicModels(), *destClass.model()));
}

void ImportClassesBox::add_atomic_model_to_class(graph::AtomicModel* model,
						 vpz::Class& srcClass,
						 vpz::Class& destClass)
{
    const vpz::AtomicModel& vpz_model = srcClass.atomicModels().get(model);
    destClass.atomicModels().add(model, vpz_model);
}

void ImportClassesBox::add_coupled_model_to_class(graph::CoupledModel* model,
						  vpz::Class& srcClass,
						  vpz::Class& destClass)
{
    graph::ModelList& list = model->getModelList();
    if (!list.empty()) {
	graph::ModelList::const_iterator it = list.begin();
	while (it!= list.end()) {
	    if (it->second->isAtomic()) {
		add_atomic_model_to_class(graph::Model::toAtomic(it->second),
					  srcClass, destClass);
	    } else {
		add_coupled_model_to_class(graph::Model::toCoupled(it->second),
					   srcClass, destClass);
	    }
	    ++it;
	}
    }
}

void ImportClassesBox::onClose()
{
    mDialog->hide_all();
    mModeling->getGVLE()->redrawModelClassBox();
}

}}

