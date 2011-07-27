/*
 * @file examples/petrinet/PetrinetCounter.cpp
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
#include <vle/devs.hpp>

namespace vle { namespace examples { namespace petrinet {

    /**
     * @brief A DEVS counter which store the date of the latest event.
     */
    class PetrinetCounter : public devs::Dynamics
    {
    public:
        PetrinetCounter(const devs::DynamicsInit& model,
                        const devs::InitEventList& events) :
            devs::Dynamics(model, events),
            mDate(0),
            mNumber(0)
        { }

        virtual ~PetrinetCounter()
        { }

        virtual void externalTransition(const devs::ExternalEventList& evts,
                                        const devs::Time& time)
        { mDate = time; mNumber += evts.size(); }

        virtual value::Value* observation(const devs::ObservationEvent& e) const
        { return e.onPort("date") ?
            value::Double::create(mDate.getValue()) :
                value::Double::create(mNumber); }

    private:
            devs::Time mDate;
            int mNumber;
    };

}}} // namespace vle examples petrinet

DECLARE_DYNAMICS(vle::examples::petrinet::PetrinetCounter)
