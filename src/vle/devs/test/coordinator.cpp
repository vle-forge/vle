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

#include "oov.hpp"
#include <cstdio>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <vle/devs/Coordinator.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/devs/Executive.hpp>
#include <vle/devs/RootCoordinator.hpp>
#include <vle/oov/Plugin.hpp>
#include <vle/utils/unit-test.hpp>
#include <vle/vpz/Classes.hpp>
#include <vle/vpz/CoupledModel.hpp>
#include <vle/vpz/Dynamics.hpp>
#include <vle/vpz/Experiment.hpp>

using namespace vle;

class Model : public vle::devs::Dynamics
{
    int state;

public:
    Model(const vle::devs::DynamicsInit& init,
          const vle::devs::InitEventList& events)
      : vle::devs::Dynamics(init, events)
    {
    }

    virtual ~Model() = default;

    virtual vle::devs::Time init(vle::devs::Time /* time */) override
    {
        state = 0;
        return 1;
    }

    virtual void output(vle::devs::Time /* time */,
                        vle::devs::ExternalEventList& output) const override
    {
        output.emplace_back("out");

        output.back().attributes() =
          std::make_shared<value::String>("My message");
    }

    virtual vle::devs::Time timeAdvance() const override
    {
        return 1.;
    }

    virtual void internalTransition(vle::devs::Time /* time */) override
    {
    }

    virtual void externalTransition(const vle::devs::ExternalEventList& events,
                                    vle::devs::Time /* time */) override
    {
        for (const auto& elem : events)
            if (elem.onPort("x"))
                state = 1;
    }

    virtual void confluentTransitions(
      vle::devs::Time time,
      const vle::devs::ExternalEventList& extEventlist) override
    {
        internalTransition(time);
        externalTransition(extEventlist, time);
    }

    virtual std::unique_ptr<vle::value::Value> observation(
      const vle::devs::ObservationEvent& /* event */) const override
    {
        return vle::value::Integer::create(1);
    }

    virtual void finish() override
    {
    }
};

class ModelDbg : public vle::devs::Dynamics
{
    int state;

public:
    ModelDbg(const vle::devs::DynamicsInit& init,
             const vle::devs::InitEventList& events)
      : vle::devs::Dynamics(init, events)
    {
    }

    virtual ~ModelDbg() = default;

    virtual vle::devs::Time init(vle::devs::Time /* time */) override
    {
        state = 0;
        return 3;
    }

    virtual void output(vle::devs::Time /* time */,
                        vle::devs::ExternalEventList& output) const override
    {
        output.emplace_back("out");

        output.back().attributes() =
          std::make_shared<value::String>("My message");
    }

    virtual vle::devs::Time timeAdvance() const override
    {
        return .1;
    }

    virtual void internalTransition(vle::devs::Time /* time */) override
    {
    }

    virtual void externalTransition(const vle::devs::ExternalEventList& events,
                                    vle::devs::Time /* time */) override
    {
        for (const auto& elem : events)
            if (elem.onPort("x"))
                state = 1;
    }

    virtual void confluentTransitions(
      vle::devs::Time time,
      const vle::devs::ExternalEventList& extEventlist) override
    {
        internalTransition(time);
        externalTransition(extEventlist, time);
    }

    virtual std::unique_ptr<vle::value::Value> observation(
      const vle::devs::ObservationEvent& /* event */) const override
    {
        return vle::value::Integer::create(2);
    }

    virtual void finish() override
    {
    }
};

class Exe : public vle::devs::Executive
{
    int state;

public:
    Exe(const vle::devs::ExecutiveInit& init,
        const vle::devs::InitEventList& events)
      : vle::devs::Executive(init, events)
    {
    }

    virtual ~Exe() = default;

    virtual vle::devs::Time init(vle::devs::Time /* time */) override
    {
        state = 0;
        return 0.1;
    }

    virtual void output(vle::devs::Time /* time */,
                        vle::devs::ExternalEventList& output) const override
    {
        output.emplace_back("out");

        output.back().attributes() =
          std::make_shared<value::String>("My message");
    }

    virtual vle::devs::Time timeAdvance() const override
    {
        return 2;
    }

