/**
 * @file vle/extension/decision/test/activity.cpp
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


#define BOOST_TEST_MAIN
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE test_activity
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <iostream>
#include <iterator>
#include <vle/value/Double.hpp>
#include <vle/extension/decision/KnowledgeBase.hpp>

namespace vmd = vle::extension::decision;
namespace vd = vle::devs;
using vle::fmt;

namespace ex {

    class KnowledgeBase : public vmd::KnowledgeBase
    {
    public:
        KnowledgeBase()
            : vmd::KnowledgeBase(), today(0), yesterday(0)
        {
            addFact("today", boost::bind(&ex::KnowledgeBase::date, this, _1));

            vmd::Rule& r1 = addRule("Rule 1");
            r1.add(boost::bind(&ex::KnowledgeBase::haveGoodTemp, this));
            r1.add(boost::bind(&ex::KnowledgeBase::isAlwaysTrue, this));

            vmd::Rule& r2 = addRule("Rule 2");
            r2.add(boost::bind(&ex::KnowledgeBase::haveGoodTemp, this));

            vmd::Activity& act1 = addActivity("act1");
            act1.addRule("Rule 1", r1);

            vmd::Activity& act2 = addActivity("act2");
            act2.addRule("Rule 2", r2);
            act2.initStartTimeFinishTime(1.5, 2.5);

            vmd::Activity& act3 = addActivity("act3");
            act3.addRule("Rule 2", r2);
            act3.initStartTimeFinishTime(3.5, 4.5);
        }

        virtual ~KnowledgeBase() {}

        /*
         * list of facts
         */

        void date(const vle::value::Value& val)
        {
            yesterday = today;
            today = val.toDouble().value();
        }

        /*
         * list of predicates
         */

        bool isAlwaysTrue() const
        {
            return true;
        }

        bool haveGoodTemp() const
        {
            return yesterday > 15.0 and today > 20.0;
        };

        double today, yesterday;
    };

    class KnowledgeBaseGraph : public vmd::KnowledgeBase
    {
    public:
        KnowledgeBaseGraph()
        {
            addActivity("act1");
            addActivity("act2");
            addActivity("act3");
            addActivity("act4");
            addActivity("act5");

            addFinishToStartConstraint("act1", "act2", 0.0, 1.0);
            addFinishToStartConstraint("act2", "act3", 0.0, 1.0);
            addFinishToStartConstraint("act2", "act4", 0.0, 1.0);
        }

        virtual ~KnowledgeBaseGraph() {}
    };

    class KnowledgeBaseGraph2 : public vmd::KnowledgeBase
    {
    public:
        KnowledgeBaseGraph2()
        {
            addActivity("A");
            addActivity("B");
            addActivity("C");
            addActivity("D");
            addActivity("E");
            addActivity("F");
            addActivity("G");

            addFinishToStartConstraint("A", "B", 0.0, 1.0);
            addFinishToStartConstraint("B", "C", 0.0, 1.0);
            addFinishToStartConstraint("B", "D", 0.0, 1.0);
            addFinishToStartConstraint("C", "E", 0.0, 1.0);
            addFinishToStartConstraint("D", "F", 0.0, 1.0);
            addFinishToStartConstraint("E", "G", 0.0, 1.0);
            addFinishToStartConstraint("F", "G", 0.0, 1.0);

            addStartToStartConstraint("C", "D", 0.0);

            addFinishToFinishConstraint("E", "F", 0.0);
        }

        virtual ~KnowledgeBaseGraph2() {}
    };

    class KnowledgeBaseGraph3 : public vmd::KnowledgeBase
    {
    public:
        KnowledgeBaseGraph3()
        {
            addActivity("A");
            addActivity("B");
            addActivity("C");
            addActivity("D");
            addActivity("E");
            addActivity("F");

            addFinishToStartConstraint("A", "B", 0.0, 1.0);
            addFinishToStartConstraint("A", "C", 0.0, 1.0);
            addFinishToStartConstraint("A", "D", 0.0, 1.0);
            addFinishToStartConstraint("B", "F", 0.0, 1.0);
            addFinishToStartConstraint("C", "E", 0.0, 1.0);
            addFinishToStartConstraint("D", "E", 0.0, 1.0);
            addFinishToStartConstraint("E", "F", 0.0, 1.0);
        }

        virtual ~KnowledgeBaseGraph3() {}
    };

    class KB4 : public vmd::KnowledgeBase
    {
    public:
        KB4()
        {
            addActivity("A", 0.0, vd::Time::infinity);
            addActivity("B", 1.0, vd::Time::infinity);
            addActivity("C", 1.0, vd::Time::infinity);
            addActivity("D", 2.0, vd::Time::infinity);
            addActivity("E", 3.0, vd::Time::infinity);
            addActivity("F", 4.0, vd::Time::infinity);
        }

        virtual ~KB4() {}
    };

} // namespace ex

