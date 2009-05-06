/**
 * @file vle/gvle/ConditionPlugin.hpp
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

#ifndef VLE_GVLE_CONDITIONPLUGIN_HPP
#define VLE_GVLE_CONDITIONPLUGIN_HPP

#include <gdkmm/pixbuf.h>
#include <vle/vpz/Condition.hpp>

namespace vle { namespace gvle {

/**
 * @brief A virtual class to define Condition plug-ins.
 * @code
 * class TmpSelector : public vle::gvle::ConditionPlugin
 * {
 * public:
 *   virtual void start()
 *   {
 *      m_cnd.addValueToPort("x", new vle::value::Double(1234.4321));
 *      m_cnd.addValueToPort("y", new vle::value::Double(4321.1234));
 *   }
 * };
 * @endcode
 */
class ConditionPlugin
{
public:
    ConditionPlugin(const std::string& name);

    virtual ~ConditionPlugin() {}

    /**
     * @brief Call to start the graphics interfaces. Use it to show the
     * Gtk::Dialog for instance. This function is called each time user selects
     * the plug-in.
     * @param condition The condition to modify.
     * @return true if successful, otherwise false
     */
    virtual bool start(vpz::Condition& condition) = 0;

    /**
     * @brief Call to start the graphic interface. Use it to show the
     * Gtk::Dialog for instance. This function is called each time user select
     * the plug-in from the parameters treeview.
     * @param condition The condition to modify.
     * @param parameters The selected parameters to modify.
     * @return true if successful, otherwise false
     */
    virtual bool start(vpz::Condition& condition,
                       const std::string& parameters) = 0;

    /**
     * @brief Get a reference to the icon of this plug-in. If the plug-in have
     * not icon, or on error, the icon function return an empty pointer.
     * @return A reference to the icon or an empty pointer.
     */
    Glib::RefPtr < Gdk::Pixbuf > icon() const;

private:
    /**
     * @brief Pointer to the icon.
     */
    mutable Glib::RefPtr < Gdk::Pixbuf > m_icon;

    /**
     * @brief Name of the plug-in taken from library name.
     */
    std::string m_name;
};

}} // namespace vle gvle

#define DECLARE_GVLE_CONDITIONPLUGIN(x) \
    extern "C" { \
        vle::gvle::ConditionPlugin* makeNewConditionPlugin( \
            const std::string& name) \
        { return new x(name); } \
    }

#endif

