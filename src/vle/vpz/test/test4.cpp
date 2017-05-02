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

#include <boost/algorithm/string.hpp>
#include <vle/utils/Context.hpp>
#include <vle/utils/unit-test.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Value.hpp>
#include <vle/vle.hpp>
#include <vle/vpz/AtomicModel.hpp>
#include <vle/vpz/CoupledModel.hpp>
#include <vle/vpz/Vpz.hpp>

using namespace vle;

void
check_remove_dyns_unittest_vpz(vpz::Project& project)
{
    vpz::Dynamics& dyns = project.dynamics();

    EnsuresNotThrow(dyns.del(std::string("unittest")), std::exception);

    Ensures(not project.dynamics().exist("new_unittest"));

    vpz::CoupledModel* top = vpz::BaseModel::toCoupled(project.model().node());
    vpz::CoupledModel* top1 =
      vpz::BaseModel::toCoupled(top->findModel("top1"));

    std::set<std::string> dynamics = dyns.getKeys();

    EnsuresNotThrow(top->purgeDynamics(dynamics), std::exception);

    vpz::AtomicModel* d = vpz::BaseModel::toAtomic(top->findModel("d"));
    vpz::AtomicModel* e = vpz::BaseModel::toAtomic(top->findModel("e"));
    vpz::AtomicModel* a = vpz::BaseModel::toAtomic(top1->findModel("a"));

    EnsuresEqual(d->dynamics(), "");
    EnsuresEqual(e->dynamics(), "");
    EnsuresEqual(a->dynamics(), "");
}

void
check_rename_dyns_unittest_vpz(vpz::Project& project)
{
    vpz::Dynamics& dyns = project.dynamics();

    EnsuresNotThrow(
      dyns.rename(std::string("unittest"), std::string("new_unittest")),
      std::exception);

    Ensures(project.dynamics().exist("new_unittest"));

    vpz::CoupledModel* top = vpz::BaseModel::toCoupled(project.model().node());

    EnsuresNotThrow(top->updateDynamics("unittest", "new_unittest"),
                    std::exception);

    vpz::AtomicModel* d = vpz::BaseModel::toAtomic(top->findModel("d"));

    EnsuresEqual(d->dynamics(), "new_unittest");
}

void
check_remove_conds_unittest_vpz(vpz::Project& project)
{
    vpz::Conditions& cnds = project.experiment().conditions();

    EnsuresNotThrow(cnds.del(std::string("cd")), std::exception);

    Ensures(not cnds.exist("cd"));

    vpz::CoupledModel* top = vpz::BaseModel::toCoupled(project.model().node());

    std::set<std::string> conditions = cnds.getKeys();

    EnsuresNotThrow(top->purgeConditions(conditions), std::exception);

    vpz::AtomicModel* d = vpz::BaseModel::toAtomic(top->findModel("d"));

    std::vector<std::string> atomConditions = d->conditions();

    Ensures(std::find(atomConditions.begin(), atomConditions.end(), "cd") ==
            atomConditions.end());
}

void
check_rename_conds_unittest_vpz(vpz::Project& project)
{
    vpz::Conditions& cnds = project.experiment().conditions();

    EnsuresNotThrow(cnds.rename(std::string("ca"), std::string("new_ca")),
                    std::exception);
    EnsuresNotThrow(cnds.rename(std::string("cb"), std::string("new_cb")),
                    std::exception);
    EnsuresNotThrow(cnds.rename(std::string("cc"), std::string("new_cc")),
                    std::exception);
    EnsuresNotThrow(cnds.rename(std::string("cd"), std::string("new_cd")),
                    std::exception);

    Ensures(project.experiment().conditions().exist("new_ca"));
    Ensures(project.experiment().conditions().exist("new_cb"));
    Ensures(project.experiment().conditions().exist("new_cc"));
    Ensures(project.experiment().conditions().exist("new_cd"));

    Ensures(not project.experiment().conditions().exist("ca"));
    Ensures(not project.experiment().conditions().exist("cb"));
    Ensures(not project.experiment().conditions().exist("cc"));
    Ensures(not project.experiment().conditions().exist("cd"));

    vpz::CoupledModel* top = vpz::BaseModel::toCoupled(project.model().node());

    EnsuresNotThrow(top->updateConditions("cd", "new_cd"), std::exception);

    vpz::AtomicModel* d = vpz::BaseModel::toAtomic(top->findModel("d"));

    std::vector<std::string> conditions = d->conditions();

    Ensures(std::find(conditions.begin(), conditions.end(), "new_cd") !=
            conditions.end());
}

