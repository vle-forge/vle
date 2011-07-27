/*
 * @file vle/gvle/modeling/petrinet/PetriNet.hpp
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


#ifndef VLE_GVLE_MODELING_PETRINET_PETRINET_HPP
#define VLE_GVLE_MODELING_PETRINET_PETRINET_HPP

#include <cmath>
#include <map>
#include <string>
#include <vector>

namespace vle {
namespace gvle {
namespace modeling {
namespace petrinet {

typedef std::vector < std::string > strings_t;
typedef std::map < std::string, std::string > buffers_t;

struct point_t
{
    int x;
    int y;

    point_t(int x = -1, int y = -1) : x(x), y(y)
    {
    }

    bool valid() const
    {
        return x != -1 and y != -1;
    }

    void invalid()
    {
        x = -1;
        y = -1;
    }

    bool operator==(const point_t& pt) const
    {
        return pt.x == x and pt.y == y;
    }

    bool operator!=(const point_t& pt) const
    {
        return pt.x != x or pt.y != y;
    }
};

typedef std::vector < point_t > points_t;

class Place
{
public:
    Place(const std::string& name, bool output,
          unsigned int initialMarking, int x, int y, int r) :
        mName(name), mOutput(output),
        mInitialMarking(initialMarking), mX(x), mY(y), mRadius(r)
    {
        computeAnchors();
    }

    Place(const std::string& conf);

    const points_t& anchors() const
    {
        return mAnchors;
    }

    bool delay() const
    {
        return mDelay;
    }

    void delay(bool delay)
    {
        mDelay = delay;
    }

    double delayValue() const
    {
        return mDelayValue;
    }

    void delayValue(double delay)
    {
        mDelayValue = delay;
    }

    void displace(int deltax, int deltay);

    unsigned int initialMarking() const
    {
        return mInitialMarking;
    }

    void initialMarking(unsigned int initialMarking)
    {
        mInitialMarking = initialMarking;
    }

    const std::string& name() const
    {
        return mName;
    }

    void name(const std::string& name)
    {
        mName = name;
    }

    bool output() const
    {
        return mOutput;
    }

    void output(bool output)
    {
        mOutput = output;
    }

    const std::string& outputPortName() const
    {
        return mOutputPortName;
    }

    void outputPortName(const std::string& outputPortName)
    {
        mOutputPortName = outputPortName;
    }

    int radius() const
    {
        return mRadius;
    }

    void radius(int radius);

    bool select(int x, int y) const
    {
        return std::sqrt((mX - x) * (mX - x) + (mY - y) * (mY - y)) <=
               mRadius;
    }

    std::string toString() const;

    int x() const
    {
        return mX;
    }

    int y() const
    {
        return mY;
    }

private:
    void computeAnchors();

    std::string mName;
    bool mOutput;
    std::string mOutputPortName;
    unsigned int mInitialMarking;
    bool mDelay;
    double mDelayValue;

    int mX;
    int mY;
    int mRadius;
    points_t mAnchors;
};

typedef std::map < std::string, Place* > places_t;

class Transition
{
public:
    Transition(const std::string& name,
               bool input,
               bool output,
               int x,
               int y,
               int w,
               int h) :
        mName(name), mInput(input), mOutput(output), mX(x), mY(y),
        mWidth(w), mHeight(h)
    {
        computeAnchors();
    }

    Transition(const std::string& conf);

    const points_t& anchors() const
    {
        return mAnchors;
    }

    void displace(int deltax, int deltay);

    bool delay() const
    {
        return mDelay;
    }

    void delay(bool delay)
    {
        mDelay = delay;
    }

    double delayValue() const
    {
        return mDelayValue;
    }

    void delayValue(double delay)
    {
        mDelayValue = delay;
    }

    int height() const
    {
        return mHeight;
    }

    void height(int height);

    bool input() const
    {
        return mInput;
    }

    void input(bool input)
    {
        mInput = input;
    }

    const std::string& inputPortName() const
    {
        return mInputPortName;
    }

    void inputPortName(const std::string& inputPortName)
    {
        mInputPortName = inputPortName;
    }

    const std::string& name() const
    {
        return mName;
    }

    void name(const std::string& name)
    {
        mName = name;
    }

    bool output() const
    {
        return mOutput;
    }

    void output(bool output)
    {
        mOutput = output;
    }

    const std::string& outputPortName() const
    {
        return mOutputPortName;
    }

    void outputPortName(const std::string& outputPortName)
    {
        mOutputPortName = outputPortName;
    }

    unsigned int priority() const
    {
        return mPriority;
    }

    void priority(unsigned int priority)
    {
        mPriority = priority;
    }

    bool select(int x, int y) const
    {
        return x >= mX and x <= mX + mWidth and y >= mY and y <= mY +
               mHeight;
    }

    std::string toString() const;

    int x() const
    {
        return mX;
    }

    int width() const
    {
        return mWidth;
    }

    void width(int w);

    int y() const
    {
        return mY;
    }

private:
    void computeAnchors();

    std::string mName;
    bool mInput;
    std::string mInputPortName;
    bool mOutput;
    std::string mOutputPortName;
    bool mDelay;
    double mDelayValue;
    unsigned int mPriority;

    int mX;
    int mY;
    int mWidth;
    int mHeight;
    points_t mAnchors;
};

typedef std::map < std::string, Transition* > transitions_t;

class Arc
{
public:
    Arc(const std::string& src, const std::string& dst,
        const points_t& pts) :
        mSource(src), mDestination(dst), mWeight(1), mInhibitor(false),
        mPoints(pts)
    {
    }

    Arc(const std::string& conf);

    points_t::iterator addPoint(points_t::iterator& it, const point_t& pt)
    {
        return mPoints.insert(it, pt);
    }

    const std::string& destination() const
    {
        return mDestination;
    }

    void displaceDestination(int deltax, int deltay)
    {
        mPoints[mPoints.size() - 1].x += deltax;
        mPoints[mPoints.size() - 1].y += deltay;
    }

    void displaceSource(int deltax, int deltay)
    {
        mPoints[0].x += deltax;
        mPoints[0].y += deltay;
    }

    bool inhibitor() const
    {
        return mInhibitor;
    }

    void inhibitor(bool inhibitor)
    {
        mInhibitor = inhibitor;
    }

    points_t& points()
    {
        return mPoints;
    }

    const std::string& source() const
    {
        return mSource;
    }

    std::string toString() const;

    unsigned int weight() const
    {
        return mWeight;
    }

    void weight(unsigned int weight)
    {
        mWeight = weight;
    }

private:
    std::string mSource;
    std::string mDestination;
    unsigned int mWeight;
    bool mInhibitor;

    points_t mPoints;
};

typedef std::vector < Arc* > arcs_t;

class PetriNet
{
public:
    PetriNet(const std::string& name, const strings_t& inputPorts,
             const strings_t& outputPorts) :
        mName(name), mWidth(INITIAL_WIDTH), mHeight(INITIAL_HEIGHT),
        mInputPorts(inputPorts), mOutputPorts(outputPorts)
    {
    }

    virtual ~PetriNet()
    {
        for (places_t::const_iterator it = mPlaces.begin();
             it != mPlaces.end(); ++it) {
            delete it->second;
        }
        for (transitions_t::const_iterator it = mTransitions.begin();
             it != mTransitions.end(); ++it) {
            delete it->second;
        }
        for (arcs_t::const_iterator it = mArcs.begin();
             it != mArcs.end(); ++it) {
            delete *it;
        }
    }

    void addArc(const std::string& conf)
    {
        mArcs.push_back(new Arc(conf));
    }

    void addArc(const std::string& src, const std::string& dst,
        const points_t& pts)
    {
        mArcs.push_back(new Arc(src, dst, pts));
    }

    void addInputPort(const std::string& name)
    {
        mInputPorts.push_back(name);
    }

    void addOutputPort(const std::string& name)
    {
        mOutputPorts.push_back(name);
    }

    void changePlaceName (const std::string& oldName, 
            const std::string& newName) {
        Place* place = mPlaces.find(oldName)->second;
        mPlaces.erase(mPlaces.find(oldName));
        mPlaces[newName] = place;
    }

    void changeTransitionName (const std::string& oldName, 
            const std::string& newName) {
        Transition* transition = mTransitions.find(oldName)->second;
        mTransitions.erase(mTransitions.find(oldName));
        mTransitions[newName] = transition;
    }

    Place* addPlace(const std::string& conf)
    {
        Place* place = new Place(conf);

        mPlaces[place->name()] = place;
        return place;
    }

    Place* addPlace(const std::string& name, bool output,
        unsigned int initialMarking, int x, int y, int r)
    {
        Place* place = new Place(name, output, initialMarking, x, y, r);

        mPlaces[name] = place;
        return place;
    }

    Transition* addTransition(const std::string& conf)
    {
        Transition* transition = new Transition(conf);

        mTransitions[transition->name()] = transition;
        return transition;
    }

    Transition* addTransition(const std::string& name,
        bool input,
        bool output,
        int x,
        int y,
        int w,
        int h)
    {
        Transition* transition = new Transition(name, input, output, x, y,
                w, h);

        mTransitions[name] = transition;
        return transition;
    }

    const arcs_t& arcs() const
    {
        return mArcs;
    }

    void displace(Place* state, int deltax, int deltay);

    void displace(Transition* state, int deltax, int deltay);

    bool existPlace(const std::string& name) const
    {
        return mPlaces.find(name) != mPlaces.end();
    }

    bool existTransition(const std::string& name) const
    {
        return mTransitions.find(name) != mTransitions.end();
    }

    int height() const
    {
        return mHeight;
    }

    void height(int height)
    {
        mHeight = height;
    }

    const strings_t& inputPorts() const
    {
        return mInputPorts;
    }

    const std::string& name() const
    {
        return mName;
    }

    const strings_t& outputPorts() const
    {
        return mOutputPorts;
    }

    void removePlace(Place* state);

    void removeTransition(Transition* state);

    void removeArc(Arc* arc);

    void resize(int deltax, int deltay)
    {
        mWidth += deltax;
        mHeight += deltay;
    }

    const places_t& places() const
    {
        return mPlaces;
    }

    Place* place(const std::string& name) const
    {
        return mPlaces.find(name)->second;
    }

    const transitions_t& transitions() const
    {
        return mTransitions;
    }

    Transition* transition(const std::string& name) const
    {
        return mTransitions.find(name)->second;
    }

    int width() const
    {
        return mWidth;
    }

    void width(int width)
    {
        mWidth = width;
    }

    static const int INITIAL_HEIGHT;
    static const int INITIAL_WIDTH;
    static const int TRANSITION_HEIGHT;
    static const int TRANSITION_WIDTH;
    static const int PLACE_MINIMAL_RADIUS;
    static const int MINIMAL_HEIGHT;
    static const int MINIMAL_WIDTH;

private:
    std::string mName;
    int mWidth;
    int mHeight;
    places_t mPlaces;
    transitions_t mTransitions;
    arcs_t mArcs;

    strings_t mInputPorts;
    strings_t mOutputPorts;
};

}
}
}
}    // namespace vle gvle modeling petrinet

#endif
