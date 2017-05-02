/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2017 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2017 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2017 INRA http://www.inra.fr
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

#include <boost/lexical_cast.hpp>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vle/utils/unit-test.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/String.hpp>
#include <vle/vle.hpp>
#include <vle/vpz/AtomicModel.hpp>
#include <vle/vpz/CoupledModel.hpp>
#include <vle/vpz/Vpz.hpp>

using namespace vle;

void
atomicmodel_vpz()
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

    EnsuresEqual(vpz.project().author(), "Gauthier Quesnel");
    EnsuresEqual(vpz.project().version(), "0.5");
    EnsuresEqual(vpz.project().date(), "Mon, 12 Feb 2007 23:40:31 +0100");

    const vpz::Model& mdl = vpz.project().model();
    Ensures(mdl.node() != nullptr);
    EnsuresEqual(mdl.node()->isAtomic(), true);
    EnsuresEqual(mdl.node()->getInputPortNumber(), 2);
    EnsuresEqual(mdl.node()->getOutputPortNumber(), 2);
    Ensures(mdl.node()->existInputPort("in1") != 0);
    Ensures(mdl.node()->existInputPort("in2") != 0);
    Ensures(mdl.node()->existOutputPort("out1") != 0);
    Ensures(mdl.node()->existOutputPort("out2") != 0);
}

void
coupledmodel_vpz()
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
    Ensures(mdl.node() != nullptr);
    EnsuresEqual(mdl.node()->isCoupled(), true);

    vpz::CoupledModel* cpl = mdl.node()->toCoupled();
    Ensures(cpl != nullptr);

    if (not cpl)
        return;

    Ensures(cpl->existOutputPort("o") != 0);
    Ensures(cpl->existInputPort("i") != 0);

    vpz::BaseModel* mdl1 = cpl->findModel("atom1");
    vpz::BaseModel* mdl2 = cpl->findModel("atom2");

    Ensures(mdl1 != nullptr);
    if (not mdl1)
        return;

    Ensures(mdl2 != nullptr);
    if (not mdl2)
        return;

    vpz::AtomicModel* atom1 = dynamic_cast<vpz::AtomicModel*>(mdl1);
    vpz::AtomicModel* atom2 = dynamic_cast<vpz::AtomicModel*>(mdl2);
    Ensures(atom1 != nullptr);
    if (not atom1)
        return;

    Ensures(atom1->existOutputPort("out"));
    Ensures(atom1->existInputPort("in"));

    Ensures(atom2 != nullptr);
    if (not atom2)
        return;

    Ensures(atom2->existInputPort("in"));
    Ensures(atom2->existOutputPort("out"));

    Ensures(cpl->existInputConnection("i", "atom1", "in"));
    Ensures(cpl->existOutputConnection("atom2", "out", "o"));
    Ensures(cpl->existInternalConnection("atom1", "out", "atom2", "in"));
}

void
dynamic_vpz()
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
      "           type=\"local\" />\n"
      " </dynamics>\n"
      "</vle_project>\n";

    vpz::Vpz vpz;
    vpz.parseMemory(xml);

    const vpz::Model& mdl(vpz.project().model());
    Ensures(mdl.node() != nullptr);
    EnsuresEqual(mdl.node()->isAtomic(), true);
    vpz::AtomicModel* atom = mdl.node()->toAtomic();

    Ensures(atom);
}

void
experiment_vpz()
{
    const char* xml =
      "<?xml version=\"1.0\"?>\n"
      "<vle_project version=\"0.5\" author=\"Gauthier Quesnel\""
      " date=\"Mon, 12 Feb 2007 23:40:31 +0100\" >\n"
      " <experiment name=\"test1\">\n"
      "  <conditions>"
      "   <condition name=\"simulation_engine\" >"
      "    <port name=\"begin\" >"
      "     <double>123.</double>"
      "    </port>"
      "    <port name=\"duration\" >"
      "     <double>0.33</double><integer>2</integer>"
      "    </port>"
      "   </condition>"
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
    const vpz::Conditions& cnds(project.experiment().conditions());

    EnsuresEqual(experiment.name(), "test1");
    EnsuresEqual(experiment.duration(), 0.33);

    std::vector<std::string> lst = cnds.conditionnames();
    EnsuresEqual(lst.size(), (std::list<std::string>::size_type)3);

    {
        auto it = std::find(lst.begin(), lst.end(), "cond1");
        Ensures(it != lst.end());
    }
    {
        auto it = std::find(lst.begin(), lst.end(), "simulation_engine");
        Ensures(it != lst.end());
    }

    lst.clear();
    lst = cnds.portnames("cond1");
    EnsuresEqual(lst.size(), (std::list<std::string>::size_type)2);

    {
        auto it = std::find(lst.begin(), lst.end(), "init1");
        Ensures(it != lst.end());
    }

    {
        auto it = std::find(lst.begin(), lst.end(), "init2");
        Ensures(it != lst.end());
    }

    const vpz::Condition& cnd1(cnds.get("cond1"));
    const vpz::Condition& cnd2(cnds.get("cond2"));

    {
        const auto& set(cnd1.getSetValues("init1"));
        const value::Double& real(set[0]->toDouble());
        EnsuresEqual(real.value(), 123.);
        const value::Integer& integer(set[1]->toInteger());
        EnsuresEqual(integer.value(), 1);
    }

    {
        const auto& set(cnd1.getSetValues("init2"));
        const value::Double& real(set[0]->toDouble());
        EnsuresEqual(real.value(), 456.);
        const value::Integer& integer(set[1]->toInteger());
        EnsuresEqual(integer.value(), 2);
    }

    {
        const auto& set = cnd2.getSetValues("init3");
        const value::Double& real(set[0]->toDouble());
        EnsuresEqual(real.value(), .123);
        const value::Integer& integer(set[1]->toInteger());
        EnsuresEqual(integer.value(), -1);
    }

    {
        const auto& set = cnd2.getSetValues("init4");
        const value::Double& real(set[0]->toDouble());
        EnsuresEqual(real.value(), .456);
        const value::Integer& integer(set[1]->toInteger());
        EnsuresEqual(integer.value(), -2);
    }
}

