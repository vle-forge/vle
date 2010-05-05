/**
 * @file vle/gvle/ModelClassBox.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
 * Copyright (C) 2003-2010 ULCO http://www.univ-littoral.fr
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
#include <vle/gvle/Editor.hpp>
#include <vle/gvle/View.hpp>
#include <vle/utils/Path.hpp>
#include <boost/algorithm/string/detail/trim.hpp>
#include <boost/algorithm/string.hpp>
#include <vle/gvle/InteractiveTypeBox.hpp>
#include <iostream>

namespace vle { namespace gvle {

ModelClassBox::ModelClassBox(BaseObjectType* cobject,
                             Glib::RefPtr < Gnome::Glade::Xml > xml):
    Gtk::TreeView(cobject),
    mXml(xml),
    mClasses_backup(0),
    mDelayTime(0)
{
    mRefTreeModel = Gtk::TreeStore::create(mColumns);
    set_model(mRefTreeModel);
    mColumnName = append_column_editable(_("Class"), mColumns.mName);

    mCellRenderer = dynamic_cast<Gtk::CellRendererText*>(
        get_column_cell_renderer(mColumnName - 1));
    mCellRenderer->property_editable() = true;
    mCellRenderer->signal_editing_started().connect(
        sigc::mem_fun(*this, &ModelClassBox::onEditionStarted));
    mCellRenderer->signal_edited().connect(
        sigc::mem_fun(*this, &ModelClassBox::onEdition));

    signal_event().connect(
        sigc::mem_fun(*this, &ModelClassBox::onExposeEvent));
    signal_cursor_changed().connect(
        sigc::mem_fun(*this, &ModelClassBox::on_cursor_changed));
    signal_button_release_event().connect(
        sigc::mem_fun(*this, &ModelClassBox::onButtonRealeaseModels));

    expand_all();
    set_rules_hint(true);
    initMenuPopupModels();
}

void ModelClassBox::on_cursor_changed()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection =  get_selection();
    if (refSelection) {
        Gtk::TreeModel::iterator iter = refSelection->get_selected();
        if (iter and (mRefTreeModel->iter_depth(iter) == 0)) {
            Gtk::TreeModel::Row row = *iter;
            mModeling->setSelectedClass(row.get_value(mColumns.mName));
        } else {
            mModeling->setSelectedClass("");
        }
    }
}

void ModelClassBox::createNewModelBox(Modeling* m)
{
    mModeling = m;
    mNewModelBox = new NewModelClassBox(mXml,m);
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

    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem(
            _("Import Classes from Vpz"),
            sigc::mem_fun(
                *this,
                &ModelClassBox::onImportClassesFromVpz)));

    mMenuPopup.accelerate(*this);
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
    Gtk::Menu::MenuList& menulist(mMenuPopup.items());
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    Gtk::TreeModel::iterator iter = refSelection->get_selected();

    if (iter and (mRefTreeModel->iter_depth(iter) == 0)) {
        menulist[1].set_sensitive(true);
    } else {
        menulist[1].set_sensitive(false);
    }

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
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection =  get_selection();

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
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection =  get_selection();
    if (refSelection) {
        Gtk::TreeModel::iterator iter = refSelection->get_selected();
        if (iter) {
            if (mRefTreeModel->iter_depth(iter) == 0) {
                Gtk::TreeModel::Row row = *iter;
                mOldName = row.get_value(mColumns.mName);
                SimpleTypeBox box(_("Name of the Class ?"), mOldName);
                std::string name = boost::trim_copy(box.run());
                if (box.valid() and not name.empty()) {
                    onRenameClass(name);
                }
            } else {
                Gtk::TreeModel::Row row = *iter;
                std::string oldname(row.get_value(mColumns.mName));
                SimpleTypeBox box(_("New name of this model?"), oldname);
                std::string newname = boost::trim_copy(box.run());
                if (box.valid() and not newname.empty() and newname != oldname) {
                    try {
                        row[mColumns.mName] = newname;
                        graph::Model::rename(row[mColumns.mModel], newname);
                        mModeling->setModified(true);
                    } catch(utils::DevsGraphError dge) {
                        row[mColumns.mName] = oldname;
                    }
                }
            }
            parseClass();
        }
    }
}

void ModelClassBox::onRenameClass(const std::string& newName)
{
    vpz::Class& oldClass = mModeling->vpz().project().classes().get(mOldName);
    graph::Model* model = oldClass.model();
    vpz::AtomicModelList& atomicModel = oldClass.atomicModels();
    mModeling->vpz().project().classes().add(newName);
    mModeling->vpz().project().classes().get(newName).setModel(model);
    mModeling->vpz().project().classes().get(newName).
        setAtomicModel(atomicModel);

    if (oldClass.model()->isCoupled()) {
        graph::CoupledModel* c_model =
            dynamic_cast<graph::CoupledModel*>(oldClass.model());

        if (mModeling->existView(c_model)) {
            mModeling->delViewOnModel(c_model);
            mModeling->addViewClass(c_model, newName);
        }
    }
    mModeling->vpz().project().classes().get(mOldName).setModel(0);
    mModeling->vpz().project().classes().del(mOldName);
}

void ModelClassBox::onExportVpz()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
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
                if (utils::Path::existFile(filename)) {
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

void ModelClassBox::onImportClassesFromVpz()
{
    Gtk::FileChooserDialog file(_("VPZ file"), Gtk::FILE_CHOOSER_ACTION_OPEN);
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
            mModeling->setModified(true);
            mModeling->importClasses(import);
            delete import;
        } catch (std::exception& E) {
            std::cout << "Exception :\n" << E.what() << "\n";
        }
    }
}

void ModelClassBox::parseClass()
{
    mRefTreeModel->clear();

    const vpz::Vpz& vpz = ((const Modeling*)mModeling)->vpz();
    vpz::Classes::const_iterator iter = vpz.project().classes().begin();
    while (iter != vpz.project().classes().end()) {
        Gtk::TreeModel::Row row = addClass(vpz.project().classes().get(
                                               iter->second.name()));
        const graph::Model* top = vpz.project().classes()
            .get(iter->second.name()).model();
        Gtk::TreeModel::Row row2 = addSubModel(row, (graph::Model*)top);
        if (top->isCoupled()) {
            parseModel(row2, (graph::CoupledModel*)top);
        }
        ++iter;
    }
    expand_all();
}

void ModelClassBox::clear()
{
    mRefTreeModel->clear();
}

Gtk::TreeModel::Row ModelClassBox::addClass(const vpz::Class& classe)
{
    Gtk::TreeModel::Row row = *(mRefTreeModel->append());
    row[mColumns.mName] = classe.name();
    return row;
}

Gtk::TreeModel::Row ModelClassBox::addSubModel(Gtk::TreeModel::Row tree,
                                               graph::Model* model)
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

void ModelClassBox::showRow(const std::string& model_name)
{
    mSearch.assign(model_name);
    mRefTreeModel->foreach(sigc::mem_fun(*this, &ModelClassBox::on_foreach));
}

bool ModelClassBox::onExposeEvent(GdkEvent* event)
{
    if (event->type == GDK_2BUTTON_PRESS) {
        mDelayTime = event->button.time;
        Gtk::TreeModel::Path path;
        Gtk::TreeViewColumn* column;
        get_cursor(path, column);
        row_activated(path, column);
        return true;
    }
    if (event->type == GDK_BUTTON_PRESS) {
        if (mDelayTime + 250 < event->button.time) {
            mDelayTime = event->button.time;
            mCellRenderer->property_editable() = true;
        } else {
            mDelayTime = event->button.time;
            mCellRenderer->property_editable() = false;
        }
    }
    return false;

}

void ModelClassBox::row_activated(const Gtk::TreeModel::Path& path,
                                  Gtk::TreeViewColumn* column)
{
    if (column) {
        Gtk::TreeIter iter = mRefTreeModel->get_iter(path);
        Gtk::TreeRow row = (*iter);
        if (mRefTreeModel->iter_depth(iter) == 0) {
            mModeling->addViewClass(
                mModeling->getClassModel(row.get_value(mColumns.mName)),
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
        set_cursor(mRefTreeModel->get_path(iter));
        return true;
    }
    return false;
}

void ModelClassBox::onEditionStarted(Gtk::CellEditable* cell_editable,
                                     const Glib::ustring& /* path */)
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    if (refSelection) {
        Gtk::TreeModel::iterator iter = refSelection->get_selected();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            mOldName = row.get_value(mColumns.mName);
        }
    }

    if(mValidateRetry)
    {
        Gtk::CellEditable* celleditable_validated = cell_editable;
        Gtk::Entry* pEntry = dynamic_cast<Gtk::Entry*>(
            celleditable_validated);
        if(pEntry)
        {
            pEntry->set_text(mInvalidTextForRetry);
            mValidateRetry = false;
            mInvalidTextForRetry.clear();
        }
    }
}

void ModelClassBox::onEdition(
    const Glib::ustring& pathString,
    const Glib::ustring& newName)
{
    Gtk::TreePath path(pathString);

    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    if (refSelection and newName != mOldName) {
        Gtk::TreeModel::iterator iter = refSelection->get_selected();
        if (iter) {
            if ((mRefTreeModel->iter_depth(iter) == 0)
                and (newName != "")
                and not mModeling->vpz().project().classes().exist(newName)) {
                onRenameClass(newName);
            } else {
                if (not newName.empty()) {
                    Gtk::TreeModel::Row row = *iter;
                    try {
                        row[mColumns.mName] = newName;
                        graph::Model::rename(row[mColumns.mModel], newName);
                        mModeling->setModified(true);
                    } catch(utils::DevsGraphError dge) {
                        row[mColumns.mName] = mOldName;
                    }
                }
            }
        }
        parseClass();
    }
}

} } // namespace vle gvle
