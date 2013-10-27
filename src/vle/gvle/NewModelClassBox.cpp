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


#include <vle/gvle/Message.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/gvle/NewModelClassBox.hpp>
#include <vle/gvle/GVLE.hpp>
#include <vle/vpz/Classes.hpp>
#include <vle/vpz/AtomicModel.hpp>
#include <vle/vpz/CoupledModel.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace vle
{
namespace gvle {

NewModelClassBox::NewModelClassBox(const Glib::RefPtr < Gtk::Builder >& xml,
                                   Modeling* m, GVLE* gvle):
        mXml(xml),
        mModeling(m),
        mGVLE(gvle)
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
        vpz::BaseModel* model = new vpz::AtomicModel(model_name, NULL);
        new_class.setModel(model);
    } else {
        new_class.setModel(new vpz::CoupledModel(model_name, NULL));
    }

    mGVLE->redrawModelClassBox();
    // No hide_all in gtk 3
    mDialog->hide();
}

}
} // namespace vle gvle
