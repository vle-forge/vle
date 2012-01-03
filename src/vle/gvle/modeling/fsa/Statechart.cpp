/*
 * @file vle/gvle/modeling/fsa/Statechart.cpp
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


#include <vle/gvle/modeling/fsa/Statechart.hpp>
#include <vle/utils/i18n.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>

namespace vle {
namespace gvle {
namespace modeling {
namespace fsa {

const int Statechart::INITIAL_HEIGHT = 400;
const int Statechart::INITIAL_WIDTH = 400;
const int Statechart::MINIMAL_HEIGHT = 100;
const int Statechart::MINIMAL_WIDTH = 100;

const std::string Statechart::ACTION_DEFINITION =
    "void %1%(const vd::Time& /*time*/)\n"      \
    "{\n"                                       \
    "}";

const std::string Statechart::ACTIVITY_DEFINITION =
    "void %1%(const vd::Time& /*time*/)\n"      \
    "{\n"                                       \
    "}";

const std::string Statechart::AFTER_DEFINITION =
    "vd::Time %1%(const vd::Time& /*time*/)\n"  \
    "{\n"                                       \
    "   return 0;\n"                            \
    "}";

const std::string Statechart::EVENT_ACTION_DEFINITION =
    "void %1%(const vd::Time& /*time*/,\n"               \
    "         const vd::ExternalEvent* /*event*/)\n"     \
    "{\n"                                                \
    "}";

const std::string Statechart::GUARD_DEFINITION =
    "bool %1%(const vd::Time& /*time*/)\n"      \
    "{\n"                                       \
    "   return false;\n"                        \
    "}";

const std::string Statechart::SEND_DEFINITION =
    "void %1%(const vd::Time& /*time*/,\n"                      \
    "         vd::ExternalEventList& /*output*/) const\n"       \
    "{\n"                                                       \
    "}";

const std::string Statechart::WHEN_DEFINITION =
    "vd::Time %1%(const vd::Time& /*time*/)\n"          \
    "{\n"                                               \
    "   return 0;\n"                                    \
    "}";

State::State(const std::string& conf)
{
    strings_t state;

    boost::split(state, conf, boost::is_any_of(","));
    mName = state[0];
    mX = boost::lexical_cast < int > (state[1]);
    mY = boost::lexical_cast < int > (state[2]);
    mWidth = boost::lexical_cast < int > (state[3]);
    mHeight = boost::lexical_cast < int > (state[4]);
    mInitial = boost::lexical_cast < bool > (state[5]);
    mInAction = state[6];
    mOutAction = state[7];
    mActivity = state[8];
    parseEventInStates(state[9]);
    computeAnchors();
}

void State::computeAnchors()
{
    mAnchors.clear();

    // top
    mAnchors.push_back(point_t(mX + mWidth / 3, mY));
    mAnchors.push_back(point_t(mX + 2 * mWidth / 3, mY));

    // bottom
    mAnchors.push_back(point_t(mX + mWidth / 3, mY + mHeight));
    mAnchors.push_back(point_t(mX + 2 * mWidth / 3, mY + mHeight));

    // left
    mAnchors.push_back(point_t(mX, mY + mHeight / 3));
    mAnchors.push_back(point_t(mX, mY + 2 * mHeight / 3));

    // right
    mAnchors.push_back(point_t(mX + mWidth, mY + mHeight / 3));
    mAnchors.push_back(point_t(mX + mWidth, mY + 2 * mHeight / 3));
}

void State::displace(int deltax, int deltay)
{
    mX += deltax;
    mY += deltay;

    for (points_t::iterator it = mAnchors.begin();
         it != mAnchors.end(); ++it) {
        it->x += deltax;
        it->y += deltay;
    }
}

void State::height(int height)
{
    mHeight = height;
    computeAnchors();
}

void State::parseEventInStates(const std::string& conf)
{
    strings_t pairs;

    boost::split(pairs, conf, boost::is_any_of("!"));
    for (unsigned int i = 0; i < pairs.size() / 2; ++i) {
        mEventInStates[pairs[i * 2]] = pairs[i * 2 + 1];
    }
}

void State::width(int width)
{
    mWidth = width;
    computeAnchors();
}

std::string State::toString() const
{
    std::string eventInStates;

    for (eventInStates_t::const_iterator it = mEventInStates.begin();
         it != mEventInStates.end(); ++it) {
        eventInStates += (fmt("%1%!%2%!") % it->first % it->second).str();
    }

    return (fmt("%1%,%2%,%3%,%4%,%5%,%6%,%7%,%8%,%9%,%10%")
            % mName % mX % mY % mWidth % mHeight % mInitial % mInAction
            % mOutAction % mActivity % eventInStates).str();
}

Transition::Transition(const std::string& conf)
{
    strings_t transition;

    boost::split(transition, conf, boost::is_any_of(","));
    mSource = transition[0];
    mDestination = transition[1];
    mEvent = transition[2];
    mGuard = transition[3];
    mWhen = transition[4];
    mAfter = transition[5];
    mAction = transition[6];
    mSend = transition[7];
    mSendType = (function_type)(boost::lexical_cast < int > (transition[8]));

    strings_t pts;

    boost::split(pts, transition[9], boost::is_any_of("!"));
    for (unsigned int i = 0; i < pts.size() - 1; ++i) {
        strings_t pt;

        boost::split(pt, pts[i], boost::is_any_of("/"));
        mPoints.push_back(point_t(boost::lexical_cast < int > (pt[0]),
                boost::lexical_cast < int > (pt[1])));
    }
}

