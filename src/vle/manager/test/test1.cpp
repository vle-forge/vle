/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#define BOOST_TEST_MAIN
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE manager_complete_test

#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <iostream>
#include <vle/vpz/Vpz.hpp>
#include <vle/manager/Manager.hpp>
#include <vle/manager/ExperimentGenerator.hpp>
#include <vle/vle.hpp>

struct F
{
    vle::Init a;

    F() : a() { }
    ~F() { }
};


BOOST_GLOBAL_FIXTURE(F)

using namespace vle;

const char *xml =
        "<?xml version=\"1.0\"?>\n"
        "<vle_project version=\"0.5\" author=\"Gauthier Quesnel\""
        " date=\"Mon, 12 Feb 2007 23:40:31 +0100\" >\n"
        " <structures>\n"
        "  <model name=\"A\" type=\"atomic\" >\n"
        "   <in>\n"
        "    <port name=\"in1\" />\n"
        "    <port name=\"in2\" />\n"
        "   </in>\n"
        "   <out>\n"
        "    <port name=\"out1\" />\n"
        "    <port name=\"out2\" />\n"
        "   </out>\n"
        "  </model>\n"
        " </structures>\n"
        " <experiment name=\"test1\" duration=\"0.33\" >\n"
        "  <conditions>"
        "   <condition name=\"cond1\" >"
        "    <port name=\"init1\" >"
        "     <double>123.</double><integer>1</integer>"
        "    </port>"
        "    <port name=\"init2\" >"
        "     <double>456.</double><integer>2</integer>"
        "    </port>"
        "   </condition>"
        "   <condition name=\"cond2\" >"
        "    <port name=\"init3\" >"
        "     <double>.123</double><integer>-1</integer>"
        "    </port>"
        "    <port name=\"init4\" >"
        "     <double>.456</double><integer>-2</integer>"
        "    </port>"
        "   </condition>"
        "  </conditions>"
        " </experiment>\n"
        "</vle_project>\n";

BOOST_AUTO_TEST_CASE(experimentgenerator_lower_than_exp)
{
    vpz::Vpz vpz;
    vpz.parseMemory(xml);

    vpz::Project& project(vpz.project());
    vpz::Conditions& cnds(project.experiment().conditions());

    {
        vpz::Condition& cnd1(cnds.get("cond1"));
        cnd1.clearValueOfPort("init1");
        cnd1.clearValueOfPort("init2");
        for (int i = 0; i < 10000; ++i) {
            cnd1.addValueToPort("init1", new value::Double(i));
            cnd1.addValueToPort("init2", new value::Double(i));
        }
    }

    {
        vpz::Condition& cnd2(cnds.get("cond2"));
        cnd2.clearValueOfPort("init3");
        cnd2.clearValueOfPort("init4");
        for (int i = 9999; i >= 0; --i) {
            cnd2.addValueToPort("init3", new value::Double(i));
            cnd2.addValueToPort("init4", new value::Double(i));
        }
    }

    manager::ExperimentGenerator expgen1(vpz, 0, 100);
    BOOST_CHECK_EQUAL(expgen1.min(), 0);
    BOOST_CHECK_EQUAL(expgen1.max(), 100);
    BOOST_CHECK_EQUAL(expgen1.size(), 10000);

    manager::ExperimentGenerator expgen2(vpz, 99, 100);
    BOOST_CHECK_EQUAL(expgen2.min(), 9900);
    BOOST_CHECK_EQUAL(expgen2.max(), 10000);
    BOOST_CHECK_EQUAL(expgen2.size(), 10000);

    manager::ExperimentGenerator expgen3(vpz, 50, 100);
    BOOST_CHECK_EQUAL(expgen3.min(), 5000);
    BOOST_CHECK_EQUAL(expgen3.max(), 5100);
    BOOST_CHECK_EQUAL(expgen3.size(), 10000);
}

BOOST_AUTO_TEST_CASE(experimentgenerator_greater_than_exp)
{
    vpz::Vpz vpz;
    vpz.parseMemory(xml);

    vpz::Project& project(vpz.project());
    vpz::Conditions& cnds(project.experiment().conditions());

    {
        vpz::Condition& cnd1(cnds.get("cond1"));
        cnd1.clearValueOfPort("init1");
        cnd1.clearValueOfPort("init2");
        for (int i = 0; i < 3; ++i) {
            cnd1.addValueToPort("init1", new value::Double(i));
            cnd1.addValueToPort("init2", new value::Double(i));
        }
    }

    {
        vpz::Condition& cnd2(cnds.get("cond2"));
        cnd2.clearValueOfPort("init3");
        cnd2.clearValueOfPort("init4");
        for (int i = 2; i >= 0; --i) {
            cnd2.addValueToPort("init3", new value::Double(i));
            cnd2.addValueToPort("init4", new value::Double(i));
        }
    }

    manager::ExperimentGenerator expgen1(vpz, 0, 5);
    BOOST_CHECK_EQUAL(expgen1.min(), 0);
    BOOST_CHECK_EQUAL(expgen1.max(), 1);
    BOOST_CHECK_EQUAL(expgen1.size(), 3);

    manager::ExperimentGenerator expgen2(vpz, 1, 5);
    BOOST_CHECK_EQUAL(expgen2.min(), 1);
    BOOST_CHECK_EQUAL(expgen2.max(), 2);
    BOOST_CHECK_EQUAL(expgen2.size(), 3);

    manager::ExperimentGenerator expgen3(vpz, 2, 5);
    BOOST_CHECK_EQUAL(expgen3.min(), 2);
    BOOST_CHECK_EQUAL(expgen3.max(), 3);
    BOOST_CHECK_EQUAL(expgen3.size(), 3);

    manager::ExperimentGenerator expgen4(vpz, 3, 5);
    BOOST_CHECK_EQUAL(expgen4.min(), expgen4.max()); /* no job for experiment
                                                        generator 4 and 5.
                                                        max() - min() returns
                                                        null. */
    BOOST_CHECK_EQUAL(expgen4.size(), 3);

    manager::ExperimentGenerator expgen5(vpz, 4, 5);
    BOOST_CHECK_EQUAL(expgen5.min(), expgen5.max());
    BOOST_CHECK_EQUAL(expgen5.size(), 3);
}

BOOST_AUTO_TEST_CASE(experimentgenerator_max_1_max_1)
{
    vpz::Vpz vpz;
    vpz.parseMemory(xml);

    vpz::Project& project(vpz.project());
    vpz::Conditions& cnds(project.experiment().conditions());

    {
        vpz::Condition& cnd1(cnds.get("cond1"));
        cnd1.clearValueOfPort("init1");
        cnd1.clearValueOfPort("init2");
        for (int i = 0; i < 7; ++i) {
            cnd1.addValueToPort("init1", new value::Double(i));
        }
        cnd1.addValueToPort("init2", new value::Double(0));
    }

    {
        vpz::Condition& cnd2(cnds.get("cond2"));
        cnd2.clearValueOfPort("init3");
        cnd2.clearValueOfPort("init4");
        for (int i = 7; i > 0; --i) {
            cnd2.addValueToPort("init3", new value::Double(i));
        }
        cnd2.addValueToPort("init4", new value::Double(7));
    }

    manager::ExperimentGenerator expgen1(vpz, 0, 1);
    BOOST_CHECK_EQUAL(expgen1.min(), 0);
    BOOST_CHECK_EQUAL(expgen1.max(), 7);
    BOOST_CHECK_EQUAL(expgen1.size(), 7);
}
