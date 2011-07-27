/*
 * @file vle/devs/test/DevsTransform.cpp
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


#include <vle/devs/Dynamics.hpp>

namespace vle { namespace unittest {

/**
 * @brief A simple unit test model.
 */
class Transform : public devs::Dynamics
{
public:
    Transform(const devs::DynamicsInit& atom,
              const devs::InitEventList& events) :
        devs::Dynamics(atom, events)
    {}

    virtual ~Transform()
    {}

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

    virtual value::Value* observation(const devs::ObservationEvent&) const
    {
        return value::Integer::create(m_counter);
    }

private:
    int m_counter;
};

}}

DECLARE_DYNAMICS(vle::unittest::Transform)
