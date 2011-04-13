/*
 * @file examples/petrinet/Simple.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
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
     * @brief Build an Petri Net model which build an output when it receives
     * mDayNumber of event.
     */
    class PetrinetTrigger : public extension::PetriNet
    {
    public:
        PetrinetTrigger(const devs::DynamicsInit& model,
                        const devs::InitEventList& events) :
            extension::PetriNet(model, events)
        {
            mDayNumber = value::toInteger(events.get("day"));
        }

        virtual ~PetrinetTrigger()
        { }

        virtual void build()
        {
            addPlace("P1");
            addPlace("P2");
            addPlace("P3");
            addPlace("P4");
            addPlace("P5");
            addOutputPlace("P6", "out");

            addInputTransition("T1", "yes");
            addInputTransition("T2", "no");
            addTransition("T3");
            addTransition("T4");
            addTransition("T5");
            addTransition("T6");
            addTransition("T7");
            addTransition("T8");

            addArc("T1", "P1");
            addArc("P1", "T3");
            addArc("P1", "T4");
            addArc("T3", "P3");
            addArc("P3", "T3");
            addArc("T4", "P3");
            addArc("P3", "T7");
            addArc("T7", "P3");
            addArc("P3", "T5");

            addArc("T2", "P2");
            addArc("P2", "T5");
            addArc("P2", "T6");
            addArc("T6", "P4");
            addArc("P4", "T6");
            addArc("P4", "T4");
            addArc("T5", "P4");

            addArc("T5", "P5");
            addArc("T6", "P5");
            addArc("P5", "T7");
            addArc("P5", "T8", mDayNumber);
            addArc("T8", "P6");

            addInitialMarking("P4", 1);
        }

    private:
        unsigned int mDayNumber;
    };

}}} // namespace vle examples petrinet

DECLARE_DYNAMICS(vle::examples::petrinet::PetrinetTrigger)
