/**
 * @file vle/gvle/ViewOutputBox.cpp
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


#include <vle/gvle/ViewOutputBox.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/utils/Path.hpp>
#include <gtkmm/filechooserdialog.h>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <limits>

namespace vle { namespace gvle {

ViewOutputBox::ViewOutputBox(Glib::RefPtr < Gnome::Glade::Xml > ref,
                             vpz::Views& views) :
    m_viewsorig(views),
    m_viewscopy(views),
    m_xml(ref),
    m_type(0),
    m_format(0),
    m_plugin(0),
    m_views(0)
{
    m_xml->get_widget("DialogViewOutput", m_dialog);
    m_xml->get_widget("spinbuttonTimestepDialogViewOutput", m_timestep);
    m_xml->get_widget("entryLocationDialogViewOutput", m_location);
    m_xml->get_widget("buttonLocationDialogViewOutput", m_directory);
    m_xml->get_widget("treeviewDialogViewOutput", m_views);
    m_xml->get_widget("textviewDataDialogViewOutput", m_data);

    initViews();
    fillCombobox();
    fillViews();
    initMenuPopupViews();

    m_timestep->set_value(1.0);
    m_timestep->set_range(std::numeric_limits < double >::epsilon(),
                          std::numeric_limits < double >::max());
    m_views->signal_button_release_event().connect(sigc::mem_fun(
            *this, &ViewOutputBox::onButtonRealeaseViews));
    m_views->signal_cursor_changed().connect(sigc::mem_fun(
            *this, &ViewOutputBox::onCursorChangedViews));
    m_type->signal_changed().connect(sigc::mem_fun(
            *this, &ViewOutputBox::onChangedType));
    m_format->signal_changed().connect(sigc::mem_fun(
            *this, &ViewOutputBox::onChangedFormat));
    m_directory->signal_clicked().connect(sigc::mem_fun(
            *this, &ViewOutputBox::onClickedDirectory));
}

ViewOutputBox::~ViewOutputBox()
{
    if (m_views) {
        m_views->remove_all_columns();
    }
    delete m_type;
    delete m_format;
    delete m_plugin;
}

void ViewOutputBox::run()
{
    if (m_dialog->run() == Gtk::RESPONSE_OK) {
        storePrevious();

        /* for each observables, we search all observable with a link to a
         * deleted view stored into the m_deletedView */
        vpz::Observables& obs(m_viewscopy.observables());
        for (vpz::Observables::iterator it = obs.begin(); it != obs.end();
             ++it) {

            /* for each observable port */
            for (vpz::Observable::iterator jt = it->second.begin();
                 jt != it->second.end(); ++jt) {

                /* for each view names */
                for (vpz::ObservablePort::iterator kt = jt->second.begin();
                     kt != jt->second.end(); ++kt) {

                    /* for each deleted view in this ViewOutputBox */
                    for (std::set < std::string >::iterator itdel =
                         m_deletedView.begin(); itdel != m_deletedView.end();
                         ++itdel) {

                        /* if a view exist, we delete them */
                        if (*kt == *itdel) {
                            jt->second.del(*itdel);
                            kt = jt->second.begin();
                        }
                    }
                }
            }
        }
        m_viewsorig = m_viewscopy; /* We copy the modified vpz::Views */
    }
    m_dialog->hide();
}

void ViewOutputBox::initViews()
{
    m_model = Gtk::ListStore::create(m_viewscolumnrecord);
    m_views->append_column("Name", m_viewscolumnrecord.name);
    m_views->set_model(m_model);
}

void ViewOutputBox::fillViews()
{
    for (vpz::Views::const_iterator it = m_viewscopy.begin();
         it != m_viewscopy.end(); ++it) {
            Gtk::TreeIter iter = m_model->append();
            if (iter) {
                Gtk::ListStore::Row row = *iter;
                row[m_viewscolumnrecord.name] = it->first;
            }
    }
    sensitive(false);
}

