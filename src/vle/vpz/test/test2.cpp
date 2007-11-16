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

#define BOOST_TEST_MAIN
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE vpz_complete_test
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <vle/vpz/Vpz.hpp>
#include <vle/vpz/Translator.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Double.hpp>
#include <limits>
#include <fstream>

using namespace vle;

BOOST_AUTO_TEST_CASE(atomicmodel_vpz)
{
    const char* xml =
        "<?xml version=\"1.0\"?>\n"
        "<vle_project version=\"0.5\" author=\"Gauthier Quesnel\""
        " date=\"Mon, 12 Feb 2007 23:40:31 +0100\" >\n"
        " <structures>\n"
        "  <model name=\"test1\" type=\"atomic\" >\n"
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

    vpz::Vpz vpz;
    vpz.parseMemory(xml);

    BOOST_REQUIRE_EQUAL(vpz.project().author(), "Gauthier Quesnel");
    BOOST_REQUIRE_CLOSE(vpz.project().version(), 0.5f, 0.01);
    BOOST_REQUIRE_EQUAL(vpz.project().date(), "Mon, 12 Feb 2007 23:40:31 +0100");

    const vpz::Model& mdl = vpz.project().model();
    BOOST_REQUIRE(mdl.model() != 0);
    BOOST_REQUIRE_EQUAL(mdl.model()->isAtomic(), true);
    BOOST_REQUIRE_EQUAL(mdl.model()->getInputPortNumber(), 2);
    BOOST_REQUIRE_EQUAL(mdl.model()->getOutputPortNumber(), 2);
    BOOST_REQUIRE(mdl.model()->existInputPort("in1") != 0);
    BOOST_REQUIRE(mdl.model()->existInputPort("in2") != 0);
    BOOST_REQUIRE(mdl.model()->existOutputPort("out1") != 0);
    BOOST_REQUIRE(mdl.model()->existOutputPort("out2") != 0);
}

BOOST_AUTO_TEST_CASE(coupledmodel_vpz)
{
    const char* xml =
        "<?xml version=\"1.0\"?>\n"
        "<vle_project version=\"0.5\" author=\"Gauthier Quesnel\""
        " date=\"Mon, 12 Feb 2007 23:40:31 +0100\" >\n"
        " <structures>\n"
        "  <model name=\"test2\" type=\"coupled\">\n"
        "   <in><port name=\"i\"/></in>\n"
        "   <out><port name=\"o\"/></out>\n"
        "   <submodels>\n"
        "    <model name=\"atom1\" type=\"atomic\""
        "           observables=\"\" conditions=\"\" dynamics=\"\">\n"
        "     <in><port name=\"in\"/></in>\n"
        "     <out><port name=\"out\"/></out>\n"
        "    </model>\n"
        "    <model name=\"atom2\" type=\"atomic\""
        "           observables=\"\" conditions=\"\" dynamics=\"\">\n"
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
        "     <origin model=\"test2\" port=\"i\" />\n"
        "     <destination model=\"atom1\" port=\"in\" />\n"
        "    </connection>\n"
        "    <connection type=\"output\">"
        "     <origin model=\"atom2\" port=\"out\" />\n"
        "     <destination model=\"test2\" port=\"o\" />\n"
        "    </connection>\n"
        "   </connections>\n"
        "  </model>\n"
        " </structures>\n"
        "</vle_project>\n";
    
    vpz::Vpz vpz;
    vpz.parseMemory(xml);

    const vpz::Model& mdl(vpz.project().model());
    BOOST_REQUIRE(mdl.model() != 0);
    BOOST_REQUIRE_EQUAL(mdl.model()->isCoupled(), true);

    graph::CoupledModel* cpl = dynamic_cast < graph::CoupledModel*
        >(mdl.model());
    BOOST_REQUIRE(cpl != 0);
    BOOST_REQUIRE(cpl->existOutputPort("o") != 0);
    BOOST_REQUIRE(cpl->existInputPort("i") != 0);

    graph::Model* mdl1 = cpl->findModel("atom1");
    graph::Model* mdl2 = cpl->findModel("atom2");
    BOOST_REQUIRE(mdl1 != 0);
    BOOST_REQUIRE(mdl2 != 0);

    graph::AtomicModel* atom1 = dynamic_cast < graph::AtomicModel* >(mdl1);
    graph::AtomicModel* atom2 = dynamic_cast < graph::AtomicModel* >(mdl2);
    BOOST_REQUIRE(atom1 != 0);
    BOOST_REQUIRE(atom1->existOutputPort("out"));
    BOOST_REQUIRE(atom1->existInputPort("in"));
    BOOST_REQUIRE(atom2 != 0);
    BOOST_REQUIRE(atom2->existInputPort("in"));
    BOOST_REQUIRE(atom2->existOutputPort("out"));

    BOOST_REQUIRE(cpl->existInputConnection("i", "atom1", "in"));
    BOOST_REQUIRE(cpl->existOutputConnection("atom2", "out", "o"));
    BOOST_REQUIRE(cpl->existInternalConnection("atom1", "out", "atom2", "in"));
}

