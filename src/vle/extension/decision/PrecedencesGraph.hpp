/**
 * @file vle/extension/decision/PrecedencesGraph.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
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
#include <set>

namespace vle { namespace extension { namespace decision {

    class PrecedencesGraph
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
