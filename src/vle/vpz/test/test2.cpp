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
#include <vle/value/Set.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Double.hpp>
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
    
    vpz::VLESaxParser sax;
    sax.parse_memory(xml);

    const vpz::Vpz& vpz(sax.vpz());
    const vpz::Project& project(vpz.project());
    const vpz::Experiment& experiment(project.experiment());
    const vpz::Replicas& replicas(project.experiment().replicas());
    const vpz::Conditions& cnds(project.experiment().conditions());

    BOOST_REQUIRE_EQUAL(experiment.name(), "test1");
    BOOST_REQUIRE_EQUAL(experiment.duration(), 0.33);
    BOOST_REQUIRE_EQUAL(experiment.seed(), (guint32)987);

    BOOST_REQUIRE_EQUAL(replicas.seed(), (guint32)987456);
    BOOST_REQUIRE_EQUAL(replicas.number(), (size_t)5);

    value::Set set;
    value::Double real;
    value::Integer integer;
    
    const vpz::Condition& cnd1(cnds.find("cond1"));
    set = cnd1.getSetValues("init1");
    real = value::to_double(set->getValue(0));
    BOOST_REQUIRE_EQUAL(real->doubleValue(), 123.);
    integer = value::to_integer(set->getValue(1));
    BOOST_REQUIRE_EQUAL(integer->intValue(), 1);

    set = cnd1.getSetValues("init2");
    real = value::to_double(set->getValue(0));
    BOOST_REQUIRE_EQUAL(real->doubleValue(), 456.);
    integer = value::to_integer(set->getValue(1));
    BOOST_REQUIRE_EQUAL(integer->intValue(), 2);

    const vpz::Condition& cnd2(cnds.find("cond2"));
    set = cnd2.getSetValues("init3");
    real = value::to_double(set->getValue(0));
    BOOST_REQUIRE_EQUAL(real->doubleValue(), .123);
    integer = value::to_integer(set->getValue(1));
    BOOST_REQUIRE_EQUAL(integer->intValue(), -1);
    
    set = cnd2.getSetValues("init4");
    real = value::to_double(set->getValue(0));
    BOOST_REQUIRE_EQUAL(real->doubleValue(), .456);
    integer = value::to_integer(set->getValue(1));
    BOOST_REQUIRE_EQUAL(integer->intValue(), -2);
}