void
check_rename_views_unittest_vpz(vpz::Views& views)
{
    EnsuresNotThrow(views.renameView("view1", "new_view1"), std::exception);
    EnsuresNotThrow(views.renameView("view2", "new_view2"), std::exception);

    Ensures(views.exist("new_view1"));
    Ensures(views.exist("new_view2"));

    Ensures(not views.exist("view1"));
    Ensures(not views.exist("view2"));

    vpz::Observables& obs_list = views.observables();
    EnsuresNotThrow(obs_list.updateView("view1", "new_view1"), std::exception);
    EnsuresNotThrow(obs_list.updateView("view2", "new_view2"), std::exception);

    Ensures(views.observables().get("obs1").hasView("new_view1"));
    Ensures(views.observables().get("obs2").hasView("new_view1"));
    Ensures(views.observables().get("obs2").hasView("new_view2"));

    Ensures(views.get("new_view1").output() == "new_view1");
    Ensures(views.get("new_view2").output() == "new_view2");
}

void
check_remove_obs_unittest_vpz(vpz::Project& project)
{
    vpz::Observables& obs_list = project.experiment().views().observables();

    EnsuresNotThrow(obs_list.del(std::string("obs1")), std::exception);
    EnsuresNotThrow(obs_list.del(std::string("obs2")), std::exception);

    Ensures(not obs_list.exist("obs1"));

    vpz::CoupledModel* top = vpz::BaseModel::toCoupled(project.model().node());
    vpz::CoupledModel* top1 =
      vpz::BaseModel::toCoupled(top->findModel("top1"));

    std::set<std::string> obs = obs_list.getKeys();

    EnsuresNotThrow(top->purgeObservable(obs), std::exception);

    vpz::AtomicModel* a = vpz::BaseModel::toAtomic(top1->findModel("a"));
    vpz::AtomicModel* b = vpz::BaseModel::toAtomic(top1->findModel("b"));
    vpz::AtomicModel* c = vpz::BaseModel::toAtomic(top1->findModel("c"));
    vpz::AtomicModel* x = vpz::BaseModel::toAtomic(top1->findModel("x"));

    EnsuresEqual(a->observables(), "");
    EnsuresEqual(b->observables(), "");
    EnsuresEqual(c->observables(), "");
    EnsuresEqual(x->observables(), "");
}

void
check_rename_observables_unittest_vpz(vpz::Project& project)
{
    vpz::Observables& obs_list = project.experiment().views().observables();
    EnsuresNotThrow(obs_list.rename("obs1", "new_obs1"), std::exception);
    EnsuresNotThrow(obs_list.rename("obs2", "new_obs2"), std::exception);

    Ensures(obs_list.exist("new_obs1"));
    Ensures(obs_list.exist("new_obs2"));

    Ensures(not obs_list.exist("obs1"));
    Ensures(not obs_list.exist("obs2"));

    vpz::CoupledModel* top = vpz::BaseModel::toCoupled(project.model().node());
    vpz::CoupledModel* top1 =
      vpz::BaseModel::toCoupled(top->findModel("top1"));

    EnsuresNotThrow(top->updateObservable("obs2", "new_obs2"), std::exception);
    EnsuresNotThrow(top->updateObservable("obs1", "new_obs1"), std::exception);

    vpz::AtomicModel* a = vpz::BaseModel::toAtomic(top1->findModel("a"));
    vpz::AtomicModel* x = vpz::BaseModel::toAtomic(top1->findModel("x"));

    EnsuresEqual(a->observables(), "new_obs2");
    EnsuresEqual(x->observables(), "new_obs1");
}

