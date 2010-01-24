/**
 * @file vle/gvle/SimpleTypeBox.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
 * Copyright (C) 2003-2010 ULCO http://www.univ-littoral.fr
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


#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/utils/Tools.hpp>

namespace vle { namespace gvle {

SimpleTypeBox::SimpleTypeBox(value::Value* b):
    Gtk::Dialog("?",true,true),
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
    Gtk::Dialog(title,true,true),
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
    hide_all();
    delete mEntry;
}

void SimpleTypeBox::makeDialog()
{
    mEntry = new Gtk::Entry();
    mEntry->set_editable(true);
    mEntry->set_activates_default(true);
    if (mBase) {
        mEntry->set_text(mBase->writeToString());
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
                    dynamic_cast < value::Integer* > (mBase)
                        ->set(utils::toLong(mEntry->get_text()));
                    return mEntry->get_text();
                }
                break;
            case(value::Value::DOUBLE):
                {
                    dynamic_cast < value::Double* > (mBase)
                        ->set(utils::toDouble(mEntry->get_text()));
                    return mEntry->get_text();
                }
                break;
            case(value::Value::STRING):
                {
                    dynamic_cast < value::String* > (mBase)
                        ->set(mEntry->get_text());
                    return mEntry->get_text();
                }
                break;
            case(value::Value::TUPLE):
                {
                    value::Tuple* tuple =
                        dynamic_cast<value::Tuple*>(mBase);
                    tuple->value().clear();
                    tuple->fill(mEntry->get_text());
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
