/*
 * @file vle/extension/decision/test/ss.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2012 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2012 INRA http://www.inra.fr
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
#define BOOST_TEST_MODULE test_precedence_constraints
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <iostream>
#include <iterator>
#include <vle/value/Double.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/extension/decision/KnowledgeBase.hpp>

struct F
{
    F() { vle::value::init(); }
    ~F() { vle::value::finalize(); }
};

BOOST_GLOBAL_FIXTURE(F)

namespace vmd = vle::extension::decision;

namespace vle { namespace extension { namespace decision { namespace ex {


    class SStest: public vmd::KnowledgeBase
    {
    public:
        SStest()
        {
            addActivity("A", 1.0, 10.0);
            addActivity("B", 0.0, 10.0);


            addStartToStartConstraint("A", "B", 4.0, 10.0);
        }

        virtual ~SStest() {}
    };

}}}} // namespace vle extension decision ex

/**
 * This test is here to exhibit a bug.
 *
 * Expected behavior :
 * A receive an ack "done" at 2
 * A : 0--WAIT--1--STARTED--2--DONE----------10
 * B : 0--WAIT-------------------6--STARTED--10
 *
 */
BOOST_AUTO_TEST_CASE(ss)
{
    vmd::ex::SStest base;
    vmd::Activities::result_t lst;

    base.processChanges(0.0);

    {
        const vmd::Activity& A =  base.activities().get("A")->second;
        const vmd::Activity& B =  base.activities().get("B")->second;
        BOOST_REQUIRE(A.isInWaitState());
        BOOST_REQUIRE(B.isInWaitState());
    }
    base.processChanges(1.0);
    base.setActivityDone("A",2.0);
    base.processChanges(2.0);
    {
        const vmd::Activity& A =  base.activities().get("A")->second;
        const vmd::Activity& B =  base.activities().get("B")->second;
        BOOST_REQUIRE(A.isInDoneState());
        BOOST_REQUIRE(B.isInWaitState());
    }
    base.processChanges(3.0);
    base.processChanges(4.0);
    base.processChanges(5.0);
    base.processChanges(6.0);
    base.processChanges(7.0);
    {
        const vmd::Activity& A =  base.activities().get("A")->second;
        const vmd::Activity& B =  base.activities().get("B")->second;
        BOOST_REQUIRE(A.isInDoneState());
        BOOST_REQUIRE(B.isInStartedState());
    }
}
