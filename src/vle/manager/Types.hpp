/**
 * @file vle/manager/Types.hpp
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


#ifndef VLE_MANAGER_TYPE_HPP
#define VLE_MANAGER_TYPE_HPP

#include <boost/multi_array.hpp>
#include <vle/oov/Plugin.hpp>
#include <vle/oov/OutputMatrix.hpp>
#include <vle/utils/Rand.hpp>
#include <vector>
#include <list>

namespace vle { namespace manager  {

    /**
     * @brief A boost share_ptr around the vle::utils::Rand class.
     */
    typedef boost::shared_ptr < utils::Rand > RandPtr;

    /**
     * @brief Define a list of filename from command line arguments argv and
     * argc.
     */
    typedef std::vector < const char* > CmdArgs;

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
    struct OutputSimulationDistant
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

    //
    // Manager simulation types
    //

    /**
     * @brief Define a two dimensional array of OutputSimulationList to store
     * result of a ManagerRun.
     */
    typedef boost::multi_array < oov::OutputMatrixViewList, 2 > OutputSimulationMatrix;

}} // namespace vle manager

#endif
