/**
 * @file vle/gvle/NewProjectBox.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
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


#include <vle/gvle/Message.hpp>
#include <vle/gvle/NewProjectBox.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/gvle/GVLEMenuAndToolbar.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Package.hpp>

namespace vle { namespace gvle {

NewProjectBox::NewProjectBox(Glib::RefPtr<Gnome::Glade::Xml> xml,
			     Modeling* m, GVLE* app) :
    mXml(xml),
    mModeling(m),
    mApp(app)
{
    xml->get_widget("DialogNewProject", mDialog);
    xml->get_widget("EntryNameProject", mEntryName);

    xml->get_widget("ButtonNewProjectApply", mButtonApply);
    mButtonApply->signal_clicked().connect(
	sigc::mem_fun(*this, &NewProjectBox::onApply));

    xml->get_widget("ButtonNewProjectCancel", mButtonCancel);
    mButtonCancel->signal_clicked().connect(
	sigc::mem_fun(*this, &NewProjectBox::onCancel));
}

NewProjectBox::~NewProjectBox()
{
    delete mButtonApply;
    delete mButtonCancel;
    delete mEntryName;
    delete mDialog;
}

void NewProjectBox::show()
{
    mEntryName->set_text("");
    mDialog->set_title("New Project");
    mDialog->show_all();
    mDialog->run();
}

void NewProjectBox::onApply()
{
    if (not mEntryName->get_text().empty()) {
	if (not exist(mEntryName->get_text())) {
	    mApp->getEditor()->closeAllTab();
	    mModeling->clearModeling();
            utils::Package::package().select(mEntryName->get_text());
	    vle::utils::Package::package().create();
	    mApp->buildPackageHierarchy();
            mApp->getMenu()->onOpenProject();
	    mDialog->hide_all();
	} else {
	    Error(_("The Project ") +
		  static_cast<std::string>(mEntryName->get_text()) +
		  (_(" already exist")));
	}
    }
}

void NewProjectBox::onCancel()
{
    mDialog->hide_all();
}

bool NewProjectBox::exist(std::string name)
{
    utils::PathList list = utils::Path::path().getInstalledPackages();
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



