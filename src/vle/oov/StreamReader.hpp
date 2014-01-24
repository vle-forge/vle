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


#ifndef VLE_OOV_STREAMREADER_HPP
#define VLE_OOV_STREAMREADER_HPP

#include <vle/DllDefines.hpp>
#include <vle/oov/Plugin.hpp>
#include <vle/utils/ModuleManager.hpp>
#include <string>

namespace vle { namespace oov {

    /**
     * @brief The StreamReader is a general class to read observation
     * information from the devs::StreamWriter classes add give this information
     * to the oov::Plugin.
     */
    class VLE_API StreamReader
    {
    public:
        /**
         * @brief Build a new StreamReader.
         */
	StreamReader(const utils::ModuleManager& modulemgr)
            : m_modulemgr(modulemgr)
        {}

        /**
         * @brief Nothing to delete.
         */
        virtual ~StreamReader()
        {}

        ///
        ////
        ///

        /**
         * @brief Call to initialise plug-in. Just before the Plugin
         * constructor. This function is used to initialise the Plugin with
         * parameter provided by the devs::StreamWritter class.
         */
        virtual void onParameter(const std::string& plugin,
                                 const std::string& package,
                                 const std::string& location,
                                 const std::string& file,
                                 value::Value* parameters,
                                 const double& time);

        /**
         * @brief Call when a new observable (the devs::Simulator and port name)
         * is attached to a view.
         */
        virtual void onNewObservable(const std::string& simulator,
                                     const std::string& parent,
                                     const std::string& portname,
                                     const std::string& view,
                                     const double& time);

        /**
         * @brief Call whe a observable (the devs::Simulator and port name) is
         * deleted from a view.
         */
        virtual void onDelObservable(const std::string& simulator,
                                     const std::string& parent,
                                     const std::string& portname,
                                     const std::string& view,
                                     const double& time);

        /**
         * @brief Call when an observation event is send to the view. The
         * plug-in must delete the attachted value. For instance:
         * @code
         * class Position : public Plugin
         * {
         *   ...
         *
         *   virtual void onValue(const std::string& simulator,
         *                      const std::string& parent,
         *                      const std::string& port,
         *                      const std::string& view,
         *                      const double& time,
         *                      value::Value* value)
         *   {
         *     if (port == "x") {
         *       m_position[(int)time].x() = value->toInteger().value();
         *     } else if (port == "y") {
         *       m_position[(int)time].y() = value->toInteger().value();
         *     }
         *     delete value;
         *   }
         *   ...
         * };
         * @endcode
         */
        virtual void onValue(const std::string& simulator,
                             const std::string& parent,
                             const std::string& port,
                             const std::string& view,
                             const double& time,
                             value::Value* value);

        /**
         * @brief Call when the simulation is finished.
         */
        virtual void onClose(const double& time);

        ///
        ////
        ///

        /**
         * @brief Get a reference to the Plugin attached to the StreamReader.
         * @return A reference to the Plugin.
         * @throw utils::InternalError if the plug-in is not initialized.
         */
        PluginPtr plugin() const;

        const utils::ModuleManager& getModuleManager() const
        { return m_modulemgr; }

    private:
        StreamReader(const StreamReader& other);
        StreamReader& operator=(const StreamReader& other);

        const utils::ModuleManager& m_modulemgr;
        PluginPtr m_plugin;

        /**
         * @brief Load the specified output plugin from the StreamDirs location.
         * @param plugin the name of the plugin.
         * @param location the location of the plugin output.
         * @throw utils::InternalError if the plugin was not found.
         */
        virtual void initPlugin(const std::string& plugin,
                                const std::string& package,
                                const std::string& location,
                                const utils::ModuleManager& modulemgr);

    };

}} // namespace vle oov

#endif
