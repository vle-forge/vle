/**
 * @file vle/gvle/MatrixBox.cpp
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


#include <vle/gvle/BooleanBox.hpp>
#include <vle/gvle/MatrixBox.hpp>
#include <vle/gvle/NewTypeBox.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/gvle/TableBox.hpp>
#include <vle/gvle/TreeViewValue.hpp>
#include <vle/gvle/ValueBox.hpp>
#include <vle/gvle/XmlTypeBox.hpp>
#include <vle/utils.hpp>
#include <boost/algorithm/string/trim.hpp>

using namespace vle;

namespace vle
{
namespace gvle {
MatrixBox::MatrixBox(value::MatrixFactory* m):
        Gtk::Dialog("Matrix",true,true),
        mValue(m)
{
    add_button(Gtk::Stock::APPLY, Gtk::RESPONSE_APPLY);
    add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);

    mBackup = mValue->clone();

    mScroll = new Gtk::ScrolledWindow();
    mScroll->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    get_vbox()->pack_start(*mScroll);

    mTable = new Gtk::Table(m->rows(), m->columns(), true);
    mScroll->add(*mTable);
    makeTable();
    resize(300,200);

    show_all();
}

MatrixBox::~MatrixBox()
{
    hide_all();

    delete mScroll;
    delete mTable;

    for (unsigned int i = 0; i != mValue->rows(); ++i) {
        for (unsigned int j = 0; j != mValue->columns(); ++j) {
            delete(*mArray)[j][i].first;
            delete(*mArray)[j][i].second;
        }
    }
    delete mArray;

}

void MatrixBox::run()
{
    int ret = Gtk::Dialog::run();
    if (ret == Gtk::RESPONSE_CANCEL) {
        value::MatrixFactory* backup = dynamic_cast<value::MatrixFactory*>(&*mBackup);
        const value::MatrixFactory::MatrixValue& matrix = backup->matrix();

        if (mValue->columns() != backup->columns() || mValue->rows() != backup->rows()) {
            mValue->resize(backup->columns(), backup->rows());
        }

        for (unsigned int i = 0; i != mValue->rows(); ++i) {
            for (unsigned int j = 0; j != mValue->columns(); ++j) {
                mValue->addValue(j, i, matrix[j][i]);
            }
        }
    }
}

void MatrixBox::makeTable()
{
    const value::MatrixFactory::MatrixValue& matrix = mValue->matrix();
    mArray = new array_type(boost::extents[mValue->columns()][mValue->rows()]);
    for (unsigned int i = 0; i != mValue->rows(); ++i) {
        for (unsigned int j = 0; j != mValue->columns(); ++j) {
            //Button
            (*mArray)[j][i].first = new Gtk::Button();
            if (matrix[j][i].get() != 0)
                (*mArray)[j][i].first->set_label(gvle::valuetype_to_string(matrix[j][i]->getType()));
            else
                (*mArray)[j][i].first->set_label("(no value)");

            mTable->attach(*(*mArray)[j][i].first, j, j+1, i , i+1, Gtk::FILL, Gtk::FILL);
            (*mArray)[j][i].first->signal_clicked().connect(sigc::bind(
                        sigc::mem_fun(*this, &MatrixBox::on_click), i, j));

            if (matrix[j][i].get() != 0) {
                (*mArray)[j][i].second = new Gtk::Tooltips();
#if GTKMM_MAJOR_VERSION == 2 && GTKMM_MINOR_VERSION > 10
                (*mArray)[j][i].first->set_has_tooltip();
#endif
                (*mArray)[j][i].second->set_tip(*(*mArray)[j][i].first, boost::trim_copy(matrix[j][i]->toString()));
            }
        }
    }
}

void MatrixBox::on_click(unsigned int i, unsigned int j)
{
    value::MatrixFactory::MatrixView view = mValue->getValue();
    value::Value& v = view[j][i];

    if (v.get() == 0) {
        NewTypeBox box(&v);
        box.run();
        if (v.get() != 0) {
            (*mArray)[j][i].first->set_label(gvle::valuetype_to_string(view[j][i]->getType()));
            (*mArray)[j][i].second = new Gtk::Tooltips();
#if GTKMM_MAJOR_VERSION == 2 && GTKMM_MINOR_VERSION > 10
            (*mArray)[j][i].first->set_has_tooltip();
#endif
            (*mArray)[j][i].second->set_tip(*(*mArray)[j][i].first, boost::trim_copy(view[j][i]->toString()));
        }
    } else {
        switch (v->getType()) {
        case(value::ValueBase::BOOLEAN): {
            value::BooleanFactory* boolean = dynamic_cast<value::BooleanFactory*>(v.get());
            BooleanBox box(boolean);
            box.run();
        }
        break;
        case(value::ValueBase::INTEGER):
                    case(value::ValueBase::DOUBLE):
                        case(value::ValueBase::STRING):
                            case(value::ValueBase::TUPLE): {
                        SimpleTypeBox box(v.get());
                        box.run();
                    }
        break;
        case(value::ValueBase::SET): {
            value::SetFactory* set = dynamic_cast<value::SetFactory*>(v.get());
            ValueBox box(set);
            box.run();
        }
        break;
        case(value::ValueBase::MAP): {
            value::MapFactory* map = dynamic_cast<value::MapFactory*>(v.get());
            ValueBox box(map);
            box.run();
        }
        break;
        case(value::ValueBase::TABLE): {
            value::TableFactory* table = dynamic_cast<value::TableFactory*>(v.get());
            TableBox box(table);
            box.run();
        }
        break;
        case(value::ValueBase::MATRIX): {
            value::MatrixFactory* matrix = dynamic_cast<value::MatrixFactory*>(v.get());
            MatrixBox box(matrix);
            box.run();
        }
        break;
        case(value::ValueBase::XMLTYPE): {
            value::XMLFactory* xml = dynamic_cast<value::XMLFactory*>(v.get());
            XmlTypeBox box(xml);
            box.run();
        }
        break;
        default:
            break;
        }
        (*mArray)[j][i].second->set_tip(*(*mArray)[j][i].first, boost::trim_copy(view[j][i]->toString()));
    }
}
}
} // namespace vle gvle