    virtual void internalTransition(vle::devs::Time /* time */) override
    {
        if (state == 0) {
            addOutputPort("atom", "out2");
            state = -1;
        }
    }

    virtual void externalTransition(const vle::devs::ExternalEventList& events,
                                    vle::devs::Time /* time */) override
    {
        for (const auto& elem : events)
            if (elem.onPort("x"))
                state = 1;
    }

    virtual void confluentTransitions(
      vle::devs::Time time,
      const vle::devs::ExternalEventList& extEventlist) override
    {
        internalTransition(time);
        externalTransition(extEventlist, time);
    }

    virtual std::unique_ptr<vle::value::Value> observation(
      const vle::devs::ObservationEvent& /* event */) const override
    {
        return vle::value::Integer::create(3);
    }

    virtual void finish() override
    {
    }
};

class ObservationModel : public vle::devs::Dynamics
{
    mutable int state;

public:
    ObservationModel(const vle::devs::DynamicsInit& init,
                     const vle::devs::InitEventList& events)
      : vle::devs::Dynamics(init, events)
    {
    }

    virtual ~ObservationModel() = default;

    virtual vle::devs::Time init(vle::devs::Time /* time */) override
    {
        state = 0;
        return 1;
    }

    virtual void output(
      vle::devs::Time /* time */,
      vle::devs::ExternalEventList& /* output */) const override
    {
        state++;
    }

    virtual vle::devs::Time timeAdvance() const override
    {
        return 1.;
    }

    virtual void internalTransition(vle::devs::Time /* time */) override
    {
        state++;
    }

    virtual void externalTransition(
      const vle::devs::ExternalEventList& /* events */,
      vle::devs::Time /* time */) override
    {
        state += 1000;
    }

    virtual void confluentTransitions(
      vle::devs::Time /* time */,
      const vle::devs::ExternalEventList& /* extEventlist */) override
    {
        state += 1000000;
    }

    virtual std::unique_ptr<vle::value::Value> observation(
      const vle::devs::ObservationEvent& /* event */) const override
    {
        return vle::value::Integer::create(state);
    }

    virtual void finish() override
    {
        state++;
    }
};

class OutputPluginSimple : public vle::oov::Plugin
{
    struct data
    {
        std::unique_ptr<vle::value::Value> value;
        int number;
    };

    std::map<std::string, data> pp_D;

public:
    OutputPluginSimple(const std::string& location)
      : vle::oov::Plugin(location)
    {
        assert(location == "toto");
    }

    virtual ~OutputPluginSimple() = default;

    virtual std::string name() const override
    {
        return "OutputPlugin";
    }

    virtual bool isCairo() const override
    {
        return false;
    }

    virtual void onParameter(const std::string& /* plugin */,
                             const std::string& /* location */,
                             const std::string& /* file */,
                             std::unique_ptr<value::Value> /* parameters */,
                             const double& /* time */) override
    {
    }

    virtual void onNewObservable(const std::string& simulator,
                                 const std::string& parent,
                                 const std::string& port,
                                 const std::string& /* view */,
                                 const double& /* time */) override
    {
        std::string key = parent + '.' + simulator + '.' + port;

        pp_D[key] = { std::unique_ptr<vle::value::Value>(), 0 };
    }

    virtual void onDelObservable(const std::string& /* simulator */,
                                 const std::string& /* parent */,
                                 const std::string& /* port */,
                                 const std::string& /* view */,
                                 const double& /* time */) override
    {
    }

    virtual void onValue(const std::string& simulator,
                         const std::string& parent,
                         const std::string& port,
                         const std::string& /* view */,
                         const double& /* time */,
                         std::unique_ptr<value::Value> value) override
    {
        std::string key = parent + '.' + simulator + '.' + port;

        auto& data = pp_D[key];
        data.value = std::move(value);
        data.number++;
    }

