/** 
 * @file graph/ModelPortList.hpp
 * @brief Represent a list of Model and input or output ports.
 * @author The vle Development Team
 * @date lun, 02 jui 2007 11:48:52 +0200
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

#ifndef VLE_GRAPH_MODELPORTLIST_HPP
#define VLE_GRAPH_MODELPORTLIST_HPP

#include <vle/graph/ModelPort.hpp>
#include <vector>



namespace vle { namespace graph {

    class ModelPortList : public std::vector < ModelPort* >
    {
    public:
        ModelPortList()
        { }

        virtual ~ModelPortList();

        /** 
         * @brief Add a new graph::ModelPort to the vector. No check is
         * performed is a connection already exist.
         * 
         * @param model The model to add.
         * @param portname The port of the model to add.
         */
        void add(Model* model, const std::string& portname);

        /** 
         * @brief Remove a graph::ModelPort from the vector. Linear complexity.
         * 
         * @param model The model to delete.
         * @param portname The port of the model to delete.
         */
        void remove(Model* model, const std::string& portname);

        /** 
         * @brief Merge the vector from graph::ModelPort vector. Linera
         * complexity.
         * 
         * @param lst The vector of graph::ModelPort to merger.
         */
        void merge(ModelPortList& lst);

        /** 
         * @brief Check if a graph::ModelPort already exist in the vector.
         * Linear complexity.
         * 
         * @param model The model to check.
         * @param portname The port of the model to check.
         * 
         * @return True if the (model, portname) exist in vector, false
         * otherwise.
         */
        bool exist(Model* model, const std::string& portname) const;

        /** 
         * @brief Check if a graph::ModelPort already exist in the vector.
         * Linear complexity.
         * 
         * @param model The model to check.
         * @param portname The port of the model to check.
         * 
         * @return True if the (model, portname) exist in vector, false
         * otherwise.
         */
        bool exist(const Model* model, const std::string& portname) const;
    };

}} // namespace vle graph

#endif