void
experiment_measures_vpz()
{
    const char* xml =
      "<?xml version=\"1.0\"?>\n"
      "<vle_project version=\"0.5\" author=\"Gauthier Quesnel\""
      " date=\"Mon, 12 Feb 2007 23:40:31 +0100\" >\n"
      " <experiment name=\"test1\">\n"
      "  <views>\n"
      "   <outputs>\n"
      "    <output name=\"x\" plugin=\"yyy\" location=\"TEMP\">\n"
      "     <string>test</string>"
      "    </output>\n"
      "    <output name=\"z\" plugin=\"xxx\" location=\"127.0.0.1:8888\" "
      "/>\n"
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
      "            output=\"x\" library=\"lib\" enable=\"false\" />\n"
      "  </views>\n"
      " </experiment>\n"
      "</vle_project>\n";

    vpz::Vpz vpz;
    vpz.parseMemory(xml);

    vpz::Project& project(vpz.project());
    vpz::Experiment& experiment(project.experiment());
    vpz::Views& views(experiment.views());

    vpz::Outputs& outputs(views.outputs());
    Ensures(outputs.outputlist().size() == 2);

    Ensures(outputs.outputlist().find("x") != outputs.outputlist().end());
    {
        vpz::Output& out(outputs.outputlist().find("x")->second);
        EnsuresEqual(out.name(), "x");
        Ensures(out.data());
        EnsuresEqual(out.data()->isString(), true);
        EnsuresEqual(out.data()->toString().value(), "test");
    }
    Ensures(outputs.outputlist().find("z") != outputs.outputlist().end());
    {
        vpz::Output& out(outputs.outputlist().find("z")->second);
        EnsuresEqual(out.name(), "z");
        EnsuresEqual(out.plugin(), "xxx");
        EnsuresEqual(out.location(), "127.0.0.1:8888");
    }

    Ensures(not views.viewlist().empty());
    Ensures((*views.viewlist().begin()).first ==
            (*views.viewlist().begin()).second.name());

    vpz::View& view(views.viewlist().begin()->second);
    EnsuresEqual(view.name(), "x");
    EnsuresEqual(view.is_enable(), false);
    view.enable();
    EnsuresEqual(view.is_enable(), true);
    view.disable();
    EnsuresEqual(view.is_enable(), false);
    view.enable();
    EnsuresEqual(view.is_enable(), true);
    EnsuresEqual(view.streamtype(), "timed");
    EnsuresEqual(view.timestep(), .05);
    EnsuresEqual(view.output(), "x");

    const vpz::Observables& obs(views.observables());
    Ensures(obs.observablelist().size() == 2);

    Ensures(obs.observablelist().find("oo") != obs.observablelist().end());
    {
        const vpz::Observable& ob = obs.observablelist().find("oo")->second;
        EnsuresEqual(ob.name(), "oo");
        EnsuresEqual(ob.observableportlist().size(),
                     (vpz::ObservablePortList::size_type)3);

        Ensures(ob.exist("x1"));
        Ensures(ob.exist("x2"));
        Ensures(ob.exist("x3"));

        const vpz::ObservablePort& port(ob.get("x1"));
        Ensures(port.exist("x"));
    }

    Ensures(obs.observablelist().find("xx") != obs.observablelist().end());
    {
        const vpz::Observable& ob = obs.observablelist().find("xx")->second;
        EnsuresEqual(ob.name(), "xx");
        EnsuresEqual(ob.observableportlist().size(),
                     (vpz::ObservablePortList::size_type)3);

        Ensures(ob.exist("x4"));
        Ensures(ob.exist("x5"));
        Ensures(ob.exist("x6"));
    }

    {
        vpz::Views views(experiment.views());
        EnsuresNotThrow(views.renameView(std::string("x"), std::string("y")),
                        std::exception);
        const vpz::View& view(views.viewlist().begin()->second);
        EnsuresEqual(view.name(), "y");
        EnsuresEqual(view.streamtype(), "timed");
        EnsuresEqual(view.timestep(), .05);
        EnsuresEqual(view.output(), "y");
    }
}

int
main()
{
    vle::Init app;

    atomicmodel_vpz();
    coupledmodel_vpz();
    dynamic_vpz();
    experiment_vpz();
    experiment_measures_vpz();

    return unit_test::report_errors();
}