void
check_model_unittest_vpz(const vpz::Model& model)
{
    Ensures(model.node());
    Ensures(model.node()->isCoupled());

    vpz::CoupledModel* cpled((vpz::CoupledModel*)model.node());
    EnsuresEqual(cpled->getName(), "top");
    Ensures(cpled->exist("top1"));
    Ensures(cpled->findModel("top1")->isCoupled());
    Ensures(cpled->exist("top2"));
    Ensures(cpled->findModel("top2")->isCoupled());
    Ensures(cpled->exist("d"));
    Ensures(cpled->findModel("d")->isAtomic());
    Ensures(cpled->exist("e"));
    Ensures(cpled->findModel("e")->isAtomic());

    {
        vpz::CoupledModel* top1((vpz::CoupledModel*)cpled->findModel("top1"));
        {
            Ensures(top1->exist("x"));
            Ensures(top1->findModel("x")->isAtomic());
            Ensures(top1->exist("a"));
            Ensures(top1->findModel("a")->isAtomic());
            Ensures(top1->exist("b"));
            Ensures(top1->findModel("b")->isAtomic());
            Ensures(top1->exist("c"));
            Ensures(top1->findModel("c")->isAtomic());
            Ensures(top1->existInputConnection("in", "x", "in"));
            Ensures(top1->existInternalConnection("x", "out", "x", "in"));
            Ensures(top1->existInternalConnection("x", "out", "a", "in"));
            Ensures(top1->existInternalConnection("x", "out", "b", "in"));
            Ensures(top1->existInternalConnection("x", "out", "a", "in"));
            Ensures(top1->existInternalConnection("x", "out", "c", "in1"));
            Ensures(top1->existInternalConnection("x", "out", "c", "in2"));
            Ensures(top1->existOutputConnection("x", "out", "out"));
        }
        vpz::CoupledModel* top2((vpz::CoupledModel*)cpled->findModel("top2"));
        {
            Ensures(top2->exist("f"));
            Ensures(top2->findModel("f")->isAtomic());
            Ensures(top2->exist("g"));
            Ensures(top2->findModel("g")->isAtomic());
            Ensures(top2->existInputConnection("in", "f", "in"));
            Ensures(top2->existInputConnection("in", "g", "in"));
        }
        Ensures(cpled->existInternalConnection("top1", "out", "d", "in"));
        Ensures(cpled->existInternalConnection("top1", "out", "e", "in1"));
        Ensures(cpled->existInternalConnection("top1", "out", "e", "in2"));
        Ensures(cpled->existInternalConnection("top1", "out", "top2", "in"));
        Ensures(cpled->existInternalConnection("top2", "out", "top1", "in"));
    }
}

void
check_dynamics_unittest_vpz(const vpz::Dynamics& dynamics)
{
    Ensures(dynamics.exist("unittest"));
    Ensures(dynamics.exist("b"));
    Ensures(dynamics.exist("a"));

    vpz::Dynamic unittest(dynamics.get("unittest"));
    EnsuresEqual(unittest.name(), "unittest");
    EnsuresEqual(unittest.library(), "DevsTransform");

    vpz::Dynamic b(dynamics.get("b"));
    EnsuresEqual(b.name(), "b");
    EnsuresEqual(b.library(), "libu");
    EnsuresEqual(b.language(), "python");

    vpz::Dynamic a(dynamics.get("a"));
    EnsuresEqual(a.name(), "a");
    EnsuresEqual(a.library(), "liba");
    EnsuresEqual(a.language(), "python");
}

