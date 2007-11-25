/**
 * @file graph/NoVLEModel.hpp
 * @author The VLE Development Team.
 * @brief Define no-vle model ie. model that use a different notation.
 */

/*
 * Copyright (C) 2004, 05, 06, 07 - The vle Development Team
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

#ifndef VLE_GRAPH_NOVLEMODEL_HPP
#define VLE_GRAPH_NOVLEMODEL_HPP

#include <vle/graph/Model.hpp>



namespace vle { namespace graph {

    /**
     * @brief Represent the NoVLE Model of VLE framework. This class is use when
     * hierarchy uses of translator.
     *
     */
    class NoVLEModel : public Model
    {
    public:
        /** 
         * @brief Constructor to intialize parent, position (0,0), size (0,0)
         * and name.
         * @param name the new name of this novle model.
         * @param parent the parent of this novle  model, can be null if parent
         * does not exist.
         */
        NoVLEModel(const std::string& name, CoupledModel* parent);

        NoVLEModel(const NoVLEModel& mdl);

        NoVLEModel& operator=(const NoVLEModel& mdl);

        virtual Model* clone() const
        { return new NoVLEModel(*this); }

        virtual ~NoVLEModel() { }

        /** 
         * @brief Return true, NoVLE is a novle model.
         * @return true.
         */
        virtual bool isNoVLE() const { return true; }

        /** 
         * @brief Return this if name is equal to the model's name. Recursive
         * function.
         * @param name The name of the model to find.
         * @return this if name is equal to the model's name, null otherwise.
         */
	virtual Model* findModel(const std::string& name) const;

        /** 
         * @brief Write the novle model in the output stream.
         * @param out output stream.
         */
        virtual void writeXML(std::ostream& out) const;
    };

}} // namespace vle graph

#endif
