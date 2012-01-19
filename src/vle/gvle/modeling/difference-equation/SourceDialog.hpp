/*
 * @file vle/gvle/modeling/difference-equation/SourceDialog.hpp
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
#include <gtkmm/builder.h>
#include <gtkmm.h>
#include <vle/gvle/Editor.hpp>

#ifdef VLE_HAVE_GTKSOURCEVIEWMM
#include <gtksourceviewmm-2.0/gtksourceviewmm.h>
#endif

namespace vle { namespace gvle { namespace modeling { namespace de {

class SourceDialog
{
public:
    SourceDialog(Glib::RefPtr<Gtk::Builder> xml,
                 const std::string& includes,
                 const std::string& computeFunction,
                 const std::string& initValueFunction,
                 const std::string& userFunctions);

    virtual ~SourceDialog();

    std::string getIncludes() const;
    std::string getComputeFunction() const;
    std::string getInitValueFunction() const;
    std::string getUserFunctions() const;

    int run();

private:
    Glib::RefPtr<Gtk::Builder> mXml;

    Gtk::Dialog*                    mDialog;
    //Notebook
    Gtk::Notebook*                  mNotebook;
    //Documents
    DocumentText*                   mIncludes;
    DocumentText*                   mComputeFunction;
    DocumentText*                   mInitValueFunction;
    DocumentText*                   mUserFunctions;
};

}}}} // namespace vle gvle modeling de

#endif
