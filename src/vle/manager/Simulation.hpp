/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef VLE_MANGER_SIMULATION_HPP
#define VLE_MANGER_SIMULATION_HPP

#include <vle/DllDefines.hpp>
#include <vle/utils/ModuleManager.hpp>
#include <vle/manager/Types.hpp>
#include <vle/vpz/Vpz.hpp>

namespace vle { namespace manager {

/**
 * @c manager::Simulation permits to run single simulation.
 *
 * The @c manager::Simulation returns a @c value::Map. The key is the
 * name of the @c devs::View and the value is a @c value::Matrix or
 * NULL if the @c value::Matrix is empty.
 *
 * @attention You are in charge to freed the simulation result @c
 * value::Map.
 */
class VLE_API Simulation
{
public:
    Simulation(LogOptions         logoptions,
               SimulationOptions  simulationoptionts,
               std::ostream      *output);

    ~Simulation();

    value::Map * run(vpz::Vpz                   *vpz,
                     const utils::ModuleManager &modulemgr,
                     Error                      *error);

private:
    Simulation(const Simulation &other);
    Simulation& operator=(const Simulation &other);

    class Pimpl;
    Pimpl *mPimpl;
};

}}

#endif
