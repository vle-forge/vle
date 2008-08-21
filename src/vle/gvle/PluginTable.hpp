/**
 * @file vle/gvle/PluginTable.hpp
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


#ifndef GUI_PLUGINTABLE_HPP
#define GUI_PLUGINTABLE_HPP

#include <vle/gvle/GVLE.hpp>
#include <gtkmm/table.h>
#include <gtkmm/radiobutton.h>
#include <map>

namespace vle
{
namespace gvle {

/**
 * @brief This Gtk::Table is use to show dynamically all plugin load by the
 * GVLE application. These plugins are store into table.
 */
class PluginTable : public Gtk::Table
{
public:
    /** define a map plugin name and radiobutton */
    typedef std::map < std::string, Gtk::RadioButton* > MapRadioButton;

    /**
     * Create new PluginTable, make all buttons and show all buttons on
     * table
     *
     * @param glve a reference to GVLE interface
     * @param group RadioButtonGroup for new RadioButton create
     * @param category category list
     * @param plugins plugins list
     */
    PluginTable(GVLE* glve, const Gtk::RadioButtonGroup& group,
                GVLE::MapCategory& category, GVLE::MapPlugin& plugins);

    /**
     * Delete all allocated RadioButton
     *
     */
    virtual ~PluginTable();

    /**
     * Show all button for all category
     *
     */
    void showAllCategory();

    /**
     * Show all button from category name
     *
     * @param name category name to show
     */
    void showCategory(const std::string& name);

private:
    void makeButtonsPlugins();

    void clear();

    void on_pluginbutton_clicked(Gtk::RadioButton* widget);

    MapRadioButton          m_radioButtons;
    GVLE*                   m_gvle;
    Gtk::RadioButton::Group m_buttonGroup;
    GVLE::MapCategory&      m_category;
    GVLE::MapPlugin&        m_plugins;
};

}
} // namespace vle gvle

#endif
