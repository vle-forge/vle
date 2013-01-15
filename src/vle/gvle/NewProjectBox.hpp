/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2013 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2013 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2013 INRA http://www.inra.fr
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


#ifndef GUI_NEWPROJECTBOX_HPP
#define GUI_NEWPROJECTBOX_HPP

#include <gtkmm/builder.h>
#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <vle/gvle/GVLE.hpp>
#include <vle/utils/Path.hpp>

namespace vle { namespace gvle {

class Modeling;
class GVLE;

/**
 * @brief A window to create a package
 */
class NewProjectBox
{
public:
    NewProjectBox(const Glib::RefPtr < Gtk::Builder >& xml,
                  Modeling* m, GVLE* app);

    ~NewProjectBox();

    void show();

private:
    Glib::RefPtr < Gtk::Builder >   mXml;

    Gtk::Dialog*                    mDialog;
    Modeling*                       mModeling;
    GVLE*                           mGVLE;

    // Text Entry
    Gtk::Entry*                     mEntryName;

    void apply();
    bool exist(const std::string& name);
};

}} //namespace vle gvle

#endif
