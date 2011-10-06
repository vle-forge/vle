/*
 * @file vle/extension/decision/test/kb.cpp
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


#define BOOST_TEST_MAIN
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE test_knowledge_base
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <iostream>
#include <iterator>
#include <vle/value/Double.hpp>
#include <vle/extension/decision/KnowledgeBase.hpp>

namespace vmd = vle::extension::decision;

namespace vle { namespace extension { namespace decision { namespace ex {

    class KnowledgeBase : public vmd::KnowledgeBase
    {
    public:
        KnowledgeBase()
            : vmd::KnowledgeBase(), today(0), yesterday(0)
        {
            addFact("today", boost::bind(&vmd::ex::KnowledgeBase::date, this, _1));

            vmd::Rule& r1 = addRule("Rule 1");
            r1.add(boost::bind(&vmd::ex::KnowledgeBase::haveGoodTemp, this));
            r1.add(boost::bind(&vmd::ex::KnowledgeBase::isAlwaysTrue, this));

            vmd::Rule& r2 = addRule("Rule 2");
            r2.add(boost::bind(&vmd::ex::KnowledgeBase::haveGoodTemp, this));

            vmd::Activity& act1 = addActivity("act1");
            act1.addRule("Rule 1", r1);

            vmd::Activity& act2 = addActivity("act2");
            act2.addRule("Rule 2", r2);
        }

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

}}}} // namespace vle extension decision ex

BOOST_AUTO_TEST_CASE(kb)
{
    vle::value::init();

    vmd::ex::KnowledgeBase base;

    vmd::Activities::result_t lst;

    base.processChanges(0.0);
    lst = base.startedActivities();
    BOOST_CHECK_EQUAL(lst.size(), vmd::Activities::result_t::size_type(0));

    base.processChanges(1.0);
    base.applyFact("today", vle::value::Double(16));
    lst = base.startedActivities();
    BOOST_CHECK_EQUAL(lst.size(), vmd::Activities::result_t::size_type(0));

    base.processChanges(2.0);
    base.applyFact("today", vle::value::Double(21));
    lst = base.startedActivities();
    BOOST_CHECK_EQUAL(lst.size(), vmd::Activities::result_t::size_type(0));

    base.processChanges(3.0);
    base.applyFact("today", vle::value::Double(18));
    lst = base.startedActivities();
    BOOST_CHECK_EQUAL(lst.size(), vmd::Activities::result_t::size_type(2));
}