void
check_experiment_unittest_vpz(const vpz::Experiment& exp)
{
    {
        Ensures(exp.conditions().exist("ca"));
        Ensures(exp.conditions().exist("cb"));
        Ensures(exp.conditions().exist("cc"));
        Ensures(exp.conditions().exist("cd"));
        {
            const vpz::Condition& c(exp.conditions().get("ca"));

            const auto& x(c.getSetValues("x"));
            Ensures(not x.empty());
            Ensures(x[0]->isDouble());
            EnsuresApproximatelyEqual(value::toDouble(x[0]), 1.2, 0.1);
        }
        {
            const vpz::Condition& c(exp.conditions().get("cb"));
            const auto& x(c.getSetValues("x"));
            Ensures(not x.empty());
            Ensures(x[0]->isDouble());
            EnsuresApproximatelyEqual(value::toDouble(x[0]), 1.3, 0.1);
        }
        {
            const vpz::Condition& c(exp.conditions().get("cc"));
            const auto& x(c.getSetValues("x"));
            Ensures(not x.empty());
            Ensures(x[0]->isDouble());
            EnsuresApproximatelyEqual(value::toDouble(x[0]), 1.4, 0.1);
        }
        {
            const vpz::Condition& c(exp.conditions().get("cd"));
            const auto& x(c.getSetValues("x"));
            Ensures(not x.empty());
            Ensures(x[0]->isDouble());
            EnsuresApproximatelyEqual(value::toDouble(x[0]), 1.5, 0.1);
        }
    }

    {
        Ensures(exp.views().outputs().exist("view1"));
        Ensures(exp.views().outputs().exist("view2"));
        {
            const vpz::Output& o(exp.views().outputs().get("view1"));
            EnsuresEqual(o.name(), "view1");
            EnsuresEqual(o.plugin(), "storage");

            Ensures(o.data().get());
            Ensures(o.data()->isMap());

            const value::Map& map(o.data()->toMap());

            Ensures(map.exist("columns"));
            Ensures(map.exist("rows"));
            Ensures(map.exist("inc_columns"));
            Ensures(map.exist("inc_rows"));

            const value::Value& columns(value::reference(map.get("columns")));
            Ensures(columns.isInteger());
            EnsuresEqual(value::toInteger(columns), 5);

            const value::Value& rows(value::reference(map.get("rows")));
            Ensures(rows.isInteger());
            EnsuresEqual(value::toInteger(rows), 100);

            const value::Value& inccolumns(
              value::reference(map.get("inc_columns")));
            Ensures(inccolumns.isInteger());
            EnsuresEqual(value::toInteger(inccolumns), 10);

            const value::Value& incrows(value::reference(map.get("inc_rows")));
            Ensures(incrows.isInteger());
            EnsuresEqual(value::toInteger(incrows), 50);
        }
        {
            const vpz::Output& o(exp.views().outputs().get("view2"));
            EnsuresEqual(o.name(), "view2");
            EnsuresEqual(o.plugin(), "storage");
            Ensures(not o.data().get());
        }
    }
    {
        Ensures(exp.views().observables().exist("obs1"));
        Ensures(exp.views().observables().exist("obs2"));
        {
            const vpz::Observable& o(exp.views().observables().get("obs1"));
            Ensures(o.exist("c"));
            const vpz::ObservablePort& c(o.get("c"));
            EnsuresEqual(c.viewnamelist().size(),
                         (vpz::ViewNameList::size_type)1);
            EnsuresEqual(c.viewnamelist().front(), "view1");
        }
        {
            const vpz::Observable& o(exp.views().observables().get("obs2"));
            Ensures(o.exist("nbmodel"));
            {
                const vpz::ObservablePort& c(o.get("nbmodel"));
                EnsuresEqual(c.viewnamelist().size(),
                             (vpz::ViewNameList::size_type)1);
                EnsuresEqual(c.viewnamelist().front(), "view1");
            }
            Ensures(o.exist("structure"));
            {
                const vpz::ObservablePort& c(o.get("structure"));
                EnsuresEqual(c.viewnamelist().size(),
                             (vpz::ViewNameList::size_type)1);
                EnsuresEqual(c.viewnamelist().front(), "view2");
            }
        }
    }
}

