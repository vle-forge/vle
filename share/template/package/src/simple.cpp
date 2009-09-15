/**
 * @file vle/share/template/package/src/simple.cpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2009 The VLE Development Team
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

namespace vle { namespace examples {

class Simple : public devs::Dynamics
{
public:
    Simple(
        const devs::DynamicsInit& init,
        const devs::InitEventList& events)
        : devs::Dynamics(init, events)
    {
    }

    virtual ~Simple()
    {
    }

    virtual devs::Time init(
        const devs::Time& /* time */)
    {
        return devs::Time::infinity;
    }

    virtual void output(
        const devs::Time& /* time */,
        devs::ExternalEventList& /* output */) const
    {
    }

    virtual devs::Time timeAdvance() const
    {
        return devs::Time::infinity;
    }

    virtual void internalTransition(
        const devs::Time& /* time */)
    {
    }

    virtual void externalTransition(
        const devs::ExternalEventList& /* event */,
        const devs::Time& /* time */)
    {
    }

    virtual devs::Event::EventType confluentTransitions(
        const devs::Time& /* time */,
        const devs::ExternalEventList& /* extEventlist */) const
    {
        return devs::Event::INTERNAL;
    }

    virtual void request(
        const devs::RequestEvent& /* event */,
        const devs::Time& /* time */,
        devs::ExternalEventList& /* output */) const
    {
    }

    virtual value::Value* observation(
        const devs::ObservationEvent& /* event */) const
    {
        return 0;
    }

    virtual void finish()
    {
    }
};

}} // namespace vle example

DECLARE_NAMED_DYNAMICS(simple, vle::examples::Simple)
