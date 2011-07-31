/*
 * @file vle/oov/Plugin.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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


#ifndef VLE_OOV_PLUGIN_HPP
#define VLE_OOV_PLUGIN_HPP

#include <vle/oov/DllDefines.hpp>
#include <vle/oov/OutputMatrix.hpp>
#include <vle/value/Set.hpp>
#include <vle/version.hpp>

#define DECLARE_OOV_PLUGIN(x)                         \
    extern "C" {                                      \
        VLE_OOV_EXPORT vle::oov::Plugin*              \
        vle_make_new_oov(const std::string& location) \
        {                                             \
            return new x(location);                   \
        }                                             \
                                                      \
        VLE_OOV_EXPORT void                           \
        vle_api_level(vle::uint32_t* major,           \
                      vle::uint32_t* minor,           \
                      vle::uint32_t* patch)           \
        {                                             \
            *major = VLE_MAJOR_VERSION;               \
            *minor = VLE_MINOR_VERSION;               \
            *patch = VLE_PATCH_VERSION;               \
        }                                             \
    }

namespace vle { namespace oov {

    /**
     * @brief The vle::oov::Plugin is a class to develop output steam plugins.
     * For instance, an output stream text (for gnuplot or R) and cairo (for
     * building vectorial image or bitmap). This class define a plugin.
     * @code
     * class Gnuplot : public Plugin
     * {
     *   //
     *   // virtual functions.
     *   //
     * };
     *
     * DECLARE_OOV_PLUGIN(Gnuplot);
     * @endcode
     */
    class VLE_OOV_EXPORT Plugin
    {
    public:
        /**
         * @brief Default constructor of the Plugin.
         * @param location this string represents the name of the default
         * directory for a devs::LocalStreamWriter or a host:port:directory for
         * a devs::DistantStreamWriter.
         */
        Plugin(const std::string& location) :
            m_location(location)
        {}

        /**
         * @brief Nothing to delete.
         */
        virtual ~Plugin()
        {}

        ///
        ////
        ///

        /**
         * @brief This plugin can be serialized into a value::Value stream.
         * @return true if this plugin is serializable, false otherwise.
         */
        virtual bool isSerializable() const
        { return false; }

        /**
         * @brief Get a value::Value stream.
         * @return A value::Value.
         */
        virtual value::Value* serialize() const
        { return 0; }

        /**
         * @brief Initialize this plugin using the value::Value stream.
         * @param A value::Value.
         */
        virtual void deserialize(const value::Value& /* vals */)
        {}

        /**
         * @brief This plugin use an OutputMatrix.
         * @return true if this plugin use and OutputMatrix, false otherwise.
         */
        virtual bool haveOutputMatrix() const
        { return false; }

        /**
         * @brief Get a reference to the OutputMatrix. If the Plugin do not use
         * and OutputMatrix, an empty OutputMatrix is returned.
         * @return A reference to the OutputMatrix.
         */
        virtual oov::OutputMatrix outputMatrix() const
        { return OutputMatrix(); }

        /**
         * @brief Get the name of the Plugin class.
         * @return The name of the plugin class.
         */
        virtual std::string name() const
        { return std::string(); }

        ///
        ////
        ///

        /**
         * @brief By default, a plugin is not a cairo plugin.
         * @return false.
         */
        virtual bool isCairo() const
        { return false; }

        /**
         * @brief Call to initialise plugin. Just before the Plugin constructor.
         * This function is used to initialise the Plugin with parameter
         * provided by the devs::StreamWriter.
         */
        virtual void onParameter(const std::string& plugin,
                                 const std::string& location,
                                 const std::string& file,
                                 value::Value* parameters,
                                 const double& time) = 0;

        /**
         * @brief Call when a new observable (the devs::Simulator and port name)
         * is attached to a view.
         */
        virtual void onNewObservable(const std::string& simulator,
                                     const std::string& parent,
                                     const std::string& port,
                                     const std::string& view,
                                     const double& time) = 0;

        /**
         * @brief Call whe a observable (the devs::Simulator and port name) is
         * deleted from a view.
         */
        virtual void onDelObservable(const std::string& simulator,
                                     const std::string& parent,
                                     const std::string& port,
                                     const std::string& view,
                                     const double& time) = 0;

        /**
         * @brief Call when an external event is send to the view.
         */
        virtual void onValue(const std::string& simulator,
                             const std::string& parent,
                             const std::string& port,
                             const std::string& view,
                             const double& time,
                             value::Value* value) = 0;

        /**
         * @brief Call when the simulation is finished.
         */
        virtual void close(const double& time) = 0;

        ///
        ////
        ///

        /**
         * @brief Get the location provide at the constructor of this Plugin.
         * @return A constant string to the location provided a the Plugin
         * constructor. This string represent a directory for a
         * devs::LocalStreamWriter or a host:port:directory for a
         * devs::DistantStreamWriter.
         */
        inline const std::string& location() const
        { return m_location; }

    private:
        std::string         m_location;
    };

    /**
     * @brief This typedef is used by the StreamReader build factory to
     * automatically destroy plugin at the end of the simulation.
     */
    typedef boost::shared_ptr < Plugin > PluginPtr;

    /**
     * @brief Define the pointer to function of the oov::Plugin plug-in.
     * @param The output location of the oov::Plugin.
     * @return A pointer to the oov::Plugin newly build.
     */
    typedef Plugin* (*OovPluginSlot)(const std::string&);

    /**
     * @brief This typedef is used to defined the dictionnary of key view name
     * and oov::PluginPtr.
     */
    typedef std::map < std::string, PluginPtr > PluginViewList;

}} // namespace vle oov

#endif
