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


#ifndef VLE_GVLE_MODELINGPLUGIN_HPP
#define VLE_GVLE_MODELINGPLUGIN_HPP

#include <vle/gvle/DllDefines.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/version.hpp>
#include <gdkmm/pixbuf.h>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#define DECLARE_GVLE_MODELINGPLUGIN(x)                              \
    extern "C" {                                                    \
        GVLE_MODULE vle::gvle::ModelingPlugin*                      \
        vle_make_new_gvle_modeling(const std::string& package,      \
                                   const std::string& library,      \
                                   const std::string& currPackage)  \
        {                                                           \
            return new x(package, library, currPackage);            \
        }                                                           \
                                                                    \
        GVLE_MODULE void                                            \
        vle_api_level(vle::uint32_t* major,                         \
                      vle::uint32_t* minor,                         \
                      vle::uint32_t* patch)                         \
        {                                                           \
            *major = VLE_MAJOR_VERSION;                             \
            *minor = VLE_MINOR_VERSION;                             \
            *patch = VLE_PATCH_VERSION;                             \
        }                                                           \
    }

namespace vle { namespace gvle {

/**
 * @brief Define a plug-in to build source code, DEVS atomic model and its
 * experiment infortion (dynamics, conditions, observations).
 */
class GVLE_API ModelingPlugin
{
public:
    /**
     * @brief Defines a buffer for C++ source code.
     */
    typedef std::string Source;

    /**
     * @brief Build a new ModelingPlugin.
     *
     * @param package The name of the package containing the plugin
     * @param library The name of the plug-in.
     * @param curr_package the name of the current package
     *
     */
    ModelingPlugin(const std::string& package, const std::string& library,
            const std::string& curr_package)
        : mPackage(package), mLibrary(library), mCurrPackage(curr_package)
    {
    }

    virtual ~ModelingPlugin()
    {}

    /**
     * @brief Call to produce a new atomic model, with input and output ports,
     * with its vpz::Dynamic, vpz::Conditions and vpz::Observables.
     *
     * @param atom A reference to the vpz::AtomicModel to modify its input or
     * output ports.
     * conditions and observables.
     * @param dynamic A reference to the current vpz::Dynamic.
     * @param conditions A reference to the list of vpz::Conditions.
     * @param observables A reference ot the list of vpz::Observables.
     * @param classname The name of the class choosed by the user.
     * @param classname The namespace choosed by the user.
     *
     * @return true if all is valid, false otherwise.
     */
    virtual bool create(vpz::AtomicModel& atom,
                        vpz::Dynamic& dynamic,
                        vpz::Conditions& conditions,
                        vpz::Observables& observables,
                        const std::string& classname,
                        const std::string& namespace_) = 0;

    /**
     * @brief Call to rewrite an atomic model, with input and output ports,
     * with its vpz::Dynamic, vpz::Conditions and vpz::Observables.
     *
     * @param atom A reference to the vpz::AtomicModel to modify its input or
     * output ports.
     * conditions and observables.
     * @param dynamic A reference to the current vpz::Dynamic.
     * @param conditions A reference to the list of vpz::Conditions.
     * @param observables A reference ot the list of vpz::Observables.
     * @param conf The last configuration of the plug-in.
     * @param buffer The entire file.
     *
     * @return true if all is valid, false otherwise.
     */
    virtual bool modify(vpz::AtomicModel& atom,
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

    const std::string& getPackage() const { return mPackage; }

    const std::string& getLibrary() const { return mLibrary; }

    const std::string& getCurrPackage() const { return mCurrPackage; }

protected:
    std::string mPackage;
    std::string mLibrary;
    std::string mCurrPackage;

    Source mSource;

private:
    /**
     * @brief Pointer to the icon.
     */
    mutable Glib::RefPtr < Gdk::Pixbuf > m_icon;
};

typedef boost::shared_ptr < ModelingPlugin > ModelingPluginPtr;
typedef ModelingPlugin* (*GvleModelingPluginSlot)(const std::string&,
                                                  const std::string&,
                                                  const std::string&);

}} // namespace vle gvle

#endif

