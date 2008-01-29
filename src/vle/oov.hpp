/** 
 * @file oov.hpp
 * @brief 
 * @author The vle Development Team
 * @date 2007-08-13
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

#ifndef VLE_OOV_OOV_HPP
#define VLE_OOV_OOV_HPP

#include <vle/oov/LocalStreamReader.hpp>
#include <vle/oov/NetStreamReader.hpp>
#include <vle/oov/OOV.hpp>
#include <vle/oov/OovLocalStreamReader.hpp>
#include <vle/oov/OovNetStreamReader.hpp>
#include <vle/oov/PluginFactory.hpp>
#include <vle/oov/Plugin.hpp>
#include <vle/oov/SimpleFile.hpp>
#include <vle/oov/StreamReader.hpp>



namespace vle {

    /** 
     * @brief The OOV library of Output Of VLE, is used to build the outputs of
     * the VLE plateform. Currently, it provides local (synchron memory
     * transfert) or distant (synchorn memory transfert via socket). The class
     * Plugin allow developer to build new type of output specialisez for
     * Gnuplot, R, etc.
     */
    namespace oov {

    } // namespace oov

} // namespace vle

#endif
