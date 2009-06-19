/**
 * @file vle/gvle/NewModelClassBox.cpp
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


#include <vle/gvle/Message.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/gvle/NewModelClassBox.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace vle
{
namespace gvle {

NewModelClassBox::NewModelClassBox(Glib::RefPtr<Gnome::Glade::Xml> xml, Modeling* m):
        mXml(xml),
        mModeling(m)
{
    xml->get_widget("DialogNewModelClass", mDialog);

    xml->get_widget("EntryNewClassName", mEntryClassName);

    xml->get_widget("ComboBoxModelType", mComboTypeModel);
    mRefTreeModel = Gtk::ListStore::create(mColumns);
    mComboTypeModel->set_model(mRefTreeModel);
    Gtk::TreeModel::Row row = *(mRefTreeModel->append());
    row[mColumns.m_col_type] = "Coupled";
    row = *(mRefTreeModel->append());
    row[mColumns.m_col_type] = "Atomic";
    mComboTypeModel->pack_start(mColumns.m_col_type);

    xml->get_widget("EntryModelName", mEntryModelName);

    xml->get_widget("ButtonOkNewModelClass", mButtonApply);
    mButtonApply->signal_clicked().connect(
        sigc::mem_fun(*this, &NewModelClassBox::on_apply));
}

NewModelClassBox::~NewModelClassBox()
{

}

void NewModelClassBox::run()
{
    mEntryClassName->set_text("");
    mEntryModelName->set_text("");

    mDialog->show_all();
    mDialog->run();
}

void NewModelClassBox::on_apply()
{
    vpz::Classes& classes = mModeling->vpz().project().classes();

    std::string class_name = mEntryClassName->get_text();
    boost::trim(class_name);

    if (class_name.empty() || classes.exist(class_name)) {
        Error(_("Enter a VALID class name."));
        return;
    }

    Gtk::TreeModel::iterator iter = mComboTypeModel->get_active();
    if (not iter) {
        Error(_("Choose a model type."));
        return;
    }
    std::string model_type = iter->get_value(mColumns.m_col_type);

    std::string model_name = mEntryModelName->get_text();
    boost::trim(model_name);
    if (model_name.empty()) {
        Error(_("Enter a model name."));
        return;
    }

    vpz::Class& new_class = classes.add(class_name);
    if (model_type == "Atomic") {
        graph::Model* model = new graph::AtomicModel(model_name, NULL);
        new_class.setModel(model);
        mModeling->vpz().project().classes().get(class_name).atomicModels().add(model, vpz::AtomicModel("", "", ""));
    } else {
        new_class.setModel(new graph::CoupledModel(model_name, NULL));
    }

    mModeling->getGVLE()->redrawModelClassBox();
    mDialog->hide_all();
}

}
} // namespace vle gvle
