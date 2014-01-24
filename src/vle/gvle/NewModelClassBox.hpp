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


#ifndef GUI_NEWMODELCLASSBOX_HPP
#define GUI_NEWMODELCLASSBOX_HPP

#include <gtkmm/builder.h>
#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <gtkmm/combobox.h>
#include <gtkmm/button.h>
#include <gtkmm/liststore.h>

namespace vle
{
namespace gvle {

class Modeling;
class GVLE;

class NewModelClassBox
{
public:
    NewModelClassBox(const Glib::RefPtr < Gtk::Builder >& xml,
                     Modeling* m, GVLE* gvle);

    ~NewModelClassBox();

    void run();

protected:
class ModelColumns : public Gtk::TreeModel::ColumnRecord
    {
    public:
        ModelColumns() {
            add(m_col_type);
        }

        Gtk::TreeModelColumn<Glib::ustring>  m_col_type;
    };

    ModelColumns                           mColumns;

private:
    Glib::RefPtr < Gtk::Builder >          mXml;
    Modeling*                              mModeling;
    GVLE*                                  mGVLE;

    Gtk::Dialog*                           mDialog;
    Gtk::Entry*                            mEntryClassName;
    Gtk::ComboBox*                         mComboTypeModel;
    Glib::RefPtr<Gtk::ListStore>           mRefTreeModel;
    Gtk::Entry*                            mEntryModelName;
    Gtk::Button*                           mButtonApply;

    void on_apply();
};

}
} // namespace vle gvle

#endif

