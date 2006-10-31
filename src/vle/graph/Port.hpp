/**
 * @file Port.hpp
 * @author The VLE Development Team.
 * @brief Represent a DEVS port for DEVS model. This class is member of the
 * DEVS graph.
 */

/*
 * Copyright (C) 2004, 05, 06 - The vle Development Team
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

#ifndef VLE_GRAPH_PORT_HPP
#define VLE_GRAPH_PORT_HPP

#include <string>

namespace vle { namespace value {

    class Value;

}} // namespace vle value

namespace vle { namespace graph {

    class Model;

    /**
     * @brief Represent a DEVS port for DEVS model. This class is member of the
     * DEVS graph.
     *
     */
    class Port
    {
    public:
        /**
         * Create a port with a string for name and a parent model.
         *
         * @param model a ptr to parent model
         * @param name a name for this port
         */
        Port(Model* model, const std::string& name);

        /**
         * Nothing to delete
	 *
         */
        ~Port()
        { }

        /**
         * get ptr to parent model
         *
         * @return ptr to parent model
         */
        inline const Model* getModel() const
        { return m_model; }

        /**
         * return parent name of port
         *
         * @return name of parent model
         */
        const std::string& getModelName() const;

        /**
         * get port name
         *
         * @return name of port
         */
	inline const std::string& getName() const
        { return m_name; }

        /** 
         * @brief get structure of value
         * 
         * @return the structure affected, can be null.
         */
        const vle::value::Value* getStructure() const
        { return m_structure; }

        /** 
         * @brief Delete previous structure.
         * 
         * @param val the value to clone affected, can be null.
         */
        void setStructure(const vle::value::Value* val);

        /** 
         * @brief Valid the value parameter with the value structure.
         * 
         * @param val the value to test.
         *
         * @return true if it's valid, otherwise false.
         */
        bool isValidWithStructure(const vle::value::Value* val);

    private:
        vle::graph::Model*      m_model;
        std::string             m_name;
        vle::value::Value*      m_structure;
    };

}} // namespace vle graph

#endif
