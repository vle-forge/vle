/**
 * @file vle/gvle/XmlTypeBox.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
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


#ifndef GUI_XMLTYPEBOX_HPP
#define GUI_XMLTYPEBOX_HPP

#include <gtkmm.h>
#include <vle/value/XML.hpp>

namespace vle
{
namespace gvle {

class XmlTypeBox : public Gtk::Dialog
{
public:
    XmlTypeBox(value::Xml*);
    XmlTypeBox(std::string&);

    virtual ~XmlTypeBox();

    void run();

private:
    Glib::RefPtr<Gtk::TextBuffer> mBuff;
    Gtk::TextView* mView;
    Gtk::ScrolledWindow mScrolledWindow;

    value::Xml* mValue;
    std::string* mXml;

    void makeBox();
};

}
} // namespace vle gvle

#endif
