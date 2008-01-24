/** 
 * @file unittest.cpp
 * @brief 
 * @author The vle Development Team
 * @date 2007-11-15
 */

/*
 * Copyright (C) 2007 - The vle Development Team
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
            return devs::Time::infinity;
        }

        virtual void internalTransition(const devs::Time& /* time */)
        {
            m_counter = 0;
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

    private:
        int m_counter;
    };

    DECLARE_NAMED_DYNAMICS(transform, vle::unittest::Transform);
}}
