/**
 * @file vle/gvle/PluginTable.cpp
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


#include <vle/gvle/PluginTable.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/utils/Tools.hpp>
#include <gtkmm/enums.h>
#include <gtkmm/image.h>

using std::string;
using std::map;

namespace vle
{
namespace gvle {

PluginTable::PluginTable(GVLE* glve, const Gtk::RadioButtonGroup& group,
                         GVLE::MapCategory& category, GVLE::MapPlugin& plugins) :
        Gtk::Table(3, 1, false),
        m_gvle(glve),
        m_buttonGroup(group),
        m_category(category),
        m_plugins(plugins)
{
    makeButtonsPlugins();
}

PluginTable::~PluginTable()
{
    MapRadioButton::iterator it = m_radioButtons.begin();
    while (it != m_radioButtons.end()) {
        delete((*it).second);
        ++it;
    }
}

void PluginTable::showAllCategory()
{
    int i = 0;
    int j = 0;

    clear();
    MapRadioButton::iterator it = m_radioButtons.begin();
    while (it != m_radioButtons.end()) {
        attach(*((*it).second), j, j + 1, i, i + 1,
               Gtk::SHRINK, Gtk::SHRINK, 2, 2);
        if (j == 2) {
            j = 0;
            i++;
        } else {
            j++;
        }
        ++it;
    }
}

void PluginTable::showCategory(const string& name)
{
    int i = 0;
    int j = 0;

    clear();
    std::pair < GVLE::MapCategory::iterator,
    GVLE::MapCategory::iterator > finded;
    finded = m_category.equal_range(name);
    GVLE::MapCategory::iterator it = finded.first;
    while (it != finded.second) {
        attach(*m_radioButtons[(*it).second], j, j + 1, i, i + 1,
               Gtk::SHRINK, Gtk::SHRINK, 2, 2);
        if (j == 2) {
            j = 0;
            i++;
        } else {
            j++;
        }
        ++it;
    }
}

void PluginTable::makeButtonsPlugins()
{
    GVLE::MapPlugin::iterator it = m_plugins.begin();

    while (it != m_plugins.end()) {
        try {
            Gtk::RadioButton* button = new Gtk::RadioButton(m_buttonGroup);
            Gtk::Image* img = new Gtk::Image((*it).second->getIcon());
            button->set_relief(Gtk::RELIEF_NONE);
            button->set_mode(false);
            button->add(*img);
            button->add_events(Gdk::BUTTON_RELEASE_MASK);
            button->signal_clicked().connect(
                sigc::bind<Gtk::RadioButton*>(sigc::mem_fun(*this,
                                              &PluginTable::on_pluginbutton_clicked), button));

            m_radioButtons[(*it).second->getFormalismName()] = button;
        } catch (const std::exception& e) {
            gvle::Error(
                (boost::format(
                     "An error occured during plugin make button.\n%1%") %
                 e.what()).str());
        }
        ++it;
    }
}

void PluginTable::clear()
{
    Gtk::Table_Helpers::TableList& lst = children();
    while (lst.empty() == false) {
        Gtk::Table_Helpers::TableList::iterator it = lst.begin();
        Gtk::Widget* w = (*it).get_widget();
        if (w) {
            w->reference();
            remove(*w);
            w->unreference();
        }
    }
}

void PluginTable::on_pluginbutton_clicked(Gtk::RadioButton* widget)
{
    if (widget->get_active()) {
        MapRadioButton::const_iterator it = m_radioButtons.begin();
        while (it != m_radioButtons.end()) {
            if ((*it).second == widget) {
                //m_gvle->onPluginButton((*it).first);
            }
            ++it;
        }
    }
}

}
} // namespace vle gvle
