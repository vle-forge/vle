/**
 * @file vle/gvle/modeling/difference-equation/SourceDialog.hpp
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

#ifdef VLE_HAVE_GTKSOURCEVIEWMM
#include <gtksourceviewmm-2.0/gtksourceviewmm.h>
#endif

namespace vle { namespace gvle { namespace modeling {

class Source : public Gtk::ScrolledWindow {
public:
    Source(const std::string& buffer);
    virtual ~Source() { }

    std::string getBuffer() const;

    virtual void undo();
    virtual void redo();

    void paste();
    void copy();
    void cut();

private:
#ifdef VLE_HAVE_GTKSOURCEVIEWMM
    gtksourceview::SourceView mView;
#else
    mutable Gtk::TextView  mView; // mutable to enable the getBuffer function,
                                  // begin() and end() are non-const.
#endif

    void init(const std::string& buffer);
    void applyEditingProperties();
};

class SourceDialog
{
public:
    SourceDialog(Glib::RefPtr<Gnome::Glade::Xml> xml,
                 const std::string& computeFunction,
                 const std::string& initValueFunction,
                 const std::string& userFunctions);
    virtual ~SourceDialog();

    std::string getComputeFunction() const;
    std::string getInitValueFunction() const;
    std::string getUserFunctions() const;

    int run();

private:
    Glib::RefPtr<Gnome::Glade::Xml> mXml;

    Gtk::Dialog*                    mDialog;
    //Notebook
    Gtk::Notebook*                  mNotebook;
    //Documents
    Source*                         mComputeFunction;
    Source*                         mInitValueFunction;
    Source*                         mUserFunctions;
};

}}} // namespace vle gvle modeling

#endif

