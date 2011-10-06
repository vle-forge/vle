/*
 * @file vle/gvle/modeling/petrinet/PetriNet.cpp
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


#include <vle/gvle/modeling/petrinet/PetriNet.hpp>
#include <vle/utils/i18n.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

namespace vle {
namespace gvle {
namespace modeling {
namespace petrinet {

const int PetriNet::INITIAL_HEIGHT = 400;
const int PetriNet::INITIAL_WIDTH = 400;
const int PetriNet::TRANSITION_HEIGHT = 40;
const int PetriNet::TRANSITION_WIDTH = 10;
const int PetriNet::PLACE_MINIMAL_RADIUS = 20;
const int PetriNet::MINIMAL_HEIGHT = 100;
const int PetriNet::MINIMAL_WIDTH = 100;

Place::Place(const std::string& conf)
{
    strings_t state;

    boost::split(state, conf, boost::is_any_of(","));
    mName = state[0];
    mX = boost::lexical_cast < int > (state[1]);
    mY = boost::lexical_cast < int > (state[2]);
    mRadius = boost::lexical_cast < int > (state[3]);
    mOutput = boost::lexical_cast < bool > (state[4]);
    mOutputPortName = state[5];
    mInitialMarking = boost::lexical_cast < unsigned int > (state[6]);
    mDelay = not state[7].empty();
    if (mDelay) {
        mDelayValue = boost::lexical_cast < double > (state[7]);
    } else {
        mDelayValue = 0;
    }
    computeAnchors();
}

void Place::computeAnchors()
{
    int offset = (int)(mRadius * std::sin(M_PI / 4));

    mAnchors.clear();
    mAnchors.push_back(point_t(mX + mRadius, mY));
    mAnchors.push_back(point_t(mX + offset, mY - offset));
    mAnchors.push_back(point_t(mX, mY - mRadius));
    mAnchors.push_back(point_t(mX - offset, mY - offset));
    mAnchors.push_back(point_t(mX - mRadius, mY));
    mAnchors.push_back(point_t(mX - offset, mY + offset));
    mAnchors.push_back(point_t(mX, mY + mRadius));
    mAnchors.push_back(point_t(mX + offset, mY + offset));
}

void Place::displace(int deltax, int deltay)
{
    mX += deltax;
    mY += deltay;

    for (points_t::iterator it = mAnchors.begin();
         it != mAnchors.end(); ++it) {
        it->x += deltax;
        it->y += deltay;
    }
}

void Place::radius(int radius)
{
    mRadius = radius;
    computeAnchors();
}

std::string Place::toString() const
{
    std::string delay;

    if (mDelay) {
        delay = (fmt("%1%") % mDelayValue).str();
    }
    return (fmt("%1%,%2%,%3%,%4%,%5%,%6%,%7%,%8%")
            % mName % mX % mY % mRadius % mOutput %
            mOutputPortName % mInitialMarking % delay).str();
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

Transition::Transition(const std::string& conf)
{
    strings_t state;

    boost::split(state, conf, boost::is_any_of(","));
    mName = state[0];
    mX = boost::lexical_cast < int > (state[1]);
    mY = boost::lexical_cast < int > (state[2]);
    mWidth = boost::lexical_cast < int > (state[3]);
    mHeight = boost::lexical_cast < int > (state[4]);
    mInput = boost::lexical_cast < bool > (state[5]);
    mInputPortName = state[6];
    mOutput = boost::lexical_cast < bool > (state[7]);
    mOutputPortName = state[8];
    mDelay = not state[9].empty();
    if (mDelay) {
        mDelayValue = boost::lexical_cast < double > (state[9]);
    } else {
        mDelayValue = 0;
    }
    mPriority = boost::lexical_cast < unsigned int > (state[10]);
    computeAnchors();
}

void Transition::computeAnchors()
{
    mAnchors.clear();

    // top
    mAnchors.push_back(point_t(mX, mY));
    mAnchors.push_back(point_t(mX + mWidth / 3, mY));
    mAnchors.push_back(point_t(mX + 2 * mWidth / 3, mY));
    mAnchors.push_back(point_t(mX + mWidth, mY));

    // bottom
    mAnchors.push_back(point_t(mX, mY + mHeight));
    mAnchors.push_back(point_t(mX + mWidth / 3, mY + mHeight));
    mAnchors.push_back(point_t(mX + 2 * mWidth / 3, mY + mHeight));
    mAnchors.push_back(point_t(mX + mWidth, mY + mHeight));
}

void Transition::displace(int deltax, int deltay)
{
    mX += deltax;
    mY += deltay;

    for (points_t::iterator it = mAnchors.begin();
         it != mAnchors.end(); ++it) {
        it->x += deltax;
        it->y += deltay;
    }
}

void Transition::height(int height)
{
    mHeight = height;
    computeAnchors();
}

void Transition::width(int width)
{
    mWidth = width;
    computeAnchors();
}

std::string Transition::toString() const
{
    std::string delay;

    if (mDelay) {
        delay = (fmt("%1%") % mDelayValue).str();
    }
    return (fmt("%1%,%2%,%3%,%4%,%5%,%6%,%7%,%8%,%9%,%10%,%11%")
            % mName % mX % mY % mWidth % mHeight % mInput %
            mInputPortName %
            mOutput % mOutputPortName % delay % mPriority).str();
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

Arc::Arc(const std::string& conf)
{
    strings_t transition;

    boost::split(transition, conf, boost::is_any_of(","));
    mSource = transition[0];
    mDestination = transition[1];
    mWeight = boost::lexical_cast < unsigned int > (transition[2]);
    mInhibitor = boost::lexical_cast < bool > (transition[3]);

    strings_t pts;

    boost::split(pts, transition[4], boost::is_any_of("!"));
    for (unsigned int i = 0; i < pts.size() - 1; ++i) {
        strings_t pt;

        boost::split(pt, pts[i], boost::is_any_of("/"));
        mPoints.push_back(point_t(boost::lexical_cast < int > (pt[0]),
                boost::lexical_cast < int > (pt[1])));
    }
}

std::string Arc::toString() const
{
    std::string pts;

    for (points_t::const_iterator it = mPoints.begin();
         it != mPoints.end(); ++it) {
        pts += (fmt("%1%/%2%!") % it->x % it->y).str();
    }

    return (fmt("%1%,%2%,%3%,%4%,%5%") % mSource % mDestination %
            mWeight % mInhibitor % pts).str();
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

void PetriNet::displace(Place* place, int deltax, int deltay)
{
    place->displace(deltax, deltay);
    for (arcs_t::iterator it = mArcs.begin(); it != mArcs.end(); ++it) {
        if ((*it)->source() == place->name()) {
            (*it)->displaceSource(deltax, deltay);
        }
        if ((*it)->source() != (*it)->destination() and
            (*it)->destination() == place->name()) {
            (*it)->displaceDestination(deltax, deltay);
        }
    }
}

void PetriNet::displace(Transition* transition, int deltax, int deltay)
{
    transition->displace(deltax, deltay);
    for (arcs_t::iterator it = mArcs.begin();
         it != mArcs.end(); ++it) {
        if ((*it)->source() == transition->name()) {
            (*it)->displaceSource(deltax, deltay);
        }
        if ((*it)->source() != (*it)->destination() and
            (*it)->destination() == transition->name()) {
            (*it)->displaceDestination(deltax, deltay);
        }
    }
}

void PetriNet::removePlace(Place* place)
{
    arcs_t::iterator it = mArcs.begin();

    while (it != mArcs.end()) {
        if ((*it)->source() == place->name() or
            (*it)->destination() == place->name()) {
            mArcs.erase(it);
            it = mArcs.begin();
        } else {
            ++it;
        }
    }
    mPlaces.erase(place->name());
}

void PetriNet::removeTransition(Transition* transition)
{
    arcs_t::iterator it = mArcs.begin();

    while (it != mArcs.end()) {
        if ((*it)->source() == transition->name() or
            (*it)->destination() == transition->name()) {
            mArcs.erase(it);
            it = mArcs.begin();
        } else {
            ++it;
        }
    }
    mTransitions.erase(transition->name());
}

void PetriNet::removeArc(Arc* arc)
{
    arcs_t::iterator it = std::find(mArcs.begin(), mArcs.end(), arc);

    mArcs.erase(it);
}

}
}
}
}    // namespace vle gvle modeling petrinet
