/**
 * @file examples/counter/Counter.hpp
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


#ifndef VLE_EXAMPLES_COUNTER_HPP
#define VLE_EXAMPLES_COUNTER_HPP

#include <vle/devs.hpp>

namespace vle { namespace examples { namespace counter {

    class Counter : public vle::devs::Dynamics
    {
    public:
        Counter(const vle::devs::DynamicsInit& model,
                const vle::devs::InitEventList& events) :
            vle::devs::Dynamics(model, events),
            m_counter(0),
            m_delay(1.0),
            m_active(false)
        { }

        virtual ~Counter()
        { }

        // DEVS Methods
        virtual void finish() { }
        virtual vle::devs::Time init(const vle::devs::Time& /* time */);
        virtual void output(const vle::devs::Time& /* time */,
                            vle::devs::ExternalEventList& /* output */) const;
        virtual vle::devs::Time timeAdvance() const;
        virtual void internalTransition(const vle::devs::Time& /* event */);
        virtual void externalTransition(const vle::devs::ExternalEventList& /* event */,
                                        const vle::devs::Time& /* time */);
        virtual vle::value::Value* observation(const vle::devs::ObservationEvent& /* event */) const;

    private:
        long m_counter;
        double m_delay;
        bool m_active;
    };

}}} // namespace vle examples counter

DECLARE_DYNAMICS(vle::examples::counter::Counter)

#endif