void
check_classes_unittest_vpz(vpz::Classes& cls)
{
    void* ptr1 = nullptr;
    void* ptr2 = nullptr;

    Ensures(cls.exist("beepbeep"));
    {
        const vpz::Class& c(cls.get("beepbeep"));
        Ensures(c.node());
        Ensures(c.node()->isCoupled());
        EnsuresEqual(c.node()->getName(), "top");
        vpz::CoupledModel* cpled((vpz::CoupledModel*)c.node());

        ptr1 = cpled;

        Ensures(cpled->exist("a"));
        Ensures(cpled->findModel("a")->isAtomic());
        Ensures(cpled->exist("b"));
        Ensures(cpled->findModel("b")->isAtomic());
        Ensures(cpled->exist("c"));
        Ensures(cpled->findModel("c")->isAtomic());
        Ensures(cpled->existOutputConnection("a", "out", "out"));
        Ensures(cpled->existOutputConnection("b", "out", "out"));
        Ensures(cpled->existOutputConnection("c", "out", "out"));
    }

    Ensures(cls.exist("beepbeepbeep"));
    {
        const vpz::Class& c(cls.get("beepbeepbeep"));
        Ensures(c.node());
        Ensures(c.node()->isCoupled());
        EnsuresEqual(c.node()->getName(), "top");
        vpz::CoupledModel* cpled((vpz::CoupledModel*)c.node());

        ptr2 = cpled;

        Ensures(cpled->exist("a"));
        Ensures(cpled->findModel("a")->isAtomic());
        Ensures(cpled->exist("b"));
        Ensures(cpled->findModel("b")->isAtomic());
        Ensures(cpled->exist("c"));
        Ensures(cpled->findModel("c")->isAtomic());
        Ensures(cpled->exist("d"));
        Ensures(cpled->findModel("d")->isCoupled());
        {
            vpz::CoupledModel* cpled_d(
              vpz::BaseModel::toCoupled(cpled->findModel("d")));

            Ensures(cpled_d->exist("a"));
            Ensures(cpled_d->findModel("a")->isAtomic());
            Ensures(cpled_d->exist("b"));
            Ensures(cpled_d->findModel("b")->isAtomic());
            Ensures(cpled_d->exist("c"));
            Ensures(cpled_d->findModel("c")->isAtomic());
            Ensures(cpled_d->existOutputConnection("a", "out", "out"));
            Ensures(cpled_d->existOutputConnection("b", "out", "out"));
            Ensures(cpled_d->existOutputConnection("c", "out", "out"));
        }
        Ensures(cpled->existOutputConnection("a", "out", "out"));
        Ensures(cpled->existOutputConnection("b", "out", "out"));
        Ensures(cpled->existOutputConnection("c", "out", "out"));
        Ensures(cpled->existOutputConnection("d", "out", "out"));
    }

    cls.rename("beepbeep", "newbeepbeep");
    cls.rename("beepbeepbeep", "newbeepbeepbeep");

    Ensures(cls.exist("newbeepbeep"));
    {
        const vpz::Class& c(cls.get("newbeepbeep"));
        Ensures(c.node());
        Ensures(c.node()->isCoupled());
        EnsuresEqual(c.node()->getName(), "top");
        vpz::CoupledModel* cpled((vpz::CoupledModel*)c.node());

        Ensures(ptr1 != cpled);

        Ensures(cpled->exist("a"));
        Ensures(cpled->findModel("a")->isAtomic());
        Ensures(cpled->exist("b"));
        Ensures(cpled->findModel("b")->isAtomic());
        Ensures(cpled->exist("c"));
        Ensures(cpled->findModel("c")->isAtomic());
        Ensures(cpled->existOutputConnection("a", "out", "out"));
        Ensures(cpled->existOutputConnection("b", "out", "out"));
        Ensures(cpled->existOutputConnection("c", "out", "out"));
    }

    Ensures(cls.exist("newbeepbeepbeep"));
    {
        const vpz::Class& c(cls.get("newbeepbeepbeep"));
        Ensures(c.node());
        Ensures(c.node()->isCoupled());
        EnsuresEqual(c.node()->getName(), "top");
        vpz::CoupledModel* cpled((vpz::CoupledModel*)c.node());

        Ensures(ptr2 != cpled);

        Ensures(cpled->exist("a"));
        Ensures(cpled->findModel("a")->isAtomic());
        Ensures(cpled->exist("b"));
        Ensures(cpled->findModel("b")->isAtomic());
        Ensures(cpled->exist("c"));
        Ensures(cpled->findModel("c")->isAtomic());
        Ensures(cpled->exist("d"));
        Ensures(cpled->findModel("d")->isCoupled());
        {
            vpz::CoupledModel* cpled_d(
              vpz::BaseModel::toCoupled(cpled->findModel("d")));

            Ensures(cpled_d->exist("a"));
            Ensures(cpled_d->findModel("a")->isAtomic());
            Ensures(cpled_d->exist("b"));
            Ensures(cpled_d->findModel("b")->isAtomic());
            Ensures(cpled_d->exist("c"));
            Ensures(cpled_d->findModel("c")->isAtomic());
            Ensures(cpled_d->existOutputConnection("a", "out", "out"));
            Ensures(cpled_d->existOutputConnection("b", "out", "out"));
            Ensures(cpled_d->existOutputConnection("c", "out", "out"));
        }
        Ensures(cpled->existOutputConnection("a", "out", "out"));
        Ensures(cpled->existOutputConnection("b", "out", "out"));
        Ensures(cpled->existOutputConnection("c", "out", "out"));
        Ensures(cpled->existOutputConnection("d", "out", "out"));
    }

    Ensures(not cls.exist("beepbeep"));
    Ensures(not cls.exist("beepbeepbeep"));

    cls.rename("newbeepbeep", "beepbeep");
    cls.rename("newbeepbeepbeep", "beepbeepbeep");
}

