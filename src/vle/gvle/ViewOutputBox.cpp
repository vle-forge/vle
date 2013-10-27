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


#include <vle/gvle/GVLE.hpp>
#include <vle/gvle/ViewOutputBox.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/Modeling.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/vpz/Vpz.hpp>
#include <gtkmm/filechooserdialog.h>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

namespace vu = vle::utils;

namespace vle { namespace gvle {

ViewOutputBox::ViewOutputBox(Modeling& modeling, GVLE* gvle,
                             const Glib::RefPtr < Gtk::Builder >& ref,
                             vpz::Views& views)
    : m_modeling(modeling), m_GVLE(gvle), m_viewscopy(views), m_xml(ref),
      m_type(0), m_format(0), m_plugin(0), m_views(0),
      m_validateRetry(false)
{
    m_xml->get_widget("DialogViewOutput", m_dialog);
    m_xml->get_widget("entryTimestepDialogViewOutput", m_timestep);
    m_xml->get_widget("entryLocationDialogViewOutput", m_location);
    m_xml->get_widget("buttonLocationDialogViewOutput", m_directory);
    m_xml->get_widget("treeviewDialogViewOutput", m_views);
    m_xml->get_widget("textviewDataDialogViewOutput", m_data);
    m_xml->get_widget("buttonEditPlugin", m_editplugin);

    fillCombobox();
    initViews();
    fillViews();
    initMenuPopupViews();

    m_timestep->set_text("1,0");

    m_cntViewButtonRelease = m_views->signal_button_release_event().connect(
        sigc::mem_fun( *this, &ViewOutputBox::onButtonRealeaseViews));
    m_cntViewCursorChanged = m_views->signal_cursor_changed().connect(
        sigc::mem_fun( *this, &ViewOutputBox::onCursorChangedViews));
    m_cntTypeChanged = m_type->signal_changed().connect(sigc::mem_fun(
            *this, &ViewOutputBox::onChangedType));
    m_cntFormatChanged = m_format->signal_changed().connect(sigc::mem_fun(
            *this, &ViewOutputBox::onChangedFormat));
    m_cntDirectoryClicked = m_directory->signal_clicked().connect(
        sigc::mem_fun( *this, &ViewOutputBox::onClickedDirectory));
    m_cntEditPluginClicked = m_editplugin->signal_clicked().connect(
        sigc::mem_fun( *this, &ViewOutputBox::onEditPlugin));
    m_cntPluginClicked = m_plugin->signal_changed().connect(
	sigc::mem_fun( *this, &ViewOutputBox::onChangedPlugin));
}

ViewOutputBox::~ViewOutputBox()
{
    if (m_views) {
        m_views->remove_all_columns();
    }
    delete m_type;
    delete m_format;
    delete m_plugin;

    m_cntViewButtonRelease.disconnect();
    m_cntViewCursorChanged.disconnect();
    m_cntTypeChanged.disconnect();
    m_cntFormatChanged.disconnect();
    m_cntDirectoryClicked.disconnect();
    m_cntEditPluginClicked.disconnect();
    m_cntPluginClicked.disconnect();
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
	m_modeling.views() = m_viewscopy;
	m_modeling.setModified(true);
    }
    m_dialog->hide();
}

void ViewOutputBox::initViews()
{
    m_model = Gtk::ListStore::create(m_viewscolumnrecord);

    m_columnName = m_views->append_column_editable(_("Name"),
						   m_viewscolumnrecord.name);
    m_cellrenderer = dynamic_cast<Gtk::CellRendererText*>(
	m_views->get_column_cell_renderer(m_columnName - 1));
    m_cellrenderer->property_editable() = true;

    m_cellrenderer->signal_editing_started().connect(
	sigc::mem_fun(*this,
		      &ViewOutputBox::onEditionStarted));

    m_cellrenderer->signal_edited().connect(
	sigc::mem_fun(*this,
		      &ViewOutputBox::onEdition) );

    m_views->set_model(m_model);
    m_data->get_buffer()->set_text("");
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
    m_editplugin->set_sensitive(false);
    m_iter = m_model->children().end();
}

void ViewOutputBox::initMenuPopupViews()
{
    mPopupActionGroup = Gtk::ActionGroup::create("initMenuPopupViews");
    mPopupActionGroup->add(Gtk::Action::create("VOBox_ContextMenu", _("Context Menu")));
    
    mPopupActionGroup->add(Gtk::Action::create("VOBox_ContextAdd", _("_Add")),
        sigc::mem_fun(*this, &ViewOutputBox::onAddViews));
    
    mPopupActionGroup->add(Gtk::Action::create("VOBox_ContextCopy", _("_Copy")),
        sigc::mem_fun(*this, &ViewOutputBox::onCopyViews));
    
    mPopupActionGroup->add(Gtk::Action::create("VOBox_ContextRemove", _("_Remove")),
        sigc::mem_fun(*this, &ViewOutputBox::onRemoveViews));
    
    mPopupActionGroup->add(Gtk::Action::create("VOBox_ContextRename", _("_Rename")),
        sigc::mem_fun(*this, &ViewOutputBox::onRenameViews));

    mPopupUIManager = Gtk::UIManager::create();
    mPopupUIManager->insert_action_group(mPopupActionGroup);

    Glib::ustring ui_info =
        "<ui>"
        "  <popup name='VOBox_Popup'>"
        "      <menuitem action='VOBox_ContextAdd'/>"
        "      <menuitem action='VOBox_ContextCopy'/>"
        "      <menuitem action='VOBox_ContextRemove'/>"
        "      <menuitem action='VOBox_ContextRename'/>"
        "  </popup>"
        "</ui>";

    try {
        mPopupUIManager->add_ui_from_string(ui_info);
        m_menu = (Gtk::Menu *) (
            mPopupUIManager->get_widget("/VOBox_Popup"));
    } catch(const Glib::Error& ex) {
        std::cerr << "building menus failed: VOBox_Popup" <<  ex.what();
    }

    if (!m_menu)
        std::cerr << "menu not found : VOBox_Popup\n";
}

void ViewOutputBox::onAddViews()
{
    storePrevious();
    sensitive(true);

    if (not m_plugin->get_model()->children().empty()) {
        SimpleTypeBox box(_("Name of the view ?"), "");
        std::string name = boost::trim_copy(box.run());
        if (box.valid() and not name.empty() and not m_viewscopy.exist(name)) {
            Gtk::TreeIter iter = m_model->append();
            if (iter) {
                Gtk::ListStore::Row row = *iter;
                row[m_viewscolumnrecord.name] = name;

                std::string outputname(buildOutputName(name));

                if (m_plugin->get_active_text().empty()) {
                    m_plugin->set_active(0);
                }

                std::string package, library;

                PluginFactory::buildPackageLibraryNames(
                    m_plugin->get_active_text(), &package, &library);

                m_viewscopy.addLocalStreamOutput(outputname, "", library,
                                                 package);
                m_viewscopy.addTimedView(name, 1.0, outputname);
                m_iter = iter;
                updateView(name);
                m_views->set_cursor(m_model->get_path(iter));
                m_deletedView.erase(name);
            }
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

	    Gtk::TreeModel::Children children = m_model->children();
	    m_iter = children.begin();
	    if (m_iter != children.end()) {
		row = *m_iter;
		updateView(row.get_value(m_viewscolumnrecord.name));
		Gtk::TreeModel::Path path = m_model->get_path(m_iter);
		m_views->set_cursor(path);
	    } else {
                sensitive(false);
            }
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
	    SimpleTypeBox box(_("Name of the view ?"), oldname);
	    std::string newname = boost::trim_copy(box.run());
	    if (box.valid() and not newname.empty()
                and not m_viewscopy.exist(newname)) {
		row[m_viewscolumnrecord.name] = newname;
		m_viewscopy.renameView(oldname, newname);
		m_viewscopy.observables().updateView(oldname, newname);
	    }
	}
    }
}


void ViewOutputBox::onCopyViews()
{
    int number = 1;
    std::string copy;

    Glib::RefPtr < Gtk::TreeView::Selection > ref = m_views->get_selection();
    if (ref) {
        Gtk::TreeModel::iterator iter = ref->get_selected();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            std::string name(row.get_value(m_viewscolumnrecord.name));
	    do {
		copy = name + "_" + boost::lexical_cast< std::string >(number);
		++number;
	    } while (m_viewscopy.exist(copy));
	    iter = m_model->append();
	    row = *iter;
	    row[m_viewscolumnrecord.name] = copy;
	    m_viewscopy.copyView(name, copy);
	    updateView(copy);
            m_views->set_cursor(m_model->get_path(iter));
            m_deletedView.erase(copy);
	}
    }
}

