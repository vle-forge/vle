/** 
 * @file Plugin.hpp
 * @brief 
 * @author The vle Development Team
 * @date 2007-07-15
 */

/*
 * Copyright (C) 2007 - The vle Development Team
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef VLE_VPZ_PLUGIN_HPP
#define VLE_VPZ_PLUGIN_HPP

#include <vle/vpz/Trame.hpp>
#include <vle/vpz/ParameterTrame.hpp>
#include <vle/vpz/NewObservableTrame.hpp>
#include <vle/vpz/DelObservableTrame.hpp>
#include <vle/vpz/ValueTrame.hpp>



namespace vle { namespace oov {

    /** 
     * @brief The vle::vpz::Plugin is a class to develop output steam plugins.
     * For instance, an output stream text (for gnuplot or R) and cairo (for
     * building vectorial image or bitmap). This class define a plugin.
     * @code
     * class Gnuplot : public Plugin
     * {
     *   // virtual functions. 
     *   //
     * };
     *
     * DECLARE_vpz_PLUGIN(Gnuplot);
     * @endcode
     */
    class Plugin
    {
    public:
        Plugin(const std::string& location) :
            m_location(location)
        { }

        virtual ~Plugin()
        { }

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
         */
        virtual void close() = 0;

    private:
        std::string         m_location;
    };


#define DECLARE_vpz_PLUGIN(x) \
    extern "C" { vle::vpz::Plugin* makeNewvpzPlugin() \
        { return new x(); } }

}} // namespace vle oov

#endif
