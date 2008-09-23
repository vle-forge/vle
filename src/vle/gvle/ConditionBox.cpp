/**
 * @file vle/gvle/ConditionBox.cpp
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


#include <vle/gvle/ConditionBox.hpp>
#include <vle/gvle/GVLE.hpp>

using namespace vle;

namespace vle
{
namespace gvle {

ConditionBox::ConditionBox(vpz::Condition* c):
        Gtk::Dialog("?",true,true),
        mCond(c),
        backup(0)
{
    add_button(Gtk::Stock::APPLY, Gtk::RESPONSE_APPLY);
    add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    set_title(c->name());
    //backup
    backup = new vpz::Condition(*c);

    mHb_top = new Gtk::HBox(true, 2);

    //Port List
    mTreeViewPort = new TreeViewPort(c);
    mTreeViewPort->signal_cursor_changed().connect(sigc::mem_fun(*this,
            &ConditionBox::on_port_clicked));
    //mTreeViewPort->makeListPort(c);
    mHb_top->pack_start(*mTreeViewPort);

    //TreeView Value
    mTreeviewValue = new TreeViewValue();
    mHb_top->pack_start(*mTreeviewValue);

    get_vbox()->pack_start(*mHb_top);
    show_all();
}

ConditionBox::~ConditionBox()
{
    hide_all();
    delete mTreeviewValue;
    delete mTreeViewPort;
    delete mHb_top;
    delete backup;
}

void ConditionBox::run()
{
    int ret = Gtk::Dialog::run();
    if (ret == Gtk::RESPONSE_CANCEL) {
        vpz::ConditionValues::const_iterator it = mCond->conditionvalues().begin();
        while (it != mCond->conditionvalues().end()) {
            mCond->del(it->first);
            ++it;
        }

        it = backup->conditionvalues().begin();
        value::VectorValue::const_iterator it_vec;
        while (it != backup->conditionvalues().end()) {
            mCond->add(it->first);
            it_vec = it->second->begin();
            while (it_vec != it->second->end()) {
                mCond->addValueToPort(it->first, *it_vec);
                ++it_vec;
            }
            ++it;
        }

    }
}

void ConditionBox::on_port_clicked()
{
    Glib::RefPtr<Gtk::TreeSelection> selection = mTreeViewPort->get_selection();
    if (selection) {
        Gtk::TreeModel::iterator it = selection->get_selected();
        if (it) {
            Gtk::TreeModel::Row row = *it;
            //std::cout << "Click on : " << row[mTreeViewPort->m_Columns.m_col_port] << "\n";
            Glib::ustring s =  row[mTreeViewPort->m_Columns.m_col_port];
            mTreeviewValue->makeTreeView(*(mCond->getSetValues(s)));
        }
    }
}

}
} // namespace vle gvle
