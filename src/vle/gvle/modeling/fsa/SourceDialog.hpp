/**
 * @file vle/gvle/modeling/fsa/SourceDialog.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
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


#ifndef VLE_GVLE_DIALOGSOURCE_HPP
#define VLE_GVLE_DIALOGSOURCE_HPP

#include <vle/version.hpp>
#include <libglademm.h>
#include <gtkmm.h>
#include <vle/gvle/Editor.hpp>

#ifdef VLE_HAVE_GTKSOURCEVIEWMM
#include <gtksourceviewmm-2.0/gtksourceviewmm.h>
#endif

namespace vle { namespace gvle { namespace modeling { namespace fsa {

class SourceDialog
{
public:
    SourceDialog(Glib::RefPtr<Gnome::Glade::Xml> xml);
    virtual ~SourceDialog();

    void add(const std::string& name, const std::string& buffer);
    std::string buffer(const std::string& name) const;

    int run();

private:
    Glib::RefPtr<Gnome::Glade::Xml> mXml;

    Gtk::Dialog*                      mDialog;
    Gtk::Notebook*                    mNotebook;
    std::map < std::string, DocumentText* > mFunctions;
};

}}}} // namespace vle gvle modeling fsa

#endif

