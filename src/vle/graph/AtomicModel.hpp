/**
 * @file vle/graph/AtomicModel.hpp
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


#ifndef VLE_GRAPH_ATOMIC_MODEL_HPP
#define VLE_GRAPH_ATOMIC_MODEL_HPP

#include <vle/graph/Model.hpp>
#include <vle/graph/DllDefines.hpp>

namespace vle { namespace graph {

    /**
     * @brief Represent the Atomic Model in DEVS formalism. This class just
     * represent the graph not the DEVS Simulator.
     */
    class VLE_GRAPH_EXPORT AtomicModel : public Model
    {
    public:
        /**
         * @brief Constructor to intialize parent, position (0,0), size (0,0)
         * and name.
         * @param name the new name of this atomic model.
         * @param parent the parent of this atomic model, can be null if parent
         * does not exist.
         */
        AtomicModel(const std::string& name, CoupledModel* parent);

        AtomicModel(const AtomicModel& mdl);

        AtomicModel& operator=(const AtomicModel& mdl);

        virtual graph::Model* clone() const
        { return new AtomicModel(*this); }

        /**
         * @brief Nothing to delete.
         */
	virtual ~AtomicModel() { }

        /**
         * @brief Return true, AtomicModel is an atomic model.
         * @return true.
         */
        virtual bool isAtomic() const { return true; }

        /**
         * @brief Return this if name is equal to the model's name. Recursive
         * function.
         * @param name The name of the model to find.
         * @return this if name is equal to the model's name, null otherwise.
         */
        virtual graph::Model* findModel(const std::string & name) const;

        /**
         * @brief Write the atomic model in the output stream.
         * @param out output stream.
         */
	virtual void writeXML(std::ostream& out) const;
    };

}} // namespace vle graph

#endif
