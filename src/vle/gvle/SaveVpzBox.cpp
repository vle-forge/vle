/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
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


#include <vle/gvle/SaveVpzBox.hpp>
#include <vle/gvle/GVLE.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/Editor.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Path.hpp>

namespace vle { namespace gvle {

SaveVpzBox::SaveVpzBox(const Glib::RefPtr < Gtk::Builder >& xml,
                       Modeling* m, GVLE* gvle) :
    mXml(xml),
    mModeling(m),
    mGVLE(gvle)
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
                    mGVLE->currentPackage().getExpDir(
                            vle::utils::PKG_SOURCE), fileName);
            Editor::Documents::const_iterator it =
                    mGVLE->getEditor()->getDocuments().find(
                            modeling->getFileName());
            mModeling->saveXML(pathFile);
            mGVLE->setTitle(modeling->getFileName());
            if (it != mGVLE->getEditor()->getDocuments().end()) {
                it->second->setTitle(pathFile, mModeling->getTopModel(), false);
            }
            
            // no hide_all in Gtk 3
            mDialog->hide();
        }
    }
}

void SaveVpzBox::onCancel()
{
    // no hide_all in Gtk 3
    mDialog->hide();
}

bool SaveVpzBox::exist(std::string name)
{
    utils::PathList list =  mGVLE->currentPackage().getExperiments(vle::utils::PKG_SOURCE);
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

