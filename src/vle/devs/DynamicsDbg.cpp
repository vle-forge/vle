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

namespace vle { namespace devs {

DynamicsDbg::DynamicsDbg(const DynamicsInit& init,
                         const InitEventList& events)
    : Dynamics(init, events), mDynamics(0),
    mName(init.model().getCompleteName())
{
    TraceDevs(fmt(_("                     %1% [DEVS] constructor")) % mName);
}

Time DynamicsDbg::init(const Time& time)
{
    TraceDevs(fmt(_("%1$20.10g %2% [DEVS] init")) % time % mName);

    Time duration(mDynamics->init(time));

    TraceDevs(fmt(_("                .... %1% [DEVS] init returns %2%")) %
              mName % duration);

    return duration;
}

void DynamicsDbg::output(const Time& time, ExternalEventList& output) const
{
    TraceDevs(fmt(_("%1$20.10g %2% [DEVS] output")) % time % mName);

    mDynamics->output(time, output);

    if (output.empty()) {
        TraceDevs(fmt(
                _("                .... %1% [DEVS] output returns "
                  "empty output")) % mName);
    } else {
        TraceDevs(fmt(
                _("                .... %1% [DEVS] output returns "
                  "%2%")) % mName % output);
    }
}

Time DynamicsDbg::timeAdvance() const
{
    TraceDevs(fmt(_("                     %1% [DEVS] ta")) % mName);

    Time time(mDynamics->timeAdvance());

    TraceDevs(fmt(_("                .... %1% [DEVS] ta returns %2%")) %
              mName % time);

    return time;
}

void DynamicsDbg::internalTransition(const Time& time)
{
    TraceDevs(fmt(_("%1$20.10g %2% [DEVS] internal transition")) % time %
              mName);

    mDynamics->internalTransition(time);
}

void DynamicsDbg::externalTransition(const ExternalEventList& event,
                                     const Time& time)
{
    TraceDevs(fmt(_("%1$20.10g %2% [DEVS] external transition: [%3%]")) % time
              % mName % event);

    mDynamics->externalTransition(event, time);
}

void DynamicsDbg::confluentTransitions(
    const Time& time,
    const ExternalEventList& extEventlist)
{
    TraceDevs(fmt(
            _("%1$20.10g %2% [DEVS] confluent transition: [%3%]")) % time %
        mName % extEventlist);

    mDynamics->confluentTransitions(time, extEventlist);
}

vle::value::Value* DynamicsDbg::observation(
    const ObservationEvent& event) const
{
    TraceDevs(fmt(_("%1$20.10g %2% [DEVS] observation: [from: '%3%'"
                    " port: '%4%']")) % event.getTime() % mName
              % event.getViewName() % event.getPortName());

    return mDynamics->observation(event);
}

void DynamicsDbg::finish()
{
    TraceDevs(fmt(_("                     %1% [DEVS] finish")) % mName);

    mDynamics->finish();
}

}} // namespace vle devs

