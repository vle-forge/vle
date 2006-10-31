/** 
 * @file Model.hpp
 * @brief 
 * @author The vle Development Team
 * @date jeu, 16 fév 2006 16:48:00 +0100
 */

/*
 * Copyright (C) 2006 - The vle Development Team
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

#ifndef VLE_VPZ_MODEL_HPP
#define VLE_VPZ_MODEL_HPP

#include <map>
#include <vle/vpz/Base.hpp>
#include <vle/graph/Model.hpp>

namespace vle { namespace vpz {

    class Model : public Base
    {
    public:
        Model();

        Model(const Model& model);

        Model& operator=(const Model& model);

        virtual ~Model();

        /** 
         * @brief Initialise the devs hierachy of model.
         * @code
         * <STRUCTURES>
         *  <MODEL NAME="toto">
         *  </MODEL>
         * </STRUCTURES>
         * @endcode
         * 
         * @param elt a reference to the STRUCTURES tag.
         *
         * @throw Exception::Internal if elt is null or not on a STRUCTURES tag.
         */
        virtual void init(xmlpp::Element* elt);

        /** 
         * @brief Write the devs hierachy of model.
         * 
         * @param elt a reference to the parent STRUCTURES tag.
         *
         * @throw Exception::Internal if elt is null.
         */
        virtual void write(xmlpp::Element* elt) const;

        /** 
         * @brief Initialise the devs hierachy of model.
         * @code
         * <MODEL NAME="toto">
         * </MODEL>
         * @endcode
         * 
         * @param elt a reference to the MODEL tag.
         *
         * @throw Exception::Internal if elt is null or not on a MODEL tag.
         */
        void initFromModel(xmlpp::Element* elt);

        /** 
         * @brief Just delete the complete list of graph::Model.
         */
        void clear();

        /** 
         * @brief Replace the model name modelname by the model provided in
         * parameter.
         * 
         * @param modelname model name to change. Must be a graph::NoVLE.
         * @param m the new model or hierarchy to push.
         */
        void addModel(const std::string& modelname, const Model& m);

        /** 
         * @brief Set a hierachy of graph::Model. If a previous hierarchy
         * already exist, it is delete same if the new is empty. This function
         * is just an affectation, no clone is build.
         * 
         * @param mdl the new graph::Model hierarchy to set.
         */
        void setModel(graph::Model* mdl);
        
        /** 
         * @brief Build a new graph::Model hierarchy an return it.
         * 
         * @return A new hierarchy, don't forget to delete it when finish.
         */
        graph::Model* model() const;

        /** 
         * @brief Get a reference to the graph::Model hierarchy.
         * 
         * @return A reference to the graph::Model, be carreful, you can damage
         * graph::Vpz instance.
         */
        graph::Model* modelRef();

        /** 
         * @brief Get a reference to the graph::Model hierarchy.
         * 
         * @return A reference to the graph::Model, be carreful, you can damage
         * graph::Vpz instance.
         */
        const graph::Model* modelRef() const;

    private:
        graph::Model*       m_graph;
    };

}} // namespace vle vpz

#endif
