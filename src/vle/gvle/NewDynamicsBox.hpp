/**
 * @file vle/gvle/NewDynamicsBox.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2009 ULCO http://www.univ-littoral.fr
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