void
check_unittest_vpz(vpz::Vpz& file)
{
    EnsuresEqual(file.project().author(), "Gauthier Quesnel");
    EnsuresEqual(file.project().version(), "0.6");

    const vpz::Model& model(file.project().model());
    check_model_unittest_vpz(model);

    const vpz::Dynamics& dynamics(file.project().dynamics());
    check_dynamics_unittest_vpz(dynamics);

    const vpz::Experiment& exp(file.project().experiment());
    check_experiment_unittest_vpz(exp);

    vpz::Classes& cls(file.project().classes());
    check_classes_unittest_vpz(cls);
}

void
check_copy_views_unittest_vpz(vpz::Views& views)
{
    EnsuresNotThrow(views.copyView("view1", "view1_1"), std::exception);
    EnsuresNotThrow(views.copyView("view1", "view1_2"), std::exception);
    EnsuresNotThrow(views.copyView("view1_1", "view1_1_1"), std::exception);

    Ensures(views.exist("view1_1"));
    Ensures(views.exist("view1_2"));
    Ensures(views.exist("view1_1_1"));

    Ensures(views.get("view1_1").output() == "view1_1");
    Ensures(views.get("view1_2").output() == "view1_2");
    Ensures(views.get("view1_1_1").output() == "view1_1_1");

    EnsuresNotThrow(views.del("view1"), std::exception);

    Ensures(views.exist("view1_1"));
    Ensures(views.exist("view1_2"));
    Ensures(views.exist("view1_1_1"));
}

void
check_equal_views_unittest_vpz(vpz::Views views)
{
    Ensures(views.exist("view1"));
    Ensures(views.exist("view2"));

    EnsuresEqual(views.get("view1") == views.get("view2"), false);
    EnsuresEqual(views.get("view1") == views.get("view1"), true);

    EnsuresNotThrow(views.copyView("view1", "view1_1"), std::exception);
    EnsuresEqual(views.get("view1") == views.get("view1_1"), false);
}

void
check_equal_dynamics_unittest_vpz(vpz::Dynamics dynamics)
{
    Ensures(dynamics.exist("a"));
    Ensures(dynamics.exist("b"));
    Ensures(dynamics.exist("unittest"));

    EnsuresEqual(dynamics.get("a") == dynamics.get("b"), false);
    EnsuresEqual(dynamics.get("unittest") == dynamics.get("unittest"), true);
}

void
check_equal_outputs_unittest_vpz(vpz::Outputs outputs)
{
    Ensures(outputs.exist("view1"));
    Ensures(outputs.exist("view2"));

    EnsuresEqual(outputs.get("view1") == outputs.get("view2"), false);
    EnsuresEqual(outputs.get("view1") == outputs.get("view1"), true);
}

void
test_remove_dyns()
{
    auto ctx = vle::utils::make_context();
    vpz::Vpz vpz;
    vpz.parseFile(VPZ_TEST_DIR "/unittest.vpz");

    EnsuresEqual(vpz.project().author(), "Gauthier Quesnel");
    EnsuresEqual(vpz.project().version(), "0.6");

    vpz::Project& proj(vpz.project());
    check_remove_dyns_unittest_vpz(proj);

    std::string str(vpz.writeToString());

    vpz.parseMemory(str);

    EnsuresEqual(vpz.project().author(), "Gauthier Quesnel");
    EnsuresEqual(vpz.project().version(), "0.6");
}

