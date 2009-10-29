/**
 * @file vle/graph/ModelPort.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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


#ifndef VLE_GRAPH_MODELPORT_HPP
#define VLE_GRAPH_MODELPORT_HPP

#include <vle/graph/DllDefines.hpp>
#include <string>

namespace vle { namespace graph {

    class Model;

    /**
     * @brief ModelPort represents the relation between a Model and an input or
     * output port specified by a constant string.
     */
    class VLE_GRAPH_EXPORT ModelPort
    {
    public:
        /**
         * @brief Build a ModelPort with the specific Model and portname.
         * @param model the model name to connect.
         * @param portname the input or output portname of this model.
         * @throw utils::DevsGraphError if model is null or portname is empty or
         * if the model does not have the portname.
         */
        ModelPort(Model* model, const std::string& portname);

        ////
        //// Get functions
        ////

        /**
         * @brief Get a reference to the Model.
         * @return Get a reference to the Model.
         */
        inline Model* model() const { return m_model; }

        /**
         * @brief Get a constant string to the Model portname.
         * @return Get a constant string ot the Model portname.
         */
        inline const std::string& port() const { return m_port; }

    private:
        /**
         * @brief No default constructor.
         */
        ModelPort() :
            m_model(0)
        { }

        Model*              m_model;
        std::string         m_port;
    };

}} // namespace vle graph

#endif