void ViewOutputBox::initMenuPopupViews()
{
    Gtk::Menu::MenuList& menulist(m_menu.items());

    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem(
            "_Add", sigc::mem_fun(
                *this, &ViewOutputBox::onAddViews)));

    menulist.push_back(
        Gtk::Menu_Helpers::MenuElem(
            "_Remove", sigc::mem_fun(
                *this, &ViewOutputBox::onRemoveViews)));
    menulist.push_back(
	Gtk::Menu_Helpers::MenuElem(
            "_Rename", sigc::mem_fun(
                *this, &ViewOutputBox::onRenameViews)));

    m_menu.accelerate(*m_views);
}

void ViewOutputBox::onAddViews()
{
    storePrevious();
    sensitive(true);

    SimpleTypeBox box("Name of the view ?");
    std::string name = boost::trim_copy(box.run());
    if (box.valid() and not name.empty() and not m_viewscopy.exist(name)) {
        Gtk::TreeIter iter = m_model->append();
        if (iter) {
            Gtk::ListStore::Row row = *iter;
            row[m_viewscolumnrecord.name] = name;

            std::string outputname(buildOutputName(name));
            m_viewscopy.addLocalStreamOutput(outputname, "", "default");
            m_viewscopy.addTimedView(name, 1.0, outputname);
            m_iter = iter;
            updateView(name);
            m_views->set_cursor(m_model->get_path(iter));
            m_deletedView.erase(name);
        }
    }
}

void ViewOutputBox::onRemoveViews()
{
    storePrevious();

    Glib::RefPtr < Gtk::TreeView::Selection > ref = m_views->get_selection();
    if (ref) {
        Gtk::TreeModel::iterator iter = ref->get_selected();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            std::string name(row.get_value(m_viewscolumnrecord.name));
            std::string output(m_viewscopy.get(name).output());
            m_viewscopy.del(name);
            m_viewscopy.outputs().del(output);
            m_model->erase(iter);
            m_deletedView.insert(name);
        }
    }
}

void ViewOutputBox::onRenameViews()
{
    storePrevious();

    Glib::RefPtr < Gtk::TreeView::Selection > ref = m_views->get_selection();
    if (ref) {
        Gtk::TreeModel::iterator iter = ref->get_selected();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            std::string oldname(row.get_value(m_viewscolumnrecord.name));
	    SimpleTypeBox box("Name of the view ?");
	    std::string newname = boost::trim_copy(box.run());
	    if (box.valid() and not newname.empty() and not m_viewscopy.exist(newname)) {
		row[m_viewscolumnrecord.name] = newname;
		m_viewscopy.renameView(oldname, newname);
		m_viewscopy.observables().updateView(oldname, newname);
	    }
	}
    }
}


void ViewOutputBox::sensitive(bool active)
{
    m_type->set_sensitive(active);
    m_timestep->set_sensitive(active);
    m_format->set_sensitive(active);
    m_location->set_sensitive(active);
    m_directory->set_sensitive(active);
    m_plugin->set_sensitive(active);
    m_data->set_sensitive(active);
}

bool ViewOutputBox::onButtonRealeaseViews(GdkEventButton* event)
{
    if (event->button == 3) {
        m_menu.popup(event->button, event->time);
    }
    return true;
}

void ViewOutputBox::onCursorChangedViews()
{
    storePrevious();

    Glib::RefPtr < Gtk::TreeView::Selection > ref = m_views->get_selection();
    if (ref) {
        Gtk::TreeModel::iterator iter = ref->get_selected();
        if (iter) {
            sensitive(true);
            Gtk::TreeModel::Row row = *iter;
            std::string name(row.get_value(m_viewscolumnrecord.name));
            updateView(name);
            m_iter = iter;
        }
    }
}

void ViewOutputBox::onChangedType()
{
    m_timestep->set_sensitive(m_type->get_active_text() == "timed");
}

void ViewOutputBox::onChangedFormat()
{
    m_directory->set_sensitive(m_format->get_active_text() == "local");
}

