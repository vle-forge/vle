/*
 * @file vle/gvle/modeling/fsa/SourceDialog.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2012 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2012 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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


#ifndef VLE_GVLE_DIALOGSOURCE_HPP
#define VLE_GVLE_DIALOGSOURCE_HPP

#include <vle/version.hpp>
#include <libglademm.h>
#include <gtkmm.h>
#include <vle/gvle/Editor.hpp>

#ifdef VLE_HAVE_GTKSOURCEVIEWMM
#include <gtksourceviewmm-2.0/gtksourceviewmm.h>
#endif

namespace vle {
namespace gvle {
namespace modeling {
namespace fsa {

class SourceDialog
{
public:
    SourceDialog(Glib::RefPtr < Gnome::Glade::Xml > xml);
    virtual ~SourceDialog();

    void add(const std::string& name, const std::string& buffer);
    std::string buffer(const std::string& name) const;

    int run();

private:
    Glib::RefPtr < Gnome::Glade::Xml > mXml;

    Gtk::Dialog* mDialog;
    Gtk::Notebook* mNotebook;
    std::map < std::string, DocumentText* > mFunctions;
};

}
}
}
}    // namespace vle gvle modeling fsa

#endif

