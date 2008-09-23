/**
 * @file vle/gvle/SimpleTypeBox.cpp
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


#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/utils.hpp>

using namespace vle;

namespace vle
{
namespace gvle {
SimpleTypeBox::SimpleTypeBox(value::ValueBase* b):
        Gtk::Dialog("?",true,true),
        mBase(b)
{
    add_button(Gtk::Stock::APPLY, Gtk::RESPONSE_APPLY);
    add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    set_default_response(Gtk::RESPONSE_APPLY);
    makeDialog();

    if (b->isInteger())
        set_title("Integer");
    else if (b->isDouble())
        set_title("Double");
    else if (b->isString())
        set_title("String");
    else if (b->isTuple())
        set_title("Tuple");

    show_all();
}

SimpleTypeBox::SimpleTypeBox(std::string title):
        Gtk::Dialog(title,true,true),
        mBase(0)
{
    add_button(Gtk::Stock::APPLY, Gtk::RESPONSE_APPLY);
    set_default_response(Gtk::RESPONSE_APPLY);
    makeDialog();
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
    if (mBase)
        mEntry->set_text(mBase->toFile());
    get_vbox()->pack_start(*mEntry);
}

std::string SimpleTypeBox::run()
{
    int ret = Gtk::Dialog::run();
    if (ret == Gtk::RESPONSE_APPLY && mBase) {
        switch (mBase->getType()) {
        case(value::ValueBase::INTEGER): {
            dynamic_cast<value::IntegerFactory*>(mBase)->set(utils::to_long(mEntry->get_text()));
            return mEntry->get_text();
        }
        break;
        case(value::ValueBase::DOUBLE): {
            dynamic_cast<value::DoubleFactory*>(mBase)->set(utils::to_double(mEntry->get_text()));
            return mEntry->get_text();
        }
        break;
        case(value::ValueBase::STRING): {
            dynamic_cast<value::StringFactory*>(mBase)->set(mEntry->get_text());
            return mEntry->get_text();
        }
        break;
        case(value::ValueBase::TUPLE): {
            value::TupleFactory* tuple = dynamic_cast<value::TupleFactory*>(mBase);
            tuple->getValue().clear();
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
    return "";
}

}
} // namespace vle gvle