    virtual std::unique_ptr<value::Matrix> finish(
      const double& /* time */) override
    {
        auto it = pp_D.find("depth0.atom.port");
        Ensures(it != pp_D.cend());
        EnsuresEqual(it->second.number, 101);
        Ensures(it->second.value);
        Ensures(it->second.value->isInteger());
        Ensures(it->second.value->toInteger().value() == 1);

        it = pp_D.find("depth0.atom2.port");
        Ensures(it != pp_D.cend());
        EnsuresEqual(it->second.number, 101);
        Ensures(it->second.value);
        Ensures(it->second.value->isInteger());
        Ensures(it->second.value->toInteger().value() == 2);

        it = pp_D.find("depth0.exe.port");
        Ensures(it != pp_D.cend());
        EnsuresEqual(it->second.number, 101);
        Ensures(it->second.value);
        Ensures(it->second.value->isInteger());
        Ensures(it->second.value->toInteger().value() == 3);
        return {};
    }
};

/* A C function to use the get() function in ModuleManager that search
 * symbol into the executable instead of a shared library.
 */

extern "C" {

VLE_MODULE vle::devs::Dynamics*
make_new_model(const vle::devs::DynamicsInit& init,
               const vle::devs::InitEventList& events)
{
    return new ::Model(init, events);
}

VLE_MODULE vle::devs::Dynamics*
make_new_model_dbg(const vle::devs::DynamicsInit& init,
                   const vle::devs::InitEventList& events)
{
    return new ::ModelDbg(init, events);
}

VLE_MODULE vle::devs::Dynamics*
exe_make_new_exe(const vle::devs::ExecutiveInit& init,
                 const vle::devs::InitEventList& events)
{
    return new ::Exe(init, events);
}

VLE_MODULE vle::devs::Dynamics*
make_new_observation_model(const vle::devs::DynamicsInit& init,
                           const vle::devs::InitEventList& events)
{
    return new ::ObservationModel(init, events);
}

VLE_MODULE vle::oov::Plugin*
make_oovplugin(const std::string& location)
{
    return new ::OutputPluginSimple(location);
}

VLE_MODULE vle::oov::Plugin*
make_oovplugin_default(const std::string& location)
{
    return new ::vletest::OutputPlugin(location);
}
}

void
instantiate_mode()
{
    Ensures(std::is_polymorphic<Model>::value == true);
    Ensures(std::is_polymorphic<ModelDbg>::value == true);
    Ensures(std::is_polymorphic<Exe>::value == true);

    bool check;

    check = std::is_base_of<vle::devs::Dynamics, Model>::value == true;
    Ensures(check);

    check = std::is_base_of<vle::devs::Dynamics, ModelDbg>::value == true;
    Ensures(check);

    check = std::is_base_of<vle::devs::Executive, Exe>::value == true;
    Ensures(check);
}

void
test_del_coupled_model()
{
    auto ctx = vle::utils::make_context();
    vpz::Dynamics dyns;
    vpz::Classes classes;
    vpz::Experiment expe;
    devs::RootCoordinator root(ctx);
    devs::Coordinator coord(ctx, dyns, classes, expe);
    vpz::CoupledModel* depth0 = new vpz::CoupledModel("depth0", nullptr);
    vpz::CoupledModel* depth1(depth0->addCoupledModel("depth1"));
    vpz::AtomicModel* depth2 = depth1->addAtomicModel("depth2");

    auto sim = coord.addModel(depth2);

    Ensures(sim != nullptr);

    delete depth0;
}