void ViewOutputBox::onClickedDirectory()
{
    Gtk::FileChooserDialog* box;
    m_xml->get_widget("DialogDirectoryChooser", box);

    if (box->run() == Gtk::RESPONSE_OK) {
        m_location->set_text(box->get_current_folder());
    }
    box->hide();
}

void ViewOutputBox::storePrevious()
{
    if (m_iter) { /* store the old view */
        if (m_model->iter_is_valid(m_iter)) {
            Gtk::TreeModel::Row row = *m_iter;
            std::string name(row.get_value(m_viewscolumnrecord.name));
            assignView(name);
        }
    }
}

void ViewOutputBox::fillCombobox()
{
    Gtk::HBox* box;
    m_xml->get_widget("comboboxTypeDialogViewOutput", box);
    m_type = new Gtk::ComboBoxText();
    m_type->show_all();
    box->pack_start(*m_type, true, true, 0);

    m_xml->get_widget("comboboxFormatDialogViewOutput", box);
    m_format = new Gtk::ComboBoxText();
    m_format->show_all();
    box->pack_start(*m_format, true, true, 0);

    m_xml->get_widget("comboboxPluginDialogViewOutput", box);
    m_plugin = new Gtk::ComboBoxText();
    m_plugin->show_all();
    box->pack_start(*m_plugin, true, true, 0);

    m_type->append_text("finish");
    m_type->append_text("event");
    m_type->append_text("timed");
    m_format->append_text("distant");
    m_format->append_text("local");

    {
        using namespace utils;
        Path::PathList paths = Path::path().getStreamDirs();
        for (Path::PathList::iterator it = paths.begin();
             it != paths.end(); ++it) {
            if (Glib::file_test(*it, Glib::FILE_TEST_EXISTS)) {
                Glib::Dir rep(*it);
                for (Glib::DirIterator in = rep.begin(); in != rep.end(); ++in) {
#ifdef G_OS_WIN32
                    if (((*in).find("lib") == 0) and
                        ((*in).rfind(".dll") == (*in).size() - 4)) {
                        m_plugin->append_text((*in).substr(3, (*in).size() - 7));
                    }
#else
                    if (((*in).find("lib") == 0) and
                        ((*in).rfind(".so") == (*in).size() - 3)) {
                        m_plugin->append_text((*in).substr(3, (*in).size() - 6));
                    }
#endif
                }
            }
        }
    }
}

void ViewOutputBox::assignView(const std::string& name)
{
    vpz::View& view(m_viewscopy.get(name));
    vpz::Output& output(m_viewscopy.outputs().get(view.output()));

    view.setType(m_type->get_active_text() == "timed" ? vpz::View::TIMED :
                 m_type->get_active_text() == "event" ? vpz::View::EVENT :
                 vpz::View::FINISH);
    view.setTimestep(m_timestep->get_value());

    if (m_format->get_active_text() == "distant") {
        output.setDistantStream(m_location->get_text(),
                                m_plugin->get_active_text());
    } else {
        output.setLocalStream(m_location->get_text(),
                              m_plugin->get_active_text());
    }

    output.setData(m_data->get_buffer()->get_text());
}

void ViewOutputBox::updateView(const std::string& name)
{
    vpz::View& view(m_viewscopy.get(name));
    vpz::Output& output(m_viewscopy.outputs().get(view.output()));

    m_type->set_active_text(view.streamtype());
    m_timestep->set_sensitive(view.type() == vpz::View::TIMED);
    m_timestep->set_value(view.timestep());
    m_format->set_active_text(output.streamformat());
    m_location->set_text(output.location());
    m_directory->set_sensitive(output.format() == vpz::Output::LOCAL);
    m_plugin->set_active_text(output.plugin());
    m_data->get_buffer()->set_text(output.data());
}

std::string ViewOutputBox::buildOutputName(const std::string& name) const
{
    std::string result(name);
    long nb = 0;

    while (m_viewscopy.outputs().exist(result)) {
        result.assign(boost::str(boost::format("%1%%2%") % name % nb));
        ++nb;
    }

    return result;
}

}} // namespace vle gvle
