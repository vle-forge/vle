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


#include <vle/gvle/XmlTypeBox.hpp>
#include <gtkmm/stock.h>

using namespace vle;

namespace vle {
namespace gvle {

XmlTypeBox::XmlTypeBox(value::Xml* xml) :
    Gtk::Dialog("XML", true),
    mValue(xml)
{
    makeBox();
    mBuff->set_text(xml->writeToString());

    set_size_request(350, 350);
    show_all();
}

XmlTypeBox::XmlTypeBox(std::string& string) :
    Gtk::Dialog("XML", true),
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
    // No hide_all in GTK 3
    hide();
    delete mView;
}

void XmlTypeBox::run()
{
    int ret = Gtk::Dialog::run();
    if (ret == Gtk::RESPONSE_APPLY) {
        if (mValue != 0) {
            mValue->set(mBuff->get_text());
        } else {
            *mXml = mBuff->get_text();
        }
    }
}

void XmlTypeBox::makeBox()
{
    add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    add_button(Gtk::Stock::OK, Gtk::RESPONSE_APPLY);

//    mScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC,
//        Gtk::POLICY_AUTOMATIC);

    mView = new Gtk::TextView();
    mBuff = mView->get_buffer();

//    mScrolledWindow.add(*mView);
    mView->set_vscroll_policy(Gtk::SCROLL_NATURAL);
    mView->set_hscroll_policy(Gtk::SCROLL_NATURAL);
//    get_vbox()->pack_start(mScrolledWindow);
    get_vbox()->pack_start(*mView);
}

}
} // namespace vle gvle
