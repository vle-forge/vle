/** 
 * @file test2.cpp
 * @brief Test unit of the vpz parser.
 * @author The vle Development Team
 */

/*
 * Copyright (C) 2007 - The vle Development Team
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <vle/vpz/SaxVPZ.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <limits>
#include <fstream>

using namespace vle;

void test_atomicmodel_vpz()
{
    const char* xml =
        "<?xml version=\"1.0\"?>\n"
        "<vle_project version=\"0.5\" author=\"Gauthier Quesnel\""
        " date=\"Mon, 12 Feb 2007 23:40:31 +0100\" >\n"
        " <structures>\n"
        "  <model name=\"test1\" type=\"atomic\" conditions=\"\" dynamics=\"\" >\n"
        "   <init>\n"
        "    <port name=\"init1\" />\n"
        "    <port name=\"init2\" />\n"
        "   </init>\n"
        "   <state>\n"
        "    <port name=\"state1\" />\n"
        "    <port name=\"state2\" />\n"
        "   </state>\n"
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
        "</vle_project>\n";
    vpz::VLESaxParser sax;
    sax.parse_memory(xml);

    const vpz::Vpz& vpz = sax.vpz();
    BOOST_REQUIRE_EQUAL(vpz.project().author(), "Gauthier Quesnel");
    BOOST_REQUIRE_CLOSE(vpz.project().version(), 0.5f, 0.01);
    BOOST_REQUIRE_EQUAL(vpz.project().date(), "Mon, 12 Feb 2007 23:40:31 +0100");

    const vpz::Model& mdl = sax.vpz().project().model();
    BOOST_REQUIRE(mdl.model() != 0);
    BOOST_REQUIRE_EQUAL(mdl.model()->isAtomic(), true);
    BOOST_REQUIRE_EQUAL(mdl.model()->getInitPortNumber(), 2);
    BOOST_REQUIRE_EQUAL(mdl.model()->getStatePortNumber(), 2);
    BOOST_REQUIRE_EQUAL(mdl.model()->getInPortNumber(), 2);
    BOOST_REQUIRE_EQUAL(mdl.model()->getOutPortNumber(), 2);
    BOOST_REQUIRE(mdl.model()->getInitPort("init1") != 0);
    BOOST_REQUIRE(mdl.model()->getInitPort("init2") != 0);
    BOOST_REQUIRE(mdl.model()->getStatePort("state1") != 0);
    BOOST_REQUIRE(mdl.model()->getStatePort("state2") != 0);
    BOOST_REQUIRE(mdl.model()->getInPort("in1") != 0);
    BOOST_REQUIRE(mdl.model()->getInPort("in2") != 0);
    BOOST_REQUIRE(mdl.model()->getOutPort("out1") != 0);
    BOOST_REQUIRE(mdl.model()->getOutPort("out2") != 0);
}

void test_coupledmodel_vpz()
{
    const char* xml =
        "<?xml version=\"1.0\"?>\n"
        "<vle_project version=\"0.5\" author=\"Gauthier Quesnel\""
        " date=\"Mon, 12 Feb 2007 23:40:31 +0100\" >\n"
        " <structures>\n"
        "  <model name=\"test1\" type=\"coupled\">\n"
        "   <in><port name=\"i\"/></in>\n"
        "   <out><port name=\"o\"/></out>\n"
        "   <submodels>\n"
        "    <model name=\"atom1\" type=\"atomic\""
        "           conditions=\"\" dynamics=\"\">\n"
        "     <in><port name=\"in\"/></in>\n"
        "     <out><port name=\"out\"/></out>\n"
        "    </model>\n"
        "    <model name=\"atom2\" type=\"atomic\""
        "           conditions=\"\" dynamics=\"\">\n"
        "     <in><port name=\"in\"/></in>\n"
        "     <out><port name=\"out\"/></out>\n"
        "    </model>\n"
        "   </submodels>\n"
        "   <connections>\n"
        "    <connection type=\"internal\">\n"
        "     <origin model=\"atom1\" port=\"out\" />\n"
        "     <destination model=\"atom2\" port=\"in\" />\n"
        "    </connection>\n"
        "    <connection type=\"input\">\n"
        "     <origin model=\"test1\" port=\"i\" />\n"
        "     <destination model=\"atom1\" port=\"in\" />\n"
        "    </connection>\n"
        "    <connection type=\"output\">"
        "     <origin model=\"atom2\" port=\"out\" />\n"
        "     <destination model=\"test1\" port=\"o\" />\n"
        "    </connection>\n"
        "   </connections>\n"
        "  </model>\n"
        " </structures>\n"
        "</vle_project>\n";
    vpz::VLESaxParser sax;
    sax.parse_memory(xml);

    const vpz::Model& mdl = sax.vpz().project().model();
    BOOST_REQUIRE(mdl.model() != 0);
    BOOST_REQUIRE_EQUAL(mdl.model()->isCoupled(), true);

    graph::CoupledModel* cpl = dynamic_cast < graph::CoupledModel* >(mdl.model());
    BOOST_REQUIRE(cpl != 0);
    BOOST_REQUIRE(cpl->getOutPort("o") != 0);
    BOOST_REQUIRE(cpl->getInPort("i") != 0);

    graph::Model* mdl1 = cpl->find("atom1");
    graph::Model* mdl2 = cpl->find("atom2");
    BOOST_REQUIRE(mdl1 != 0);
    BOOST_REQUIRE(mdl2 != 0);

    graph::AtomicModel* atom1 = dynamic_cast < graph::AtomicModel* >(mdl1);
    graph::AtomicModel* atom2 = dynamic_cast < graph::AtomicModel* >(mdl2);
    BOOST_REQUIRE(atom1 != 0);
    BOOST_REQUIRE(atom1->getOutPort("out") != 0);
    BOOST_REQUIRE(atom1->getInPort("in") != 0);
    BOOST_REQUIRE(atom2 != 0);
    BOOST_REQUIRE(atom2->getInPort("in") != 0);
    BOOST_REQUIRE(atom2->getOutPort("out") != 0);

    BOOST_REQUIRE(cpl->getInputConnection("i", "atom1", "in") != 0);
    BOOST_REQUIRE(cpl->getOutputConnection("atom2", "out", "o") != 0);
    BOOST_REQUIRE(cpl->getInternalConnection("atom1", "out", "atom2", "in") != 0);
}

void test_dynamic_vpz()
{
    const char* xml =
        "<?xml version=\"1.0\"?>\n"
        "<vle_project version=\"0.5\" author=\"Gauthier Quesnel\""
        " date=\"Mon, 12 Feb 2007 23:40:31 +0100\" >\n"
        " <structures>\n"
        "  <model name=\"test1\" type=\"atomic\""
        "         dynamics=\"dyn1\" conditions=\"cnd1\" />\n"
        " </structures>\n"
        " <dynamics>\n"
        "  <dynamic name=\"dyn1\" library=\"celldevs\""
        "           model=\"celldevs\""
        "           type=\"local\" />\n"
        " </dynamics>\n"
        "</vle_project>\n";

    vpz::VLESaxParser sax;
    sax.parse_memory(xml);

    const vpz::Model& mdl = sax.vpz().project().model();
    BOOST_REQUIRE(mdl.model() != 0);
    BOOST_REQUIRE_EQUAL(mdl.model()->isAtomic(), true);

    graph::AtomicModel* atom(
        reinterpret_cast < graph::AtomicModel* >(mdl.model()));

    const vpz::AtomicModel& vatom = mdl.atomicModels().get(atom);
    BOOST_REQUIRE_EQUAL(vatom.dynamics(), "dyn1");
    BOOST_REQUIRE_EQUAL(vatom.conditions(), "cnd1");

    const vpz::Dynamics& dyns = sax.vpz().project().dynamics();
    const vpz::Dynamic& dyn = dyns.find(vatom.dynamics());

    BOOST_REQUIRE_EQUAL(dyn.name(), "dyn1");
    BOOST_REQUIRE_EQUAL(dyn.library(), "celldevs");
    BOOST_REQUIRE_EQUAL(dyn.model(), "celldevs");
    BOOST_REQUIRE_EQUAL(dyn.type(), vpz::Dynamic::LOCAL);
    BOOST_REQUIRE_EQUAL(dyn.language(), "");
}

void test_experiment_vpz()
{
    const char* xml=
        "<?xml version=\"1.0\"?>\n"
        "<vle_project version=\"0.5\" author=\"Gauthier Quesnel\""
        " date=\"Mon, 12 Feb 2007 23:40:31 +0100\" >\n"
        " <experiment name=\"test1\" duration=\"0.33\" seed=\"987\">\n"
        "  <replicas seed=\"987456\" number=\"5\" />\n"
        " </experiment>\n"
        "</vle_project>\n";
    
    vpz::VLESaxParser sax;
    sax.parse_memory(xml);

    const vpz::Vpz& vpz(sax.vpz());
    const vpz::Project& project(vpz.project());
    const vpz::Experiment& experiment(project.experiment());
    const vpz::Replicas& replicas(project.experiment().replicas());

    BOOST_REQUIRE_EQUAL(experiment.name(), "test1");
    BOOST_REQUIRE_EQUAL(experiment.duration(), 0.33);
    BOOST_REQUIRE_EQUAL(experiment.seed(), (guint32)987);

    BOOST_REQUIRE_EQUAL(replicas.seed(), (guint32)987456);
    BOOST_REQUIRE_EQUAL(replicas.number(), (size_t)5);
}

boost::unit_test_framework::test_suite*
init_unit_test_suite(int, char* [])
{
    boost::unit_test_framework::test_suite* test;

    test = BOOST_TEST_SUITE("vpz test");
    test->add(BOOST_TEST_CASE(&test_atomicmodel_vpz));
    test->add(BOOST_TEST_CASE(&test_coupledmodel_vpz));
    test->add(BOOST_TEST_CASE(&test_dynamic_vpz));
    test->add(BOOST_TEST_CASE(&test_experiment_vpz));
    return test;
}