std::string Transition::toString() const
{
    std::string pts;

    for (points_t::const_iterator it = mPoints.begin();
         it != mPoints.end(); ++it) {
        pts += (fmt("%1%/%2%!") % it->x % it->y).str();
    }

    return (fmt("%1%,%2%,%3%,%4%,%5%,%6%,%7%,%8%,%9%,%10%")
            % mSource % mDestination % mEvent % mGuard % mWhen % mAfter
            % mAction % mSend % mSendType % pts).str();
}

std::string Statechart::clause(const State* state) const
{
    std::string str;
    const std::string& inAction = state->inAction();
    const std::string& activity = state->activity();
    const std::string& outAction = state->outAction();

    if (not inAction.empty()) {
        str += (fmt(" << inAction(&%1%::%2%)") % mName % inAction).str();
    }
    if (not activity.empty()) {
        str += (fmt(" << activity(&%1%::%2%)") % mName % activity).str();
    }
    if (not outAction.empty()) {
        str += (fmt(" << outAction(&%1%::%2%)") % mName % outAction).str();
    }
    for (eventInStates_t::const_iterator it = state->eventInStates().begin();
         it != state->eventInStates().end(); ++it) {
        str += (fmt(" << eventInState(\"%1%\", &%2%::%3%)") % it->first
                % mName % it->second).str();
    }
    return str;
}

std::string Statechart::clause(const Transition* transition) const
{
    std::string str;
    const std::string& event = transition->event();
    const std::string& guard = transition->guard();
    const std::string& after = transition->after();
    const std::string& when = transition->when();
    const std::string& action = transition->action();
    const std::string& send = transition->send();

    if (not event.empty()) {
        str += (fmt(" << event(\"%1%\")") % event).str();
    }
    if (not after.empty()) {
        if (transition->afterType() == CONSTANT) {
            str += (fmt(" << after(%1%)") % after).str();
        } else {
            str += (fmt(" << after(&%1%::%2%)") % mName % after).str();
        }
    }
    if (not when.empty()) {
        if (transition->whenType() == CONSTANT) {
            str += (fmt(" << when(%1%)") % when).str();
        } else {
            str += (fmt(" << when(&%1%::%2%)") % mName % when).str();
        }
    }
    if (not guard.empty()) {
        str += (fmt(" << guard(&%1%::%2%)") % mName % guard).str();
    }
    if (not action.empty()) {
        str += (fmt(" << action(&%1%::%2%)") % mName % action).str();
    }
    if (not send.empty()) {
        if (transition->sendType() == CONSTANT) {
            str += (fmt(" << send(std::string(\"%1%\"))") % send).str();
        } else {
            str += (fmt(" << send(&%1%::%2%)") % mName % send).str();
        }
    }
    return str;
}

void Statechart::displace(State* state, int deltax, int deltay)
{
    state->displace(deltax, deltay);
    for (transitions_t::iterator it = mTransitions.begin();
         it != mTransitions.end(); ++it) {
        if ((*it)->source() == state->name()) {
            (*it)->displaceSource(deltax, deltay);
        }
        if ((*it)->source() != (*it)->destination() and
            (*it)->destination() == state->name()) {
            (*it)->displaceDestination(deltax, deltay);
        }
    }
}

std::string Statechart::initialState() const
{
    for (states_t::const_iterator it = mStates.begin(); it != mStates.end();
         ++it) {
        if (it->second->initial()) {
            return it->first;
        }
    }
    return std::string();
}

void Statechart::removeState(State* state)
{
    transitions_t::iterator it = mTransitions.begin();
    while (it != mTransitions.end()) {
        if ((*it)->source() == state->name() or
            (*it)->destination() == state->name()) {
            mTransitions.erase(it);
            it = mTransitions.begin();
        } else {
            ++it;
        }
    }
    mStates.erase(state->name());
}

void Statechart::removeTransition(Transition* transition)
{
    transitions_t::iterator it = std::find(mTransitions.begin(),
            mTransitions.end(), transition);

    mTransitions.erase(it);
}

void Statechart::resizeState(State* state, int width, int height)
{
    transitions_t::iterator it = mTransitions.begin();
    int deltax = width - state->width();
    int deltay = height - state->height();

    while (it != mTransitions.end()) {
        if ((*it)->source() == state->name()) {
            const point_t pt = (*it)->points().front();

            if (pt.x != state->x() and pt.y != state->y()) {
                (*it)->displaceSource(deltax, deltay);
            }
        }
        if ((*it)->destination() == state->name()) {
            const point_t pt = (*it)->points().back();

            if (pt.x != state->x() and pt.y != state->y()) {
                (*it)->displaceDestination(deltax, deltay);
            }
        }
        ++it;
    }
    if (deltax != 0) {
        state->width(width);
    }
    if (deltay != 0) {
        state->height(height);
    }
}

}
}
}
}    // namespace vle gvle modeling fsa
