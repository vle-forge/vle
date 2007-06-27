/**
 * @file graph/AtomicModel.hpp
 * @author The VLE Development Team.
 * @brief Represent the Atomic Model in DEVS formalism. This class just
 * represent the graph not the DEVS Simulator.
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

#ifndef VLE_GRAPH_ATOMIC_MODEL_HPP
#define VLE_GRAPH_ATOMIC_MODEL_HPP

#include <vle/graph/Model.hpp>



namespace vle { namespace graph {

    class CoupledModel;

    /**
     * @brief Represent the Atomic Model in DEVS formalism. This class just
     * represent the graph not the DEVS Simulator.
     *
     *
     */
    class AtomicModel : public Model
    {
    public:
	AtomicModel(CoupledModel* parent);

	AtomicModel(const AtomicModel& model);
        
        AtomicModel(const std::string& name, CoupledModel* parent);

	virtual ~AtomicModel();

	virtual Model* clone() const;

	virtual void writeXML(std::ostream& out) const;

        virtual bool isAtomic() const
        { return true; }

        virtual bool isCoupled() const
        { return false; }

        virtual bool isNoVLE() const
        { return false; }

	virtual Model* findModel(const std::string & name) const;
    };

}} // namespace vle graph

#endif
