/*
 * @file examples/equation/confluentNosync_output.cpp
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

/**
 * This dynamic ensures that at most one external event occurs at
 * any time t.
 */

#include <vle/devs/DynamicsDbg.hpp>
#include <vle/utils/Exception.hpp>

namespace vle { namespace examples { namespace equation {

namespace vd = vle::devs;
namespace vu = vle::utils;

class confluentNosync_output : public vd::Dynamics
{
public:
    confluentNosync_output(const devs::DynamicsInit& model,
        const devs::InitEventList& events):
         vd::Dynamics(model,events)
    {
        lastWakeUp = vd::Time::negativeInfinity;
    }
    ~confluentNosync_output()
    {
    }
    virtual vle::devs::Time init(const vle::devs::Time& /* time */)
    {
        return vd::Time::infinity;
    }
    vle::devs::Time timeAdvance() const
    {
        return vd::Time::infinity;
    }
    void externalTransition(
        const vle::devs::ExternalEventList& /* event */,
        const vle::devs::Time& time)
    {
        if(time == lastWakeUp){
            throw vu::InternalError("two values at the same time");
        }
        lastWakeUp = time;
    }
    vd::Time lastWakeUp;
};

}}} // namespace vle examples equation

DECLARE_DYNAMICS_DBG(vle::examples::equation::confluentNosync_output)