BOOST_AUTO_TEST_CASE(dynamic_vpz)
{
    const char* xml =
        "<?xml version=\"1.0\"?>\n"
        "<vle_project version=\"0.5\" author=\"Gauthier Quesnel\""
        " date=\"Mon, 12 Feb 2007 23:40:31 +0100\" >\n"
        " <structures>\n"
        "  <model name=\"test3\" type=\"atomic\""
        "         observables=\"\" dynamics=\"dyn1\" conditions=\"cnd1\" />\n"
        " </structures>\n"
        " <dynamics>\n"
        "  <dynamic name=\"dyn1\" library=\"celldevs\""
        "           model=\"celldevs\""
        "           type=\"local\" />\n"
        " </dynamics>\n"
        "</vle_project>\n";

    vpz::Vpz vpz;
    vpz.parseMemory(xml);

    const vpz::Model& mdl(vpz.project().model());
    BOOST_REQUIRE(mdl.model() != 0);
    BOOST_REQUIRE_EQUAL(mdl.model()->isAtomic(), true);

    graph::AtomicModel* atom(
        reinterpret_cast < graph::AtomicModel* >(mdl.model()));

    const vpz::AtomicModel& vatom = mdl.atomicModels().get(atom);
    BOOST_REQUIRE_EQUAL(vatom.dynamics(), "dyn1");
    BOOST_REQUIRE_EQUAL(vatom.conditions(), "cnd1");

    const vpz::Dynamics& dyns(vpz.project().dynamics());
    const vpz::Dynamic& dyn(dyns.get(vatom.dynamics()));

    BOOST_REQUIRE_EQUAL(dyn.name(), "dyn1");
    BOOST_REQUIRE_EQUAL(dyn.library(), "celldevs");
    BOOST_REQUIRE_EQUAL(dyn.model(), "celldevs");
    BOOST_REQUIRE_EQUAL(dyn.type(), vpz::Dynamic::LOCAL);
    BOOST_REQUIRE_EQUAL(dyn.language(), "");
}

