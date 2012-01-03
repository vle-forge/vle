/*
 * @file examples/equation/confluentNosync_perturb.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2012 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2012 INRA http://www.inra.fr
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
 * This dynamic sends a y_nosync value of 10 at 1.0 (in first bag)
 */

#include <vle/devs/DynamicsDbg.hpp>

namespace vle { namespace examples { namespace equation {

namespace vd = vle::devs;
namespace vv = vle::value;

class confluentNosync_perturb : public vd::Dynamics
{
public:
    enum State {BeforeOutput, Output, AfterOutput};
    confluentNosync_perturb(const devs::DynamicsInit& model,
        const devs::InitEventList& events):
         vd::Dynamics(model,events)
    {
    }
    ~confluentNosync_perturb()
    {
    }
    virtual vle::devs::Time init(const vle::devs::Time& /* time */)
    {
        return 1.0;
    }

    vle::devs::Time timeAdvance() const
    {
        return vd::Time::infinity;
    }

    void output(const vle::devs::Time& /* time */,
        vle::devs::ExternalEventList& output) const
    {
        vd::ExternalEvent* ev = new vd::ExternalEvent("y_nosync");
        ev->putAttribute("value", new vv::String("y_nosync"));
        ev->putAttribute("value", new vv::Double(10));
        output.addEvent(ev);
    }
};

}}} // namespace vle examples equation

DECLARE_DYNAMICS_DBG(vle::examples::equation::confluentNosync_perturb)
