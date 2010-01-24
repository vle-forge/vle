/**
 * @file examples/qss/ladybirdplantlouse.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
 * Copyright (C) 2003-2010 ULCO http://www.univ-littoral.fr
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


#ifndef EXAMPLES_QSS_LADYBIRD_PLANT_LOUSE_HPP
#define EXAMPLES_QSS_LADYBIRD_PLANT_LOUSE_HPP 1

#include <vle/extension/differential-equation/QSS.hpp>

namespace vle { namespace examples { namespace qss {

class Ladybirdplantlouse : public extension::QSS::Multiple
{
public:
    Ladybirdplantlouse(const devs::DynamicsInit& model,
		       const devs::InitEventList& events);

    virtual ~Ladybirdplantlouse();

    virtual double compute(unsigned int i, const devs::Time& time) const;

private:
    double a;
    double b;
    double d;
    double e;

    Var x;
    Var y;
};

}}} // namespace vle examples qss
#endif