BOOST_AUTO_TEST_CASE(experiment_vpz)
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
    
    vpz::Vpz vpz;
    vpz.parseMemory(xml);

    const vpz::Project& project(vpz.project());
    const vpz::Experiment& experiment(project.experiment());
    const vpz::Replicas& replicas(project.experiment().replicas());
    const vpz::Conditions& cnds(project.experiment().conditions());

    BOOST_REQUIRE_EQUAL(experiment.name(), "test1");
    BOOST_REQUIRE_EQUAL(experiment.duration(), 0.33);
    BOOST_REQUIRE_EQUAL(experiment.seed(), (guint32)987);

    BOOST_REQUIRE_EQUAL(replicas.seed(), (guint32)987456);
    BOOST_REQUIRE_EQUAL(replicas.number(), (size_t)5);

    std::list < std::string > lst;
    cnds.conditionnames(lst);
    BOOST_REQUIRE_EQUAL(lst.size(), (std::list < std::string >::size_type)2);
    BOOST_REQUIRE_EQUAL(*lst.begin(), "cond1");
    BOOST_REQUIRE_EQUAL(*lst.rbegin(), "cond2");
    lst.clear();

    cnds.portnames("cond1", lst);
    BOOST_REQUIRE_EQUAL(lst.size(), (std::list < std::string >::size_type)2);
    BOOST_REQUIRE_EQUAL(*lst.begin(), "init1");
    BOOST_REQUIRE_EQUAL(*lst.rbegin(), "init2");

    value::Set set;
    value::Double real;
    value::Integer integer;
    
    const vpz::Condition& cnd1(cnds.get("cond1"));
    set = cnd1.getSetValues("init1");
    real = value::toDoubleValue(set->getValue(0));
    BOOST_REQUIRE_EQUAL(real->doubleValue(), 123.);
    integer = value::toIntegerValue(set->getValue(1));
    BOOST_REQUIRE_EQUAL(integer->intValue(), 1);

    set = cnd1.getSetValues("init2");
    real = value::toDoubleValue(set->getValue(0));
    BOOST_REQUIRE_EQUAL(real->doubleValue(), 456.);
    integer = value::toIntegerValue(set->getValue(1));
    BOOST_REQUIRE_EQUAL(integer->intValue(), 2);

    const vpz::Condition& cnd2(cnds.get("cond2"));
    set = cnd2.getSetValues("init3");
    real = value::toDoubleValue(set->getValue(0));
    BOOST_REQUIRE_EQUAL(real->doubleValue(), .123);
    integer = value::toIntegerValue(set->getValue(1));
    BOOST_REQUIRE_EQUAL(integer->intValue(), -1);
    
    set = cnd2.getSetValues("init4");
    real = value::toDoubleValue(set->getValue(0));
    BOOST_REQUIRE_EQUAL(real->doubleValue(), .456);
    integer = value::toIntegerValue(set->getValue(1));
    BOOST_REQUIRE_EQUAL(integer->intValue(), -2);
}

