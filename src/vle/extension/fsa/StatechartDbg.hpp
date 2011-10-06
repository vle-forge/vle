/*
 * @file vle/extension/fsa/StatechartDbg.hpp
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


#ifndef VLE_EXTENSION_FSA_STATECHART_DBG_HPP
#define VLE_EXTENSION_FSA_STATECHART_DBG_HPP 1

#include <vle/extension/fsa/Statechart.hpp>
#include <vle/utils/Trace.hpp>

namespace vle { namespace extension { namespace fsa {

template < typename UserModel >
class StatechartDbg : public UserModel
{
public:
    StatechartDbg(const devs::DynamicsInit& model,
                  const devs::InitEventList& events)
        : UserModel(model, events), mName(model.model().getCompleteName())
    {
        TraceExtension(
            fmt(_("                     %1% [Statechart] constructor"))
            % mName);
    }

    virtual ~StatechartDbg() {}

    virtual devs::Time init(const devs::Time& time)
    {
        mLastState = UserModel::currentState();
        TraceExtension(
            fmt(_("%1$20.10g %2% [Statechart] initial state = %3%"))
            % time % mName % UserModel::currentState());

        return UserModel::init(time);
    }

    virtual void output(const devs::Time& time,
                        devs::ExternalEventList& output) const
    {
        UserModel::output(time, output);

        if (not output.empty()) {
            TraceExtension(
                fmt(_("%1$20.10g %2% [Statechart] output: %3%"))
                % time % mName % output);
        }
    }

    virtual void internalTransition(const devs::Time& time)
    {
        UserModel::internalTransition(time);

        if (mLastState != UserModel::currentState()) {
            TraceExtension(
                fmt(_("%1$20.10g %2% [Statechart] transition: %3% -> %4%"))
                % time % mName % mLastState % UserModel::currentState());
            mLastState = UserModel::currentState();
        }
    }

    virtual void externalTransition(const devs::ExternalEventList& events,
                                    const devs::Time& time)
    {
        devs::ExternalEventList::const_iterator it = events.begin();

        while (it != events.end()) {
            if ((*it)->haveAttributes()) {
                TraceExtension(
                    fmt(_("%1$20.10g %2% [Statechart] event on %3% "    \
                          "with attributes: %4%"))
                    % time % mName % (*it)->getPortName() % (
                        (*it)->getAttributes().writeToString()));
            } else {
                TraceExtension(
                    fmt(_("%1$20.10g %2% [Statechart] event on %3%")));
            }
            ++it;
        }

        UserModel::externalTransition(events, time);
    }

private:
    std::string mName;
    int mLastState;
};

}}} // namespace vle extension fsa

#define DECLARE_FSA_STATECHART_DBG(mdl)                                 \
    extern "C" {                                                        \
        vle::devs::Dynamics*                                            \
        makeNewDynamics(const vle::devs::DynamicsInit& init,            \
                        const vle::devs::InitEventList& events)         \
        {                                                               \
            return new vle::extension::fsa::StatechartDbg               \
                < mdl >(init, events);                                  \
        }                                                               \
    }

#define DECLARE_NAMED_FSA_STATECHART_DBG(name, mdl)                     \
    extern "C" {                                                        \
        vle::devs::Dynamics*                                            \
        makeNewDynamics##name(const vle::devs::DynamicsInit& init,      \
                              const vle::devs::InitEventList& events)   \
        {                                                               \
            return new vle::extension::fsa::StatechartDbg               \
                < mdl >(init, events);                                  \
        }                                                               \
    }

#endif
