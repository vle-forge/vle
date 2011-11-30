/*
 * @file vle/manager/Types.hpp
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


#ifndef VLE_MANAGER_TYPE_HPP
#define VLE_MANAGER_TYPE_HPP

#include <vle/manager/DllDefines.hpp>
#include <vle/oov/Plugin.hpp>
#include <vle/oov/OutputMatrix.hpp>
#include <vector>
#include <list>

namespace vle { namespace manager  {

    /**
     * @brief Define a list of filename from command line arguments argv and
     * argc.
     */
    typedef std::list < std::string > CmdArgs;

    //
    // JustRun output types

    /**
     * @brief Define a list of oov::OutputMatrixViewList.
     */
    typedef std::vector < oov::OutputMatrixViewList > OutputSimulationList;

    /**
     * @brief Define a dictionary of key vpz filename and index. Index is the
     * index in the OutputSimulationList.
     */
    typedef std::map < std::string,
            oov::OutputMatrixViewList::size_type > OutputSimulationNames;


    //
    // Distant simulation types
    //

    /**
     * @brief
     */
    struct VLE_MANAGER_EXPORT OutputSimulationDistant
    {
        OutputSimulationDistant(int instance, int replica,
                                const oov::OutputMatrixViewList& outputs) :
            instance(instance),
            replica(replica),
            outputs(outputs)
        {
        }

        int instance;
        int replica;
        oov::OutputMatrixViewList outputs;
    };

    /**
     * @brief
     */
    typedef std::vector < OutputSimulationDistant > OutputSimulationDistantList;

}} // namespace vle manager

#endif