void
test_rename_dyns()
{
    auto ctx = vle::utils::make_context();
    vpz::Vpz vpz;
    vpz.parseFile(VPZ_TEST_DIR "/unittest.vpz");

    EnsuresEqual(vpz.project().author(), "Gauthier Quesnel");
    EnsuresEqual(vpz.project().version(), "0.6");

    vpz::Project& proj(vpz.project());
    check_rename_dyns_unittest_vpz(proj);

    std::string str(vpz.writeToString());
    vpz.parseMemory(str);
    EnsuresEqual(vpz.project().author(), "Gauthier Quesnel");
    EnsuresEqual(vpz.project().version(), "0.6");
}

void
test_remove_conds()
{
    auto ctx = vle::utils::make_context();
    vpz::Vpz vpz;
    vpz.parseFile(VPZ_TEST_DIR "/unittest.vpz");

    EnsuresEqual(vpz.project().author(), "Gauthier Quesnel");
    EnsuresEqual(vpz.project().version(), "0.6");

    vpz::Project& proj(vpz.project());
    check_remove_conds_unittest_vpz(proj);

    std::string str(vpz.writeToString());
    vpz.parseMemory(str);
    EnsuresEqual(vpz.project().author(), "Gauthier Quesnel");
    EnsuresEqual(vpz.project().version(), "0.6");
}

void
test_rename_conds()
{
    auto ctx = vle::utils::make_context();
    vpz::Vpz vpz;
    vpz.parseFile(VPZ_TEST_DIR "/unittest.vpz");

    EnsuresEqual(vpz.project().author(), "Gauthier Quesnel");
    EnsuresEqual(vpz.project().version(), "0.6");

    vpz::Project& proj(vpz.project());
    check_rename_conds_unittest_vpz(proj);

    std::string str(vpz.writeToString());
    vpz.parseMemory(str);
    EnsuresEqual(vpz.project().author(), "Gauthier Quesnel");
    EnsuresEqual(vpz.project().version(), "0.6");
}

void
test_rename_views()
{
    auto ctx = vle::utils::make_context();
    vpz::Vpz vpz;
    vpz.parseFile(VPZ_TEST_DIR "/unittest.vpz");

    EnsuresEqual(vpz.project().author(), "Gauthier Quesnel");
    EnsuresEqual(vpz.project().version(), "0.6");

    vpz::Views& views(vpz.project().experiment().views());
    check_rename_views_unittest_vpz(views);
}

void
test_remove_observables()
{
    auto ctx = vle::utils::make_context();
    vpz::Vpz vpz;
    vpz.parseFile(VPZ_TEST_DIR "/unittest.vpz");

    EnsuresEqual(vpz.project().author(), "Gauthier Quesnel");
    EnsuresEqual(vpz.project().version(), "0.6");

    vpz::Project& proj(vpz.project());
    check_remove_obs_unittest_vpz(proj);

    std::string str(vpz.writeToString());
    vpz.parseMemory(str);
    EnsuresEqual(vpz.project().author(), "Gauthier Quesnel");
    EnsuresEqual(vpz.project().version(), "0.6");
}

void
test_rename_observables()
{
    auto ctx = vle::utils::make_context();
    vpz::Vpz vpz;
    vpz.parseFile(VPZ_TEST_DIR "/unittest.vpz");

    EnsuresEqual(vpz.project().author(), "Gauthier Quesnel");
    EnsuresEqual(vpz.project().version(), "0.6");

    vpz::Project& proj(vpz.project());
    check_rename_observables_unittest_vpz(proj);

    std::string str(vpz.writeToString());
    vpz.parseMemory(str);
    EnsuresEqual(vpz.project().author(), "Gauthier Quesnel");
    EnsuresEqual(vpz.project().version(), "0.6");
}

