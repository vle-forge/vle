/**
 * @file examples/equation/A1.hpp
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


#ifndef EXAMPLES_EQUATION_A1_HPP
#define EXAMPLES_EQUATION_A1_HPP

#include <vle/extension/DifferenceEquation.hpp>

namespace vle { namespace examples { namespace equation {

    class A1 : public extension::DifferenceEquation::Simple
    {
        Var a;

    public:
        A1(const devs::DynamicsInit& model,
           const devs::InitEventList& events) :
            extension::DifferenceEquation::Simple(model, events)
        { a = createVar("a"); }

        virtual ~A1() { }

        virtual double compute(const devs::Time& time);
        virtual double initValue(const devs::Time& time);
    };

}}} // namespace vle examples equation

DECLARE_NAMED_DYNAMICS(A1, vle::examples::equation::A1)

#endif
