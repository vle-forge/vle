/** 
 * @file graph/ModelPort.hpp
 * @brief Represent the relation between a Model and an input or output port
 * specified by a constant string.
 * @author The vle Development Team
 * @date lun, 02 jui 2007 11:42:39 +0200
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

#ifndef VLE_GRAPH_MODELPORT_HPP
#define VLE_GRAPH_MODELPORT_HPP

#include <string>



namespace vle { namespace graph {

    class Model;

    /** 
     * @brief Represent the relation between a Model and an input or output port
     * specified by a constant string.
     */
    class ModelPort
    {
    public:
        ModelPort(Model* model, const std::string& portname) :
            model__(model),
            port__(portname)
        { }


        //
        /// Get funtions
        //

        inline Model* model() const
        { return model__; }

        inline const std::string& port() const
        { return port__; }

    private:
        ModelPort() :
            model__(0)
        { }

        Model*      model__;
        std::string port__;
    };

}} // namespace vle graph

#endif
