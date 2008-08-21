/**
 * @file vle/gvle/DynamicBox.cpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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


#include <vle/gvle/DynamicBox.hpp>
#include <vle/gvle/WarningBox.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Tools.hpp>

#include <iostream>

using namespace vle;

namespace vle
{
namespace gvle {
DynamicBox::DynamicBox(Glib::RefPtr<Gnome::Glade::Xml> xml):
        mXml(xml)
{
    xml->get_widget("DialogDynamic", mDialog);

    xml->get_widget("HBoxDynamicLibrary", mHboxCombo);

    mCombo = new Gtk::ComboBoxText();
    mHboxCombo->pack_start(*mCombo);

    xml->get_widget("EntryDynamicLocationHost", mLocationHost);
    xml->get_widget("EntryDynamicLocationPort", mLocationPort);

    xml->get_widget("EntryDynamicModel", mModel);

    xml->get_widget("EntryDynamicLanguage", mLanguage);

    xml->get_widget("ButtonDynamicApply", mButtonApply);
    mButtonApply->signal_clicked().connect(
        sigc::mem_fun(*this, &DynamicBox::on_apply));

    xml->get_widget("ButtonDynamicCancel", mButtonCancel);
    mButtonCancel->signal_clicked().connect(
        sigc::mem_fun(*this, &DynamicBox::on_cancel));
}

DynamicBox::~DynamicBox()
{
    delete mCombo;
}

void DynamicBox::show(vpz::Dynamic* dyn)
{
    mDyn = dyn;
    mDialog->set_title("Dynamic " + dyn->name());
    makeCombo();
    if (mDyn->location() != "") {
        int pos = dyn->location().find_last_of(":");
        mLocationHost->set_text(dyn->location().substr(0, pos));
        mLocationPort->set_text(dyn->location().substr(pos+1));
    } else {
        mLocationHost->set_text("");
        mLocationPort->set_text("");
    }
    mModel->set_text(dyn->model());
    mLanguage->set_text(dyn->language());

    mDialog->show_all();
    mDialog->run();
}

void DynamicBox::makeCombo()
{
    mCombo->clear();

    utils::Path::PathList paths = utils::Path::path().getSimulatorDirs();
    utils::Path::PathList::iterator it = paths.begin();

    while (it != paths.end()) {
        if (Glib::file_test(*it, Glib::FILE_TEST_EXISTS)) {
            Glib::Dir rep(*it);
            Glib::DirIterator in = rep.begin();
            while (in != rep.end()) {
#ifdef G_OS_WIN32
                if (((*in).find("lib") == 0) && ((*in).rfind(".dll") == (*in).size() - 4)) {
                    mCombo->append_text((*in).substr(3, (*in).size() - 7));
                }
#else
                if (((*in).find("lib") == 0) && ((*in).rfind(".so") == (*in).size() - 3)) {
                    mCombo->append_text((*in).substr(3, (*in).size() - 6));
                }
#endif
                in++;
            }
        }
        it++;
    }
    mCombo->set_active_text(mDyn->library());
}

void DynamicBox::on_apply()
{
    std::string error = "";
    if (mCombo->get_active_text() == "") {
        error += "Set a library to this Dynamic.\n";
    }

    if ((mLocationHost->get_text()=="") xor(mLocationPort->get_text()=="")) {
        error += "Set a CORRECT and COMPLETE location.\n";
    }

    if ((mLocationPort->get_text()!="") && (!utils::is_int(mLocationPort->get_text()))) {
        error += "Set a CORRECT port.\n";
    }

    if (error != "") {
        WarningBox box(error);
        box.run();
    } else {
        mDyn->setLibrary(mCombo->get_active_text());
        if (mLocationHost->get_text() != "") {
            mDyn->setDistantDynamics(mLocationHost->get_text(), utils::to_int(mLocationPort->get_text()));
        } else {
            mDyn->setLocalDynamics();
        }
        mDyn->setModel(mModel->get_text());
        mDyn->setLanguage(mLanguage->get_text());
        mDialog->hide();
    }
}

void DynamicBox::on_cancel()
{
    mDialog->hide();
}

}
} // namespace vle gvle