BOOST_AUTO_TEST_CASE(experiment_measures_vpz)
{
    const char* xml=
        "<?xml version=\"1.0\"?>\n"
        "<vle_project version=\"0.5\" author=\"Gauthier Quesnel\""
        " date=\"Mon, 12 Feb 2007 23:40:31 +0100\" >\n"
        " <experiment name=\"test1\" duration=\"0.33\" seed=\"987\">\n"
        "  <replicas seed=\"987456\" number=\"5\" />\n"
        "  <views>\n"
        "   <outputs>\n"
        "    <output name=\"x\" format=\"local\" "
        "            plugin=\"yyy\" location=\"TEMP\">\n"
        "   <![CDATA["
        "test"
        "]]>"
        "    </output>\n"
        "    <output name=\"z\" format=\"distant\" "
        "            plugin=\"xxx\" location=\"127.0.0.1:8888\" />\n"
        "   </outputs>\n"
        "   <observables>\n"
        "    <observable name=\"oo\" >\n"
        "     <port name=\"x1\">\n"
        "      <attachedview name=\"x\" />\n"
        "     </port>\n"
        "     <port name=\"x2\" />"
        "     <port name=\"x3\" />"
        "    </observable>\n"
        "    <observable name=\"xx\" >\n"
        "     <port name=\"x4\" />"
        "     <port name=\"x5\" />"
        "     <port name=\"x6\" />"
        "    </observable>\n"
        "   </observables>\n"
        "   <view name=\"x\" type=\"timed\" timestep=\".05\""
        "            output=\"x\" library=\"lib\" />\n"
        "  </views>\n"
        " </experiment>\n"
        "</vle_project>\n";
    
    vpz::Vpz vpz;
    vpz.parseMemory(xml);

    const vpz::Project& project(vpz.project());
    const vpz::Experiment& experiment(project.experiment());
    const vpz::Views& views(experiment.views());
    
    const vpz::Outputs& outputs(views.outputs());
    BOOST_REQUIRE(outputs.outputlist().size() == 2);

    BOOST_REQUIRE(outputs.outputlist().find("x") != outputs.outputlist().end());
    {
        const vpz::Output& out(outputs.outputlist().find("x")->second);
        BOOST_REQUIRE_EQUAL(out.name(), "x");
        BOOST_REQUIRE_EQUAL(out.format(), vpz::Output::LOCAL);
        BOOST_REQUIRE_EQUAL(out.data(), "test");
    }
    BOOST_REQUIRE(outputs.outputlist().find("z") != outputs.outputlist().end());
    {
        const vpz::Output& out(outputs.outputlist().find("z")->second);
        BOOST_REQUIRE_EQUAL(out.name(), "z");
        BOOST_REQUIRE_EQUAL(out.format(), vpz::Output::DISTANT);
        BOOST_REQUIRE_EQUAL(out.plugin(), "xxx");
        BOOST_REQUIRE_EQUAL(out.location(), "127.0.0.1:8888");
    }


    BOOST_REQUIRE(not views.viewlist().empty());
    BOOST_REQUIRE((*views.viewlist().begin()).first
                  == (*views.viewlist().begin()).second.name());

    const vpz::View& view(views.viewlist().begin()->second);
    BOOST_REQUIRE_EQUAL(view.name(), "x");
    BOOST_REQUIRE_EQUAL(view.streamtype(), "timed");
    BOOST_REQUIRE_EQUAL(view.timestep(), .05);
    BOOST_REQUIRE_EQUAL(view.output(), "x");

    const vpz::Observables& obs(views.observables());
    BOOST_REQUIRE(obs.observablelist().size() == 2);

    BOOST_REQUIRE(obs.observablelist().find("oo") !=
                  obs.observablelist().end());
    {
        const vpz::Observable& ob = obs.observablelist().find("oo")->second;
        BOOST_REQUIRE_EQUAL(ob.name(), "oo");
        BOOST_REQUIRE_EQUAL(ob.observableportlist().size(),
                            (vpz::ObservablePortList::size_type)3);

        BOOST_REQUIRE(ob.exist("x1"));
        BOOST_REQUIRE(ob.exist("x2"));
        BOOST_REQUIRE(ob.exist("x3"));

        const vpz::ObservablePort& port(ob.get("x1"));
        BOOST_REQUIRE(port.exist("x"));
    }

    BOOST_REQUIRE(obs.observablelist().find("xx") !=
                  obs.observablelist().end());
    {
        const vpz::Observable& ob = obs.observablelist().find("xx")->second;
        BOOST_REQUIRE_EQUAL(ob.name(), "xx");
        BOOST_REQUIRE_EQUAL(ob.observableportlist().size(),
                            (vpz::ObservablePortList::size_type)3);

        BOOST_REQUIRE(ob.exist("x4"));
        BOOST_REQUIRE(ob.exist("x5"));
        BOOST_REQUIRE(ob.exist("x6"));
    }
}

BOOST_AUTO_TEST_CASE(translator_vpz)
{
    const char* xml=
        "<?xml version=\"1.0\"?>\n"
        "<vle_project version=\"0.5\" author=\"Gauthier Quesnel\""
        " date=\"Mon, 12 Feb 2007 23:40:31 +0100\" >\n"
        " <structures>\n"
        "  <model name=\"test1\" type=\"novle\" translator=\"xxx\" >\n"
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
        " <translators>\n"
        "  <translator name=\"tr1\" library=\"trlib\">\n"
        "   <![CDATA["
        "<?xml version=\"1.0\"?>"
        "]]>"
        "  </translator>\n"
        " </translators>\n"
        "</vle_project>\n";
    
    vpz::Vpz vpz;
    vpz.parseMemory(xml);

    const vpz::Project& project(vpz.project());
    const vpz::NoVLEs& novles(project.novles());

    BOOST_REQUIRE(novles.exist("tr1"));
    const vpz::NoVLE& novle(novles.get("tr1"));

    BOOST_REQUIRE_EQUAL(novle.name(), "tr1");
    BOOST_REQUIRE_EQUAL(novle.library(), "trlib");
    BOOST_REQUIRE_EQUAL(novle.data(), "<?xml version=\"1.0\"?>");
    
    const vpz::Model& mdl = vpz.project().model();
    BOOST_REQUIRE(mdl.model() != 0);
    BOOST_REQUIRE_EQUAL(mdl.model()->isNoVLE(), true);
}
