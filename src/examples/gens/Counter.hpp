/**
 * @file src/examples/gens/Counter.hpp
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

namespace vle { namespace examples { namespace gens {

    class Counter : public devs::Dynamics
    {
    public:
        Counter(const graph::AtomicModel& model,
                const devs::InitEventList& event);

        virtual ~Counter() { }

        virtual devs::Time init(const devs::Time& /* time */);

        virtual void output(const devs::Time& time,
                            devs::ExternalEventList& output) const;

        virtual devs::Time timeAdvance() const;

        virtual void internalTransition(const devs::Time& event);

        virtual void externalTransition(const devs::ExternalEventList& event,
                                        const devs::Time& time);

        virtual value::Value observation(
            const devs::ObservationEvent& event) const;

    private:
        long m_counter;
        bool m_active;
    };

}}} // namespace vle examples gens

DECLARE_NAMED_DYNAMICS(counter, vle::examples::gens::Counter);

#endif
