/**
 * @file vle/gvle/XmlTypeBox.cpp
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


#include <vle/gvle/XmlTypeBox.hpp>
#include <iostream>

using namespace vle;

namespace vle
{
namespace gvle {

XmlTypeBox::XmlTypeBox(value::XMLFactory* xml):
        Gtk::Dialog("XML",true,true),
        mValue(xml)
{
    makeBox();
    mBuff->set_text(xml->toString());

    set_size_request(350, 350);
    show_all();
}

XmlTypeBox::XmlTypeBox(std::string& string):
        Gtk::Dialog("XML",true,true),
        mValue(0),
        mXml(&string)
{
    makeBox();
    mBuff->set_text(string);

    set_size_request(350, 350);
    show_all();
}

XmlTypeBox::~XmlTypeBox()
{
    hide_all();
    delete mView;
}

void XmlTypeBox::run()
{
    int ret = Gtk::Dialog::run();
    if (ret == Gtk::RESPONSE_APPLY) {
        if (mValue != 0)
            mValue->set(mBuff->get_text());
        else
            *mXml = mBuff->get_text();
    }
}

void XmlTypeBox::makeBox()
{
    add_button(Gtk::Stock::APPLY, Gtk::RESPONSE_APPLY);
    add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);

    mScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

    mView = new Gtk::TextView();
    mBuff = mView->get_buffer();

    mScrolledWindow.add(*mView);
    get_vbox()->pack_start(mScrolledWindow);
}

}
} // namespace vle gvle
