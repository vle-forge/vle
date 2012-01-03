/*
 * @file vle/gvle/NewDynamicsBox.hpp
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


#ifndef VLE_GVLE__NEWDYNAMICSBOX_HPP
#define VLE_GVLE__NEWDYNAMICSBOX_HPP

#include <libglademm.h>
#include <gtkmm.h>

namespace vle { namespace gvle {

class NewDynamicsBox
{
public:
    NewDynamicsBox(Glib::RefPtr<Gnome::Glade::Xml> xml);
    virtual ~NewDynamicsBox() { }

    std::string getClassName() const
    { return mEntryClassName->get_text(); }

    std::string getNamespace() const
    { return mEntryNamespace->get_text(); }

    int run();

private:
    void onApply();
    void onCancel();

    Glib::RefPtr<Gnome::Glade::Xml> mXml;

    Gtk::Dialog*                    mDialog;
    //Entry
    Gtk::Entry*                     mEntryClassName;
    Gtk::Entry*                     mEntryNamespace;
    //Button
    Gtk::Button*                    mButtonApply;
    Gtk::Button*                    mButtonCancel;
};

}} // namespace vle gvle

#endif

