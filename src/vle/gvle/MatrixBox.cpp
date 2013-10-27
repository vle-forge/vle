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

#include <vle/gvle/BooleanBox.hpp>
#include <vle/gvle/MatrixBox.hpp>
#include <vle/gvle/NewTypeBox.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/gvle/TableBox.hpp>
#include <vle/gvle/TreeViewValue.hpp>
#include <vle/gvle/ValueBox.hpp>
#include <vle/gvle/XmlTypeBox.hpp>
#include <vle/gvle/GVLE.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <gtkmm/stock.h>

using namespace vle;

namespace vle {
namespace gvle {
MatrixBox::MatrixBox(value::Matrix* m) :
    Gtk::Dialog(_("Matrix"), true),
    mValue(m)
{
    add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    add_button(Gtk::Stock::OK, Gtk::RESPONSE_APPLY);

    mBackup = mValue->clone();

    mScroll = new Gtk::ScrolledWindow();
    mScroll->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    get_vbox()->pack_start(*mScroll);

    //mTable = new Gtk::Table(m->rows(), m->columns(), true);
    mTable = new Gtk::Grid();
    mScroll->add(*mTable);
    makeTable();
    resize(300, 200);

    show_all();
}

MatrixBox::~MatrixBox()
{
    // gtk 3 : plus de hide_all
    hide ();

    delete mScroll;
    delete mTable;

    for (unsigned int i = 0; i != mValue->rows(); ++i) {
        for (unsigned int j = 0; j != mValue->columns(); ++j) {
            delete (*mArray)[j][i].first;
        }
    }
    delete mArray;

}

void MatrixBox::run()
{
    int ret = Gtk::Dialog::run();
    if (ret == Gtk::RESPONSE_CANCEL) {
        value::Matrix* backup = dynamic_cast < value::Matrix* > (&*mBackup);
        const value::MatrixValue& matrix = backup->matrix();

        if (mValue->columns() != backup->columns() || mValue->rows() !=
            backup->rows()) {
            mValue->resize(backup->columns(), backup->rows());
        }

        for (unsigned int i = 0; i != mValue->rows(); ++i) {
            for (unsigned int j = 0; j != mValue->columns(); ++j) {
                mValue->add(j, i, matrix[j][i]);
            }
        }
    }
}

void MatrixBox::makeTable()
{
    const value::MatrixValue& matrix = mValue->matrix();
    mArray = new array_type(boost::extents[mValue->columns()][mValue->rows(
                                    )]);
    for (unsigned int i = 0; i != mValue->rows(); ++i) {
        for (unsigned int j = 0; j != mValue->columns(); ++j) {
            (*mArray)[j][i].first = new Gtk::Button();
            (*mArray)[j][i].second = NULL;  // ne sert jamais, mais top complexe a enlever pour moi ...
            if (matrix[j][i] != 0) {
                (*mArray)[j][i].first->set_label(
                    boost::trim_copy(matrix[j][i]->writeToString()));
            } else {
                (*mArray)[j][i].first->set_label(_("(no value)"));
            }

            mTable->attach(*(*mArray)[j][i].first,
                           j,
                           j + 1,
                           i,
                           i + 1 /*,
                           Gtk::FILL,
                           Gtk::FILL */ );
            (*mArray)[j][i].first->signal_clicked().connect(
                sigc::bind(
                    sigc::mem_fun(*this, &MatrixBox::on_click), i, j));

            if (matrix[j][i] != 0) {
                (*mArray)[j][i].first->set_has_tooltip();
                (*mArray)[j][i].first->set_tooltip_text(
                    gvle::valuetype_to_string(matrix[j][i]->getType()));
            }
        }
    }
}

void MatrixBox::on_click(unsigned int i, unsigned int j)
{
    value::MatrixView view = mValue->value();
    value::Value* v= view[j][i];

    if (v == 0) {
        NewTypeBox box;
        v = box.launch();
        if (v != 0) {
            view[j][i] = v;
            (*mArray)[j][i].first->set_label(gvle::valuetype_to_string(view[j][i]->getType()));
            (*mArray)[j][i].first->set_has_tooltip();
            (*mArray)[j][i].first->set_tooltip_text(boost::trim_copy(view[j][i]->writeToString()));
        }
    }

    if (v != 0) {
        switch (v->getType()) {
        case (value::Value::BOOLEAN):
        {
            value::Boolean* boolean = dynamic_cast < value::Boolean* > (v);
            BooleanBox box(boolean);
            box.run();
        }
        break;
        case (value::Value::INTEGER):
        case (value::Value::DOUBLE):
        case (value::Value::STRING):
        case (value::Value::TUPLE):
        {
            SimpleTypeBox box(v);
            box.run();
        }
        break;
        case (value::Value::SET):
        {
            value::Set* set = dynamic_cast < value::Set* > (v);
            ValueBox box(set);
            box.run();
        }
        break;
        case (value::Value::MAP):
        {
            value::Map* map = dynamic_cast < value::Map* > (v);
            ValueBox box(map);
            box.run();
        }
        break;
        case (value::Value::TABLE):
        {
            value::Table* table = dynamic_cast < value::Table* > (v);
            TableBox box(table);
            box.run();
        }
        break;
        case (value::Value::MATRIX):
        {
            value::Matrix* matrix = dynamic_cast < value::Matrix* > (v);
            MatrixBox box(matrix);
            box.run();
        }
        break;
        case (value::Value::XMLTYPE):
            {
            value::Xml* xml = dynamic_cast < value::Xml* > (v);
            XmlTypeBox box(xml);
            box.run();
        }
        break;
        default:
            break;
        }
        (*mArray)[j][i].first->set_tooltip_text(boost::trim_copy(v->writeToString()));
    }
}
}
} // namespace vle gvle
