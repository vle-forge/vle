/**
 * @file src/vle/oov/Plugin.hpp
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




#ifndef VLE_VPZ_PLUGIN_HPP
#define VLE_VPZ_PLUGIN_HPP

#include <vle/vpz/Trame.hpp>
#include <vle/vpz/ParameterTrame.hpp>
#include <vle/vpz/NewObservableTrame.hpp>
#include <vle/vpz/DelObservableTrame.hpp>
#include <vle/vpz/ValueTrame.hpp>
#include <vle/vpz/EndTrame.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>

namespace vle { namespace oov {

    /** 
     * @brief The vle::vpz::Plugin is a class to develop output steam plugins.
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
    class Plugin
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
        { }

        virtual ~Plugin()
        { }

        /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

        virtual bool isSerializable() const
        { return false; }

        virtual value::Value serialize() const
        { return value::Value(); }

        virtual void deserialize(value::Value& /* vals */)
        { }

        virtual std::string name() const
        { return std::string(); }

        /*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

        /** 
         * @brief By default, a plugin is not a cairo plugin.
         * @return false.
         */
        virtual bool isCairo() const
        { return false; }

        /** 
         * @brief Call to initialise plugin. Just before the Plugin constructor.
         * This function is used to initialise the Plugin with parameter
         * provided by the Vpz class.
         * 
         * @param trame
         */
        virtual void onParameter(const vpz::ParameterTrame& trame) = 0;

        /** 
         * @brief Call when a new observable (the devs::Simulator and port name)
         * is attached to a view.
         * 
         * @param trame 
         */
        virtual void onNewObservable(const vpz::NewObservableTrame& trame) = 0;

        /** 
         * @brief Call whe a observable (the devs::Simulator and port name) is
         * deleted from a view.
         * 
         * @param trame 
         */
        virtual void onDelObservable(const vpz::DelObservableTrame& trame) = 0;

        /** 
         * @brief Call when an external event is send to the view.
         * 
         * @param trame 
         */
        virtual void onValue(const vpz::ValueTrame& trame) = 0;

        /** 
         * @brief Call when the simulation is finished.
         *
         * @param trame
         */
        virtual void close(const vpz::EndTrame& trame) = 0;

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
     * @brief This typedef is used to defined the dictionnary of key view name
     * and oov::PluginPtr.
     */
    typedef std::map < std::string, PluginPtr > PluginViewList;
    
#define DECLARE_OOV_PLUGIN(x) \
    extern "C" { vle::oov::Plugin* makeNewOovPlugin(const std::string& location) \
        { return new x(location); } }

}} // namespace vle oov

#endif