void
test_connection()
{
    auto ctx = vle::utils::make_context();
    vpz::Vpz vpz;
    vpz.parseFile(VPZ_TEST_DIR "/unittest.vpz");

    const vpz::Model& model(vpz.project().model());
    Ensures(model.node());
    Ensures(model.node()->isCoupled());

    vpz::CoupledModel* cpled((vpz::CoupledModel*)model.node());
    EnsuresEqual(cpled->getName(), "top");
    Ensures(cpled->exist("top1"));
    Ensures(cpled->exist("top2"));
    Ensures(cpled->exist("d"));
    Ensures(cpled->exist("e"));

    vpz::CoupledModel* top1((vpz::CoupledModel*)cpled->findModel("top1"));
    vpz::AtomicModel* x((vpz::AtomicModel*)top1->findModel("x"));

    vpz::ModelPortList out;
    x->getAtomicModelsTarget("out", out);

    EnsuresEqual(out.size(), (vpz::ModelPortList::size_type)10);
}

void
test_read_write_read()
{
    auto ctx = vle::utils::make_context();
    vpz::Vpz vpz;
    vpz.parseFile(VPZ_TEST_DIR "/unittest.vpz");
    check_unittest_vpz(vpz);

    vpz.clear();
    vpz.parseFile(VPZ_TEST_DIR "/unittest.vpz");
    check_unittest_vpz(vpz);
    vpz.clear();
    vpz.parseFile(VPZ_TEST_DIR "/unittest.vpz");
    check_unittest_vpz(vpz);
    vpz.clear();

    vpz.parseFile(VPZ_TEST_DIR "/unittest.vpz");
    check_unittest_vpz(vpz);

    std::string str(vpz.writeToString());

    vpz.parseMemory(str);
    check_unittest_vpz(vpz);
}

void
test_read_write_read2()
{
    auto ctx = vle::utils::make_context();
    vpz::Vpz vpz;
    vpz.parseFile(VPZ_TEST_DIR "/unittest.vpz");
    vpz::Vpz vpz2(vpz);

    check_unittest_vpz(vpz);
    check_unittest_vpz(vpz2);
}

void
test_copy_del_views()
{
    auto ctx = vle::utils::make_context();
    vpz::Vpz vpz;
    vpz.parseFile(VPZ_TEST_DIR "/unittest.vpz");

    EnsuresEqual(vpz.project().author(), "Gauthier Quesnel");
    EnsuresEqual(vpz.project().version(), "0.6");

    vpz::Views& views(vpz.project().experiment().views());
    check_copy_views_unittest_vpz(views);
}

void
test_equal_dynamics()
{
    auto ctx = vle::utils::make_context();
    vpz::Vpz vpz;
    vpz.parseFile(VPZ_TEST_DIR "/unittest.vpz");

    EnsuresEqual(vpz.project().author(), "Gauthier Quesnel");
    EnsuresEqual(vpz.project().version(), "0.6");

    vpz::Dynamics& dynamics(vpz.project().dynamics());
    check_equal_dynamics_unittest_vpz(dynamics);
}

void
test_equal_outputs()
{
    auto ctx = vle::utils::make_context();
    vpz::Vpz vpz;
    vpz.parseFile(VPZ_TEST_DIR "/unittest.vpz");

    EnsuresEqual(vpz.project().author(), "Gauthier Quesnel");
    EnsuresEqual(vpz.project().version(), "0.6");

    vpz::Outputs& outputs(vpz.project().experiment().views().outputs());
    check_equal_outputs_unittest_vpz(outputs);
}

void
test_equal_views()
{
    auto ctx = vle::utils::make_context();
    vpz::Vpz vpz;
    vpz.parseFile(VPZ_TEST_DIR "/unittest.vpz");

    EnsuresEqual(vpz.project().author(), "Gauthier Quesnel");
    EnsuresEqual(vpz.project().version(), "0.6");

    vpz::Views& views(vpz.project().experiment().views());
    check_equal_views_unittest_vpz(views);
}

int
main()
{
    vle::Init app;

    test_remove_dyns();
    test_rename_dyns();
    test_remove_conds();
    test_rename_conds();
    test_rename_views();
    test_remove_observables();
    test_rename_observables();
    test_connection();
    test_read_write_read();
    test_read_write_read2();
    test_copy_del_views();
    test_equal_dynamics();
    test_equal_outputs();
    test_equal_views();

    return unit_test::report_errors();
}
