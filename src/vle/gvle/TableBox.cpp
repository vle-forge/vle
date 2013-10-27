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


#include <vle/gvle/TableBox.hpp>
#include <vle/utils/Tools.hpp>
#include <gtkmm/stock.h>

using namespace vle;

namespace vu = vle::utils;

namespace vle {
namespace gvle {

TableBox::TableBox(value::Table* t) :
    Gtk::Dialog("Table", true),
    mValue(t)
{
    add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    add_button(Gtk::Stock::OK, Gtk::RESPONSE_APPLY);

    mScroll = new Gtk::ScrolledWindow();
    mScroll->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    get_vbox()->pack_start(*mScroll);

    //mTable = new Gtk::Table(t->height(), t->width(), true);
    mTable = new Gtk::Grid();
    mScroll->add(*mTable);
    makeTable();
    resize(300, 200);

    show_all();
}

TableBox::~TableBox()
{
    // No hide_all in Gtk 3
    hide();
    delete mScroll;
    delete mTable;

    for (index i = 0; i != mValue->height(); ++i) {
        for (index j = 0; j != mValue->width(); ++j) {
            delete (*mArray)[j][i];
        }
    }
    delete mArray;
}

void TableBox::run()
{
    int ret = Gtk::Dialog::run();
    if (ret == Gtk::RESPONSE_APPLY) {
        value::TableValue& table = mValue->value();
        for (index i = 0; i != mValue->height(); ++i) {
            for (index j = 0; j != mValue->width(); ++j) {
                double x = vu::convert < double > (
                        (*mArray)[j][i]->get_text(),
                        true);
                table[j][i] = x;
            }
        }
    }
}

void TableBox::makeTable()
{
    mArray = new array_type(boost::extents[mValue->width()][mValue->height(
                                                                )]);
    for (index i = 0; i != mValue->height(); ++i) {
        for (index j = 0; j != mValue->width(); ++j) {
            (*mArray)[j][i] = new Gtk::Entry();
            (*mArray)[j][i]->set_width_chars(10);
            (*mArray)[j][i]->set_text(utils::toScientificString(mValue->
                    get(j, i), true));
            (*mArray)[j][i]->set_editable(true);
            mTable->attach(*(*mArray)[j][i],
                j,
                j + 1,
                i,
                i + 1 /*,
                Gtk::FILL,
                Gtk::FILL */);
        }
    }
}

}
} // namespace vle gvle
