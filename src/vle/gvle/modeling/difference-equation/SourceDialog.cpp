/*
 * @file vle/gvle/modeling/difference-equation/SourceDialog.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
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


#include <vle/gvle/modeling/difference-equation/SourceDialog.hpp>
#include <vle/gvle/Settings.hpp>

namespace vle { namespace gvle { namespace modeling { namespace de {

SourceDialog::SourceDialog(Glib::RefPtr<Gnome::Glade::Xml> xml,
                           const std::string& includes,
                           const std::string& computeFunction,
                           const std::string& initValueFunction,
                           const std::string& userFunctions) :
    mXml(xml)
{
    xml->get_widget("DialogSourceBox", mDialog);
    xml->get_widget("NotebookSource", mNotebook);

    mIncludes = Gtk::manage(new DocumentText(includes));
    mComputeFunction = Gtk::manage(new DocumentText(computeFunction));
    mInitValueFunction = Gtk::manage(new DocumentText(initValueFunction));
    mUserFunctions = Gtk::manage(new DocumentText(userFunctions));

    mNotebook->append_page(*mIncludes, std::string("Includes"));
    mNotebook->append_page(*mInitValueFunction, std::string("InitValue"));
    mNotebook->append_page(*mComputeFunction, std::string("Compute"));
    mNotebook->append_page(*mUserFunctions, std::string("User"));
}

SourceDialog::~SourceDialog()
{
    mNotebook->remove_page(*mIncludes);
    mNotebook->remove_page(*mComputeFunction);
    mNotebook->remove_page(*mInitValueFunction);
    mNotebook->remove_page(*mUserFunctions);
    mDialog->hide_all();
}

std::string SourceDialog::getIncludes() const
{
    return mIncludes->getBuffer();
}

std::string SourceDialog::getComputeFunction() const
{
    return mComputeFunction->getBuffer();
}

std::string SourceDialog::getInitValueFunction() const
{
    return mInitValueFunction->getBuffer();
}

std::string SourceDialog::getUserFunctions() const
{
    return mUserFunctions->getBuffer();
}

int SourceDialog::run()
{
    mDialog->show_all();
    return mDialog->run();
}

}}}} // namespace vle gvle modeling de