void ViewOutputBox::onEditionStarted(
    Gtk::CellEditable* cellEditable, const Glib::ustring& /* path */)
{
    storePrevious();

    Glib::RefPtr < Gtk::TreeView::Selection > ref = m_views->get_selection();
    if (ref) {
        Gtk::TreeModel::iterator iter = ref->get_selected();
        if (iter) {
            Gtk::TreeModel::Row row = *iter;
            m_oldName = row.get_value(m_viewscolumnrecord.name);
	}
    }

    if(m_validateRetry) {
	Gtk::CellEditable* celleditable_validated = cellEditable;
	Gtk::Entry* pEntry = dynamic_cast<Gtk::Entry*>(celleditable_validated);
        if(pEntry) {
	    pEntry->set_text(m_invalidTextForRetry);
	    m_validateRetry = false;
	    m_invalidTextForRetry.clear();
	}
    }
}

void ViewOutputBox::onEdition(
        const Glib::ustring& pathString,
        const Glib::ustring& newName)
{
    Gtk::TreePath path(pathString);

    if (not m_viewscopy.exist(newName) and newName != "") {
	Glib::RefPtr < Gtk::TreeView::Selection > ref =
	    m_views->get_selection();
	if (ref) {
	    Gtk::TreeModel::iterator iter = ref->get_selected();
	    if (iter) {
		Gtk::TreeModel::Row row = *iter;
		row[m_viewscolumnrecord.name] = newName;
		m_viewscopy.renameView(m_oldName, newName);
		m_viewscopy.observables().updateView(m_oldName, newName);

		m_validateRetry = true;
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
        m_menu->popup(event->button, event->time);
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
            Gtk::TreeModel::Row row = *iter;
            std::string name(row.get_value(m_viewscolumnrecord.name));

            sensitive(true);
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
    m_xml->get_widget(_("DialogDirectoryChooser"), box);

    if (box->run() == Gtk::RESPONSE_OK) {
        m_location->set_text(box->get_current_folder());
    }
    box->hide();
}

void ViewOutputBox::onEditPlugin()
{
    std::string name;

    {
        Glib::RefPtr < Gtk::TreeView::Selection > ref =
            m_views->get_selection();

        if (ref) {
            Gtk::TreeModel::iterator iter = ref->get_selected();

            if (iter) {
                Gtk::TreeModel::Row row = *iter;

                name.assign(row.get_value(m_viewscolumnrecord.name));
            }
        }
    }

    if (not name.empty()) {
        PluginFactory& plf = m_GVLE->pluginFactory();

        try {
            OutputPluginPtr plugin =
                plf.getOutputPlugin(m_plugin->get_active_text());
            vpz::View& view(m_viewscopy.get(name));
            vpz::Output& output(m_viewscopy.outputs().get(view.output()));

            plugin->start(output, view);
            if (output.data()) {
                m_data->get_buffer()->set_text(output.data()->writeToXml());
            }
        } catch(const std::exception& e) {
            Error(e.what());
        }
    }
}

void ViewOutputBox::onChangedPlugin()
{
    Glib::RefPtr < Gtk::TreeView::Selection > ref = m_views->get_selection();

    if (ref) {
        Gtk::TreeModel::iterator iter = ref->get_selected();

        if (ref->get_selected()) {
            Gtk::TreeModel::Row row = *iter;
            std::string name(row.get_value(m_viewscolumnrecord.name));
            vpz::View& view(m_viewscopy.get(name));
            vpz::Output& output(m_viewscopy.outputs().get(view.output()));

            std::string package, library;

            PluginFactory::buildPackageLibraryNames(m_plugin->get_active_text(),
                                                    &package,
                                                    &library);

            if (output.plugin() != library or
                output.package() != package) {
                if (m_format->get_active_text() == "distant") {
                    output.setDistantStream(m_location->get_text(), library,
                                            package);
                } else {
                    output.setLocalStream(m_location->get_text(), library,
                                          package);
                }
                updateView(name);
            }
        }
    }
}

void ViewOutputBox::storePrevious()
{
    if (m_iter != m_model->children().end()) {
        Gtk::TreeModel::Row row = *m_iter;
        std::string name(row.get_value(m_viewscolumnrecord.name));

        assignView(name);
    }
}

void ViewOutputBox::fillCombobox()
{
    Gtk::Box* box;
    m_xml->get_widget(("comboboxTypeDialogViewOutput"), box);
    m_type = new Gtk::ComboBoxText();
    m_type->show_all();
    box->pack_start(*m_type, true, true, 0);

    m_xml->get_widget(_("comboboxFormatDialogViewOutput"), box);
    m_format = new Gtk::ComboBoxText();
    m_format->show_all();
    box->pack_start(*m_format, true, true, 0);

    m_xml->get_widget(_("comboboxPluginDialogViewOutput"), box);
    m_plugin = new Gtk::ComboBoxText();
    m_plugin->show_all();
    box->pack_start(*m_plugin, true, true, 0);

    m_type->append ("finish");
    m_type->append ("event");
    m_type->append ("timed");
    m_format->append ("distant");
    m_format->append ("local");

    {
        utils::ModuleList lst;
        utils::ModuleList::iterator it;
        m_GVLE->pluginFactory().getOutputPlugins(&lst);

        for (it = lst.begin(); it != lst.end(); ++it) {
            m_plugin->append (it->package + "/" + it->library);
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

    {
        try {
            double x = vu::convert < double >(m_timestep->get_text(), true);
            view.setTimestep(x);
        } catch(const std::exception& e) {
            Error(e.what());
        }
    }

    std::string package, library;

    PluginFactory::buildPackageLibraryNames(m_plugin->get_active_text(),
                                            &package, &library);

    if (m_format->get_active_text() == "distant") {
        output.setDistantStream(m_location->get_text(), library, package);
    } else {
        output.setLocalStream(m_location->get_text(), library, package);
    }

    if (not m_data->get_buffer()->get_text().empty()) {
        try {
            value::Value* result = vpz::Vpz::parseValue(
                m_data->get_buffer()->get_text());
            output.setData(result);
        } catch (const std::exception& e) {
            Error("Error: parametrization plug-in error");
            output.setData(0);
        }
    }
}

void ViewOutputBox::updateView(const std::string& name)
{
    vpz::View& view(m_viewscopy.get(name));
    vpz::Output& output(m_viewscopy.outputs().get(view.output()));

    m_type->set_active_text(view.streamtype());
    m_timestep->set_sensitive(view.type() == vpz::View::TIMED);
    m_timestep->set_text(utils::toScientificString(view.timestep(), true));

    m_format->set_active_text(output.streamformat());
    m_location->set_text(output.location());
    m_directory->set_sensitive(output.format() == vpz::Output::LOCAL);

    std::string pluginname;

    PluginFactory::buildPluginName(output.package(), output.plugin(),
                                   &pluginname);

    m_plugin->set_active_text(pluginname);

    {
        utils::ModuleList lst;
        utils::ModuleList::iterator it;

        m_GVLE->pluginFactory().getGvleOutputPlugins(&lst);

        for (it = lst.begin(); it != lst.end(); ++it) {
            if (it->package == output.package() and
                it->library == output.plugin()) {
                break;
            }
        }

        if (it != lst.end()) {
            m_editplugin->set_sensitive(true);
        } else {
            m_editplugin->set_sensitive(false);
        }

        if (output.data()) {
            m_data->get_buffer()->set_text(output.data()->writeToXml());
        } else {
            m_data->get_buffer()->set_text("");
        }
    }
}

std::string ViewOutputBox::buildOutputName(const std::string& name) const
{
    std::string result(name);
    long nb = 0;

    while (m_viewscopy.outputs().exist(result)) {
        result.assign(boost::str(fmt("%1%%2%") % name % nb));
        ++nb;
    }

    return result;
}

}} // namespace vle gvle
