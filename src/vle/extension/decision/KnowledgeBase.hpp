/**
 * @file vle/extension/decision/KnowledgeBase.hpp
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


#ifndef VLE_EXTENSION_DECISION_KNOWLEDGEBASE_HPP
#define VLE_EXTENSION_DECISION_KNOWLEDGEBASE_HPP

#include <vle/extension/decision/Facts.hpp>
#include <vle/extension/decision/Rules.hpp>
#include <vle/extension/decision/Activities.hpp>
#include <vle/extension/DllDefines.hpp>

namespace vle { namespace extension { namespace decision {

class VLE_EXTENSION_EXPORT KnowledgeBase
{
public:
    /**
     * @brief Define the return of the processChanges function. A boolean
     * if this function has changed at least one activity and a date to
     * wake up the knowledge base.
     */
    typedef std::pair < bool, devs::Time > Result;

    void addFact(const std::string& name, const Fact& fact)
    { m_facts.add(name, fact); }

    void applyFact(const std::string& name, const value::Value& value)
    { m_facts.apply(name, value); }

    Rule& addRule(const std::string& name)
    { return m_rules.add(name); }

    Rule& addRule(const std::string& name, const Rule& rule)
    { return m_rules.add(name, rule); }

    Rule& addPredicateToRule(const std::string& name, const Predicate& pred)
    { return m_rules.add(name, pred); }

    Activity& addActivity(const std::string& name)
    { return m_activities.add(name); }

    Activity& addActivity(const std::string& name,
                          const devs::Time& start,
                          const devs::Time& finish)
    { return m_activities.add(name, start, finish); }

    Activity& addActivity(const std::string& name,
                          const Activity& act)
    { return m_activities.add(name, act); }

    /**
     * @brief The predecessor activity (i) must start before the successor
     * activity (j) can start.
     * @param acti The (i) activity in SiSj relationship.
     * @param actj The (j) activity in SiSj relationship.
     * @param maxtimelag time lag in SiSj relationship.
     */
    void addStartToStartConstraint(const std::string& acti,
                                   const std::string& actj,
                                   const devs::Time& maxtimelag)
    { m_activities.addStartToStartConstraint(acti, actj, 0, maxtimelag); }

    /**
     * @brief The predecessor activity (i) must start before the successor
     * activity (j) can start.
     * @param acti The (i) activity in SiSj relationship.
     * @param actj The (j) activity in SiSj relationship.
     * @param mintimelag time lag in SiSj relationship.
     * @param maxtimelag time lag in SiSj relationship.
     */
    void addStartToStartConstraint(const std::string& acti,
                                   const std::string& actj,
                                   const devs::Time& mintimelag,
                                   const devs::Time& maxtimelag)
    { m_activities.addStartToStartConstraint(acti, actj, mintimelag,
                                             maxtimelag); }

    /**
     * @brief The predecessor activity (i) must finish before the successor
     * activity (j) can start.
     * @param acti The (i) activity in FiSj relationship.
     * @param actj The (j) activity in FiSj relationship.
     * @param mintimelag minimal time lag in FiSj relationship.
     * @param maxtimelag maximal time lag in FiSj relationship.
     */
    void addFinishToStartConstraint(const std::string& acti,
                                    const std::string& actj,
                                    const devs::Time& mintimelag,
                                    const devs::Time& maxtimelag)
    { m_activities.addFinishToStartConstraint(
            acti, actj, mintimelag, maxtimelag); }

    /**
     * @brief The predecessor activity (i) must finish before the successor
     * activity (j) can finish.
     * @param acti The (i) activity in FiFj relationship.
     * @param actj The (j) activity in FiFj relationship.
     * @param timelag time lag in FiFj relationship.
     */
    void addFinishToFinishConstraint(const std::string& acti,
                                     const std::string& actj,
                                     const devs::Time& maxtimelag)
    { m_activities.addFinishToFinishConstraint(acti, actj, 0, maxtimelag); }

    /**
     * @brief The predecessor activity (i) must finish before the successor
     * activity (j) can finish.
     * @param acti The (i) activity in FiFj relationship.
     * @param actj The (j) activity in FiFj relationship.
     * @param mintimelag time lag in FiFj relationship.
     * @param maxtimelag time lag in FiFj relationship.
     */
    void addFinishToFinishConstraint(const std::string& acti,
                                     const std::string& actj,
                                     const devs::Time& mintimelag,
                                     const devs::Time& maxtimelag)
    { m_activities.addFinishToFinishConstraint(
            acti, actj, mintimelag, maxtimelag); }

    const Facts& facts() const { return m_facts; }
    const Rules& rules() const { return m_rules; }
    const Activities& activities() const { return m_activities; }

    /**
     * @brief Compute the next date when change in activity status.
     * @param time The current time.
     * @return A date in range ]devs::Time::negativeInfinity,
     * devs::Time::infinity[.
     */
    devs::Time nextDate(const devs::Time& time)
    { return m_activities.nextDate(time); }

    devs::Time duration(const devs::Time& time);

    /**
     * @brief This function is call to change the states of the activities.
     * @return The next date on which the knowledge base must be change
     * (temporal constraints, temporal precedence constraints)
     * ]-devs::negativeInfinity, devs::infinity[ and a boolean if a change
     * has come.
     */
    Result processChanges(const devs::Time& time);

    /**
     * @brief Change the stage of the activity from STARTED to DONE.
     * @param name.
     */
    void setActivityDone(const std::string& name,
                         const devs::Time& date);
    /**
     * @brief Change the stage of the activity from * to FAILED.
     * @param name.
     */
    void setActivityFailed(const std::string& name,
                           const devs::Time& date);

    const Activities::result_t& waitedActivities() const
    { return m_activities.waitedAct(); }

    const Activities::result_t& startedActivities() const
    { return m_activities.startedAct(); }

    const Activities::result_t& failedActivities() const
    { return m_activities.failedAct(); }

    const Activities::result_t& doneActivities() const
    { return m_activities.doneAct(); }

    const Activities::result_t& endedActivities() const
    { return m_activities.endedAct(); }

    const Activities::result_t& latestWaitedActivities() const
    { return m_activities.latestWaitedAct(); }

    const Activities::result_t& latestStartedActivities() const
    { return m_activities.latestStartedAct(); }

    const Activities::result_t& latestFailedActivities() const
    { return m_activities.latestFailedAct(); }

    const Activities::result_t& latestDoneActivities() const
    { return m_activities.latestDoneAct(); }

    const Activities::result_t& latestEndedActivities() const
    { return m_activities.latestEndedAct(); }

    void clearLatestActivitiesLists()
    { m_activities.clearLatestActivitiesLists(); }

private:
    Facts m_facts;
    Rules m_rules;
    Activities m_activities;

    static void unionLists(Activities::result_t& last,
                           Activities::result_t& recent);
};

inline std::ostream& operator<<(std::ostream& o, const KnowledgeBase& kb)
{
    return o << kb.facts() << "\n" << kb.rules() << "\n"
        << kb.activities() << "\n";
}

struct VLE_EXTENSION_EXPORT CompareActivities
{
    inline bool
        operator()(Activities::iterator x, Activities::iterator y) const
        {
            return &x->second < &y->second;
        }
};

}}} // namespace vle model decision

#endif
