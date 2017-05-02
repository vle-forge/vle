/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2017 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2017 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2017 INRA http://www.inra.fr
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

#include <cassert>
#include <limits>
#include <sstream>
#include <vle/devs/DynamicsDbg.hpp>
#include <vle/devs/DynamicsInit.hpp>
#include <vle/utils/ContextPrivate.hpp>
#include <vle/utils/i18n.hpp>

namespace {

void
print_external_event(const vle::devs::ExternalEventList& events,
                     vle::utils::ContextPtr ctx) noexcept
{
    std::ostringstream oss;

    for (const auto& event : events) {
        oss << '[' << event.getPortName();

        if (event.attributes().get()) {
            oss << ": ";
            event.attributes()->writeString(oss);
            oss << ']';
        } else {
            oss << ": null]";
        }
    }

    vDbg(ctx, "%s\n", oss.str().c_str());
}

} // anonymous namespace

namespace vle {
namespace devs {

DynamicsDbg::DynamicsDbg(const DynamicsInit& init, const InitEventList& events)
  : Dynamics(init, events)
  , mName(init.model.getCompleteName())
{
    vDbg(context(),
         _("                     %s [DEVS] constructor\n"),
         mName.c_str());
}

Time
DynamicsDbg::init(Time time)
{
    assert(mDynamics && "DynamicsDbg: missing set(Dynamics)");

    vDbg(context(),
         _("%.*g %s [DEVS] init\n"),
         std::numeric_limits<double>::max_digits10,
         time,
         mName.c_str());

    Time duration(mDynamics->init(time));

    vDbg(context(),
         _("                .... %s [DEVS] init returns %.*g\n"),
         mName.c_str(),
         std::numeric_limits<double>::max_digits10,
         duration);

    return duration;
}

void
DynamicsDbg::output(Time time, ExternalEventList& output) const
{
    assert(mDynamics && "DynamicsDbg: missing set(Dynamics)");

    vDbg(context(),
         _("%.*g %s [DEVS] output\n"),
         std::numeric_limits<double>::max_digits10,
         time,
         mName.c_str());

    mDynamics->output(time, output);
    vDbg(context(),
         _("                .... %s [DEVS] output returns \n"),
         mName.c_str());

    if (output.empty()) {
        vDbg(context(), _("no event\n"));
    } else {
        ::print_external_event(output, context());
    }
}

Time
DynamicsDbg::timeAdvance() const
{
    assert(mDynamics && "DynamicsDbg: missing set(Dynamics)");

    vDbg(context(), _("                     %s [DEVS] ta\n"), mName.c_str());

    Time time(mDynamics->timeAdvance());

    vDbg(context(),
         _("                .... %s [DEVS] ta returns %.*g\n"),
         mName.c_str(),
         std::numeric_limits<double>::max_digits10,
         time);

    return time;
}

void
DynamicsDbg::internalTransition(Time time)
{
    assert(mDynamics && "DynamicsDbg: missing set(Dynamics)");

    vDbg(context(),
         _("%.*g %s [DEVS] internal transition\n"),
         std::numeric_limits<double>::max_digits10,
         time,
         mName.c_str());

    mDynamics->internalTransition(time);
}

void
DynamicsDbg::externalTransition(const ExternalEventList& event, Time time)
{
    assert(mDynamics && "DynamicsDbg: missing set(Dynamics)");

    vDbg(context(),
         _("%.*g %s [DEVS] external transition:\n"),
         std::numeric_limits<double>::max_digits10,
         time,
         mName.c_str());

    ::print_external_event(event, context());

    mDynamics->externalTransition(event, time);
}

void
DynamicsDbg::confluentTransitions(Time time,
                                  const ExternalEventList& extEventlist)
{
    assert(mDynamics && "DynamicsDbg: missing set(Dynamics)");

    vDbg(context(),
         _("%.*g %s [DEVS] confluent transition:\n"),
         std::numeric_limits<double>::max_digits10,
         time,
         mName.c_str());

    ::print_external_event(extEventlist, context());

    mDynamics->confluentTransitions(time, extEventlist);
}

std::unique_ptr<vle::value::Value>
DynamicsDbg::observation(const ObservationEvent& event) const
{
    assert(mDynamics && "DynamicsDbg: missing set(Dynamics)");

    vDbg(context(),
         _("%.*g %s [DEVS] observation: [from: '%s' port:"
           " '%s']\n"),
         std::numeric_limits<double>::max_digits10,
         event.getTime(),
         mName.c_str(),
         event.getViewName().c_str(),
         event.getPortName().c_str());

    return mDynamics->observation(event);
}

void
DynamicsDbg::finish()
{
    assert(mDynamics && "DynamicsDbg: missing set(Dynamics)");

    vDbg(
      context(), _("                     %s [DEVS] finish\n"), mName.c_str());

    mDynamics->finish();
}
}
} // namespace vle devs
