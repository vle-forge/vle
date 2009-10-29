/**
 * @file examples/counter/GeneratorWrapper.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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


#ifndef EXAMPLES_RANDOM_GENERATOR_WRAPPER_HPP
#define EXAMPLES_RANDOM_GENERATOR_WRAPPER_HPP

#include <vle/devs.hpp>
#include <vle/value.hpp>
#include <Generator.hpp>

namespace vle { namespace examples { namespace generator {

    class GeneratorWrapper : public vle::devs::Dynamics
    {
    public:
        GeneratorWrapper(const vle::devs::DynamicsInit& model,
                         const vle::devs::InitEventList& events);

        virtual ~GeneratorWrapper();

        virtual vle::devs::Time init(const vle::devs::Time& /* time */);

        virtual void output(const vle::devs::Time& time,
                            vle::devs::ExternalEventList& output) const;

        virtual vle::devs::Time timeAdvance() const;
        virtual void internalTransition(
                            const vle::devs::Time& event);
        virtual void finish() { }

    private:
        vle::devs::InitEventList m_events;
        Generator*  m_generator;
        double      m_timeStep;
        double      m_time;
    };

}}} // namespace vle examples generator

DECLARE_DYNAMICS(vle::examples::generator::GeneratorWrapper)

#endif
