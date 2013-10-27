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


#include <vle/gvle/ModelClassBox.hpp>
#include <vle/gvle/GVLE.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/Editor.hpp>
#include <vle/gvle/View.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/gvle/InteractiveTypeBox.hpp>
#include <vle/utils/Path.hpp>
#include <boost/algorithm/string/detail/trim.hpp>
#include <boost/algorithm/string.hpp>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/stock.h>

namespace vle { namespace gvle {

ModelClassBox::ModelClassBox(BaseObjectType* cobject,
                             const Glib::RefPtr < Gtk::Builder >& xml):
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

    Glib::RefPtr<Gtk::TreeSelection> selection = Gtk::TreeView::get_selection();

    selection->set_select_function(
        sigc::mem_fun(*this, &ModelClassBox::onSelect) );

    expand_all();
    set_rules_hint(true);
    initMenuPopupModels();

    set_has_tooltip();
    signal_query_tooltip().connect(
        sigc::mem_fun(*this, &ModelClassBox::onQueryTooltip));
}

bool ModelClassBox::onSelect(
    const Glib::RefPtr<Gtk::TreeModel>& model,
    const Gtk::TreeModel::Path& path, bool info)
{
    const Gtk::TreeModel::iterator iter = model->get_iter(path);

    View* view = mGVLE->getEditor()->getDocumentDrawingArea()->getView();

    vpz::BaseModel* mdl;

    if (iter) {
        Gtk::TreeModel::Row row = *iter;
        mdl = (vpz::BaseModel*)row[mColumns.mModel];
        if (mdl) {
            if (info) {
                view->removeFromSelectedModel(mdl);
            } else {
                mGVLE->getModelTreeBox()->selectNone();
                if (not view->existInSelectedModels(mdl)) {
                    if(mdl->getParent()) {
                        mGVLE->addViewClass(mdl->getParent(),
                                                getClassName(path));
                        mGVLE->getEditor()->
                            openTabVpz(mModeling->getFileName(), mdl->getParent());
                    } else {
                        mGVLE->addViewClass(mdl,
                                                getClassName(path));
                    }
                    view = mGVLE->getEditor()->getDocumentDrawingArea()->getView();
                }
                view->clearSelectedModels();
                view->addModelToSelectedModels(mdl);
            }
            view->redraw();
        } else {
            mGVLE->getModelTreeBox()->selectNone();
            mGVLE->addViewClass(mModeling->getClassModel(row.get_value(mColumns.mName)),
                                    row.get_value(mColumns.mName));
        }
    }
    return true;
}

void ModelClassBox::selectNone()
{
    Glib::RefPtr<Gtk::TreeSelection> selection = get_selection();
    selection->unselect_all();
}

Gtk::TreeModel::iterator ModelClassBox::getModelRow(const vpz::BaseModel* mdl,
                                                    Gtk::TreeModel::Children child)
{
    Gtk::TreeModel::iterator iter = child.begin();

    while (iter != child.end()) {
        if ((*iter).get_value(mColumns.mModel) == mdl) {
            return iter;
        }
        if (!((*iter).children().empty())) {
            Gtk::TreeModel::iterator iterin = getModelRow(mdl, (*iter).children());
            if (iterin != (*iter).children().end()) {
                return iterin;
            }
        }
        iter++;
    }
    return iter;
}

void ModelClassBox::showRow(const vpz::BaseModel* mdl)
{
    if (mdl->getParent()) {
        Gtk::TreeModel::iterator iter = getModelRow(mdl->getParent(),
                                                    mRefTreeModel->children());
        expand_to_path(Gtk::TreePath(iter));
    }

    Gtk::TreeModel::iterator iter = getModelRow(mdl, mRefTreeModel->children());
    set_cursor(Gtk::TreePath(iter));
}

void ModelClassBox::onCursorChanged()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
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

void ModelClassBox::createNewModelBox(Modeling* m, GVLE* gvle)
{
    mModeling = m;
    mGVLE = gvle;
    mNewModelBox = new NewModelClassBox(mXml, m, gvle);
}

void ModelClassBox::initMenuPopupModels()
{
    mPopupActionGroup = Gtk::ActionGroup::create("initMenuPopupModels");
    mPopupActionGroup->add(Gtk::Action::create("ModelCB_ContextMenu", _("Context Menu")));
    
    mPopupActionGroup->add(Gtk::Action::create("ModelCB_ContextAdd", _("_Add")),
        sigc::mem_fun(*this, &ModelClassBox::onAdd));
    
    mPopupActionGroup->add(Gtk::Action::create("ModelCB_ContextRemove", _("_Remove")),
        sigc::mem_fun(*this, &ModelClassBox::onRemove));
    
    mPopupActionGroup->add(Gtk::Action::create("ModelCB_ContextRename", _("_Rename")),
        sigc::mem_fun(*this, &ModelClassBox::onRename));

    mPopupActionGroup->add(Gtk::Action::create("ModelCB_ContextExportClass", _("_Export Class")),
        sigc::mem_fun(*this, &ModelClassBox::onExportVpz));
    
    mPopupActionGroup->add(Gtk::Action::create("ModelCB_ContextImportClass", _("Import Model as _Class")),
        sigc::mem_fun(*this, &ModelClassBox::onImportModelAsClass));
    
    mPopupActionGroup->add(Gtk::Action::create("ModelCB_ContextImportVpz", _("Import Classes from Vpz")),
        sigc::mem_fun(*this, &ModelClassBox::onImportClassesFromVpz));

    mPopupUIManager = Gtk::UIManager::create();
    mPopupUIManager->insert_action_group(mPopupActionGroup);

    Glib::ustring ui_info =
        "<ui>"
        "  <popup name='ModelCB_Popup'>"
        "      <menuitem action='ModelCB_ContextAdd'/>"
        "      <menuitem action='ModelCB_ContextRemove'/>"
        "      <menuitem action='ModelCB_ContextRename'/>"
        "      <menuitem action='ModelCB_ContextExportClass'/>"
        "      <menuitem action='ModelCB_ContextImportClass'/>"
        "      <menuitem action='ModelCB_ContextImportVpz'/>"
        "  </popup>"
        "</ui>";

    try {
        mPopupUIManager->add_ui_from_string(ui_info);
        mMenuPopup = (Gtk::Menu *) (
            mPopupUIManager->get_widget("/ModelCB_Popup"));
    } catch(const Glib::Error& ex) {
        std::cerr << "building menus failed: ModelCB_Popup " <<  ex.what();
    }

    if (!mMenuPopup)
        std::cerr << "menu not found : ModelCB_Popup\n";
}

ModelClassBox::~ModelClassBox()
{
    // Seleument hide sur gtk 3
    hide();
    delete mNewModelBox;
}

bool ModelClassBox::onButtonRealeaseModels(GdkEventButton* event)
{
    Glib::RefPtr <Gtk::Action> action  = mPopupActionGroup->get_action ("ModelCB_ContextRemove");
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    Gtk::TreeModel::iterator iter = refSelection->get_selected();

    if (iter and (mRefTreeModel->iter_depth(iter) == 0)) {
        action->set_sensitive(true);
    } else {
        action->set_sensitive(false);
    }

    if (event->button == 3) {
        mMenuPopup->popup(event->button, event->time);
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
                vpz::CoupledModel* c_model = dynamic_cast<vpz::CoupledModel*>(class_.model());
                if (mGVLE->existView(c_model)) {
                    mGVLE->delViewOnModel(c_model);
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
                        vpz::BaseModel::rename(row[mColumns.mModel], newname);
                        mGVLE->setModified(true);
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
    vpz::BaseModel* model = oldClass.model();
    // vpz::AtomicModelList& atomicModel = oldClass.atomicModels();
    mModeling->vpz().project().classes().add(newName);
    mModeling->vpz().project().classes().get(newName).setModel(model);
    // mModeling->vpz().project().classes().get(newName).
    //     setAtomicModel(atomicModel);

    if (oldClass.model()->isCoupled()) {
        vpz::CoupledModel* c_model =
            dynamic_cast<vpz::CoupledModel*>(oldClass.model());

        if (mGVLE->existView(c_model)) {
            mGVLE->delViewOnModel(c_model);
            mGVLE->addViewClass(c_model, newName);
            mGVLE->getEditor()->openTabVpz(mModeling->getFileName(), c_model);
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
            vpz::BaseModel* model = currentClass.model();

            Gtk::FileChooserDialog file(_("VPZ file"), Gtk::FILE_CHOOSER_ACTION_SAVE);
            file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
            file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
            Glib::RefPtr<Gtk::FileFilter> filter = Gtk::FileFilter::create ();
            filter->set_name(_("Vle Project gZipped"));
            filter->add_pattern("*.vpz");
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
    Glib::RefPtr<Gtk::FileFilter> filter = Gtk::FileFilter::create ();
    filter->set_name(_("Vle Project gZipped"));
    filter->add_pattern("*.vpz");
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
            Error(E.what());
        }
    }
    parseClass();
}

void ModelClassBox::onImportClassesFromVpz()
{
    Gtk::FileChooserDialog file(_("VPZ file"), Gtk::FILE_CHOOSER_ACTION_OPEN);
    file.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    file.add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
    Glib::RefPtr<Gtk::FileFilter> filter =  Gtk::FileFilter::create ();
    filter->set_name(_("Vle Project gZipped"));
    filter->add_pattern("*.vpz");
    file.add_filter(filter);

    if (file.run() == Gtk::RESPONSE_OK) {
        std::string project_file = file.get_filename();
        try {
            vpz::Vpz* import = new vpz::Vpz(project_file);
            mGVLE->setModified(true);
            mGVLE->importClasses(import);
            delete import;
        } catch (std::exception& E) {
            Error(E.what());
        }
    }
    parseClass();
}

void ModelClassBox::parseClass()
{
    mRefTreeModel->clear();

    const vpz::Vpz& vpz = ((const Modeling*)mModeling)->vpz();
    vpz::Classes::const_iterator iter = vpz.project().classes().begin();
    while (iter != vpz.project().classes().end()) {
        Gtk::TreeModel::Row row = addClass(vpz.project().classes().get(
                                               iter->second.name()));
        const vpz::BaseModel* top = vpz.project().classes()
            .get(iter->second.name()).model();
        Gtk::TreeModel::Row row2 = addSubModel(row, (vpz::BaseModel*)top);
        if (top->isCoupled()) {
            parseModel(row2, (vpz::CoupledModel*)top);
        }
        ++iter;
    }
    expand_all();
}

void ModelClassBox::clear()
{
    mRefTreeModel->clear();
}

bool ModelClassBox::onQueryTooltip(int wx,int wy, bool keyboard_tooltip,
                                   const Glib::RefPtr<Gtk::Tooltip>& tooltip)
{
    Gtk::TreeModel::iterator iter;
    Glib::ustring card;

    if (get_tooltip_context_iter(wx, wy, keyboard_tooltip, iter) == true) {
        Gtk::TreeModel::Path path = Gtk::TreePath(iter);
        Gtk::TreeIter iter = mRefTreeModel->get_iter(path);
        Gtk::TreeRow row = (*iter);
        if (mRefTreeModel->iter_depth(iter) == 0) {
            card = mModeling->getIdCard(row.get_value(mColumns.mName));
        } else {
            card = mModeling->getClassIdCard(row.get_value(mColumns.mModel));
        }
        tooltip->set_markup(card);
        set_tooltip_row(tooltip, Gtk::TreePath(iter));
        return true;
    }
    return false;
}

Gtk::TreeModel::Row ModelClassBox::addClass(const vpz::Class& classe)
{
    Gtk::TreeModel::Row row = *(mRefTreeModel->append());
    row[mColumns.mName] = classe.name();
    return row;
}

Gtk::TreeModel::Row ModelClassBox::addSubModel(Gtk::TreeModel::Row tree,
                                               vpz::BaseModel* model)
{
    Gtk::TreeModel::Row row = *(mRefTreeModel->append(tree.children()));
    row[mColumns.mName] = model->getName();
    row[mColumns.mModel] = model;
    return row;
}

void ModelClassBox::parseModel(Gtk::TreeModel::Row row,
                               const vpz::CoupledModel* top)
{
    const vpz::ModelList& list = top->getModelList();
    vpz::ModelList::const_iterator it = list.begin();

    while (it != list.end()) {
        Gtk::TreeModel::Row row2 = addSubModel(row, it->second);
        if (it->second->isCoupled() == true) {
            parseModel(row2, (vpz::CoupledModel*)it->second);
        }
        ++it;
    }
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
    if (column && path) {
        Gtk::TreeIter iter = mRefTreeModel->get_iter(path);
        Gtk::TreeRow row = (*iter);
        if (mRefTreeModel->iter_depth(iter) == 0) {
            mGVLE->addViewClass(
                mModeling->getClassModel(row.get_value(mColumns.mName)),
                row.get_value(mColumns.mName));
        } else {
            mGVLE->addViewClass(row.get_value(mColumns.mModel),
                                getClassName(path));
        }
    }
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
                        vpz::BaseModel::rename(row[mColumns.mModel], newName);
                        mGVLE->setModified(true);
                    } catch(utils::DevsGraphError dge) {
                        row[mColumns.mName] = mOldName;
                    }
                }
            }
        }
        parseClass();
    }
}

std::string ModelClassBox::getClassName(const Gtk::TreeModel::Path& path)
{

    std::vector <std::string> Vec;
    Glib::ustring str = path.to_string();
    boost::split(Vec, str, boost::is_any_of(":"));
    Gtk::TreeIter iterClass = mRefTreeModel->get_iter(Vec[0]);
    Gtk::TreeRow rowClass = (*iterClass);

    return rowClass.get_value(mColumns.mName);
}

} } // namespace vle gvle
