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

namespace vle { namespace extension { namespace decision {

    class KnowledgeBase
    {
    public:
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

        Activity& addActivity(const std::string& name, const Activity& act)
        { return m_activities.add(name, act); }

        /**
         * @brief The predecessor activity (i) must start before the successor
         * activity (j) can start.
         * @param acti The (i) activity in SiSj relationship.
         * @param actj The (j) activity in SiSj relationship.
         * @param timelag time lag in SiSj relationship.
         */
        void addStartToStartConstraint(const std::string& acti,
                                       const std::string& actj,
                                       const devs::Time& timelag = 0.0)
        { m_activities.addStartToStartConstraint(acti, actj, timelag); }

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
                                         const devs::Time& timelag = 0.0)
        { m_activities.addFinishToFinishConstraint(acti, actj, timelag); }

        const Facts& facts() const { return m_facts; }
        const Rules& rules() const { return m_rules; }
        const Activities& activities() const { return m_activities; }

        /**
         * @brief This function is call to change the states of the activities.
         */
        void processChanges(const devs::Time& time);

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
        { return m_waitedAct; }

        const Activities::result_t& startedActivities() const
        { return m_startedAct; }

        const Activities::result_t& failedActivities() const
        { return m_failedAct; }

        const Activities::result_t& doneActivities() const
        { return m_doneAct; }

        const Activities::result_t& endedActivities() const
        { return m_endedAct; }

        const Activities::result_t& latestWaitedActivities() const
        { return m_latestWaitedAct; }

        const Activities::result_t& latestStartedActivities() const
        { return m_latestStartedAct; }

        const Activities::result_t& latestFailedActivities() const
        { return m_latestFailedAct; }

        const Activities::result_t& latestDoneActivities() const
        { return m_latestDoneAct; }

        const Activities::result_t& latestEndedActivities() const
        { return m_latestEndedAct; }

    private:
        Facts m_facts;
        Rules m_rules;
        Activities m_activities;

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

        void buildLatestList(const Activities::result_t& in,
                             Activities::result_t& out);
    };

    inline std::ostream& operator<<(std::ostream& o, const KnowledgeBase& kb)
    {
        return o << kb.facts() << "\n" << kb.rules() << "\n"
            << kb.activities() << "\n";
    }

}}} // namespace vle model decision

#endif
