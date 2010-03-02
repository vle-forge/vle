/**
 * @file vle/gvle/modeling/fsa/Statechart.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2010 ULCO http://www.univ-littoral.fr
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


#ifndef VLE_GVLE_MODELING_FSA_STATECHART_HPP
#define VLE_GVLE_MODELING_FSA_STATECHART_HPP

#include <map>
#include <string>
#include <vector>

namespace vle { namespace gvle { namespace modeling {

enum function_type { CONSTANT, FUNCTION };

typedef std::vector < std::string > strings_t;
typedef std::map < std::string, std::string > buffers_t;

struct point_t
{
    int x;
    int y;

    point_t(int x = -1, int y = -1) : x(x), y(y)
    { }

    bool valid() const
    { return x != -1 and y != -1; }

    void invalid()
    { x = -1; y = -1; }

    bool operator==(const point_t& pt) const
    { return pt.x == x and pt.y == y; }

    bool operator!=(const point_t& pt) const
    { return pt.x != x or pt.y != y; }
};

typedef std::vector < point_t > points_t;
typedef std::map < std::string, std::string > eventInStates_t;

class State
{
public:
    State(const std::string& name, bool initial, int x, int y, int w, int h) :
        mName(name), mInitial(initial), mX(x), mY(y), mWidth(w), mHeight(h)
    { computeAnchors(); }

    State(const std::string& conf);

    const std::string& activity() const
    { return mActivity; }

    void activity(const std::string& act)
    { mActivity = act; }

    const points_t& anchors() const
    { return mAnchors; }

    void displace(int deltax, int deltay);

    const eventInStates_t& eventInStates() const
    { return mEventInStates; }

    std::string eventInState(const std::string& name) const
    {
        eventInStates_t::const_iterator it = mEventInStates.find(name);
        if (it != mEventInStates.end()) {
            return it->second;
        } else {
            return std::string();
        }
    }

    void eventInStates(const std::string& event, const std::string& action)
    { mEventInStates[event] = action; }

    void eventInStates(const eventInStates_t& eventInStates)
    {
        for (eventInStates_t::const_iterator it = eventInStates.begin();
             it != eventInStates.end(); ++it) {
            mEventInStates[it->first] = it->second;
        }
    }

    int height() const
    { return mHeight; }

    void height(int height);

    const std::string& inAction() const
    { return mInAction; }

    void inAction(const std::string& act)
    { mInAction = act; }

    bool initial() const
    { return mInitial; }

    void initial(bool initial)
    { mInitial = initial; }

    const std::string& name() const
    { return mName; }

    void name(const std::string& name)
    { mName = name; }

    const std::string& outAction() const
    { return mOutAction; }

    void outAction(const std::string& act)
    { mOutAction = act; }

    void parseEventInStates(const std::string& conf);

    bool select(int x, int y) const
    { return x >= mX and x <= mX + mWidth and y >= mY and y <= mY + mHeight; }

    std::string toString() const;

    int x() const
    { return mX; }

    int width() const
    { return mWidth; }

    void width(int w);

    int y() const
    { return mY; }

private:
    void computeAnchors();

    std::string mName;
    bool        mInitial;
    std::string mInAction;
    std::string mOutAction;
    std::string mActivity;
    eventInStates_t mEventInStates;

    int         mX;
    int         mY;
    int         mWidth;
    int         mHeight;
    points_t    mAnchors;
};

class Transition
{
public:
    Transition(const std::string& src, const std::string& dst,
               const points_t& pts) :
        mSource(src), mDestination(dst), mPoints(pts)
    { }

    Transition(const std::string& conf);

    const std::string& action() const
    { return mAction; }

    void action(const std::string& act)
    { mAction = act; }

    points_t::iterator addPoint(points_t::iterator& it, const point_t& pt)
    { return mPoints.insert(it, pt); }

    const std::string& after() const
    { return mAfter; }

    void after(const std::string& aft, function_type func_type)
    { mAfter = aft; mAfterType = func_type; }

    function_type afterType() const
    { return mAfterType; }

    const std::string& destination() const
    { return mDestination; }

    void displaceDestination(int deltax, int deltay)
    {
        mPoints[mPoints.size() - 1].x += deltax;
        mPoints[mPoints.size() - 1].y += deltay;
    }

    void displaceSource(int deltax, int deltay)
    { mPoints[0].x += deltax; mPoints[0].y += deltay; }

    const std::string& event() const
    { return mEvent; }

    void event(const std::string& evt)
    { mEvent = evt; }

    const std::string& guard() const
    { return mGuard; }

    void guard(const std::string& grd)
    { mGuard = grd; }

    const std::string& send() const
    { return mSend; }

    void send(const std::string& opt, function_type func_type)
    { mSend = opt; mSendType = func_type; }

    function_type sendType() const
    { return mSendType; }

    points_t& points()
    { return mPoints; }

    std::string toString() const;

    const std::string& when() const
    { return mWhen; }

    void when(const std::string& whn, function_type func_type)
    { mWhen = whn; mWhenType = func_type; }

    function_type whenType() const
    { return mWhenType; }

    const std::string& source() const
    { return mSource; }

private:
    std::string   mSource;
    std::string   mDestination;
    std::string   mEvent;
    std::string   mGuard;
    std::string   mWhen;
    function_type mWhenType;
    std::string   mAfter;
    function_type mAfterType;
    std::string   mAction;
    std::string   mSend;
    function_type mSendType;

    points_t      mPoints;
};

typedef std::map < std::string, State* > states_t;
typedef std::vector < Transition* > transitions_t;

class Statechart
{
public:
    Statechart(const std::string& name, const strings_t& inputPorts,
               const strings_t& outputPorts) :
        mName(name), mWidth(INITIAL_WIDTH), mHeight(INITIAL_HEIGHT),
        mInputPorts(inputPorts), mOutputPorts(outputPorts)
    { }

    virtual ~Statechart()
    {
        for (states_t::const_iterator it = states().begin();
             it != states().end(); ++it) {
            delete it->second;
        }
    }

    const buffers_t& actions() const
    { return mActions; }

    std::string action(const std::string& name) const
    {
        buffers_t::const_iterator it = mActions.find(name);
        if (it != mActions.end()) {
            return it->second;
        } else {
            return std::string();
        }
    }

    void action(const std::string& name, const std::string& buffer)
    { mActions[name] = buffer; }

    const buffers_t& activities() const
    { return mActivities; }

    std::string activity(const std::string& name) const
    {
        buffers_t::const_iterator it = mActivities.find(name);
        if (it != mActivities.end()) {
            return it->second;
        } else {
            return std::string();
        }
    }

    void activity(const std::string& name, const std::string& buffer)
    { mActivities[name] = buffer; }

    void addInputPort(const std::string& name)
    { mInputPorts.push_back(name); }

    void addOutputPort(const std::string& name)
    { mOutputPorts.push_back(name); }

    const buffers_t& afters() const
    { return mAfters; }

    std::string after(const std::string& name) const
    {
        buffers_t::const_iterator it = mAfters.find(name);
        if (it != mAfters.end()) {
            return it->second;
        } else {
            return std::string();
        }
    }

    void after(const std::string& name, const std::string& buffer)
    { mAfters[name] = buffer; }

    void addState(const std::string& conf)
    {
        State* state = new State(conf);

        mStates[state->name()] = state;
    }

    void addState(const std::string& name, bool initial,
                  int x, int y, int w, int h)
    { mStates[name] = new State(name, initial, x, y, w, h); }

    void addTransition(const std::string& conf)
    { mTransitions.push_back(new Transition(conf)); }

    void addTransition(const std::string& src, const std::string& dst,
                       const points_t& pts)
    { mTransitions.push_back(new Transition(src, dst, pts)); }

    std::string clause(const Transition* transition) const;

    std::string clause(const State* state) const;

    void displace(State* state, int deltax, int deltay);

    const buffers_t& eventActions() const
    { return mEventActions; }

    std::string eventAction(const std::string& name) const
    {
        buffers_t::const_iterator it = mEventActions.find(name);
        if (it != mEventActions.end()) {
            return it->second;
        } else {
            return std::string();
        }
    }

    void eventAction(const std::string& name, const std::string& buffer)
    { mEventActions[name] = buffer; }

    const buffers_t& guards() const
    { return mGuards; }

    std::string guard(const std::string& name) const
    {
        buffers_t::const_iterator it = mGuards.find(name);
        if (it != mGuards.end()) {
            return it->second;
        } else {
            return std::string();
        }
    }

    void guard(const std::string& name, const std::string& buffer)
    { mGuards[name] = buffer; }

    int height() const
    { return mHeight; }

    void height(int height)
    { mHeight = height; }

    std::string initialState() const;

    const strings_t& inputPorts() const
    { return mInputPorts; }

    const std::string& name() const
    { return mName; }

    const strings_t& outputPorts() const
    { return mOutputPorts; }

    void removeState(State* state);

    void removeTransition(Transition* transition);

    void resize(int deltax, int deltay)
    { mWidth += deltax; mHeight += deltay; }

    void resizeState(State* state, int widht, int height);

    const buffers_t& sends() const
    { return mSends; }

    std::string send(const std::string& name) const
    {
        buffers_t::const_iterator it = mSends.find(name);
        if (it != mSends.end()) {
            return it->second;
        } else {
            return std::string();
        }
    }

    void send(const std::string& name, const std::string& buffer)
    { mSends[name] = buffer; }

    State* state(const std::string& name) const
    { return mStates.find(name)->second; }

    const states_t& states() const
    { return mStates; }

    const transitions_t& transitions() const
    { return mTransitions; }

    const buffers_t& whens() const
    { return mWhens; }

    std::string when(const std::string& name) const
    {
        buffers_t::const_iterator it = mWhens.find(name);
        if (it != mWhens.end()) {
            return it->second;
        } else {
            return std::string();
        }
    }

    void when(const std::string& name, const std::string& buffer)
    { mWhens[name] = buffer; }

    int width() const
    { return mWidth; }

    void width(int width)
    { mWidth = width; }

    static const int INITIAL_HEIGHT;
    static const int INITIAL_WIDTH;
    static const int MINIMAL_HEIGHT;
    static const int MINIMAL_WIDTH;

    static const std::string ACTION_DEFINITION;
    static const std::string ACTIVITY_DEFINITION;
    static const std::string AFTER_DEFINITION;
    static const std::string EVENT_ACTION_DEFINITION;
    static const std::string GUARD_DEFINITION;
    static const std::string SEND_DEFINITION;
    static const std::string WHEN_DEFINITION;

private:
    std::string   mName;
    int           mWidth;
    int           mHeight;
    states_t      mStates;
    transitions_t mTransitions;

    buffers_t     mActions;
    buffers_t     mActivities;
    buffers_t     mAfters;
    buffers_t     mGuards;
    buffers_t     mEventActions;
    buffers_t     mSends;
    buffers_t     mWhens;

    strings_t     mInputPorts;
    strings_t     mOutputPorts;
};

}}} // namespace vle gvle modeling

#endif
