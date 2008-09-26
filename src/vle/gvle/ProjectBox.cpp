/**
 * @file vle/gvle/ProjectBox.cpp
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


#include <vle/gvle/Message.hpp>
#include <vle/gvle/ProjectBox.hpp>
#include <vle/utils/Tools.hpp>

namespace vle { namespace gvle {

ProjectBox::ProjectBox(Glib::RefPtr<Gnome::Glade::Xml> xml, vpz::Project* project):
        mXml(xml),
        mProject(project),
        mCalendar(xml)
{
    xml->get_widget("DialogProject", mDialog);

    xml->get_widget("EntryAuthor", mEntryAuthor);

    xml->get_widget("EntryDate", mEntryDate);

    xml->get_widget("ButtonCalendar", mButtonCalendar);
    mButtonCalendar->signal_clicked().connect(
        sigc::mem_fun(*this, &ProjectBox::on_calendar));

    xml->get_widget("ButtonNow", mButtonNow);
    mButtonNow->signal_clicked().connect(
        sigc::mem_fun(*this, &ProjectBox::on_now));

    xml->get_widget("EntryVersion", mEntryVersion);

    xml->get_widget("ButtonProjectApply", mButtonApply);
    mButtonApply->signal_clicked().connect(
        sigc::mem_fun(*this, &ProjectBox::on_apply));

    xml->get_widget("ButtonProjectCancel", mButtonCancel);
    mButtonCancel->signal_clicked().connect(
        sigc::mem_fun(*this, &ProjectBox::on_cancel));
}

void ProjectBox::show()
{
    mBackupDate = mProject->date();

    {
        std::string name(mProject->author());
        if (name.empty()) {
            name = Glib::get_real_name();
            if (name.empty()) {
                name = Glib::get_user_name();
            }
        }
        mEntryAuthor->set_text(name);
    }

    mEntryDate->set_text(mProject->date());
    mEntryVersion->set_text(utils::to_string(mProject->version()));

    mDialog->show_all();
    mDialog->run();
}

void ProjectBox::on_calendar()
{
    std::string date;
    mCalendar.get_date(date);
    mEntryDate->set_text(date);
}

void ProjectBox::on_now()
{
    mProject->setDate(utils::get_current_date());
    mEntryDate->set_text(mProject->date());
}

void ProjectBox::on_apply()
{
    std::string error = "";

    if (mEntryAuthor->get_text() == "") {
        error += "Set an author for this project.\n";
    }

    if (mEntryVersion->get_text() == "") {
        error += "Set an number of version for this project.\n";
    }

    if (error == "") {
        mDialog->hide();
        mProject->setAuthor(mEntryAuthor->get_text());
        mProject->setDate(mEntryDate->get_text());
        mProject->setVersion(mEntryVersion->get_text());
    } else {
        Error(error);
    }
}

void ProjectBox::on_cancel()
{
    mProject->setDate(mBackupDate);
    mDialog->hide();
}

}} // namespace vle gvle
