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
#define BOOST_TEST_MODULE translator_complete_test
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/output_test_stream.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <vle/value/Value.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Double.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/vpz/AtomicModel.hpp>
#include <vle/vpz/CoupledModel.hpp>
#include <vle/utils/Path.hpp>
#include <vle/vle.hpp>

struct F
{
    vle::Init a;

    F() : a() { }
    ~F() { }
};

BOOST_GLOBAL_FIXTURE(F)

using namespace vle;

void check_remove_dyns_unittest_vpz(vpz::Project& project)
{
    vpz::Dynamics& dyns = project.dynamics();

    BOOST_REQUIRE_NO_THROW(dyns.del(std::string("unittest")));

    BOOST_REQUIRE(not project.dynamics().exist("new_unittest"));

    vpz::CoupledModel* top = vpz::BaseModel::toCoupled(project.model().model());
    vpz::CoupledModel* top1 = vpz::BaseModel::toCoupled(top->findModel("top1"));

    std::set < std::string > dynamics = dyns.getKeys();

    BOOST_REQUIRE_NO_THROW(top->purgeDynamics(dynamics));

    vpz::AtomicModel* d = vpz::BaseModel::toAtomic(top->findModel("d"));
    vpz::AtomicModel* e = vpz::BaseModel::toAtomic(top->findModel("e"));
    vpz::AtomicModel* a = vpz::BaseModel::toAtomic(top1->findModel("a"));

    BOOST_REQUIRE_EQUAL(d->dynamics(), "");
    BOOST_REQUIRE_EQUAL(e->dynamics(), "");
    BOOST_REQUIRE_EQUAL(a->dynamics(), "");
}

void check_rename_dyns_unittest_vpz(vpz::Project& project)
{
    vpz::Dynamics& dyns = project.dynamics();

    BOOST_REQUIRE_NO_THROW(
	dyns.rename(std::string("unittest"), std::string("new_unittest")));

    BOOST_REQUIRE(project.dynamics().exist("new_unittest"));

    vpz::CoupledModel* top = vpz::BaseModel::toCoupled(project.model().model());

    BOOST_REQUIRE_NO_THROW(top->updateDynamics("unittest", "new_unittest"));

    vpz::AtomicModel* d = vpz::BaseModel::toAtomic(top->findModel("d"));

    BOOST_REQUIRE_EQUAL(d->dynamics(), "new_unittest");
}

void check_remove_conds_unittest_vpz(vpz::Project& project)
{
    vpz::Conditions& cnds = project.experiment().conditions();

    BOOST_REQUIRE_NO_THROW(cnds.del(std::string("cd")));

    BOOST_REQUIRE(not cnds.exist("cd"));

    vpz::CoupledModel* top = vpz::BaseModel::toCoupled(project.model().model());

    std::set < std::string > conditions = cnds.getKeys();

    BOOST_REQUIRE_NO_THROW(top->purgeConditions(conditions));

    vpz::AtomicModel* d = vpz::BaseModel::toAtomic(top->findModel("d"));

    std::vector < std::string > atomConditions = d->conditions();

    BOOST_REQUIRE(
        std::find(atomConditions.begin(),
                  atomConditions.end(), "cd") == atomConditions.end());

}

