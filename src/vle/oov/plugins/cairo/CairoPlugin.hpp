/** 
 * @file CairoPlugin.hpp
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

#ifndef VLE_OOV_CAIRO_PLUGIN_HPP
#define VLE_OOV_CAIRO_PLUGIN_HPP

#include <vle/oov/Plugin.hpp>
#include <cairomm/cairomm.h>


namespace vle { namespace oov {

    /** 
     * @brief The vle::oov::CairoPlugin is a class to develop cairo output steam
     * plugins. Do not forget the plugin declaration:
     * @code
     * DECLARE_OOV_PLUGIN(Gnuplot);
     * @endcode
     */
    class CairoPlugin : public Plugin
    {
    public:
        /** 
         * @brief Default constructor of the CairoPlugin.
         * @param location this string represents the name of the default
         * directory for a devs::LocalStreamWriter or a host:port:directory for
         * a devs::DistantStreamWriter.
         */
        CairoPlugin(const std::string& location) :
            Plugin(location)
        { }

        virtual ~CairoPlugin()
        { }

        /** 
         * @brief Return true, CairoPlugin is a Cairo plugin.
         * @return true.
         */
        virtual bool isCairo() const
        { return true; }

        /** 
         * @brief build a new context from the specified surface to the
         * CairoPlugin.
         * @param the surface to use with the context.
         */
        inline void setContext(Cairo::RefPtr < Cairo::Surface > surf)
        { m_ctx = Cairo::Context::create(surf); }

        /** 
         * @brief Retrieve the current context of the CairoPlugin.
         * @return the current context.
         */
        inline Cairo::RefPtr < Cairo::Context > context() const
        { return m_ctx; }

    private:
        Cairo::RefPtr < Cairo::Context > m_ctx;
    };

}} // namespace vle oov

#endif
