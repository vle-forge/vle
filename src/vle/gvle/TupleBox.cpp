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


#include <vle/gvle/TupleBox.hpp>
#include <vle/utils/Tools.hpp>
#include <gtkmm/stock.h>

namespace vle {
namespace gvle {

TupleBox::TupleBox(value::Tuple* t) :
    Gtk::Dialog("Tuple", true),
    mValue(t)
{
    add_button(Gtk::Stock::APPLY, Gtk::RESPONSE_APPLY);
    add_button(Gtk::Stock::OK, Gtk::RESPONSE_CANCEL);

    mScroll = new Gtk::ScrolledWindow();
    mScroll->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    get_vbox()->pack_start(*mScroll);

    //mTable = new Gtk::Table(mValue->size(), 1, true);
    mTable = new Gtk::Grid();
    mScroll->add(*mTable);
    makeTable();
    resize(300, 200);

    show_all();
}

TupleBox::~TupleBox()
{
    // No hide_all in GTK 3
    hide();
}

void TupleBox::run()
{
    int ret = Gtk::Dialog::run();
    if (ret == Gtk::RESPONSE_APPLY) {
        value::TupleValue& tuple = mValue->value();
        for (value::Tuple::size_type i = 0; i != mValue->size(); ++i) {
            double x = utils::convert < double > ((*mArray)[i]->get_text(),
                                                  true);
            tuple[i] = x;
        }
    }
}

void TupleBox::makeTable()
{
    mArray = new array_type(boost::extents[mValue->size()]);
    for (value::Tuple::size_type i = 0; i != mValue->size(); ++i) {
        (*mArray)[i] = new Gtk::Entry();
        (*mArray)[i]->set_text(utils::toScientificString(mValue->at(i),
                true));
        (*mArray)[i]->set_editable(true);
        mTable->attach(*(*mArray)[i], 0, 1, i, i + 1);
    }
}

}
}  // namespace vle gvle
