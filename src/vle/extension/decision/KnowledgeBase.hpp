/*
 * @file vle/extension/decision/KnowledgeBase.hpp
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


#ifndef VLE_EXTENSION_DECISION_KNOWLEDGEBASE_HPP
#define VLE_EXTENSION_DECISION_KNOWLEDGEBASE_HPP

#include <vle/extension/decision/Facts.hpp>
#include <vle/extension/decision/Rules.hpp>
#include <vle/extension/decision/Activities.hpp>
#include <vle/extension/decision/Table.hpp>
#include <vle/extension/DllDefines.hpp>

namespace vle { namespace extension { namespace decision {

typedef Table < Fact > FactsTable;
typedef Table < Predicate > PredicatesTable;
typedef Table < Activity::AckFct > AcknowledgeFunctions;
typedef Table < Activity::OutFct > OutputFunctions;
typedef Table < Activity::UpdateFct > UpdateFunctions;

inline std::ostream& operator<<(std::ostream& o, const FactsTable& kb)
{
    o << "facts: ";
    for (FactsTable::const_iterator it = kb.begin(); it != kb.end(); ++it) {
        o << " (" << it->first << ")";
    }
    return o;
}

template < typename F >
struct f
{
    f(const std::string& name, F func)
        : name(name), func(func)
    {}

    std::string name;
    F func;
};

template < typename F >
struct p
{
    p(const std::string& name, F func)
        : name(name), func(func)
    {}

    std::string name;
    F func;
};

template < typename F >
struct o
{
    o(const std::string& name, F func)
        : name(name), func(func)
    {}

    std::string name;
    F func;
};

template < typename F >
struct a
{
    a(const std::string& name, F func)
        : name(name), func(func)
    {}

    std::string name;
    F func;
};

template < typename F >
struct u
{
    u(const std::string& name, F func)
        : name(name), func(func)
    {}

    std::string name;
    F func;
};

template < typename X >
struct AddFacts
{
    AddFacts(X kb) : kb(kb) {}

    X kb;
};


template < typename X >
struct AddPredicates
{
    AddPredicates(X kb) : kb(kb) {}

    X kb;
};

template < typename X >
struct AddOutputFunctions
{
    AddOutputFunctions(X kb) : kb(kb) {}

   X kb;
};

template < typename X >
struct AddAcknowledgeFunctions
{
    AddAcknowledgeFunctions(X kb) : kb(kb) {}

    X kb;
};

template < typename X >
struct AddUpdateFunctions
{
    AddUpdateFunctions(X kb) : kb(kb) {}

    X kb;
};

/**
 * @brief KnowledgeBase stores facts, rules, activites and precedence
 * constraints to build a plan activity model.
 * @code
 * // To add facts, predicates, outputs and acknowledge functions, use the
 * // following methods:
 * addFacts(this) +=
 *      F("rain", &KnowledgeBase::rain);
 *
 * addPredicates(this) +=
 *      P("pred 1", &KnowledgeBase::isAlwaysTrue),
 *      P("pred 2", &KnowledgeBase::isAlwaysFalse),
 *      P("pred 3", &KnowledgeBase::randIsTrue,
 *      P("pred 4", &KnowledgeBase::randIsFalse);
 *
 * addOutputFunctions(this) +=
 *      O("output function", &KnowledgeBase::out);
 *
 * addAcknowledgeFunctions(this) +=
 *      A("ack function", &KnowledgeBase::ack);
 *
 * // And use a Parser to fill KnowledgeBase with datas.
 * @endcode
 */
class VLE_EXTENSION_EXPORT KnowledgeBase
{
public:
    /**
     * @brief Define the return of the processChanges function. A boolean
     * if this function has changed at least one activity and a date to
     * wake up the knowledge base.
     */
    typedef std::pair < bool, devs::Time > Result;