void test_experiment_measures_vpz()
{
    const char* xml=
        "<?xml version=\"1.0\"?>\n"
        "<vle_project version=\"0.5\" author=\"Gauthier Quesnel\""
        " date=\"Mon, 12 Feb 2007 23:40:31 +0100\" >\n"
        " <experiment name=\"test1\" duration=\"0.33\" seed=\"987\">\n"
        "  <replicas seed=\"987456\" number=\"5\" />\n"
        "  <measures>\n"
        "   <outputs>\n"
        "    <output name=\"x\" type=\"local\" format=\"text\" />\n"
        "    <output name=\"y\" format=\"sdml\" />\n"
        "    <output name=\"z\" format=\"eov\""
        "            plugin=\"xxx\" location=\"127.0.0.1:8888\" />\n"
        "    <output name=\"a\" format=\"net\" plugin=\"zzz\" />\n"
        "   </outputs>\n"
        "   <measure name=\"x\" type=\"timed\" time_step=\".05\""
        "            output=\"x\" >\n"
        "    <observable name=\"oo\" group=\"a\" index=\"1\" />\n"
        "     <ports>"
        "      <port name=\"x1\" />"
        "      <port name=\"x2\" />"
        "      <port name=\"x3\" />"
        "     </ports>\n"
        "    <observable name=\"xx\" group=\"b\" index=\"2\" />\n"
        "     <ports>"
        "      <port name=\"x4\" />"
        "      <port name=\"x5\" />"
        "      <port name=\"x6\" />"
        "     </ports>"
        "   </measure>\n"
        "  </measures>\n"
        " <experiment>\n"
        " </experiment>\n"
        "</vle_project>\n";
    
    vpz::VLESaxParser sax;
    sax.parse_memory(xml);

    const vpz::Vpz& vpz(sax.vpz());
    const vpz::Project& project(vpz.project());
    const vpz::Experiment& experiment(project.experiment());
    const vpz::Measures& measures(experiment.measures());
    
    const vpz::Outputs& outputs(measures.outputs());
    BOOST_REQUIRE(outputs.size() == 4);

    BOOST_REQUIRE(outputs.find("x") != outputs.end());
    {
        const vpz::Output& out(outputs.find("x")->second);
        BOOST_REQUIRE_EQUAL(out.name(), "x");
        BOOST_REQUIRE_EQUAL(out.format(), vpz::Output::TEXT);
    }
    BOOST_REQUIRE(outputs.find("y") != outputs.end());
    {
        const vpz::Output& out(outputs.find("y")->second);
        BOOST_REQUIRE_EQUAL(out.name(), "y");
        BOOST_REQUIRE_EQUAL(out.format(), vpz::Output::SDML);
    }
    BOOST_REQUIRE(outputs.find("z") != outputs.end());
    {
        const vpz::Output& out(outputs.find("z")->second);
        BOOST_REQUIRE_EQUAL(out.name(), "z");
        BOOST_REQUIRE_EQUAL(out.format(), vpz::Output::EOV);
        BOOST_REQUIRE_EQUAL(out.plugin(), "xxx");
        BOOST_REQUIRE_EQUAL(out.location(), "127.0.0.1:8888");
    }
    BOOST_REQUIRE(outputs.find("a") != outputs.end());
    {
        const vpz::Output& out(outputs.find("a")->second);
        BOOST_REQUIRE_EQUAL(out.name(), "a");
        BOOST_REQUIRE_EQUAL(out.format(), vpz::Output::NET);
        BOOST_REQUIRE_EQUAL(out.plugin(), "zzz");
    }


    const vpz::MeasureList& lst(measures.measures());
    BOOST_REQUIRE(not lst.empty());
    BOOST_REQUIRE((*lst.begin()).first == (*lst.begin()).second.name());

    const vpz::Measure& measure(lst.begin()->second);
    BOOST_REQUIRE_EQUAL(measure.name(), "x");
    BOOST_REQUIRE_EQUAL(measure.streamtype(), "timed");
    BOOST_REQUIRE_EQUAL(measure.timestep(), .05);
    BOOST_REQUIRE_EQUAL(measure.output(), "x");

    const vpz::ObservableList& obs(measure.observables());
    BOOST_REQUIRE(obs.size() == 2);

    BOOST_REQUIRE(obs.find("oo") != obs.end());
    {
        const vpz::Observable& ob = obs.find("oo")->second;
        BOOST_REQUIRE_EQUAL(ob.name(), "oo");
        BOOST_REQUIRE_EQUAL(ob.group(), "a");
        BOOST_REQUIRE_EQUAL(ob.index(), 1);
        const vpz::ObservablePort& ports(ob.ports());
        BOOST_REQUIRE_EQUAL(ports.size(), (vpz::ObservablePort::size_type)3);
        BOOST_REQUIRE(ports.find("x1") != ports.end());
        BOOST_REQUIRE(ports.find("x2") != ports.end());
        BOOST_REQUIRE(ports.find("x3") != ports.end());
    }

    BOOST_REQUIRE(obs.find("xx") != obs.end());
    {
        const vpz::Observable& ob = obs.find("xx")->second;
        BOOST_REQUIRE_EQUAL(ob.name(), "xx");
        BOOST_REQUIRE_EQUAL(ob.group(), "b");
        BOOST_REQUIRE_EQUAL(ob.index(), 2);
        const vpz::ObservablePort& ports(ob.ports());
        BOOST_REQUIRE_EQUAL(ports.size(), (vpz::ObservablePort::size_type)3);
        BOOST_REQUIRE(ports.find("x4") != ports.end());
        BOOST_REQUIRE(ports.find("x5") != ports.end());
        BOOST_REQUIRE(ports.find("x6") != ports.end());
    }
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
