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


#include <vle/gvle/ImportClassesBox.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/gvle/GVLE.hpp>
#include <vle/gvle/InteractiveTypeBox.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/vpz/AtomicModel.hpp>
#include <vle/vpz/CoupledModel.hpp>
#include <vle/vpz/Classes.hpp>
#include <vle/vpz/Vpz.hpp>

namespace vle { namespace gvle {

/**
 * ClassesTreeView
 */
ImportClassesBox::ClassesTreeView::ClassesTreeView(
    BaseObjectType* cobject,
    const Glib::RefPtr < Gtk::Builder >& /*refGlade*/) :
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

    if ((context->get_actions () & Gdk::ACTION_MOVE) != 0)
	context->drag_finish(true, false, time);
}

/**
 * ImportClassesBox
 */
ImportClassesBox::ImportClassesBox(const Glib::RefPtr < Gtk::Builder >& xml,
				   Modeling* modeling, GVLE* gvle) :
    mXml(xml),
    mModeling(modeling),
    mGVLE(gvle)
{
    xml->get_widget("DialogImportClassesBox", mDialog);

    //TreeViews
    xml->get_widget_derived("TreeViewProjectClasses", mProjectClasses);
    mProjectClasses->setParent(this);

    xml->get_widget_derived("TreeViewVpzClasses", mVpzClasses);
    mVpzClasses->setParent(this);

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
    mDialog->hide();

    mGVLE->redrawModelClassBox();

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

    vpz::BaseModel* import = srcClass.model();
    if (import->isCoupled()) {
	vpz::CoupledModel* model = vpz::BaseModel::toCoupled(import->clone());
	destClass.setModel(model);
    } else {
	vpz::AtomicModel* model = vpz::BaseModel::toAtomic(import->clone());
	destClass.setModel(model);
    }
    // std::for_each(srcClass.atomicModels().begin(),
    //     	  srcClass.atomicModels().end(),
    //     	  vpz::CopyAtomicModel(destClass.atomicModels(), *destClass.model()));
}

}}

