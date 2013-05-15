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


#include <vle/gvle/NewProjectBox.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/gvle/Editor.hpp>
#include <vle/gvle/GVLEMenuAndToolbar.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/i18n.hpp>

namespace vle { namespace gvle {

NewProjectBox::NewProjectBox(const Glib::RefPtr < Gtk::Builder >& xml,
                             Modeling* m, GVLE* gvle)
    : mXml(xml), mModeling(m), mGVLE(gvle)
{
    xml->get_widget("DialogNewProject", mDialog);
    xml->get_widget("EntryNameProject", mEntryName);
}

NewProjectBox::~NewProjectBox()
{
}

void NewProjectBox::show()
{
    mDialog->set_title("New Project");
    mDialog->show_all();
    mDialog->set_default_response(-3);

    if (mDialog->run() == Gtk::RESPONSE_ACCEPT) {
        apply();
    }

    mDialog->hide_all();
}

void NewProjectBox::apply()
{
    if (not mEntryName->get_text().empty()) {
    if (not exist(mEntryName->get_text())) {
        mGVLE->getEditor()->closeAllTab();
        mModeling->clearModeling();
        mGVLE->setTitle(mModeling->getFileName());
        mGVLE->currentPackage().select(mEntryName->get_text());
        mGVLE->pluginFactory().update();
        mGVLE->currentPackage().create();
        mGVLE->buildPackageHierarchy();
        mGVLE->getMenu()->onOpenProject();
    } else {
            Error(fmt(_("The project `%1%' already exists in VLE home "
                         "directory")) % mEntryName->get_text().raw());
	}
    }
}

bool NewProjectBox::exist(const std::string& name)
{
    utils::PathList list = utils::Path::path().getBinaryPackages();
    utils::PathList::const_iterator it = list.begin();
    while (it != list.end()) {
        if (utils::Path::basename(*it) == name) {
            return true;
        }
        ++it;
    }
    return false;
}

}} // namespace vle gvle
