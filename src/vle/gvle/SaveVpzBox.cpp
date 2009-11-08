/**
 * @file vle/gvle/SaveVpzBox.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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


#include <vle/gvle/SaveVpzBox.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Package.hpp>

namespace vle
{

namespace gvle {

SaveVpzBox::SaveVpzBox(Glib::RefPtr<Gnome::Glade::Xml> xml, Modeling* m) :
    mXml(xml),
    mModeling(m)
{
    xml->get_widget("DialogSaveVpz", mDialog);
    xml->get_widget("EntryNameVpz", mEntryName);

    xml->get_widget("ButtonSaveVpzApply", mButtonApply);
    mButtonApply->signal_clicked().connect(
	sigc::mem_fun(*this, &SaveVpzBox::onApply));

    xml->get_widget("ButtonSaveVpzCancel", mButtonCancel);
    mButtonCancel->signal_clicked().connect(
    sigc::mem_fun(*this, &SaveVpzBox::onCancel));
}

SaveVpzBox::~SaveVpzBox()
{
    delete mButtonApply;
    delete mButtonCancel;
    delete mEntryName;
    delete mDialog;
}

void SaveVpzBox::show()
{
    mEntryName->set_text("");
    mDialog->set_title("Save VPZ");
    mDialog->show_all();
    mDialog->run();
}

void SaveVpzBox::onApply()
{
    if (not mEntryName->get_text().empty()) {
	if (not exist(mEntryName->get_text()) or
	    gvle::Question(_("Do you want to replace the file ") +
		    static_cast<std::string>(mEntryName->get_text()))) {
	    const Modeling* modeling = (const Modeling*)(mModeling);
	    std::string fileName = mEntryName->get_text();
	    vpz::Vpz::fixExtension(fileName);
	    std::string pathFile = Glib::build_filename(
		utils::Path::path().getPackageExpDir(), fileName);
	    Editor::Documents::const_iterator it = modeling
		->getDocuments().find(modeling->getFileName());
	    mModeling->saveXML(pathFile);
	    if (it != modeling->getDocuments().end()) {
		it->second->setTitle(pathFile, mModeling->getTopModel(), false);
	    }
	    mDialog->hide_all();
	}
    }
}

void SaveVpzBox::onCancel()
{
    mDialog->hide_all();
}

bool SaveVpzBox::exist(std::string name)
{
    utils::PathList list = utils::Path::path().getInstalledExperiments();
    utils::PathList::const_iterator it = list.begin();
    while (it != list.end()) {
	if (utils::Path::basename(*it) == name) {
	    return true;
	}
	++it;
    }
    return false;
}

}
}

