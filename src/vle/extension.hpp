/**
 * @file vle/extension.hpp
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


#ifndef VLE_EXTENSION_EXTENSION_HPP
#define VLE_EXTENSION_EXTENSION_HPP

#include <vle/extension/CellDevs.hpp>
#include <vle/extension/CellQSS.hpp>
#include <vle/extension/CombinedQSS.hpp>
#include <vle/extension/DifferenceEquation.hpp>
#include <vle/extension/DSDevs.hpp>
#include <vle/extension/PetriNet.hpp>
#include <vle/extension/QSS2.hpp>
#include <vle/extension/QSS.hpp>



namespace vle {

    /** 
     * @brief The extension namespace represents the DEVS extensions provides
     * by the VLE framework. We provide CellDEVS (cellular automata), CellQSS
     * (cellular automata with QSS integration), QSS and QSS2 ordinary
     * differential equation and DSdevs, a executive model.
     */
    namespace extension {

    } // namespace extension

} // namespace vle

#endif