void check_rename_conds_unittest_vpz(vpz::Project& project)
{
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

    vpz::CoupledModel* top = vpz::BaseModel::toCoupled(project.model().model());

    BOOST_REQUIRE_NO_THROW(top->updateConditions("cd", "new_cd"));

    vpz::AtomicModel* d = vpz::BaseModel::toAtomic(top->findModel("d"));

    std::vector < std::string > conditions = d->conditions();

    BOOST_REQUIRE(
        std::find(conditions.begin(),
                  conditions.end(), "new_cd") != conditions.end());
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

void check_remove_obs_unittest_vpz(vpz::Project& project)
{
    vpz::Observables& obs_list = project.experiment().views().observables();

    BOOST_REQUIRE_NO_THROW(obs_list.del(std::string("obs1")));
    BOOST_REQUIRE_NO_THROW(obs_list.del(std::string("obs2")));

    BOOST_REQUIRE(not obs_list.exist("obs1"));

    vpz::CoupledModel* top = vpz::BaseModel::toCoupled(project.model().model());
    vpz::CoupledModel* top1 = vpz::BaseModel::toCoupled(top->findModel("top1"));

    std::set < std::string > obs = obs_list.getKeys();

    BOOST_REQUIRE_NO_THROW(top->purgeObservable(obs));

    vpz::AtomicModel* a = vpz::BaseModel::toAtomic(top1->findModel("a"));
    vpz::AtomicModel* b = vpz::BaseModel::toAtomic(top1->findModel("b"));
    vpz::AtomicModel* c = vpz::BaseModel::toAtomic(top1->findModel("c"));
    vpz::AtomicModel* x = vpz::BaseModel::toAtomic(top1->findModel("x"));

    BOOST_REQUIRE_EQUAL(a->observables(), "");
    BOOST_REQUIRE_EQUAL(b->observables(), "");
    BOOST_REQUIRE_EQUAL(c->observables(), "");
    BOOST_REQUIRE_EQUAL(x->observables(), "");
}

void check_rename_observables_unittest_vpz(vpz::Project& project)
{
    vpz::Observables& obs_list = project.experiment().views().observables();
    BOOST_REQUIRE_NO_THROW(obs_list.rename("obs1", "new_obs1"));
    BOOST_REQUIRE_NO_THROW(obs_list.rename("obs2", "new_obs2"));

    BOOST_REQUIRE(obs_list.exist("new_obs1"));
    BOOST_REQUIRE(obs_list.exist("new_obs2"));

    BOOST_REQUIRE(not obs_list.exist("obs1"));
    BOOST_REQUIRE(not obs_list.exist("obs2"));

    vpz::CoupledModel* top = vpz::BaseModel::toCoupled(project.model().model());
    vpz::CoupledModel* top1 = vpz::BaseModel::toCoupled(top->findModel("top1"));

    BOOST_REQUIRE_NO_THROW(top->updateObservable("obs2", "new_obs2"));
    BOOST_REQUIRE_NO_THROW(top->updateObservable("obs1", "new_obs1"));

    vpz::AtomicModel* a = vpz::BaseModel::toAtomic(top1->findModel("a"));
    vpz::AtomicModel* x = vpz::BaseModel::toAtomic(top1->findModel("x"));

    BOOST_REQUIRE_EQUAL(a->observables(), "new_obs2");
    BOOST_REQUIRE_EQUAL(x->observables(), "new_obs1");
}


void check_model_unittest_vpz(const vpz::Model& model)
{
    BOOST_REQUIRE(model.model());
    BOOST_REQUIRE(model.model()->isCoupled());

    vpz::CoupledModel* cpled((vpz::CoupledModel*)model.model());
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
        vpz::CoupledModel* top1((vpz::CoupledModel*)cpled->findModel("top1"));
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
        vpz::CoupledModel* top2((vpz::CoupledModel*)cpled->findModel("top2"));
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
    BOOST_REQUIRE_EQUAL(unittest.library(), "DevsTransform");

    vpz::Dynamic b(dynamics.get("b"));
    BOOST_REQUIRE_EQUAL(b.name(), "b");
    BOOST_REQUIRE_EQUAL(b.library(), "libu");
    BOOST_REQUIRE_EQUAL(b.language(), "python");

    vpz::Dynamic a(dynamics.get("a"));
    BOOST_REQUIRE_EQUAL(a.name(), "a");
    BOOST_REQUIRE_EQUAL(a.library(), "liba");
    BOOST_REQUIRE_EQUAL(a.language(), "python");
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
            BOOST_REQUIRE(x.get(0)->isDouble());
            BOOST_CHECK_CLOSE(value::toDouble(x.get(0)), 1.2, 0.1);
        }
        {
            const vpz::Condition& c(exp.conditions().get("cb"));
            const value::Set& x(c.getSetValues("x"));
            BOOST_REQUIRE(not x.value().empty());
            BOOST_REQUIRE(x.get(0)->isDouble());
            BOOST_CHECK_CLOSE(value::toDouble(x.get(0)), 1.3, 0.1);
        }
        {
            const vpz::Condition& c(exp.conditions().get("cc"));
            const value::Set& x(c.getSetValues("x"));
            BOOST_REQUIRE(not x.value().empty());
            BOOST_REQUIRE(x.get(0)->isDouble());
            BOOST_CHECK_CLOSE(value::toDouble(x.get(0)), 1.4, 0.1);
        }
        {
            const vpz::Condition& c(exp.conditions().get("cd"));
            const value::Set& x(c.getSetValues("x"));
            BOOST_REQUIRE(not x.value().empty());
            BOOST_REQUIRE(x.get(0)->isDouble());
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

            BOOST_REQUIRE(o.data() != (value::Value*)0);
            BOOST_REQUIRE(o.data()->isMap());

            const value::Map* map(value::toMapValue(o.data()));

            BOOST_REQUIRE(map->exist("columns"));
            BOOST_REQUIRE(map->exist("rows"));
            BOOST_REQUIRE(map->exist("inc_columns"));
            BOOST_REQUIRE(map->exist("inc_rows"));

            const value::Value& columns(value::reference(map->get("columns")));
            BOOST_REQUIRE(columns.isInteger());
            BOOST_REQUIRE_EQUAL(value::toInteger(columns), 5);

            const value::Value& rows(value::reference(map->get("rows")));
            BOOST_REQUIRE(rows.isInteger());
            BOOST_REQUIRE_EQUAL(value::toInteger(rows), 100);

            const value::Value&
                inccolumns(value::reference(map->get("inc_columns")));
            BOOST_REQUIRE(inccolumns.isInteger());
            BOOST_REQUIRE_EQUAL(value::toInteger(inccolumns), 10);

            const value::Value& incrows(value::reference(map->get("inc_rows")));
            BOOST_REQUIRE(incrows.isInteger());
            BOOST_REQUIRE_EQUAL(value::toInteger(incrows), 50);
        }
        {
            const vpz::Output& o(exp.views().outputs().get("view2"));
            BOOST_REQUIRE_EQUAL(o.name(), "view2");
            BOOST_REQUIRE_EQUAL(o.format(), vpz::Output::LOCAL);
            BOOST_REQUIRE_EQUAL(o.plugin(), "storage");
            BOOST_REQUIRE_EQUAL(o.data(), (value::Value*)0);
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

void check_classes_unittest_vpz(vpz::Classes& cls)
{
    void* ptr1 = 0;
    void* ptr2 = 0;

    BOOST_REQUIRE(cls.exist("beepbeep"));
    {
        const vpz::Class& c(cls.get("beepbeep"));
        BOOST_REQUIRE(c.model());
        BOOST_REQUIRE(c.model()->isCoupled());
        BOOST_REQUIRE_EQUAL(c.model()->getName(), "top");
        vpz::CoupledModel* cpled((vpz::CoupledModel*)c.model());

        ptr1 = cpled;

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
        vpz::CoupledModel* cpled((vpz::CoupledModel*)c.model());

        ptr2 = cpled;

        BOOST_REQUIRE(cpled->exist("a"));
        BOOST_REQUIRE(cpled->findModel("a")->isAtomic());
        BOOST_REQUIRE(cpled->exist("b"));
        BOOST_REQUIRE(cpled->findModel("b")->isAtomic());
        BOOST_REQUIRE(cpled->exist("c"));
        BOOST_REQUIRE(cpled->findModel("c")->isAtomic());
        BOOST_REQUIRE(cpled->exist("d"));
        BOOST_REQUIRE(cpled->findModel("d")->isCoupled());
        {
            vpz::CoupledModel* cpled_d(
                vpz::BaseModel::toCoupled(cpled->findModel("d")));

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

    cls.rename("beepbeep", "newbeepbeep");
    cls.rename("beepbeepbeep", "newbeepbeepbeep");

    BOOST_REQUIRE(cls.exist("newbeepbeep"));
    {
        const vpz::Class& c(cls.get("newbeepbeep"));
        BOOST_REQUIRE(c.model());
        BOOST_REQUIRE(c.model()->isCoupled());
        BOOST_REQUIRE_EQUAL(c.model()->getName(), "top");
        vpz::CoupledModel* cpled((vpz::CoupledModel*)c.model());

        BOOST_REQUIRE(ptr1 != cpled);

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

    BOOST_REQUIRE(cls.exist("newbeepbeepbeep"));
    {
        const vpz::Class& c(cls.get("newbeepbeepbeep"));
        BOOST_REQUIRE(c.model());
        BOOST_REQUIRE(c.model()->isCoupled());
        BOOST_REQUIRE_EQUAL(c.model()->getName(), "top");
        vpz::CoupledModel* cpled((vpz::CoupledModel*)c.model());

        BOOST_REQUIRE(ptr2 != cpled);

        BOOST_REQUIRE(cpled->exist("a"));
        BOOST_REQUIRE(cpled->findModel("a")->isAtomic());
        BOOST_REQUIRE(cpled->exist("b"));
        BOOST_REQUIRE(cpled->findModel("b")->isAtomic());
        BOOST_REQUIRE(cpled->exist("c"));
        BOOST_REQUIRE(cpled->findModel("c")->isAtomic());
        BOOST_REQUIRE(cpled->exist("d"));
        BOOST_REQUIRE(cpled->findModel("d")->isCoupled());
        {
            vpz::CoupledModel* cpled_d(
                vpz::BaseModel::toCoupled(cpled->findModel("d")));

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

    BOOST_REQUIRE(not cls.exist("beepbeep"));
    BOOST_REQUIRE(not cls.exist("beepbeepbeep"));

    cls.rename("newbeepbeep", "beepbeep");
    cls.rename("newbeepbeepbeep", "beepbeepbeep");
}

void check_unittest_vpz(vpz::Vpz& file)
{
    BOOST_REQUIRE_EQUAL(file.project().author(), "Gauthier Quesnel");
    BOOST_REQUIRE_EQUAL(file.project().version(), "0.6");

    const vpz::Model& model(file.project().model());
    check_model_unittest_vpz(model);

    const vpz::Dynamics& dynamics(file.project().dynamics());
    check_dynamics_unittest_vpz(dynamics);

    const vpz::Experiment& exp(file.project().experiment());
    check_experiment_unittest_vpz(exp);

    vpz::Classes& cls(file.project().classes());
    check_classes_unittest_vpz(cls);
}

void check_copy_views_unittest_vpz(vpz::Views& views)
{
    BOOST_REQUIRE_NO_THROW(views.copyView("view1", "view1_1"));
    BOOST_REQUIRE_NO_THROW(views.copyView("view1", "view1_2"));
    BOOST_REQUIRE_NO_THROW(views.copyView("view1_1", "view1_1_1"));

    BOOST_REQUIRE(views.exist("view1_1"));
    BOOST_REQUIRE(views.exist("view1_2"));
    BOOST_REQUIRE(views.exist("view1_1_1"));

    BOOST_CHECK(views.get("view1_1").output() == "view1_1");
    BOOST_CHECK(views.get("view1_2").output() == "view1_2");
    BOOST_CHECK(views.get("view1_1_1").output() == "view1_1_1");

    BOOST_REQUIRE_NO_THROW(views.del("view1"));

    BOOST_REQUIRE(views.exist("view1_1"));
    BOOST_REQUIRE(views.exist("view1_2"));
    BOOST_REQUIRE(views.exist("view1_1_1"));

}

void check_equal_views_unittest_vpz(vpz::Views views)
{
    BOOST_REQUIRE(views.exist("view1"));
    BOOST_REQUIRE(views.exist("view2"));

    BOOST_REQUIRE_EQUAL(views.get("view1") == views.get("view2"), false);
    BOOST_REQUIRE_EQUAL(views.get("view1") == views.get("view1"), true);

    BOOST_REQUIRE_NO_THROW(views.copyView("view1", "view1_1"));
    BOOST_REQUIRE_EQUAL(views.get("view1") == views.get("view1_1"), false);
}


void check_equal_dynamics_unittest_vpz(vpz::Dynamics dynamics)
{
    BOOST_REQUIRE(dynamics.exist("a"));
    BOOST_REQUIRE(dynamics.exist("b"));
    BOOST_REQUIRE(dynamics.exist("unittest"));

    BOOST_REQUIRE_EQUAL(dynamics.get("a") == dynamics.get("b"), false);
    BOOST_REQUIRE_EQUAL(dynamics.get("unittest") == dynamics.get("unittest"), true);
}

void check_equal_outputs_unittest_vpz(vpz::Outputs outputs)
{
    BOOST_REQUIRE(outputs.exist("view1"));
    BOOST_REQUIRE(outputs.exist("view2"));

    BOOST_REQUIRE_EQUAL(outputs.get("view1") == outputs.get("view2"), false);
    BOOST_REQUIRE_EQUAL(outputs.get("view1") == outputs.get("view1"), true);
}

BOOST_AUTO_TEST_CASE(test_remove_dyns)
{
    vpz::Vpz vpz;
    vpz.parseFile(utils::Path::path().getTemplate("unittest.vpz"));

    BOOST_REQUIRE_EQUAL(vpz.project().author(), "Gauthier Quesnel");
    BOOST_REQUIRE_EQUAL(vpz.project().version(), "0.6");

    vpz::Project proj(vpz.project());
    check_remove_dyns_unittest_vpz(proj);

    std::string str(vpz.writeToString());

    delete vpz.project().model().model();
    vpz.clear();

    vpz.parseMemory(str);
}

BOOST_AUTO_TEST_CASE(test_rename_dyns)
{
    vpz::Vpz vpz;
    vpz.parseFile(utils::Path::path().getTemplate("unittest.vpz"));

    BOOST_REQUIRE_EQUAL(vpz.project().author(), "Gauthier Quesnel");
    BOOST_REQUIRE_EQUAL(vpz.project().version(), "0.6");

    vpz::Project proj(vpz.project());
    check_rename_dyns_unittest_vpz(proj);

    std::string str(vpz.writeToString());
    delete vpz.project().model().model();
    vpz.clear();

    vpz.parseMemory(str);
}

BOOST_AUTO_TEST_CASE(test_remove_conds)
{
    vpz::Vpz vpz;
    vpz.parseFile(utils::Path::path().getTemplate("unittest.vpz"));

    BOOST_REQUIRE_EQUAL(vpz.project().author(), "Gauthier Quesnel");
    BOOST_REQUIRE_EQUAL(vpz.project().version(), "0.6");

    vpz::Project proj(vpz.project());
    check_remove_conds_unittest_vpz(proj);

    std::string str(vpz.writeToString());
    delete vpz.project().model().model();
    vpz.clear();

    vpz.parseMemory(str);
}

BOOST_AUTO_TEST_CASE(test_rename_conds)
{
    vpz::Vpz vpz;
    vpz.parseFile(utils::Path::path().getTemplate("unittest.vpz"));

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
    vpz.parseFile(utils::Path::path().getTemplate("unittest.vpz"));

    BOOST_REQUIRE_EQUAL(vpz.project().author(), "Gauthier Quesnel");
    BOOST_REQUIRE_EQUAL(vpz.project().version(), "0.6");

    vpz::Views& views(vpz.project().experiment().views());
    check_rename_views_unittest_vpz(views);

    delete vpz.project().model().model();
}

BOOST_AUTO_TEST_CASE(test_remove_observables)
{
    vpz::Vpz vpz;
    vpz.parseFile(utils::Path::path().getTemplate("unittest.vpz"));

    BOOST_REQUIRE_EQUAL(vpz.project().author(), "Gauthier Quesnel");
    BOOST_REQUIRE_EQUAL(vpz.project().version(), "0.6");

    vpz::Project proj(vpz.project());
    check_remove_obs_unittest_vpz(proj);

    std::string str(vpz.writeToString());
    delete vpz.project().model().model();
    vpz.clear();

    vpz.parseMemory(str);
}

BOOST_AUTO_TEST_CASE(test_rename_observables)
{
    vpz::Vpz vpz;
    vpz.parseFile(utils::Path::path().getTemplate("unittest.vpz"));

    BOOST_REQUIRE_EQUAL(vpz.project().author(), "Gauthier Quesnel");
    BOOST_REQUIRE_EQUAL(vpz.project().version(), "0.6");

    vpz::Project proj(vpz.project());
    check_rename_observables_unittest_vpz(proj);

    std::string str(vpz.writeToString());
    delete vpz.project().model().model();
    vpz.clear();

    vpz.parseMemory(str);
}


BOOST_AUTO_TEST_CASE(test_connection)
{
    vpz::Vpz vpz;
    vpz.parseFile(utils::Path::path().getTemplate("unittest.vpz"));

    const vpz::Model& model(vpz.project().model());
    BOOST_REQUIRE(model.model());
    BOOST_REQUIRE(model.model()->isCoupled());

    vpz::CoupledModel* cpled((vpz::CoupledModel*)model.model());
    BOOST_REQUIRE_EQUAL(cpled->getName(), "top");
    BOOST_REQUIRE(cpled->exist("top1"));
    BOOST_REQUIRE(cpled->exist("top2"));
    BOOST_REQUIRE(cpled->exist("d"));
    BOOST_REQUIRE(cpled->exist("e"));

    vpz::CoupledModel* top1((vpz::CoupledModel*)cpled->findModel("top1"));
    vpz::AtomicModel* x((vpz::AtomicModel*)top1->findModel("x"));

    vpz::ModelPortList out;
    x->getAtomicModelsTarget("out", out);

    BOOST_REQUIRE_EQUAL(out.size(), (vpz::ModelPortList::size_type)10);

    delete vpz.project().model().model();
}

BOOST_AUTO_TEST_CASE(test_read_write_read)
{
    vpz::Vpz vpz;
    vpz.parseFile(utils::Path::path().getTemplate("unittest.vpz"));
    check_unittest_vpz(vpz);
    delete vpz.project().model().model();
    vpz.clear();
    vpz.parseFile(utils::Path::path().getTemplate("unittest.vpz"));
    check_unittest_vpz(vpz);
    delete vpz.project().model().model();
    vpz.clear();
    vpz.parseFile(utils::Path::path().getTemplate("unittest.vpz"));
    check_unittest_vpz(vpz);
    delete vpz.project().model().model();
    vpz.clear();

    vpz.parseFile(utils::Path::path().getTemplate("unittest.vpz"));
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
    vpz.parseFile(utils::Path::path().getTemplate("unittest.vpz"));
    vpz::Vpz vpz2(vpz);

    check_unittest_vpz(vpz);
    check_unittest_vpz(vpz2);

    delete vpz.project().model().model();
    delete vpz2.project().model().model();
}

BOOST_AUTO_TEST_CASE(test_copy_del_views)
{
    vpz::Vpz vpz;
    vpz.parseFile(utils::Path::path().getTemplate("unittest.vpz"));

    BOOST_REQUIRE_EQUAL(vpz.project().author(), "Gauthier Quesnel");
    BOOST_REQUIRE_EQUAL(vpz.project().version(), "0.6");

    vpz::Views& views(vpz.project().experiment().views());
    check_copy_views_unittest_vpz(views);

    delete vpz.project().model().model();
}

BOOST_AUTO_TEST_CASE(test_equal_dynamics)
{
    vpz::Vpz vpz;
    vpz.parseFile(utils::Path::path().getTemplate("unittest.vpz"));

    BOOST_REQUIRE_EQUAL(vpz.project().author(), "Gauthier Quesnel");
    BOOST_REQUIRE_EQUAL(vpz.project().version(), "0.6");

    vpz::Dynamics& dynamics(vpz.project().dynamics());
    check_equal_dynamics_unittest_vpz(dynamics);
    delete vpz.project().model().model();
}

BOOST_AUTO_TEST_CASE(test_equal_outputs)
{
    vpz::Vpz vpz;
    vpz.parseFile(utils::Path::path().getTemplate("unittest.vpz"));

    BOOST_REQUIRE_EQUAL(vpz.project().author(), "Gauthier Quesnel");
    BOOST_REQUIRE_EQUAL(vpz.project().version(), "0.6");

    vpz::Outputs& outputs(vpz.project().experiment().views().outputs());
    check_equal_outputs_unittest_vpz(outputs);
    delete vpz.project().model().model();
}

BOOST_AUTO_TEST_CASE(test_equal_views)
{
    vpz::Vpz vpz;
    vpz.parseFile(utils::Path::path().getTemplate("unittest.vpz"));

    BOOST_REQUIRE_EQUAL(vpz.project().author(), "Gauthier Quesnel");
    BOOST_REQUIRE_EQUAL(vpz.project().version(), "0.6");

    vpz::Views& views(vpz.project().experiment().views());
    check_equal_views_unittest_vpz(views);
    delete vpz.project().model().model();
}

