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


#include <vle/devs/DynamicsDbg.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/i18n.hpp>
#include <cassert>

namespace vle { namespace devs {

DynamicsDbg::DynamicsDbg(const DynamicsInit& init,
                         const InitEventList& events)
    : Dynamics(init, events)
    , mName(init.model().getCompleteName())
{
    TraceDevs((fmt(_("                     %1% [DEVS] constructor"))
               % mName).str().c_str());
}

Time DynamicsDbg::init(Time time)
{
    assert(mDynamics && "DynamicsDbg: missing set(Dynamics)");

    TraceDevs((fmt(_("%1$20.10g %2% [DEVS] init"))
               % time % mName).str().c_str());

    Time duration(mDynamics->init(time));

    TraceDevs((fmt(_("                .... %1% [DEVS] init returns %2%")) %
               mName % duration).str().c_str());

    return duration;
}

void DynamicsDbg::output(Time time, ExternalEventList& output) const
{
    assert(mDynamics && "DynamicsDbg: missing set(Dynamics)");

    TraceDevs((fmt(_("%1$20.10g %2% [DEVS] output"))
               % time % mName).str().c_str());

    mDynamics->output(time, output);

    if (output.empty()) {
        TraceDevs((fmt(
                _("                .... %1% [DEVS] output returns "
                  "empty output")) % mName).str().c_str());
    } else {
        TraceDevs((fmt(
                _("                .... %1% [DEVS] output returns "
                  "%2%")) % mName % output).str().c_str());
    }
}

Time DynamicsDbg::timeAdvance() const
{
    assert(mDynamics && "DynamicsDbg: missing set(Dynamics)");

    TraceDevs((fmt(_("                     %1% [DEVS] ta"))
               % mName).str().c_str());

    Time time(mDynamics->timeAdvance());

    TraceDevs((fmt(_("                .... %1% [DEVS] ta returns %2%")) %
               mName % time).str().c_str());

    return time;
}

void DynamicsDbg::internalTransition(Time time)
{
    assert(mDynamics && "DynamicsDbg: missing set(Dynamics)");

    TraceDevs((fmt(_("%1$20.10g %2% [DEVS] internal transition")) % time %
               mName).str().c_str());

    mDynamics->internalTransition(time);
}

void DynamicsDbg::externalTransition(const ExternalEventList& event,
                                     Time time)
{
    assert(mDynamics && "DynamicsDbg: missing set(Dynamics)");

    TraceDevs((fmt(_("%1$20.10g %2% [DEVS] external transition: [%3%]"))
               % time % mName % event).str().c_str());

    mDynamics->externalTransition(event, time);
}

void DynamicsDbg::confluentTransitions(
    Time time,
    const ExternalEventList& extEventlist)
{
    assert(mDynamics && "DynamicsDbg: missing set(Dynamics)");

    TraceDevs(
        (fmt(_("%1$20.10g %2% [DEVS] confluent transition: [%3%]"))
         % time % mName % extEventlist).str().c_str());

    mDynamics->confluentTransitions(time, extEventlist);
}

std::unique_ptr<vle::value::Value>
DynamicsDbg::observation(const ObservationEvent& event) const
{
    assert(mDynamics && "DynamicsDbg: missing set(Dynamics)");

    TraceDevs((fmt(_("%1$20.10g %2% [DEVS] observation: [from: '%3%'"
                     " port: '%4%']")) % event.getTime() % mName
               % event.getViewName() % event.getPortName()).str().c_str());

    return mDynamics->observation(event);
}

void DynamicsDbg::finish()
{
    assert(mDynamics && "DynamicsDbg: missing set(Dynamics)");

    TraceDevs((fmt(_("                     %1% [DEVS] finish"))
               % mName).str().c_str());

    mDynamics->finish();
}

}} // namespace vle devs
