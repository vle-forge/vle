/**
 * @file vle/extension/decision/Activity.hpp
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


#ifndef VLE_EXTENSION_DECISION_ACTIVITY_HPP
#define VLE_EXTENSION_DECISION_ACTIVITY_HPP

#include <vle/extension/decision/Rules.hpp>
#include <vle/devs/Time.hpp>

namespace vle { namespace extension { namespace decision {

    class Activity
    {
    public:
        /** Defines the state of the temporal constraints. You can use multiple
         * value for a same DateType by combining the with operator|. For
         * instance:
         * @code
         * vle::model::decision::Activity::DateType v = START | FINISH;
         * @endcode
         */
        enum DateType {
            START = 1 << 0,  /**< Define a strong begin date activity. */
            FINISH = 1 << 1, /**< Define a strong end date activity. */
            MINS = 1 << 2,   /**< The minimum value of the begin range of the
                               activity. */
            MAXS = 1 << 3,   /**< The maximum value of the begin range of the
                               activity. */
            MINF = 1 << 4,   /**< The minimum value of the end range of the
                               activity. */
            MAXF = 1 << 5    /**< The maximum value of the end range of the
                               activity. */
        };

        /** Defines the state of an activity. */
        enum State {
            WAIT, /**< Before the activation of the activity. */
            STARTED, /**< If the activity is launched and operation too. */
            DONE, /**< Activity is finish, but need to check all FF precedence
                    constraint. */
            ENDED, /**< If the activity and operation are done. */
            FAILED /**< If the operation failed. */
        };

        Activity()
            : m_state(WAIT), m_waitall(true), m_date((Activity::DateType)0),
            m_start(devs::Time::negativeInfinity),
            m_finish(devs::Time::infinity),
            m_minstart(devs::Time::negativeInfinity),
            m_maxstart(devs::Time::negativeInfinity),
            m_minfinish(devs::Time::negativeInfinity),
            m_maxfinish(devs::Time::negativeInfinity),
            m_started(devs::Time::negativeInfinity),
            m_finished(devs::Time::negativeInfinity),
            m_done(devs::Time::negativeInfinity)
        {}

        void addRule(const std::string& name, const Rule& rule)
        { m_rules.add(name, rule); }

        Rule& addRule(const std::string& name)
        { return m_rules.add(name, Rule()); }

        void setRules(const Rules& rules)
        { m_rules = rules; }

        bool validRules() const;

        //
        // manage time constraint
        //

        void initStartTimeFinishTime(const devs::Time& start,
                                     const devs::Time& finish);

        void initStartTimeFinishRange(const devs::Time& time,
                                      const devs::Time& minfinish,
                                      const devs::Time& maxfinish);

        void initStartRangeFinishTime(const devs::Time& minstart,
                                      const devs::Time& maxstart,
                                      const devs::Time& time);

        void initStartRangeFinishRange(const devs::Time& minstart,
                                       const devs::Time& maxstart,
                                       const devs::Time& minfinish,
                                       const devs::Time& maxfinish);

        /**
         * @brief Activity have valid time constraint if the time is:
         * - starttime <= time
         * - time <= finishtime
         * - starttime <= time < finishtime
         *
         * @param time The time to check.
         * @return True if constraint are valid.
         */
        bool isValidTimeConstraint(const devs::Time& time) const;
        bool isBeforeTimeConstraint(const devs::Time& time) const;
        bool isAfterTimeConstraint(const devs::Time& time) const;

        const State& state() const { return m_state; }
        bool isInWaitState() const { return m_state == WAIT; }
        bool isInStartedState() const { return m_state == STARTED; }
        bool isInDoneState() const { return m_state == DONE; }
        bool isInFailedState() const { return m_state == FAILED; }
        bool isInEndedState() const { return m_state == ENDED; }

        void wait()
        { m_state = WAIT; }
        void start(const devs::Time& date)
        { m_state = STARTED; startedDate(date); }
        void done(const devs::Time& date)
        { m_state = DONE; doneDate(date); }
        void end(const devs::Time& date)
        { m_state = ENDED; finishedDate(date); }
        void fail(const devs::Time& date)
        { m_state = FAILED; finishedDate(date); }

        const Rules& rules() const { return m_rules; }
        const DateType& date() const { return m_date; }
        const devs::Time& start() const { return m_start; }
        const devs::Time& finish() const { return m_finish; }
        const devs::Time& minstart() const { return m_minstart; }
        const devs::Time& maxstart() const { return m_maxstart; }
        const devs::Time& minfinish() const { return m_minfinish; }
        const devs::Time& maxfinish() const { return m_maxfinish; }

        const devs::Time& startedDate() const { return m_started; }
        const devs::Time& finishedDate() const { return m_finished; }
        const devs::Time& doneDate() const { return m_done; }

        bool waitAllFsBeforeStart() const { return m_waitall; }
        bool waitOnlyOneFsBeforeStart() const { return not m_waitall; }
        void waitAllFs() { m_waitall = true; }
        void waitOnlyOneFs() { m_waitall = false; }

    private:
        void startedDate(const devs::Time& date) { m_started = date; }
        void doneDate(const devs::Time& date) { m_done = date; }
        void finishedDate(const devs::Time& date) { m_finished = date; }

        State m_state;
        Rules m_rules;

        bool m_waitall; /**< if true, all FF relationship must be valid, if
                          false, only one can be used. */

        DateType m_date;

        devs::Time m_start,;
        devs::Time m_finish;
        devs::Time m_minstart;
        devs::Time m_maxstart;
        devs::Time m_minfinish;
        devs::Time m_maxfinish;

        devs::Time m_started; /** Date when the activity is started. */
        devs::Time m_finished; /** Date when the activity is finished. */
        devs::Time m_done; /**< Date when the activity is done. */
    };

    inline std::ostream& operator<<(
        std::ostream& o, const Activity::State& dt)
    {
        return o << "state: " <<
            ((dt == Activity::WAIT) ? "wait" :
             (dt == Activity::STARTED) ? "started" :
             (dt == Activity::DONE) ? "done" :
             (dt == Activity::ENDED) ? "ended" : "failed");
    }

    inline std::ostream& operator<<(
        std::ostream& out, const Activity& a)
    {
        out << a.state() << " " << a.rules() << " Temporal ctr: ";

        switch (a.date() & (Activity::START | Activity::FINISH | Activity::MINS
                            | Activity::MAXS | Activity::MINF |
                            Activity::MAXF)) {
        case Activity::START:
        case Activity::FINISH:
        case Activity::START | Activity::FINISH:
            out << "[" << a.start().toString() << "," << a.finish().toString()
                << "]";
            break;
        case Activity::START | Activity::MAXF:
        case Activity::START | Activity::MINF:
        case Activity::START | Activity::MINF | Activity::MAXF:
            out << "[" << a.start().toString() << ", ["
                << a.minfinish().toString() << ","
                << a.maxfinish().toString() << "]]";
            break;
        case Activity::FINISH | Activity::MAXS:
        case Activity::FINISH | Activity::MINS:
        case Activity::FINISH | Activity::MINS | Activity::MAXS:
            out << "[[" << a.minstart().toString() << ","
                << a.maxstart().toString() << "],"
                << a.finish().toString() << "]";
            break;
        case Activity::MAXF:
        case Activity::MINF:
        case Activity::MINF | Activity::MAXF:
        case Activity::MAXS:
        case Activity::MAXS | Activity::MAXF:
        case Activity::MAXS | Activity::MINF:
        case Activity::MAXS | Activity::MINF | Activity::MAXF:
        case Activity::MINS:
        case Activity::MINS | Activity::MAXF:
        case Activity::MINS | Activity::MINF:
        case Activity::MINS | Activity::MINF | Activity::MAXF:
        case Activity::MINS | Activity::MAXS:
        case Activity::MINS | Activity::MAXS | Activity::MAXF:
        case Activity::MINS | Activity::MAXS | Activity::MINF:
        case Activity::MINS | Activity::MAXS | Activity::MINF | Activity::MAXF:
            out << "[["
                << a.minstart().toString() << ","
                << a.maxstart().toString() << "],["
                << a.minfinish().toString() << ","
                << a.maxfinish().toString() << "]]";
            break;
        default:
            out << "[internal error, report bug]";
        }

        if (not a.startedDate().isNegativeInfinity()) {
            out << " Started at: (" << a.startedDate().toString()<< ")";
        }

        if (not a.doneDate().isNegativeInfinity()) {
            out << " Done at: (" << a.doneDate().toString() << ")";
        }

        if (not a.finishedDate().isNegativeInfinity()) {
            out << " Finished at: (" << a.finishedDate().toString() << "))";
        }

        return out;
    }

}}} // namespace vle model decision

#endif