void
test_loading_dynamics_from_executable()
{
    auto ctx = vle::utils::make_context();
    // Build a simple Vpz object with an atomic model in a coupled model
    // with the previously defined devs::Dynamics.
    vpz::Vpz vpz;

    vpz.project().experiment().setDuration(100.0);
    vpz.project().experiment().setBegin(0.0);

    vpz.project().experiment().views().addStreamOutput(
      "output", "toto", "make_oovplugin", "");

    vpz.project().experiment().views().add(
      vpz::View("The_view", vle::vpz::View::Type::TIMED, "output", 1.0));

    vpz::Observable& obs =
      vpz.project().experiment().views().addObservable(vpz::Observable("obs"));
    vpz::ObservablePort& port = obs.add("port");
    port.add("The_view");

    {
        auto x = vpz.project().dynamics().dynamiclist().emplace(
          "dyn_1", vpz::Dynamic("dyn_1"));
        Ensures(x.second == true);
        x.first->second.setLibrary("make_new_model");
    }

    {
        auto x = vpz.project().dynamics().dynamiclist().emplace(
          "dyn_3", vpz::Dynamic("dyn_3"));
        Ensures(x.second == true);
        x.first->second.setLibrary("make_new_model_dbg");
    }

    {
        auto x = vpz.project().dynamics().dynamiclist().emplace(
          "dyn_2", vpz::Dynamic("dyn_2"));
        Ensures(x.second == true);
        x.first->second.setLibrary("exe_make_new_exe");
    }

    vpz::CoupledModel* depth0 = new vpz::CoupledModel("depth0", nullptr);
    auto* atom = depth0->addAtomicModel("atom");
    atom->setDynamics("dyn_1");
    atom->addOutputPort("out");
    atom->setObservables("obs");

    auto* atomdbg = depth0->addAtomicModel("atom2");
    atomdbg->setDynamics("dyn_3");
    atomdbg->addOutputPort("out");
    atomdbg->setObservables("obs");
    atomdbg->setDebug();

    auto* exe = depth0->addAtomicModel("exe");
    exe->setDynamics("dyn_2");
    exe->addOutputPort("out");
    exe->setObservables("obs");

    vpz.project().model().setGraph(std::unique_ptr<vpz::BaseModel>(depth0));

    devs::RootCoordinator root(ctx);
    root.load(vpz);
    vpz.clear();
    root.init();
    while (root.run())
        ;
    root.finish();
}

void
test_observation_event()
{
    auto ctx = vle::utils::make_context();
    vpz::Vpz vpz;

    vpz.project().experiment().setDuration(100.0);
    vpz.project().experiment().setBegin(0.0);

    vpz.project().experiment().views().addStreamOutput(
      "output", "toto", "make_oovplugin_default", "");

    vpz.project().experiment().views().add(vpz::View(
      "The_view",
      vle::vpz::View::Type::FINISH | vle::vpz::View::Type::INTERNAL |
        vle::vpz::View::Type::EXTERNAL | vle::vpz::View::Type::CONFLUENT |
        vle::vpz::View::Type::OUTPUT,
      "output"));

    vpz::Observable& obs =
      vpz.project().experiment().views().addObservable(vpz::Observable("obs"));
    vpz::ObservablePort& port = obs.add("port");
    port.add("The_view");

    {
        auto x = vpz.project().dynamics().dynamiclist().emplace(
          "dyn_1", vpz::Dynamic("dyn_1"));
        Ensures(x.second == true);
        x.first->second.setLibrary("make_new_observation_model");
    }

    vpz::CoupledModel* depth0 = new vpz::CoupledModel("depth0", nullptr);
    auto* atom = depth0->addAtomicModel("ObservationModel");
    atom->setDynamics("dyn_1");
    atom->addOutputPort("out");
    atom->setObservables("obs");

    vpz.project().model().setGraph(std::unique_ptr<vpz::BaseModel>(depth0));

    devs::RootCoordinator root(ctx);
    root.load(vpz);
    vpz.clear();
    root.init();
    while (root.run())
        ;
    std::unique_ptr<value::Map> out = root.outputs();

    Ensures(out);

    value::Matrix& matrix = out->getMatrix("The_view");
    EnsuresEqual(matrix.columns(), (std::size_t)2);
    EnsuresEqual(matrix.rows(), (std::size_t)101);

    for (std::size_t i = 1, ei = 10; i != ei; ++i) {
        EnsuresEqual(value::toDouble(matrix(0, i)), i);
        EnsuresEqual(value::toInteger(matrix(1, i)), static_cast<int>(i * 2));
    }

    root.finish();
}

