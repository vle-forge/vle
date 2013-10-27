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

#include <gtkmm/stock.h>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/Table.hpp>


namespace vu = vle::utils;

namespace vle { namespace gvle {

SimpleTypeBox::SimpleTypeBox(value::Value* b):
    Gtk::Dialog("?",true),
    mBase(b),
    mValid(false)
{
    add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
    set_default_response(Gtk::RESPONSE_OK);
    makeDialog();

    if (b->isInteger()) {
        set_title(_("Integer"));
    } else if (b->isDouble()) {
        set_title(_("Double"));
    } else if (b->isString()) {
        set_title(_("String"));
    } else if (b->isTuple()) {
        set_title(_("Tuple"));
    }

    show_all();
}

SimpleTypeBox::SimpleTypeBox(const std::string& title,
			     const std::string& value) :
    Gtk::Dialog(title,true),
    mBase(0),
    mValid(false)
{
    add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
    set_default_response(Gtk::RESPONSE_OK);
    makeDialog();
    mEntry->set_text(value);
    show_all();
}

SimpleTypeBox::~SimpleTypeBox()
{
    // No hide_all in Gtk 3
    hide ();
    delete mEntry;
}

void SimpleTypeBox::makeDialog()
{
    mEntry = new Gtk::Entry();
    mEntry->set_editable(true);
    mEntry->set_activates_default(true);
    if (mBase) {
        if (mBase->getType() == value::Value::TUPLE) {
            std::string buffer;
            value::Tuple* tuple =
                        dynamic_cast < value::Tuple* >(mBase);

            for (value::Tuple::size_type i = 0; i < tuple->size(); ++i) {
                buffer += (fmt("%1% ") %
                          utils::toScientificString((*tuple)[i],true)).str();
            }
            mEntry->set_text(buffer);
        } else if (mBase->getType() == value::Value::DOUBLE) {
            value::Double* v = dynamic_cast < value::Double* >(mBase);
            mEntry->set_text(utils::toScientificString(v->value(),true));
        } else {
            mEntry->set_text(mBase->writeToString());
        }
    }
    get_vbox()->pack_start(*mEntry);
}

std::string SimpleTypeBox::run()
{
    mValid = Gtk::Dialog::run() == Gtk::RESPONSE_OK;
    if (mValid) {
        if (mBase) {
            switch (mBase->getType()) {
            case(value::Value::INTEGER):
                {
                    dynamic_cast < value::Integer* >(mBase)
                        ->set(utils::to < int32_t >(mEntry->get_text()));
                    return mEntry->get_text();
                }
                break;
            case(value::Value::DOUBLE):
                {
                    double x = vu::convert < double >(mEntry->get_text(),true);
                    dynamic_cast < value::Double* >(mBase)->set(x);
                    return mEntry->get_text();
                }
                break;
            case(value::Value::STRING):
                {
                    dynamic_cast < value::String* >(mBase)
                        ->set(mEntry->get_text());
                    return mEntry->get_text();
                }
                break;
            case(value::Value::TUPLE):
                {
                    value::Tuple* tuple =
                        dynamic_cast < value::Tuple* >(mBase);

                    tuple->value().clear();
                    try {
                        tuple->fill(mEntry->get_text());
                    } catch (...) {
                        Error("Bad format for tuple: numeric with space.");
                    }
                    return mEntry->get_text();
                }
                break;
            default:
                break;
            }
        } else {
            return mEntry->get_text();
        }
    }
    return std::string();
}

}} // namespace vle gvle
