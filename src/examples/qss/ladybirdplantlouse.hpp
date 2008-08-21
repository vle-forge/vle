/**
 * @file examples/qss/ladybirdplantlouse.hpp
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


#ifndef VLE_TUTORIAL_0_LADYBIRD_PLANT_LOUSE_HPP
#define VLE_TUTORIAL_0_LADYBIRD_PLANT_LOUSE_HPP

#include <vle/extension/CombinedQSS.hpp>

namespace vle { namespace examples { namespace qss {

    class Ladybirdplantlouse : public extension::CombinedQss
    {
    public:
        Ladybirdplantlouse(const graph::AtomicModel& model,
                           const devs::InitEventList& events);

        virtual ~Ladybirdplantlouse();

        virtual double compute(unsigned int i) const;

    private:
        double a; 
        double b;
        double d;
        double e; 

    };

}}} // namespace vle examples qss
#endif
