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
#include <vle/gvle/Message.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Socket.hpp>
#include <cmath>

namespace vle { namespace gvle {

DynamicBox::DynamicBox(Glib::RefPtr<Gnome::Glade::Xml> xml) :
    mXml(xml)
{
    xml->get_widget("DialogDynamic", mDialog);

    {
        Gtk::HBox* box;
        xml->get_widget("HBoxDynamicLibrary", box);
        mComboLibrary = new Gtk::ComboBoxText();
        box->pack_start(*mComboLibrary);
    }

    {
        Gtk::HBox* box;
        xml->get_widget("HBoxDynamicPackage", box);
        mComboPackage = new Gtk::ComboBoxText();
        box->pack_start(*mComboPackage);
    }

    xml->get_widget("EntryDynamicLocationHost", mLocationHost);
    xml->get_widget("SpinbuttonDynamicLocationPort", mLocationPort);
    xml->get_widget("EntryDynamicModel", mModel);

    {
        Gtk::HBox* box;
        xml->get_widget("HboxDynamicLanguage", box);
        mLanguage = new Gtk::ComboBoxText();
        box->pack_start(*mLanguage);

        mLanguage->append_text("c++");
        mLanguage->append_text("python");
    }

    xml->get_widget("ButtonDynamicApply", mButtonApply);
    xml->get_widget("ButtonDynamicCancel", mButtonCancel);

    mButtonApply->signal_clicked().connect(
        sigc::mem_fun(*this, &DynamicBox::on_apply));
    mButtonCancel->signal_clicked().connect(
        sigc::mem_fun(*this, &DynamicBox::on_cancel));
    mComboPackage->signal_changed().connect(
        sigc::mem_fun(*this, &DynamicBox::onComboPackageChange));
}

DynamicBox::~DynamicBox()
{
    delete mComboLibrary;
    delete mComboPackage;
    delete mLanguage;
}

void DynamicBox::show(vpz::Dynamic* dyn)
{
    mDyn = dyn;

    mDialog->set_title((fmt(_("Dynamics: %1%")) % dyn->name()).str());
    makeComboPackage();
    makeComboLibrary();

    if (not mDyn->location().empty()) {
        std::string ip;
        int port;

        utils::net::explodeStringNet(dyn->location(), ip, port);

        mLocationHost->set_text(ip);
        mLocationPort->set_value(port);
    } else {
        mLocationHost->set_text("");
        mLocationPort->set_value(0);
    }

    mModel->set_text(dyn->model());

    if (dyn->language().empty()) {
        mLanguage->set_active_text("c++");
    } else {
        mLanguage->set_active_text(dyn->language());
    }

    mDialog->show_all();
    mDialog->run();
}

void DynamicBox::makeComboLibrary()
{
    mComboLibrary->clear();

    utils::PathList paths;

    if (mComboPackage->get_active_text().empty()) {
        paths = utils::Path::path().getSimulatorDirs();
    } else {
        paths.push_back(utils::Path::path().getExternalPackageLibDir(
                mComboPackage->get_active_text()));
    }
    utils::PathList::iterator it = paths.begin();

    while (it != paths.end()) {
        if (Glib::file_test(*it, Glib::FILE_TEST_EXISTS)) {
            Glib::Dir rep(*it);
            Glib::DirIterator in = rep.begin();
            while (in != rep.end()) {
#ifdef G_OS_WIN32
                if (((*in).find("lib") == 0) &&
                    ((*in).rfind(".dll") == (*in).size() - 4)) {
                    mComboLibrary->append_text((*in).substr(3, (*in).size() - 7));
                }
#else
                if (((*in).find("lib") == 0) &&
                    ((*in).rfind(".so") == (*in).size() - 3)) {
                    mComboLibrary->append_text((*in).substr(3, (*in).size() - 6));
                }
#endif
                in++;
            }
        }
        it++;
    }
    mComboLibrary->set_active_text(mDyn->library());
}

void DynamicBox::makeComboPackage()
{
    mComboPackage->clear();

    utils::PathList paths = utils::Path::path().getInstalledPackages();

    paths.sort();
    for (utils::PathList::const_iterator i = paths.begin(), e = paths.end();
         i != e; ++i) {
        mComboPackage->append_text(*i);
    }

    mComboPackage->set_active_text(mDyn->package());
}

void DynamicBox::onComboPackageChange()
{
    makeComboLibrary();
}

void DynamicBox::on_apply()
{
    mValid = true;
    if (mComboLibrary->get_active_text().empty()) {
        Error(_("Set a library to this Dynamic"));
        mValid = false;
        return;
    }

    mDyn->setLibrary(mComboLibrary->get_active_text());
    mDyn->setPackage(mComboPackage->get_active_text());

    if (mLocationHost->get_text().empty()) {
        mDyn->setLocalDynamics();
    } else {
        int port = (int)std::floor(std::abs(mLocationPort->get_value()));
        mDyn->setDistantDynamics(mLocationHost->get_text(), port);
    }

    mDyn->setModel(mModel->get_text());
    if (mLanguage->get_active_text() == "c++") {
        mDyn->setLanguage("");
    } else {
        mDyn->setLanguage(mLanguage->get_active_text());
    }
    mDialog->hide();
}

void DynamicBox::on_cancel()
{
    mValid = false;
    mDialog->hide();
}

}} // namespace vle gvle
