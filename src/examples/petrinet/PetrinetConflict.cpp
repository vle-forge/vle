/*
 * @file examples/petrinet/PetrinetConflict.cpp
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


#include <vle/extension/PetriNet.hpp>

namespace vle { namespace examples { namespace petrinet {

    /**
     * @brief Build an Petri Net model with a conflict between
     * external event and send an event to output DEVS port.
     */
    class PetrinetConflict : public extension::PetriNet
    {
    public:
        PetrinetConflict(const devs::DynamicsInit& model,
                 const devs::InitEventList& events) :
            extension::PetriNet(model, events)
        {
        }

        virtual ~PetrinetConflict()
        { }

        virtual void build()
        {
            addPlace("P1");
            addOutputPlace("P2", "out");
            addInputTransition("T1", "in");
            addTransition("T2", 0.1);
            addTransition("T3");
            addArc("T1", "P1");
            addArc("P1", "T2");
            addArc("T2", "P2");
            addArc("P2", "T3");
        }
    };

}}} // namespace vle examples petrinet

DECLARE_DYNAMICS(vle::examples::petrinet::PetrinetConflict)
