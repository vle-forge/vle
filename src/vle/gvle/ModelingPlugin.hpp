/**
 * @file vle/gvle/ModelingPlugin.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
 * Copyright (C) 2003-2010 ULCO http://www.univ-littoral.fr
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


#ifndef VLE_GVLE_MODELINGPLUGIN_HPP
#define VLE_GVLE_MODELINGPLUGIN_HPP

#include <vle/gvle/DllDefines.hpp>
#include <gdkmm/pixbuf.h>
#include <vle/graph/AtomicModel.hpp>
#include <vle/vpz/AtomicModels.hpp>
#include <vle/vpz/Vpz.hpp>
#include <string>
#include <vector>

namespace vle { namespace gvle {

/**
 * @brief Define a plug-in to build source code, DEVS atomic model and its
 * experiment infortion (dynamics, conditions, observations).
 */
class VLE_GVLE_EXPORT ModelingPlugin
{
public:
    /**
     * @brief Defines a buffer for C++ source code.
     */
    typedef std::string Source;

    /**
     * @brief Build a new ModelingPlugin.
     * @param name The name of the plug-in.
     */
    ModelingPlugin(const std::string& name)
        : mName(name)
    {}

    virtual ~ModelingPlugin()
    {}

    /**
     * @brief Call to produce a new atomic model, with input and output ports,
     * with its vpz::Dynamic, vpz::Conditions and vpz::Observables.
     *
     * @param atom A reference to the graph::AtomicModel to modify its input or
     * output ports.
     * @param model A reference to the vpz::AtomicModel to modify its dynamic,
     * conditions and observables.
     * @param dynamic A reference to the current vpz::Dynamic.
     * @param conditions A reference to the list of vpz::Conditions.
     * @param observables A reference ot the list of vpz::Observables.
     * @param classname The name of the class choosed by the user.
     * @param classname The namespace choosed by the user.
     *
     * @return true if all is valid, false otherwise.
     */
    virtual bool create(graph::AtomicModel& atom,
                        vpz::AtomicModel& model,
                        vpz::Dynamic& dynamic,
                        vpz::Conditions& conditions,
                        vpz::Observables& observables,
                        const std::string& classname,
                        const std::string& namespace_) = 0;

    /**
     * @brief Call to rewrite an atomic model, with input and output ports,
     * with its vpz::Dynamic, vpz::Conditions and vpz::Observables.
     *
     * @param atom A reference to the graph::AtomicModel to modify its input or
     * output ports.
     * @param model A reference to the vpz::AtomicModel to modify its dynamic,
     * conditions and observables.
     * @param dynamic A reference to the current vpz::Dynamic.
     * @param conditions A reference to the list of vpz::Conditions.
     * @param observables A reference ot the list of vpz::Observables.
     * @param conf The last configuration of the plug-in.
     * @param buffer The entire file.
     *
     * @return true if all is valid, false otherwise.
     */
    virtual bool modify(graph::AtomicModel& atom,
                        vpz::AtomicModel& model,
                        vpz::Dynamic& dynamic,
                        vpz::Conditions& conditions,
                        vpz::Observables& observables,
                        const std::string& conf,
                        const std::string& buffer) = 0;

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
     * @brief Get the C++ source code to write into the dynamic library.
     * @return A constant reference to the source code.
     */
    const Source& source() const { return mSource; }

    /**
     * @brief Get a reference to the icon of this plug-in. If the plug-in have
     * not icon, or on error, the icon function return an empty pointer.
     * @return A reference to the icon or an empty pointer.
     */
    Glib::RefPtr < Gdk::Pixbuf > icon() const;

protected:
    std::string mName;
    Source mSource;

private:
    /**
     * @brief Pointer to the icon.
     */
    mutable Glib::RefPtr < Gdk::Pixbuf > m_icon;
};

}} // namespace vle gvle

#define DECLARE_GVLE_MODELINGPLUGIN(x) \
    extern "C" { \
        vle::gvle::ModelingPlugin* makeNewModelingPlugin( \
            const std::string& name) \
        { return new x(name); } \
    }

#endif