void
test_observation_event_disabled()
{
    auto ctx = vle::utils::make_context();
    vpz::Vpz vpz;

    vpz.project().experiment().setDuration(100.0);
    vpz.project().experiment().setBegin(0.0);

    vpz.project().experiment().views().addStreamOutput(
      "output", "toto", "make_oovplugin_default", "");

    auto& v = vpz.project().experiment().views().add(vpz::View(
      "The_view",
      vle::vpz::View::Type::FINISH | vle::vpz::View::Type::INTERNAL |
        vle::vpz::View::Type::EXTERNAL | vle::vpz::View::Type::CONFLUENT |
        vle::vpz::View::Type::OUTPUT,
      "output"));

    vpz::Observable& obs =
      vpz.project().experiment().views().addObservable(vpz::Observable("obs"));
    vpz::ObservablePort& port = obs.add("port");
    port.add("The_view");

    {
        auto x = vpz.project().dynamics().dynamiclist().emplace(
          "dyn_1", vpz::Dynamic("dyn_1"));
        Ensures(x.second == true);
        x.first->second.setLibrary("make_new_observation_model");
    }

    vpz::CoupledModel* depth0 = new vpz::CoupledModel("depth0", nullptr);
    auto* atom = depth0->addAtomicModel("ObservationModel");
    atom->setDynamics("dyn_1");
    atom->addOutputPort("out");
    atom->setObservables("obs");

    vpz.project().model().setGraph(std::unique_ptr<vpz::BaseModel>(depth0));

    {
        vpz::Vpz copy(vpz);
        devs::RootCoordinator root(ctx);
        root.load(copy);
        copy.clear();
        root.init();

        while (root.run())
            ;

        std::unique_ptr<value::Map> out = root.outputs();

        Ensures(out);

        root.finish();
    }

    v.disable();

    {
        vpz::Vpz copy(vpz);
        devs::RootCoordinator root(ctx);
        root.load(copy);
        copy.clear();
        root.init();

        while (root.run())
            ;

        std::unique_ptr<value::Map> out = root.outputs();
        Ensures(out == nullptr);

        root.finish();
    }

    v.enable();

    {
        vpz::Vpz copy(vpz);
        devs::RootCoordinator root(ctx);
        root.load(copy);
        copy.clear();
        root.init();

        while (root.run())
            ;

        std::unique_ptr<value::Map> out = root.outputs();
        Ensures(out);

        root.finish();
    }
}

void
test_observation_timed_disabled()
{
    auto ctx = vle::utils::make_context();
    vpz::Vpz vpz;

    vpz.project().experiment().setDuration(100.0);
    vpz.project().experiment().setBegin(0.0);

    vpz.project().experiment().views().addStreamOutput(
      "output", "toto", "make_oovplugin_default", "");

    auto& v = vpz.project().experiment().views().add(
      vpz::View("The_view", vle::vpz::View::Type::TIMED, "output", 1.0));

    vpz::Observable& obs =
      vpz.project().experiment().views().addObservable(vpz::Observable("obs"));
    vpz::ObservablePort& port = obs.add("port");
    port.add("The_view");

    {
        auto x = vpz.project().dynamics().dynamiclist().emplace(
          "dyn_1", vpz::Dynamic("dyn_1"));
        Ensures(x.second == true);
        x.first->second.setLibrary("make_new_observation_model");
    }

    vpz::CoupledModel* depth0 = new vpz::CoupledModel("depth0", nullptr);
    auto* atom = depth0->addAtomicModel("ObservationModel");
    atom->setDynamics("dyn_1");
    atom->addOutputPort("out");
    atom->setObservables("obs");

    vpz.project().model().setGraph(std::unique_ptr<vpz::BaseModel>(depth0));

    {
        vpz::Vpz copy(vpz);
        devs::RootCoordinator root(ctx);
        root.load(copy);
        copy.clear();
        root.init();

        while (root.run())
            ;

        std::unique_ptr<value::Map> out = root.outputs();

        Ensures(out);

        root.finish();
    }

    v.disable();

    {
        vpz::Vpz copy(vpz);
        devs::RootCoordinator root(ctx);
        root.load(copy);
        copy.clear();
        root.init();

        while (root.run())
            ;

        std::unique_ptr<value::Map> out = root.outputs();
        Ensures(out == nullptr);

        root.finish();
    }

    v.enable();

    {
        vpz::Vpz copy(vpz);
        devs::RootCoordinator root(ctx);
        root.load(copy);
        copy.clear();
        root.init();

        while (root.run())
            ;

        std::unique_ptr<value::Map> out = root.outputs();
        Ensures(out);

        root.finish();
    }
}

int
main()
{
    vle::Init app;

    instantiate_mode();
    test_del_coupled_model();
    test_loading_dynamics_from_executable();
    test_observation_event();
    test_observation_event_disabled();
    test_observation_timed_disabled();

    return unit_test::report_errors();
}
