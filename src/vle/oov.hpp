/*
 * @file vle/oov.hpp
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


#ifndef VLE_OOV_OOV_HPP
#define VLE_OOV_OOV_HPP

#include <vle/oov/LocalStreamReader.hpp>
#include <vle/oov/NetStreamReader.hpp>
#include <vle/oov/OOV.hpp>
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