    /**
     * @brief Add a fac into the facts tables.
     * @param name
     * @param fact
     */
    void addFact(const std::string& name, const Fact& fact)
    { mFactsTable.add(name, fact); }

    void applyFact(const std::string& name, const value::Value& value)
    { mFactsTable[name](value); }

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
     * @param name Name of the activity to done.
     * @param date Date when activity failed.
     * @throw utils::ArgError if activity is not in START state.
     */
    void setActivityDone(const std::string& name,
                         const devs::Time& date);
    /**
     * @brief Change the stage of the activity from * to FAILED. This function
     * does nothing if activity is already in FAILED state.
     * @param name Name of the activity to failed.
     * @param date Date when activity failed.
     * @throw utils::ArgError if activity is in FAILED state.
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

    /**
     * @brief Return true if at least on list of activities (waited, started,
     * failed, done or ended lists) is not empty.
     * @return true or false.
     */
    bool haveActivityInLatestActivitiesLists() const
    {
        return not (m_activities.latestWaitedAct().empty() and
                    m_activities.latestStartedAct().empty() and
                    m_activities.latestFailedAct().empty() and
                    m_activities.latestDoneAct().empty() and
                    m_activities.latestEndedAct().empty());
    }

    /**
     * @brief Clear the lists of activities (waited, started, failed, done or
     * ended lists).
     */
    void clearLatestActivitiesLists()
    { m_activities.clearLatestActivitiesLists(); }

    /**
     * @brief Get the table of available facts.
     * @return Table of available facts.
     */
    const FactsTable& facts() const { return mFactsTable; }

    /**
     * @brief Get the table of available predicates.
     * @return Table of available predicates.
     */
    const PredicatesTable& predicates() const { return mPredicatesTable; }

    /**
     * @brief Get the table of available acknowledge functions.
     * @return Table of available acknowledge functions.
     */
    const AcknowledgeFunctions& acknowledgeFunctions() const
    { return mAckFunctions; }

    /**
     * @brief Get the table of available update functions.
     * @return Table of available update functions.
     */
    const UpdateFunctions& updateFunctions() const
    { return mUpdateFunctions; }

    /**
     * @brief Get the table of available output functions;
     * @return Table of available output functions.
     */
    const OutputFunctions& outputFunctions() const { return mOutFunctions; }

    /**
     * @brief Get the table of available facts.
     * @return Table of available facts.
     */
    FactsTable& facts() { return mFactsTable; }

    /**
     * @brief Get the table of available predicates.
     * @return Table of available predicates.
     */
    PredicatesTable& predicates() { return mPredicatesTable; }

    /**
     * @brief Get the table of available acknowledge functions.
     * @return Table of available acknowledge functions.
     */
    AcknowledgeFunctions& acknowledgeFunctions() { return mAckFunctions; }

    /**
     * @brief Get the table of available output functions;
     * @return Table of available output functions.
     */
    OutputFunctions& outputFunctions() { return mOutFunctions; }

    /**
     * @brief Get the table of available update functions.
     * @return Table of available update functions.
     */
    UpdateFunctions& updateFunctions() { return mUpdateFunctions; }

    template < typename X >
        AddFacts < X > addFacts(X obj)
        {
            return AddFacts < X >(obj);
        }

    template < typename X >
        f < X > F(const std::string& name, X func)
        {
            return f < X >(name, func);
        }

    template < typename X >
        AddPredicates < X > addPredicates(X obj)
        {
            return AddPredicates < X >(obj);
        }

    template < typename X >
        p < X > P(const std::string& name, X func)
        {
            return p < X >(name, func);
        }

    template < typename X >
        AddOutputFunctions < X > addOutputFunctions(X obj)
        {
            return AddOutputFunctions < X >(obj);
        }

    template < typename X >
        o < X > O(const std::string& name, X func)
        {
            return o < X >(name, func);
        }

    template < typename X >
        AddAcknowledgeFunctions < X > addAcknowledgeFunctions(X obj)
        {
            return AddAcknowledgeFunctions < X >(obj);
        }

