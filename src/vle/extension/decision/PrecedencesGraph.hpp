/*
 * @file vle/extension/decision/PrecedencesGraph.hpp
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


#ifndef VLE_EXTENSION_DECISION_PRECEDENCESGRAPH_HPP
#define VLE_EXTENSION_DECISION_PRECEDENCESGRAPH_HPP

#include <vle/extension/decision/PrecedenceConstraint.hpp>
#include <vle/extension/DllDefines.hpp>
#include <set>

namespace vle { namespace extension { namespace decision {

class VLE_EXTENSION_EXPORT PrecedencesGraph
{
public:
    typedef std::multiset < PrecedenceConstraint,
            PrecedenceConstraintCompareFirst > PrecedencesOut;

    typedef std::multiset < PrecedenceConstraint,
            PrecedenceConstraintCompareSecond > PrecedencesIn;

    typedef PrecedencesIn::iterator iteratorIn;
    typedef PrecedencesOut::iterator iteratorOut;

    typedef std::pair < iteratorIn, iteratorOut > findIn;
    typedef std::pair < iteratorOut, iteratorOut > findOut;

    void add(const PrecedenceConstraint& p);

    findIn
        findPrecedenceIn(PrecedenceConstraint::iterator activity) const
        {
            return m_lstin.equal_range(
                PrecedenceConstraint(activity, activity,
                                     PrecedenceConstraint::FS,
                                     0.0, 0.0));
        }

    findOut
        findPrecedenceOut(PrecedenceConstraint::iterator activity) const
        {
            return m_lstout.equal_range(
                PrecedenceConstraint(activity, activity,
                                     PrecedenceConstraint::FS,
                                     0.0, 0.0));
        }

    const PrecedencesIn& in() const { return m_lstin; }
    const PrecedencesOut& out() const { return m_lstout; }

private:
    /** Defines the constraints. */
    PrecedencesIn m_lstin;
    PrecedencesOut m_lstout;
};

inline std::ostream&
operator<<(std::ostream& o, const PrecedencesGraph& p)
{
    o << "PrecedencesGraph:\n1) Out list:\n";
    for (PrecedencesGraph::iteratorOut i = p.in().begin();
         i != p.in().end(); ++i) {
        o << "- (" << (*i) << ")\n";
    }
    o << "PrecedencesGraph:\n1) In list:\n";
    for (PrecedencesGraph::iteratorIn i = p.out().begin();
         i != p.out().end(); ++i) {
        o << "- (" << (*i) << ")\n";
    }
    return o << "\n";
}

}}} // namespace vle model decision

#endif
