/**
 * @file vle/extension.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
 * Copyright (C) 2003-2009 ULCO http://www.univ-littoral.fr
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
#include <vle/extension/Decision.hpp>
#include <vle/extension/DifferenceEquation.hpp>
#include <vle/extension/DifferentialEquation.hpp>
#include <vle/extension/DSDevs.hpp>
#include <vle/extension/PetriNet.hpp>

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
