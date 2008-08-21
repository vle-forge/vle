/**
 * @file vle/devs/test/models.cpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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


#include <vle/devs/Dynamics.hpp>

namespace vle { namespace unittest {

    /** 
     * @brief A simple unit test model.
     */
    class Transform : public devs::Dynamics
    {
    public:
        Transform(const graph::AtomicModel& atom,
                  const devs::InitEventList& events) :
            devs::Dynamics(atom, events)
        { }

        virtual ~Transform()
        { }

        virtual devs::Time init(const devs::Time& /* time */)
        {
            m_counter = 0;
            return 0.0;
        }
        
        virtual devs::Time timeAdvance() const
        {
            return 1.0;
        }

        virtual void internalTransition(const devs::Time& /* time */)
        {
            m_counter = 1;
        }

        virtual void externalTransition(
                        const devs::ExternalEventList& events,
                        const devs::Time& /* time */)
        {
            m_counter = m_counter + events.size();
        }

        virtual void output(const devs::Time& /* time */,
                            devs::ExternalEventList& output) const
        {
            for (int i = 0; i < m_counter; ++i) {
                output.addEvent(buildEvent("out"));
            }
        }

        virtual value::Value observation(const devs::ObservationEvent&) const
        {
            return value::IntegerFactory::create(m_counter);
        }

    private:
        int m_counter;
    };

    DECLARE_NAMED_DYNAMICS(transform, vle::unittest::Transform)
}}
