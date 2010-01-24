/**
 * @file vle/devs/DynamicsDbg.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
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


#include <vle/devs/DynamicsDbg.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/i18n.hpp>

namespace vle { namespace devs {

DynamicsDbg::DynamicsDbg(const DynamicsInit& init,
                         const InitEventList& events)
    : Dynamics(init, events), mDynamics(0)
{
    TraceDevs(fmt(_("                     %1% [DEVS] constructor")) %
              getModelName()); }

Time DynamicsDbg::init(const Time& time)
{
    TraceDevs(fmt(_("%1$20.10g %2% [DEVS] init")) % time % getModelName());

    return mDynamics->init(time);
}

void DynamicsDbg::output(const Time& time, ExternalEventList& output) const
{
    TraceDevs(fmt(_("%1$20.10g %2% [DEVS] output")) % time % getModelName());

    mDynamics->output(time, output);

    if (output.empty()) {
        TraceDevs(fmt(
                _("                .... %1% [DEVS] output returns "
                  "empty output")) % getModelName());
    } else {
        TraceDevs(fmt(
                _("                .... %1% [DEVS] output returns "
                  "%2%")) % getModelName() % output);
    }
}

Time DynamicsDbg::timeAdvance() const
{
    TraceDevs(fmt(_("                     %1% [DEVS] ta")) % getModelName());

    Time time(mDynamics->timeAdvance());

    TraceDevs(fmt(_("                .... %1% [DEVS] ta returns %2%")) %
              getModelName() % time);

    return time;
}

void DynamicsDbg::internalTransition(const Time& time)
{
    TraceDevs(fmt(_("%1$20.10g %2% [DEVS] internal transition")) % time %
              getModelName());

    mDynamics->internalTransition(time);
}

void DynamicsDbg::externalTransition(const ExternalEventList& event,
                                     const Time& time)
{
    TraceDevs(fmt(_("%1$20.10g %2% [DEVS] external transition: [%3%]")) % time
              % getModelName() % event);

    mDynamics->externalTransition(event, time);
}

Event::EventType DynamicsDbg::confluentTransitions(
    const Time& time,
    const ExternalEventList& extEventlist) const
{
    TraceDevs(fmt(
            _("%1$20.10g %2% [DEVS] confluent transition: [%3%]")) % time %
        getModelName() % extEventlist);

    return mDynamics->confluentTransitions(time, extEventlist);
}

void DynamicsDbg::request(const RequestEvent& event,
                          const Time& time,
                          ExternalEventList& output) const
{
    TraceDevs(fmt(_("%1$20.10g %2% [DEVS] request: [%3%]")) % time %
              getModelName() % event);

    mDynamics->request(event, time, output);

    if (output.empty()) {
        TraceDevs(fmt(
                _("                .... %1% [DEVS] request returns empty "
                  "output")) % getModelName());
    } else {
        TraceDevs(fmt(
                _("                .... %1% [DEVS] request returns %2%")) %
            getModelName() % output);
    }
}

vle::value::Value* DynamicsDbg::observation(
    const ObservationEvent& event) const
{
    TraceDevs(fmt(_("%1$20.10g %2% [DEVS] observation: [%3%]")) %
              event.getTime() % getModelName() % event);

    return mDynamics->observation(event);
}

void DynamicsDbg::finish()
{
    TraceDevs(fmt(_("                     %1% [DEVS] finish")) % getModelName());

    mDynamics->finish();
}

}} // namespace vle devs

