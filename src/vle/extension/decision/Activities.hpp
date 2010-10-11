/*
 * @file vle/extension/decision/Activities.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
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


#ifndef VLE_EXTENSION_DECISION_ACTIVITIES_HPP
#define VLE_EXTENSION_DECISION_ACTIVITIES_HPP

#include <vle/extension/decision/Activity.hpp>
#include <vle/extension/decision/PrecedenceConstraint.hpp>
#include <vle/extension/decision/PrecedencesGraph.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/extension/DllDefines.hpp>

namespace vle { namespace extension { namespace decision {

class VLE_EXTENSION_EXPORT Activities
{
public:
    typedef std::map < std::string, Activity > activities_t;
    typedef activities_t::const_iterator const_iterator;
    typedef activities_t::iterator iterator;
    typedef activities_t::size_type size_type;
    typedef std::vector < iterator > result_t;
    typedef std::vector < const_iterator > const_result_t;

    /**
     * @brief Define the type of the process function. The first element of
     * the std::pair, a bool if the knowledge base needs update, the second
     * element, the next date to wake up the activity.
     */
    typedef std::pair < bool, devs::Time > Result;

    Activity& add(const std::string& name,
                  const Activity& act,
                  const Activity::OutFct& out = Activity::OutFct(),
                  const Activity::AckFct& ack = Activity::AckFct());
    Activity& add(const std::string& name,
                  const Activity::OutFct& out = Activity::OutFct(),
                  const Activity::AckFct& ack = Activity::AckFct());
    Activity& add(const std::string& name,
                  const devs::Time& start,
                  const devs::Time& end,
                  const Activity::OutFct& out = Activity::OutFct(),
                  const Activity::AckFct& ack = Activity::AckFct());

    void addPrecedenceConstraint(const PrecedenceConstraint& pc)
    { m_graph.add(pc); }

    /**
     * @brief The predecessor activity (i) must start before the successor
     * activity (j) can start.
     * @param acti The (i) activity in SiSj relationship.
     * @param actj The (j) activity in SiSj relationship.
     * @param maxtimelag time lag in SiSj relationship [0..maxtimelag].
     */
    void addStartToStartConstraint(const std::string& acti,
                                   const std::string& actj,
                                   const devs::Time& maxtimelag)
    { addStartToStartConstraint(acti, actj, 0, maxtimelag); }

    /**
     * @brief The predecessor activity (i) must start before the successor
     * activity (j) can start.
     * @param acti The (i) activity in SiSj relationship.
     * @param actj The (j) activity in SiSj relationship.
     * @param mintimelag min time lag in FiSj relationship, >= 0.0.
     * @param maxtimelag time lag in FiSj relationship >= mintimelag.
     */
    void addStartToStartConstraint(const std::string& acti,
                                   const std::string& actj,
                                   const devs::Time& mintimelag,
                                   const devs::Time& maxtimelag);

    /**
     * @brief The predecessor activity (i) must finish before the successor
     * activity (j) can finish.
     * @param acti The (i) activity in FiFj relationship.
     * @param actj The (j) activity in FiFj relationship.
     * @param maxtimelag time lag in FiFj relationship [0..maxtimelag].
     */
    void addFinishToFinishConstraint(const std::string& acti,
                                     const std::string& actj,
                                     const devs::Time& maxtimelag)
    { addFinishToFinishConstraint(acti, actj, 0, maxtimelag); }

    /**
     * @brief The predecessor activity (i) must finish before the successor
     * activity (j) can finish.
     * @param acti The (i) activity in FiFj relationship.
     * @param actj The (j) activity in FiFj relationship.
     * @param mintimelag min time lag in FiSj relationship, >= 0.0.
     * @param maxtimelag time lag in FiSj relationship >= mintimelag.
     */
    void addFinishToFinishConstraint(const std::string& acti,
                                     const std::string& actj,
                                     const devs::Time& mintimelag,
                                     const devs::Time& maxtimelag);

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
     * @param maxtimelag min time lag in FiSj relationship, >= 0.0.
     */
    void addFinishToStartConstraint(const std::string& acti,
                                    const std::string& actj,
                                    const devs::Time& maxtimelag)
    { addFinishToStartConstraint(acti, actj, 0, maxtimelag); }

    /**
     * @brief Compute the next date when change in activity status.
     * @param time The current time.
     * @return A date in range ]devs::Time::negativeInfinity,
     * devs::Time::infinity[.
     */
    devs::Time nextDate(const devs::Time& time);

    /**
     * @brief Change the state of activities
     * @param time
     * @return The next date on which a status change.
     */
    Result process(const devs::Time& time);

    /**
     * @brief Returns true if the activity exists, false otherwise
     * @param name the name of an activity
     * @return true if the activity exists, false otherwise
     */
    bool exist(const std::string& name) const
    {
        return (m_lst.find(name) != m_lst.end());
    }

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

    const Activities::result_t& waitedAct() const
    { return m_waitedAct; }
    const Activities::result_t& startedAct() const
    { return m_startedAct; }
    const Activities::result_t& failedAct() const
    { return m_failedAct; }
    const Activities::result_t& doneAct() const
    { return m_doneAct; }
    const Activities::result_t& endedAct() const
    { return m_endedAct; }

    const Activities::result_t& latestWaitedAct() const
    { return m_latestWaitedAct; }
    const Activities::result_t& latestStartedAct() const
    { return m_latestStartedAct; }
    const Activities::result_t& latestFailedAct() const
    { return m_latestFailedAct; }
    const Activities::result_t& latestDoneAct() const
    { return m_latestDoneAct; }
    const Activities::result_t& latestEndedAct() const
    { return m_latestEndedAct; }

    void removeWaitedAct(Activities::iterator it);
    void removeStartedAct(Activities::iterator it);
    void removeFailedAct(Activities::iterator it);
    void removeDoneAct(Activities::iterator it);
    void removeEndedAct(Activities::iterator it);
    void addWaitedAct(Activities::iterator it);
    void addStartedAct(Activities::iterator it);
    void addFailedAct(Activities::iterator it);
    void addDoneAct(Activities::iterator it);
    void addEndedAct(Activities::iterator it);

    void setWaitedAct(Activities::iterator it);
    void setStartedAct(Activities::iterator it);
    void setFailedAct(Activities::iterator it);
    void setDoneAct(Activities::iterator it);
    void setEndedAct(Activities::iterator it);

    static void removeAct(Activities::result_t& lst,
                          Activities::iterator it);
    static void addAct(Activities::result_t& lst,
                       Activities::iterator it);
    void updateLatestActivitiesList(Activities::result_t& lst,
                                    Activities::iterator it);

    void clearLatestActivitiesLists();

private:
    activities_t     m_lst;
    PrecedencesGraph m_graph;

    Activities::result_t m_waitedAct;
    Activities::result_t m_startedAct;
    Activities::result_t m_failedAct;
    Activities::result_t m_doneAct;
    Activities::result_t m_endedAct;

    Activities::result_t m_latestWaitedAct;
    Activities::result_t m_latestStartedAct;
    Activities::result_t m_latestFailedAct;
    Activities::result_t m_latestDoneAct;
    Activities::result_t m_latestEndedAct;


    Result processWaitState(iterator activity, const devs::Time& time);
    Result processStartedState(iterator activity, const devs::Time& time);
    Result processDoneState(iterator activity, const devs::Time& time);
    Result processFailedState(iterator activity, const devs::Time& time);
    Result processEndedState(iterator activity, const devs::Time& time);

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
