/**
 * @file examples/equation/D2.hpp
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


#ifndef EXAMPLES_EQUATION_D2_HPP
#define EXAMPLES_EQUATION_D2_HPP

#include <vle/extension/DifferenceEquation.hpp>

namespace vle { namespace examples { namespace equation {

    class D2 : public vle::extension::DifferenceEquation
    {
    public:
        D2(const vle::graph::AtomicModel& model,
           const vle::devs::InitEventList& events);

        virtual ~D2() { }

        virtual double compute(const vle::devs::Time& time) const;
    };

}}} // namespace vle examples equation

DECLARE_NAMED_DYNAMICS(D2,vle::examples::equation::D2)

#endif
