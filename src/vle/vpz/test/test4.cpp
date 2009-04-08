/**
 * @file vle/vpz/test/test4.cpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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
#define BOOST_TEST_MODULE translator_complete_test
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/output_test_stream.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>

#include <vle/value.hpp>
#include <vle/utils.hpp>
#include <vle/graph.hpp>
#include <vle/vpz.hpp>

struct F
{
    F() { vle::value::init(); }
    ~F() { vle::value::finalize(); }
};

BOOST_GLOBAL_FIXTURE(F)

using namespace vle;

void check_rename_conds_unittest_vpz(vpz::Project& project)
{
    vpz::AtomicModelList &lst = project.model().atomicModels();
    vpz::Conditions& cnds = project.experiment().conditions();

    BOOST_REQUIRE_NO_THROW(
	cnds.rename(std::string("ca"), std::string("new_ca")));
    BOOST_REQUIRE_NO_THROW(
	cnds.rename(std::string("cb"), std::string("new_cb")));
    BOOST_REQUIRE_NO_THROW(
	cnds.rename(std::string("cc"), std::string("new_cc")));
    BOOST_REQUIRE_NO_THROW(
	cnds.rename(std::string("cd"), std::string("new_cd")));

    BOOST_REQUIRE(project.experiment().conditions().exist("new_ca"));
    BOOST_REQUIRE(project.experiment().conditions().exist("new_cb"));
    BOOST_REQUIRE(project.experiment().conditions().exist("new_cc"));
    BOOST_REQUIRE(project.experiment().conditions().exist("new_cd"));

    BOOST_CHECK(not project.experiment().conditions().exist("ca"));
    BOOST_CHECK(not project.experiment().conditions().exist("cb"));
    BOOST_CHECK(not project.experiment().conditions().exist("cc"));
    BOOST_CHECK(not project.experiment().conditions().exist("cd"));

    BOOST_REQUIRE_NO_THROW(
	lst.updateCondition(std::string("ca"), std::string("new_ca")));
    BOOST_REQUIRE_NO_THROW(
	lst.updateCondition(std::string("cb"), std::string("new_cb")));
    BOOST_REQUIRE_NO_THROW(
	lst.updateCondition(std::string("cc"), std::string("new_cc")));
    BOOST_REQUIRE_NO_THROW(
	lst.updateCondition(std::string("cd"), std::string("new_cd")));

    BOOST_REQUIRE_THROW(
	lst.updateCondition(std::string("ca"), std::string("new_cd")),
	utils::ArgError);
    BOOST_REQUIRE_THROW(
	lst.updateCondition(std::string("ca"), std::string("new_cd")),
	utils::ArgError);
    BOOST_REQUIRE_THROW(
	lst.updateCondition(std::string("ca"), std::string("new_cd")),
	utils::ArgError);
    BOOST_REQUIRE_THROW(
	lst.updateCondition(std::string("ca"), std::string("new_cd")),
	utils::ArgError);
}

void check_rename_views_unittest_vpz(vpz::Views& views)
{
    BOOST_REQUIRE_NO_THROW(views.renameView("view1", "new_view1"));
    BOOST_REQUIRE_NO_THROW(views.renameView("view2", "new_view2"));

    BOOST_REQUIRE(views.exist("new_view1"));
    BOOST_REQUIRE(views.exist("new_view2"));

    BOOST_CHECK(not views.exist("view1"));
    BOOST_CHECK(not views.exist("view2"));

    vpz::Observables& obs_list = views.observables();
    BOOST_REQUIRE_NO_THROW(obs_list.updateView("view1", "new_view1"));
    BOOST_REQUIRE_NO_THROW(obs_list.updateView("view2", "new_view2"));

    BOOST_CHECK(views.observables().get("obs1").hasView("new_view1"));
    BOOST_CHECK(views.observables().get("obs2").hasView("new_view1"));
    BOOST_CHECK(views.observables().get("obs2").hasView("new_view2"));

    BOOST_CHECK(views.get("new_view1").output() == "new_view1");
    BOOST_CHECK(views.get("new_view2").output() == "new_view2");
}


void check_model_unittest_vpz(const vpz::Model& model)
{
    BOOST_REQUIRE(model.model());
    BOOST_REQUIRE(model.model()->isCoupled());

    graph::CoupledModel* cpled((graph::CoupledModel*)model.model());
    BOOST_REQUIRE_EQUAL(cpled->getName(), "top");
    BOOST_REQUIRE(cpled->exist("top1"));
    BOOST_REQUIRE(cpled->findModel("top1")->isCoupled());
    BOOST_REQUIRE(cpled->exist("top2"));
    BOOST_REQUIRE(cpled->findModel("top2")->isCoupled());
    BOOST_REQUIRE(cpled->exist("d"));
    BOOST_REQUIRE(cpled->findModel("d")->isAtomic());
    BOOST_REQUIRE(cpled->exist("e"));
    BOOST_REQUIRE(cpled->findModel("e")->isAtomic());

    {
        graph::CoupledModel* top1((graph::CoupledModel*)cpled->findModel("top1"));
        {
            BOOST_REQUIRE(top1->exist("x"));
            BOOST_REQUIRE(top1->findModel("x")->isAtomic());
            BOOST_REQUIRE(top1->exist("a"));
            BOOST_REQUIRE(top1->findModel("a")->isAtomic());
            BOOST_REQUIRE(top1->exist("b"));
            BOOST_REQUIRE(top1->findModel("b")->isAtomic());
            BOOST_REQUIRE(top1->exist("c"));
            BOOST_REQUIRE(top1->findModel("c")->isAtomic());
            BOOST_REQUIRE(top1->existInputConnection("in", "x", "in"));
            BOOST_REQUIRE(top1->existInternalConnection("x", "out", "x", "in"));
            BOOST_REQUIRE(top1->existInternalConnection("x", "out", "a", "in"));
            BOOST_REQUIRE(top1->existInternalConnection("x", "out", "b", "in"));
            BOOST_REQUIRE(top1->existInternalConnection("x", "out", "a", "in"));
            BOOST_REQUIRE(top1->existInternalConnection("x", "out", "c", "in1"));
            BOOST_REQUIRE(top1->existInternalConnection("x", "out", "c", "in2"));
            BOOST_REQUIRE(top1->existOutputConnection("x", "out", "out"));
        }
        graph::CoupledModel* top2((graph::CoupledModel*)cpled->findModel("top2"));
        {
            BOOST_REQUIRE(top2->exist("f"));
            BOOST_REQUIRE(top2->findModel("f")->isAtomic());
            BOOST_REQUIRE(top2->exist("g"));
            BOOST_REQUIRE(top2->findModel("g")->isAtomic());
            BOOST_REQUIRE(top2->existInputConnection("in", "f", "in"));
            BOOST_REQUIRE(top2->existInputConnection("in", "g", "in"));
        }
        BOOST_REQUIRE(cpled->existInternalConnection("top1", "out", "d", "in"));
        BOOST_REQUIRE(cpled->existInternalConnection("top1", "out", "e", "in1"));
        BOOST_REQUIRE(cpled->existInternalConnection("top1", "out", "e", "in2"));
        BOOST_REQUIRE(cpled->existInternalConnection("top1", "out", "top2", "in"));
        BOOST_REQUIRE(cpled->existInternalConnection("top2", "out", "top1", "in"));
    }
}

void check_dynamics_unittest_vpz(const vpz::Dynamics& dynamics)
{
    BOOST_REQUIRE(dynamics.exist("unittest"));
    BOOST_REQUIRE(dynamics.exist("b"));
    BOOST_REQUIRE(dynamics.exist("a"));

    vpz::Dynamic unittest(dynamics.get("unittest"));
    BOOST_REQUIRE_EQUAL(unittest.name(), "unittest");
    BOOST_REQUIRE_EQUAL(unittest.library(), "unittest");
    BOOST_REQUIRE_EQUAL(unittest.model(), "transform");
    BOOST_REQUIRE_EQUAL(unittest.type(),  vpz::Dynamic::LOCAL);

    vpz::Dynamic b(dynamics.get("b"));
    BOOST_REQUIRE_EQUAL(b.name(), "b");
    BOOST_REQUIRE_EQUAL(b.library(), "libu");
    BOOST_REQUIRE_EQUAL(b.model(), "libub");
    BOOST_REQUIRE_EQUAL(b.language(), "python");
    BOOST_REQUIRE_EQUAL(b.type(),  vpz::Dynamic::LOCAL);

    vpz::Dynamic a(dynamics.get("a"));
    BOOST_REQUIRE_EQUAL(a.name(), "a");
    BOOST_REQUIRE_EQUAL(a.library(), "liba");
    BOOST_REQUIRE_EQUAL(a.model(), "libua");
    BOOST_REQUIRE_EQUAL(a.language(), "python");
    BOOST_REQUIRE_EQUAL(a.location(), "192.168.1.1:324");
    BOOST_REQUIRE_EQUAL(a.type(),  vpz::Dynamic::DISTANT);
}

void check_experiment_unittest_vpz(const vpz::Experiment& exp)
{
    {
        BOOST_REQUIRE(exp.conditions().exist("ca"));
        BOOST_REQUIRE(exp.conditions().exist("cb"));
        BOOST_REQUIRE(exp.conditions().exist("cc"));
        BOOST_REQUIRE(exp.conditions().exist("cd"));
        {
            const vpz::Condition& c(exp.conditions().get("ca"));
            const value::Set& x(c.getSetValues("x"));
            BOOST_REQUIRE(not x.value().empty());
            BOOST_REQUIRE(x.get(0).isDouble());
            BOOST_CHECK_CLOSE(value::toDouble(x.get(0)), 1.2, 0.1);
        }
        {
            const vpz::Condition& c(exp.conditions().get("cb"));
            const value::Set& x(c.getSetValues("x"));
            BOOST_REQUIRE(not x.value().empty());
            BOOST_REQUIRE(x.get(0).isDouble());
            BOOST_CHECK_CLOSE(value::toDouble(x.get(0)), 1.3, 0.1);
        }
        {
            const vpz::Condition& c(exp.conditions().get("cc"));
            const value::Set& x(c.getSetValues("x"));
            BOOST_REQUIRE(not x.value().empty());
            BOOST_REQUIRE(x.get(0).isDouble());
            BOOST_CHECK_CLOSE(value::toDouble(x.get(0)), 1.4, 0.1);
        }
        {
            const vpz::Condition& c(exp.conditions().get("cd"));
            const value::Set& x(c.getSetValues("x"));
            BOOST_REQUIRE(not x.value().empty());
            BOOST_REQUIRE(x.get(0).isDouble());
            BOOST_CHECK_CLOSE(value::toDouble(x.get(0)), 1.5, 0.1);
        }
    }

    {
        BOOST_REQUIRE(exp.views().outputs().exist("view1"));
        BOOST_REQUIRE(exp.views().outputs().exist("view2"));
        {
            const vpz::Output& o(exp.views().outputs().get("view1"));
            BOOST_REQUIRE_EQUAL(o.name(), "view1");
            BOOST_REQUIRE_EQUAL(o.format(), vpz::Output::LOCAL);
            BOOST_REQUIRE_EQUAL(o.plugin(), "storage");
            BOOST_REQUIRE_EQUAL(boost::algorithm::trim_copy(o.data()),
                                "10 10 1 1");
        }
        {
            const vpz::Output& o(exp.views().outputs().get("view2"));
            BOOST_REQUIRE_EQUAL(o.name(), "view2");
            BOOST_REQUIRE_EQUAL(o.format(), vpz::Output::LOCAL);
            BOOST_REQUIRE_EQUAL(o.plugin(), "storage");
            BOOST_REQUIRE(o.data().empty());
        }
    }
    {
        BOOST_REQUIRE(exp.views().observables().exist("obs1"));
        BOOST_REQUIRE(exp.views().observables().exist("obs2"));
        {
            const vpz::Observable& o(exp.views().observables().get("obs1"));
            BOOST_REQUIRE(o.exist("c"));
            const vpz::ObservablePort& c(o.get("c"));
            BOOST_REQUIRE_EQUAL(c.viewnamelist().size(),
                                (vpz::ViewNameList::size_type)1);
            BOOST_REQUIRE_EQUAL(c.viewnamelist().front(), "view1");
        }
        {
            const vpz::Observable& o(exp.views().observables().get("obs2"));
            BOOST_REQUIRE(o.exist("nbmodel"));
            {
                const vpz::ObservablePort& c(o.get("nbmodel"));
                BOOST_REQUIRE_EQUAL(c.viewnamelist().size(),
                                    (vpz::ViewNameList::size_type)1);
                BOOST_REQUIRE_EQUAL(c.viewnamelist().front(), "view1");
            }
            BOOST_REQUIRE(o.exist("structure"));
            {
                const vpz::ObservablePort& c(o.get("structure"));
                BOOST_REQUIRE_EQUAL(c.viewnamelist().size(),
                                    (vpz::ViewNameList::size_type)1);
                BOOST_REQUIRE_EQUAL(c.viewnamelist().front(), "view2");
            }
        }
    }
}

void check_classes_unittest_vpz(const vpz::Classes& cls)
{
    BOOST_REQUIRE(cls.exist("beepbeep"));
    {
        const vpz::Class& c(cls.get("beepbeep"));
        BOOST_REQUIRE(c.model());
        BOOST_REQUIRE(c.model()->isCoupled());
        BOOST_REQUIRE_EQUAL(c.model()->getName(), "top");
        graph::CoupledModel* cpled((graph::CoupledModel*)c.model());

        BOOST_REQUIRE(cpled->exist("a"));
        BOOST_REQUIRE(cpled->findModel("a")->isAtomic());
        BOOST_REQUIRE(cpled->exist("b"));
        BOOST_REQUIRE(cpled->findModel("b")->isAtomic());
        BOOST_REQUIRE(cpled->exist("c"));
        BOOST_REQUIRE(cpled->findModel("c")->isAtomic());
        BOOST_REQUIRE(cpled->existOutputConnection("a", "out", "out"));
        BOOST_REQUIRE(cpled->existOutputConnection("b", "out", "out"));
        BOOST_REQUIRE(cpled->existOutputConnection("c", "out", "out"));
    }
    BOOST_REQUIRE(cls.exist("beepbeepbeep"));
    {
        const vpz::Class& c(cls.get("beepbeepbeep"));
        BOOST_REQUIRE(c.model());
        BOOST_REQUIRE(c.model()->isCoupled());
        BOOST_REQUIRE_EQUAL(c.model()->getName(), "top");
        graph::CoupledModel* cpled((graph::CoupledModel*)c.model());

        BOOST_REQUIRE(cpled->exist("a"));
        BOOST_REQUIRE(cpled->findModel("a")->isAtomic());
        BOOST_REQUIRE(cpled->exist("b"));
        BOOST_REQUIRE(cpled->findModel("b")->isAtomic());
        BOOST_REQUIRE(cpled->exist("c"));
        BOOST_REQUIRE(cpled->findModel("c")->isAtomic());
        BOOST_REQUIRE(cpled->exist("d"));
        BOOST_REQUIRE(cpled->findModel("d")->isCoupled());
        {
            graph::CoupledModel* cpled_d(
                graph::Model::toCoupled(cpled->findModel("d")));

            BOOST_REQUIRE(cpled_d->exist("a"));
            BOOST_REQUIRE(cpled_d->findModel("a")->isAtomic());
            BOOST_REQUIRE(cpled_d->exist("b"));
            BOOST_REQUIRE(cpled_d->findModel("b")->isAtomic());
            BOOST_REQUIRE(cpled_d->exist("c"));
            BOOST_REQUIRE(cpled_d->findModel("c")->isAtomic());
            BOOST_REQUIRE(cpled_d->existOutputConnection("a", "out", "out"));
            BOOST_REQUIRE(cpled_d->existOutputConnection("b", "out", "out"));
            BOOST_REQUIRE(cpled_d->existOutputConnection("c", "out", "out"));
        }
        BOOST_REQUIRE(cpled->existOutputConnection("a", "out", "out"));
        BOOST_REQUIRE(cpled->existOutputConnection("b", "out", "out"));
        BOOST_REQUIRE(cpled->existOutputConnection("c", "out", "out"));
        BOOST_REQUIRE(cpled->existOutputConnection("d", "out", "out"));
    }
}

void check_unittest_vpz(const vpz::Vpz& file)
{
    BOOST_REQUIRE_EQUAL(file.project().author(), "Gauthier Quesnel");
    BOOST_REQUIRE_EQUAL(file.project().version(), "0.6");

    const vpz::Model& model(file.project().model());
    check_model_unittest_vpz(model);

    const vpz::Dynamics& dynamics(file.project().dynamics());
    check_dynamics_unittest_vpz(dynamics);

    const vpz::Experiment& exp(file.project().experiment());
    check_experiment_unittest_vpz(exp);

    const vpz::Classes& cls(file.project().classes());
    check_classes_unittest_vpz(cls);
}

BOOST_AUTO_TEST_CASE(test_rename_conds)
{
    vpz::Vpz vpz;
    vpz.parseFile(utils::Path::path().buildPrefixSharePath(
            utils::Path::path().getPrefixDir(), "examples", "unittest.vpz"));

    BOOST_REQUIRE_EQUAL(vpz.project().author(), "Gauthier Quesnel");
    BOOST_REQUIRE_EQUAL(vpz.project().version(), "0.6");

    vpz::Project proj(vpz.project());
    check_rename_conds_unittest_vpz(proj);

    std::string str(vpz.writeToString());
    delete vpz.project().model().model();
    vpz.clear();

    vpz.parseMemory(str);
}

BOOST_AUTO_TEST_CASE(test_rename_views)
{
    vpz::Vpz vpz;
    vpz.parseFile(utils::Path::path().buildPrefixSharePath(
            utils::Path::path().getPrefixDir(), "examples", "unittest.vpz"));

    BOOST_REQUIRE_EQUAL(vpz.project().author(), "Gauthier Quesnel");
    BOOST_REQUIRE_EQUAL(vpz.project().version(), "0.6");

    vpz::Views& views(vpz.project().experiment().views());
    check_rename_views_unittest_vpz(views);

    delete vpz.project().model().model();
}


BOOST_AUTO_TEST_CASE(test_connection)
{
    vpz::Vpz vpz;
    vpz.parseFile(utils::Path::path().buildPrefixSharePath(
            utils::Path::path().getPrefixDir(), "examples", "unittest.vpz"));

    const vpz::Model& model(vpz.project().model());
    BOOST_REQUIRE(model.model());
    BOOST_REQUIRE(model.model()->isCoupled());

    graph::CoupledModel* cpled((graph::CoupledModel*)model.model());
    BOOST_REQUIRE_EQUAL(cpled->getName(), "top");
    BOOST_REQUIRE(cpled->exist("top1"));
    BOOST_REQUIRE(cpled->exist("top2"));
    BOOST_REQUIRE(cpled->exist("d"));
    BOOST_REQUIRE(cpled->exist("e"));

    graph::CoupledModel* top1((graph::CoupledModel*)cpled->findModel("top1"));
    graph::AtomicModel* x((graph::AtomicModel*)top1->findModel("x"));

    graph::TargetModelList out;
    x->getTargetPortList("out", out);

    BOOST_REQUIRE_EQUAL(out.size(), (graph::TargetModelList::size_type)10);

    delete vpz.project().model().model();
}

BOOST_AUTO_TEST_CASE(test_read_write_read)
{
    vpz::Vpz vpz;
    vpz.parseFile(utils::Path::buildPrefixSharePath(
            utils::Path::path().getPrefixDir(), "examples", "unittest.vpz"));
    check_unittest_vpz(vpz);
    delete vpz.project().model().model();
    vpz.clear();
    vpz.parseFile(utils::Path::path().buildPrefixSharePath(
            utils::Path::path().getPrefixDir(), "examples", "unittest.vpz"));
    check_unittest_vpz(vpz);
    delete vpz.project().model().model();
    vpz.clear();
    vpz.parseFile(utils::Path::path().buildPrefixSharePath(
            utils::Path::path().getPrefixDir(), "examples", "unittest.vpz"));
    check_unittest_vpz(vpz);
    delete vpz.project().model().model();
    vpz.clear();

    vpz.parseFile(utils::Path::path().buildPrefixSharePath(
            utils::Path::path().getPrefixDir(), "examples", "unittest.vpz"));
    check_unittest_vpz(vpz);

    std::string str(vpz.writeToString());
    delete vpz.project().model().model();
    vpz.clear();

    vpz.parseMemory(str);
    check_unittest_vpz(vpz);
}

BOOST_AUTO_TEST_CASE(test_read_write_read2)
{
    vpz::Vpz vpz;
    vpz.parseFile(utils::Path::buildPrefixSharePath(
            utils::Path::path().getPrefixDir(), "examples", "unittest.vpz"));
    vpz::Vpz vpz2(vpz);

    check_unittest_vpz(vpz);
    check_unittest_vpz(vpz2);

    delete vpz.project().model().model();
    delete vpz2.project().model().model();
}
