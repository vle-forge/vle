/**
 * @file vle/extension/decision/Activities.hpp
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


#ifndef VLE_EXTENSION_DECISION_ACTIVITIES_HPP
#define VLE_EXTENSION_DECISION_ACTIVITIES_HPP

#include <vle/extension/decision/Activity.hpp>
#include <vle/extension/decision/PrecedenceConstraint.hpp>
#include <vle/extension/decision/PrecedencesGraph.hpp>
#include <vle/utils/Exception.hpp>

namespace vle { namespace extension { namespace decision {

    class Activities
    {
    public:
        typedef std::map < std::string, Activity > activities_t;
        typedef activities_t::const_iterator const_iterator;
        typedef activities_t::iterator iterator;
        typedef activities_t::size_type size_type;
        typedef std::vector < iterator > result_t;
        typedef std::vector < const_iterator > const_result_t;

        Activity& add(const std::string& name, const Activity& act);
        Activity& add(const std::string& name);
        Activity& add(const std::string& name,
                      const devs::Time& start,
                      const devs::Time& end);

	void addPrecedenceConstraint(const PrecedenceConstraint& pc)
	{ m_graph.add(pc); }

        /**
         * @brief The predecessor activity (i) must start before the successor
         * activity (j) can start.
         * @param acti The (i) activity in SiSj relationship.
         * @param actj The (j) activity in SiSj relationship.
         * @param timelag time lag in SiSj relationship.
         */
        void addStartToStartConstraint(const std::string& acti,
                                       const std::string& actj,
                                       const devs::Time& timelag);

        /**
         * @brief The predecessor activity (i) must finish before the successor
         * activity (j) can finish.
         * @param acti The (i) activity in FiFj relationship.
         * @param actj The (j) activity in FiFj relationship.
         * @param timelag time lag in FiFj relationship.
         */
        void addFinishToFinishConstraint(const std::string& acti,
                                         const std::string& actj,
                                         const devs::Time& timelag);

        /**
         * @brief The predecessor activity (i) must finish in (done or finished
         * state) before the successor activity (j) can start.
         * @param acti The (i) activity in FiSj relationship.
         * @param actj The (j) activity in FiSj relationship.
         * @param mintimelag min time lag in FiSj relationship, >= 0.0.
         * @param maxtimelag time lag in FiSj relationship >= mintimelag.
         */
        void addFinishToStartConstraint(const std::string& acti,
                                        const std::string& actj,
                                        const devs::Time& mintimelag,
                                        const devs::Time& maxtimelag);

        /**
         * @brief The predecessor activity (i) must finish in (done or finished
         * state) before the successor activity (j) can start.
         * @param acti The (i) activity in FiSj relationship.
         * @param actj The (j) activity in FiSj relationship.
         * @param mintimelag min time lag in FiSj relationship, >= 0.0.
         */
        void addFinishToStartConstraint(const std::string& acti,
                                        const std::string& actj,
                                        const devs::Time& mintimelag);

        /**
         * @brief Change the state of activities
         * @param time
         * @param waitact
         * @param starttact
         * @param doneact
         * @param failedact
         * @param endedact
         */
        void process(const devs::Time& time, result_t& waitact,
                     result_t& starttact, result_t& doneact,
                     result_t& failedact, result_t& endedact);

        iterator begin() { return m_lst.begin(); }
        const_iterator begin() const { return m_lst.begin(); }
        iterator end() { return m_lst.end(); }
        const_iterator end() const { return m_lst.end(); }
        size_type size() const { return m_lst.size(); }

	const_iterator get(const std::string& name) const
	{
	    const_iterator it = m_lst.find(name);
	    if (it == m_lst.end()) {
		throw utils::ArgError("Decision: unknow activity");
	    }
	    return it;
	}

	iterator get(const std::string& name)
	{
	    iterator it = m_lst.find(name);
	    if (it == m_lst.end()) {
		throw utils::ArgError("Decision: unknow activity");
	    }
	    return it;
	}

	const PrecedencesGraph& precedencesGraph() const
	{ return m_graph; }

    private:
        activities_t    m_lst;
        PrecedencesGraph m_graph;

        bool processWaitState(iterator activity, const devs::Time& time,
                              result_t& waitedact, result_t& startedact,
                              result_t& doneact, result_t& failedact,
                              result_t& endedact);

        bool processStartedState(iterator activity, const devs::Time& time,
                                 result_t& waitedact, result_t& startedact,
                                 result_t& doneact, result_t& failedact,
                                 result_t& endedact);

        bool processDoneState(iterator activity, const devs::Time& time,
                              result_t& waitedact, result_t& startedact,
                              result_t& doneact, result_t& failedact,
                              result_t& endedact);

        bool processFailedState(iterator activity, const devs::Time& time,
                                result_t& waitedact, result_t& startedact,
                                result_t& doneact, result_t& failedact,
                                result_t& endedact);

        bool processEndedState(iterator activity, const devs::Time& time,
                               result_t& waitedact, result_t& startedact,
                               result_t& doneact, result_t& failedact,
                               result_t& endedact);

        PrecedenceConstraint::Result updateState(iterator activity,
                                                 const devs::Time& time);
    };

    inline std::ostream& operator<<(
        std::ostream& o, const Activities& a)
    {
        o << "Activities: \n";
        for (Activities::activities_t::const_iterator it = a.begin(); it !=
             a.end(); ++it) {
            o << "  (" << it->first << "," << it->second << ")\n";
        }
        return o << "\n";
    }

    inline std::ostream& operator<<(
        std::ostream& o, const Activities::result_t& r)
    {
        o << "Activities: ";
        for (Activities::result_t::const_iterator it = r.begin(); it !=
             r.end(); ++it) {
            o << "(" << (*it)->first << ")";
            o << ((it + 1 == r.end()) ? "." : ",");
        }
        return o << "\n";
    }

}}} // namespace vle model decision

#endif