BOOST_AUTO_TEST_CASE(kb)
{
    vle::value::init();

    ex::KnowledgeBase base;

    vmd::Activities::result_t lst;
    double date = 0.0;

    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(0));
    ++date;

    base.applyFact("today", vle::value::Double(16));
    base.processChanges(date);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(0));
    ++date;

    base.applyFact("today", vle::value::Double(21));
    base.processChanges(date);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));
    ++date;

    base.applyFact("today", vle::value::Double(18));
    base.processChanges(date);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.waitedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));
    lst = base.failedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));
    ++date;

    base.applyFact("today", vle::value::Double(22));
    base.processChanges(date);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));
}

BOOST_AUTO_TEST_CASE(kb2)
{
    vle::value::init();

    ex::KnowledgeBaseGraph base;

    vmd::Activities::result_t lst;

    base.processChanges(0.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(0.0);
    lst = base.waitedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(3));

    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));

    for (vmd::Activities::result_t::iterator it = lst.begin(); it !=
         lst.end(); ++it) {
        (*it)->second.end(0.0);
    }

    base.processChanges(0.0);
    lst = base.waitedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));

    for (vmd::Activities::result_t::iterator it = lst.begin(); it !=
         lst.end(); ++it) {
        (*it)->second.end(0.0);
    }

    base.processChanges(0.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));

    for (vmd::Activities::result_t::iterator it = lst.begin(); it !=
         lst.end(); ++it) {
        (*it)->second.end(0.0);
    }

    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));
}

BOOST_AUTO_TEST_CASE(kb3)
{
    vle::value::init();

    ex::KnowledgeBaseGraph2 base;

    vmd::Activities::result_t lst;

    base.processChanges(0.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));
    base.setActivityDone("A", 0.0);

    base.processChanges(0.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));

    base.processChanges(0.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));
    base.setActivityDone("B", 0.0);

    base.processChanges(0.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(0.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));
    base.setActivityDone("C", 0.0);

    base.processChanges(0.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));

    base.processChanges(0.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));
    base.setActivityDone("D", 0.0);

    base.processChanges(0.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));
    base.setActivityDone("E", 0.0);

    base.processChanges(0.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));
    base.setActivityDone("F", 0.0);

    base.processChanges(0.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));
}

BOOST_AUTO_TEST_CASE(Activity4)
{
    vle::value::init();

    ex::KnowledgeBaseGraph3 base;

    vmd::Activities::result_t lst;

    base.processChanges(0.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(1));
    base.setActivityDone("A", 0.0);

    base.processChanges(0.0);
    lst = base.startedActivities();
    BOOST_REQUIRE_EQUAL(lst.size(), vmd::Activities::result_t::size_type(3));
    base.setActivityDone("B", 0.0);
}

BOOST_AUTO_TEST_CASE(ActivitiesnextDate1)
{
    vle::value::init();

    ex::KB4 base;

    vmd::Activities::result_t lst;

    BOOST_REQUIRE_CLOSE((double)base.nextDate(0.), 0., 1.);
    base.processChanges(0.);
    base.setActivityDone("A", 0.5);
    BOOST_REQUIRE_CLOSE((double)base.nextDate(1.), 1., 1.);
    base.processChanges(1.);
    base.setActivityDone("B", 1.5);
    BOOST_REQUIRE_CLOSE((double)base.nextDate(1.), 1., 1.);
    base.processChanges(1.);
    base.setActivityDone("C", 1.5);
    BOOST_REQUIRE_CLOSE((double)base.nextDate(2.), 2., 1.);
    base.processChanges(2.);
    base.setActivityDone("D", 2.5);
    BOOST_REQUIRE_CLOSE((double)base.nextDate(3.), 3., 1.);
    base.processChanges(3.);
    base.setActivityDone("E", 3.5);
    BOOST_REQUIRE_CLOSE((double)base.nextDate(4.), 4., 1.);
    base.processChanges(4.);
    base.setActivityDone("F", 4.5);
}

BOOST_AUTO_TEST_CASE(ActivitiesnextDate2)
{
    vle::value::init();

    ex::KB4 base;

    vmd::Activities::result_t lst;

    BOOST_REQUIRE_CLOSE((double)base.duration(0.), 0., 1.);
    base.processChanges(0.);
    base.setActivityDone("A", 0.5);
    BOOST_REQUIRE_CLOSE((double)base.duration(.5), .5, 1.);
    base.processChanges(1.);
    base.setActivityDone("B", 1.);
    BOOST_REQUIRE_CLOSE((double)base.duration(1.), 0., 1.);
    base.processChanges(1.);
    base.setActivityDone("C", 1.5);
    BOOST_REQUIRE_CLOSE((double)base.duration(1.5), .5, 1.);
    base.processChanges(2.);
    base.setActivityDone("D", 2.5);
    BOOST_REQUIRE_CLOSE((double)base.duration(2.5), .5, 1.);
    base.processChanges(3.);
    base.setActivityDone("E", 3.5);
    BOOST_REQUIRE_CLOSE((double)base.duration(3.5), .5, 1.);
    base.processChanges(4.);
    base.setActivityDone("F", 4.5);
}