    template < typename X >
        a < X > A(const std::string& name, X func)
        {
            return a < X >(name, func);
        }

    template < typename X >
        AddUpdateFunctions < X > addUpdateFunctions(X obj)
        {
            return AddUpdateFunctions < X >(obj);
        }

    template < typename X >
        u < X > U(const std::string& name, X func)
        {
            return u < X >(name, func);
        }

private:
    Rules m_rules;
    Activities m_activities;

    FactsTable mFactsTable;
    PredicatesTable mPredicatesTable;
    AcknowledgeFunctions mAckFunctions;
    OutputFunctions mOutFunctions;
    UpdateFunctions mUpdateFunctions;

    static void unionLists(Activities::result_t& last,
                           Activities::result_t& recent);
};

template < typename X, typename F >
AddFacts < X > operator+=(AddFacts < X > add, f < F > pred)
{
    add.kb->facts().add(pred.name, boost::bind(pred.func, add.kb, _1));
    return add;
}

template < typename X, typename F >
AddFacts < X > operator,(AddFacts < X > add, f < F > pred)
{
    add.kb->facts().add(pred.name, boost::bind(pred.func, add.kb, _1));
    return add;
}

template < typename X, typename F >
AddPredicates < X > operator+=(AddPredicates < X > add, p < F > pred)
{
    add.kb->predicates().add(pred.name, boost::bind(pred.func, add.kb));
    return add;
}

template < typename X, typename F >
AddPredicates < X > operator,(AddPredicates < X > add, p < F > pred)
{
    add.kb->predicates().add(pred.name, boost::bind(pred.func, add.kb));
    return add;
}

template < typename X, typename F >
AddAcknowledgeFunctions < X > operator+=(AddAcknowledgeFunctions < X > add,
                                         a < F > pred)
{
    add.kb->acknowledgeFunctions().add(pred.name,
                                       boost::bind(pred.func, add.kb, _1, _2));
    return add;
}

template < typename X, typename F >
AddAcknowledgeFunctions < X > operator,(AddAcknowledgeFunctions < X > add,
                                        a < F > pred)
{
    add.kb->acknowledgeFunctions().add(pred.name,
                                       boost::bind(pred.func, add.kb, _1, _2));
    return add;
}

template < typename X, typename F >
AddOutputFunctions < X > operator+=(AddOutputFunctions < X > add,
                                    o < F > pred)
{
    add.kb->outputFunctions().add(pred.name,
                                  boost::bind(pred.func, add.kb, _1, _2, _3));
    return add;
}

template < typename X, typename F >
AddOutputFunctions < X > operator,(AddOutputFunctions < X > add,
                                   o < F > pred)
{
    add.kb->outputFunctions().add(pred.name,
                                  boost::bind(pred.func, add.kb, _1, _2, _3));
    return add;
}

template < typename X, typename F >
AddUpdateFunctions < X > operator+=(AddUpdateFunctions < X > add,
                                    u < F > pred)
{
    add.kb->updateFunctions().add(pred.name,
                                  boost::bind(pred.func, add.kb, _1, _2));
    return add;
}

template < typename X, typename F >
AddUpdateFunctions < X > operator,(AddUpdateFunctions < X > add,
                                   u < F > pred)
{
    add.kb->updateFunctions().add(pred.name,
                                  boost::bind(pred.func, add.kb, _1, _2));
    return add;
}

inline std::ostream& operator<<(std::ostream& o, const KnowledgeBase& kb)
{
    return o << kb.facts() << "\n" << kb.rules() << "\n"
        << kb.activities() << "\n";
}

struct VLE_EXTENSION_EXPORT CompareActivities
{
    inline bool operator()(Activities::iterator x, Activities::iterator y) const
    {
        return &x->second < &y->second;
    }
};

}}} // namespace vle model decision

#endif
