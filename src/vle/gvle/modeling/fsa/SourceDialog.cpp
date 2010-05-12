/**
 * @file vle/gvle/modeling/fsa/SourceDialog.cpp
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


#include <vle/gvle/modeling/fsa/SourceDialog.hpp>
#include <vle/gvle/Settings.hpp>

namespace vle { namespace gvle { namespace modeling { namespace fsa {

SourceDialog::SourceDialog(Glib::RefPtr<Gnome::Glade::Xml> xml) : mXml(xml)
{
    xml->get_widget("SourceDialog", mDialog);
    xml->get_widget("NotebookSource", mNotebook);
}

SourceDialog::~SourceDialog()
{
    for (std::map < std::string, DocumentText* >::const_iterator it =
             mFunctions.begin(); it != mFunctions.end(); ++it) {
        mNotebook->remove_page(*it->second);
    }
    mDialog->hide_all();
}

void SourceDialog::add(const std::string& name, const std::string& buffer)
{
    mFunctions[name] = Gtk::manage(new DocumentText(buffer));
    mNotebook->append_page(*mFunctions[name], name);
}

std::string SourceDialog::buffer(const std::string& name) const
{
    return mFunctions.find(name)->second->getBuffer();
}

int SourceDialog::run()
{
    mDialog->show_all();
    return mDialog->run();
}

}}}} // namespace vle gvle modeling fsa
