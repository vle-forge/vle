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
#include <boost/format.hpp>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <vle/devs/Coordinator.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/devs/DynamicsDbg.hpp>
#include <vle/devs/Executive.hpp>
#include <vle/devs/RootCoordinator.hpp>
#include <vle/oov/Plugin.hpp>
#include <vle/utils/Filesystem.hpp>
#include <vle/utils/unit-test.hpp>
#include <vle/value/Set.hpp>
#include <vle/vpz/Classes.hpp>
#include <vle/vpz/CoupledModel.hpp>
#include <vle/vpz/Dynamics.hpp>
#include <vle/vpz/Experiment.hpp>

using namespace vle;

#define xstringify(a) stringify(a)
#define stringify(a) #a

#define DECLARE_DYNAMICS_SYMBOL(symbol_, model_)                              \
    extern "C" {                                                              \
    VLE_MODULE vle::devs::Dynamics* symbol_(                                  \
      const vle::devs::DynamicsInit& init,                                    \
      const vle::devs::InitEventList& events)                                 \
    {                                                                         \
        return new model_(init, events);                                      \
    }                                                                         \
    }

#define DECLARE_EXECUTIVE_SYMBOL(symbol_, model_)                             \
    extern "C" {                                                              \
    VLE_MODULE vle::devs::Dynamics* symbol_(                                  \
      const vle::devs::ExecutiveInit& init,                                   \
      const vle::devs::InitEventList& events)                                 \
    {                                                                         \
        std::string symbol_test(xstringify(symbol_));                         \
        Ensures(symbol_test.length() > 4);                                    \
        Ensures(symbol_test.compare(0, 4, "exe_") == 0);                      \
        return new model_(init, events);                                      \
    }                                                                         \
    }

#define DECLARE_OOV_SYMBOL(symbol_, model_)                                   \
    extern "C" {                                                              \
    VLE_MODULE vle::oov::Plugin* symbol_(const std::string& location)         \
    {                                                                         \
        return new model_(location);                                          \
    }                                                                         \
    }

class MyBeep : public devs::Dynamics
{
public:
    MyBeep(const devs::DynamicsInit& model, const devs::InitEventList& events)
      : devs::Dynamics(model, events)
    {
    }

    virtual devs::Time init(devs::Time /* time */) override
    {
        return 0.0;
    }

    virtual devs::Time timeAdvance() const override
    {
        return 1.0;
    }

    virtual void output(devs::Time /* time */,
                        devs::ExternalEventList& output) const override
    {
        output.emplace_back("out");
    }
};

class Branch : public devs::Executive
{
public:
    Branch(const devs::ExecutiveInit& mdl, const devs::InitEventList& events)
      : devs::Executive(mdl, events)
    {
    }

    virtual devs::Time init(devs::Time /* time */) override
    {
        return 10.0;
    }

    virtual devs::Time timeAdvance() const override
    {
        return 10.0;
    }

    virtual void internalTransition(devs::Time /* time */) override
    {
        std::vector<std::string> modelname;

        {
            vpz::ModelList::const_iterator it =
              coupledmodel().getModelList().begin();
            vpz::ModelList::const_iterator et =
              coupledmodel().getModelList().end();

            for (; it != et; ++it)
                if (it->first != getModelName())
                    modelname.push_back(it->first);
        }

        {
            for (std::size_t i = 0, ei = modelname.size(); i != ei; ++i)
                delModel(modelname[i]);
        }
    }

    virtual void output(devs::Time /*time*/,
                        devs::ExternalEventList& output) const override
    {
        output.emplace_back("out");
        output.back().addString("branch");
    }
};

class Counter : public devs::Dynamics
{
public:
    Counter(const devs::DynamicsInit& model, const devs::InitEventList& events)
      : devs::Dynamics(model, events)
      , m_counter(0)
      , m_active(false)
    {
    }

    virtual devs::Time init(devs::Time /* time */) override
    {
        m_counter = 0;
        return devs::infinity;
    }

    virtual void output(devs::Time /* time */,
                        devs::ExternalEventList& /* output */) const override
    {
    }

    virtual devs::Time timeAdvance() const override
    {
        if (m_active)
            return devs::Time(0.0);

        return devs::infinity;
    }

    virtual void internalTransition(devs::Time /* event */) override
    {
        m_active = false;
    }

    virtual void externalTransition(const devs::ExternalEventList& events,
                                    devs::Time /* time */) override
    {
        m_counter += events.size();
        m_active = true;
    }

    virtual std::unique_ptr<value::Value> observation(
      const devs::ObservationEvent& ev) const override
    {
        if (ev.onPort("c")) {
            if (m_counter > 100 and m_counter < 500)
                return {};
            return value::Double::create(m_counter);
        }

        if (ev.onPort("value"))
            return value::Integer::create(0);

        return {};
    }

private:
    long m_counter;
    bool m_active;
};

class DeleteConnection : public devs::Executive
{
public:
    DeleteConnection(const devs::ExecutiveInit& init,
                     const devs::InitEventList& events)
      : devs::Executive(init, events)
    {
        mAlarm = events.getDouble("alarm");
        mModelSource = events.getString("model source");
        mPortSource = events.getString("port source");
        mModelDest = events.getString("model dest");
        mPortDest = events.getString("port dest");

        mRemaining = mAlarm;
    }

    virtual ~DeleteConnection()
    {
    }

    virtual devs::Time init(devs::Time /*time*/)
    {
        return timeAdvance();
    }

    virtual devs::Time timeAdvance() const
    {
        return mRemaining;
    }

    virtual void internalTransition(devs::Time time)
    {
        devs::Time tmp =
          std::abs(std::min(time, mAlarm) - std::max(time, mAlarm));

        if (tmp < 0.1) {
            mRemaining = devs::infinity;
            removeConnection(mModelSource, mPortSource, mModelDest, mPortDest);
        } else {
            mRemaining = tmp;
        }
    }

    virtual void externalTransition(const devs::ExternalEventList& /*events*/,
                                    devs::Time time)
    {
        mRemaining = std::abs(std::min(time, mAlarm) - std::max(time, mAlarm));
    }

    devs::Time mAlarm;
    devs::Time mRemaining;
    devs::Time mLast;
    std::string mModelSource;
    std::string mPortSource;
    std::string mModelDest;
    std::string mPortDest;
};

class Transform : public devs::Dynamics
{
public:
    Transform(const devs::DynamicsInit& atom,
              const devs::InitEventList& events)
      : devs::Dynamics(atom, events)
    {
    }

    virtual devs::Time init(devs::Time /* time */) override
    {
        m_counter = 0;
        return 0.0;
    }

    virtual devs::Time timeAdvance() const override
    {
        return 1.0;
    }

    virtual void internalTransition(devs::Time /* time */) override
    {
        m_counter = 1;
    }

    virtual void externalTransition(const devs::ExternalEventList& events,
                                    devs::Time /* time */) override
    {
        m_counter = m_counter + events.size();
    }

    virtual void output(devs::Time /* time */,
                        devs::ExternalEventList& output) const override
    {
        for (int i = 0; i < m_counter; ++i)
            output.emplace_back("out");
    }

    virtual std::unique_ptr<value::Value> observation(
      const devs::ObservationEvent&) const override
    {
        return value::Integer::create(m_counter);
    }

private:
    int m_counter;
};

class Confluent_transitionA : public devs::Dynamics
{
public:
    Confluent_transitionA(const devs::DynamicsInit& atom,
                          const devs::InitEventList& events)
      : devs::Dynamics(atom, events)
    {
    }

    virtual devs::Time init(devs::Time /* time */) override
    {
        return 1.0;
    }

    virtual devs::Time timeAdvance() const override
    {
        return devs::infinity;
    }

    virtual void internalTransition(devs::Time /* time */) override
    {
    }

    virtual void externalTransition(const devs::ExternalEventList& /*events*/,
                                    devs::Time /* time */) override
    {
    }

    virtual void confluentTransitions(
      devs::Time /*time*/,
      const devs::ExternalEventList& /*extEventlist*/) override
    {
    }

    virtual void output(devs::Time /*time*/,
                        devs::ExternalEventList& output) const override
    {
        output.emplace_back("out");
    }

    virtual std::unique_ptr<value::Value> observation(
      const devs::ObservationEvent&) const override
    {
        return 0;
    }
};

class Confluent_transitionB : public devs::Dynamics
{
public:
    Confluent_transitionB(const devs::DynamicsInit& atom,
                          const devs::InitEventList& events)
      : devs::Dynamics(atom, events)
      , state(0)
    {
    }

    virtual devs::Time init(devs::Time /* time */) override
    {
        return 1.0;
    }

    virtual devs::Time timeAdvance() const override
    {
        if (state < 2) {
            return 0;
        } else {
            return devs::infinity;
        }
    }

    virtual void internalTransition(devs::Time /* time */) override
    {
        state++;
    }

    virtual void externalTransition(const devs::ExternalEventList& /*events*/,
                                    devs::Time /* time */) override
    {
        // should not have an external transition, rather a confluent
        throw "error confluent_transition::internalTransition";
    }

    virtual void confluentTransitions(
      devs::Time /*time*/,
      const devs::ExternalEventList& /*extEventlist*/) override
    {
    }

    virtual void output(devs::Time /* time */,
                        devs::ExternalEventList& /*output*/) const override
    {
    }

    virtual std::unique_ptr<value::Value> observation(
      const devs::ObservationEvent&) const override
    {
        return 0;
    }
    int state;
};

class Confluent_transitionC : public devs::Dynamics
{
    mutable int i = 0;

public:
    Confluent_transitionC(const devs::DynamicsInit& atom,
                          const devs::InitEventList& events)
      : devs::Dynamics(atom, events)
    {
    }

    virtual devs::Time init(devs::Time /* time */) override
    {
        Ensures(i == 0);
        i++;

        return 0;
    }

    virtual devs::Time timeAdvance() const override
    {
        Ensures(i == 3);
        i = 1;
        return 1.0;
    }

    virtual void internalTransition(devs::Time /* time */) override
    {
        Ensures(i == 2);
        i++;
    }

    virtual void output(devs::Time /*time*/,
                        devs::ExternalEventList& output) const override
    {
        Ensures(i == 1);
        i++;
        output.emplace_back("out");
    }
};

class Confluent_transitionD : public devs::Dynamics
{
    int i = 0;

public:
    Confluent_transitionD(const devs::DynamicsInit& atom,
                          const devs::InitEventList& events)
      : devs::Dynamics(atom, events)
    {
        Ensures(i == 0);
        i++;
    }

    virtual devs::Time init(devs::Time /* time */) override
    {
        Ensures(i == 1);
        i++;

        return 0;
    }

    virtual devs::Time timeAdvance() const override
    {
        return 1;
    }

    virtual void internalTransition(devs::Time /* time */) override
    {
        EnsuresNotReached();
    }

    virtual void externalTransition(const devs::ExternalEventList& /*events*/,
                                    devs::Time /* time */) override
    {
        EnsuresNotReached();
    }

    virtual void confluentTransitions(
      devs::Time /*time*/,
      const devs::ExternalEventList& /*extEventlist*/) override
    {
        Ensures(i > 1);
    }
};

class GenExecutive : public devs::Executive
{
    enum state
    {
        INIT,
        IDLE,
        ADDMODEL,
        DELMODEL
    };

    std::stack<std::string> m_stacknames;
    state m_state;

public:
    GenExecutive(const devs::ExecutiveInit& mdl,
                 const devs::InitEventList& events)
      : devs::Executive(mdl, events)
    {
    }

    virtual devs::Time init(devs::Time /* time */) override
    {
        vpz::Dynamic dyn("gensMyBeep");
        dyn.setPackage("");
        dyn.setLibrary("dynamics_MyBeep");
        dynamics().add(dyn);

        m_state = INIT;

        return 0.0;
    }

    virtual devs::Time timeAdvance() const override
    {
        switch (m_state) {
        case INIT:
            return 0.0;
        case IDLE:
            return 0.0;
        case ADDMODEL:
            return 1.0;
        case DELMODEL:
            return 1.0;
        }
        throw utils::InternalError("GenExecutive ta");
    }

    virtual void internalTransition(devs::Time time) override
    {
        switch (m_state) {
        case INIT:
            m_state = IDLE;
            break;
        case IDLE:
            if (time < 50.0) {
                m_state = ADDMODEL;
            } else {
                m_state = DELMODEL;
            }
            break;
        case ADDMODEL:
            add_new_model();
            m_state = IDLE;
            break;
        case DELMODEL:
            del_first_model();
            m_state = IDLE;
            break;
        }
    }

    virtual std::unique_ptr<value::Value> observation(
      const devs::ObservationEvent& ev) const override
    {
        if (ev.onPort("nbmodel"))
            return value::Integer::create(get_nb_model());

        if (ev.onPort("structure")) {
            std::ostringstream out;
            coupledmodel().writeXML(out);
            return value::String::create(out.str());
        }

        if (ev.onPort("adjacency_matrix")) {
            auto set = value::Set::create();
            set->toSet().add(value::Integer::create(1));
            if (get_nb_model() > 0 and ev.getTime() < 50.0) {
                set->toSet().add(value::String::create("add"));
                std::string name =
                  (boost::format("MyBeep_%1%") % m_stacknames.size()).str();
                set->toSet().add(value::String::create(name));
                set->toSet().add(value::String::create("2"));
                std::string edge = name + std::string(" counter ");
                set->toSet().add(value::String::create(edge));
            } else if (get_nb_model() > 0) {
                set->toSet().add(value::String::create("delete"));
                std::string name =
                  (boost::format("MyBeep_%1%") % (get_nb_model())).str();
                set->toSet().add(value::String::create(name));
            }

            return set;
        }

        if (ev.onPort("value"))
            return value::Integer::create(1);

        return devs::Executive::observation(ev);
    }

    void add_new_model()
    {
        printf("add_new_model starts\n");
        std::string name(
          (boost::format("MyBeep_%1%") % m_stacknames.size()).str());

        std::vector<std::string> outputs{ "out" }, inputs{};

        createModel(name, inputs, outputs, "gensMyBeep");
        addConnection(name, "out", "counter", "in");

        m_stacknames.push(name);
        printf("add_new_model finished: %s\n", name.c_str());
    }

    void del_first_model()
    {
        printf("del_first_model starts\n");
        if (m_stacknames.empty()) {
            throw utils::InternalError(
              boost::format("Cannot delete any model, the executive have no "
                            "element.")
                .str());
        }

        delModel(m_stacknames.top());
        printf("del_first_model finished: %s\n", m_stacknames.top().c_str());
        m_stacknames.pop();
    }

    int get_nb_model() const
    {
        auto size = coupledmodel().getModelList().size();
        Ensures(size > 0);
        Ensures(size <= std::numeric_limits<int>::max());

        return static_cast<int>(size - 1);
    }
};

class Leaf : public devs::Executive
{
public:
    Leaf(const devs::ExecutiveInit& mdl, const devs::InitEventList& events)
      : devs::Executive(mdl, events)
    {
    }

    virtual devs::Time init(devs::Time /* time */) override
    {
        return 10.0;
    }

    virtual devs::Time timeAdvance() const override
    {
        return 10.0;
    }

    virtual void output(devs::Time /*time*/,
                        devs::ExternalEventList& output) const override
    {
        output.emplace_back("out");
        output.back().addString("leaf");
    }
};

class Root : public devs::Executive
{
public:
    Root(const devs::ExecutiveInit& mdl, const devs::InitEventList& events)
      : devs::Executive(mdl, events)
    {
    }

    virtual devs::Time init(devs::Time /* time */) override
    {
        return 10.0;
    }

    virtual devs::Time timeAdvance() const override
    {
        return 10.0;
    }

    virtual void internalTransition(devs::Time /* time */) override
    {
        std::vector<std::string> modelname;

        {
            vpz::ModelList::const_iterator it =
              coupledmodel().getModelList().begin();
            vpz::ModelList::const_iterator et =
              coupledmodel().getModelList().end();

            for (; it != et; ++it)
                if (it->first != getModelName())
                    // and it->first != "counter")
                    modelname.push_back(it->first);
        }

        {
            for (std::size_t i = 0, ei = modelname.size(); i != ei; ++i)
                delModel(modelname[i]);
        }
    }
};

DECLARE_DYNAMICS_SYMBOL(dynamics_MyBeep, MyBeep)
DECLARE_DYNAMICS_SYMBOL(dynamics_counter, Counter)
DECLARE_DYNAMICS_SYMBOL(dynamics_transform, Transform)
DECLARE_DYNAMICS_SYMBOL(dynamics_confluent_transitionA, Confluent_transitionA)
DECLARE_DYNAMICS_SYMBOL(dynamics_confluent_transitionB, Confluent_transitionB)
DECLARE_DYNAMICS_SYMBOL(dynamics_confluent_transitionC, Confluent_transitionC)
DECLARE_DYNAMICS_SYMBOL(dynamics_confluent_transitionD, Confluent_transitionD)
DECLARE_EXECUTIVE_SYMBOL(exe_branch, Branch)
DECLARE_EXECUTIVE_SYMBOL(exe_deleteconnection, DeleteConnection)
DECLARE_EXECUTIVE_SYMBOL(exe_genexecutive, GenExecutive)
DECLARE_EXECUTIVE_SYMBOL(exe_leaf, Leaf)
DECLARE_EXECUTIVE_SYMBOL(exe_root, Root)
DECLARE_OOV_SYMBOL(oov_plugin, vletest::OutputPlugin)

void
test_normal_behaviour()
{
    auto ctx = vle::utils::make_context();
    vle::utils::Path p(DEVS_TEST_DIR);
    vle::utils::Path::current_path(p);

    vpz::Vpz file(DEVS_TEST_DIR "/confluent_transition.vpz");
    devs::RootCoordinator root(ctx);

    auto& dyna = file.project().dynamics().get("confluent_transitionA");
    dyna.setLibrary("dynamics_confluent_transitionC");
    auto& dynb = file.project().dynamics().get("confluent_transitionB");
    dynb.setLibrary("dynamics_confluent_transitionD");

    try {
        root.load(file);
        file.clear();
        root.init();

        while (root.run())
            ;
        std::unique_ptr<value::Map> out = root.outputs();
        Ensures(not out);
        root.finish();
    } catch (const std::exception& e) {
        EnsuresNotReached();
    }
}

void
test_gensvpz()
{
    auto ctx = vle::utils::make_context();
    vle::utils::Path p(DEVS_TEST_DIR);
    vle::utils::Path::current_path(p);

    vpz::Vpz file(DEVS_TEST_DIR "/gens.vpz");
    devs::RootCoordinator root(ctx);

    root.load(file);
    file.clear();
    root.init();

    while (root.run())
        ;
    std::unique_ptr<value::Map> out = root.outputs();
    root.finish();

    Ensures(out);
    EnsuresEqual(out->size(), 2);

    /* get result of simulation */
    value::Matrix& matrix = out->getMatrix("view1");
    std::cout << matrix << '\n';

    EnsuresEqual(matrix.rows(), (std::size_t)101);
    EnsuresEqual(matrix.columns(), (std::size_t)3);

    EnsuresEqual(value::toDouble(matrix(0, 0)), 0);
    EnsuresEqual(value::toDouble(matrix(1, 0)), 0);
    EnsuresEqual(value::toInteger(matrix(2, 0)), 1);

    EnsuresEqual(value::toDouble(matrix(0, 10)), 10);
    EnsuresEqual(value::toDouble(matrix(1, 10)), 55);
    EnsuresEqual(value::toInteger(matrix(2, 10)), 11);

    EnsuresEqual(value::toDouble(matrix(0, 14)), 14);
    Ensures(not matrix(1, 14));
    EnsuresEqual(value::toInteger(matrix(2, 14)), 15);

    EnsuresEqual(value::toDouble(matrix(0, 31)), 31);
    Ensures(not matrix(1, 31));
    EnsuresEqual(value::toInteger(matrix(2, 31)), 32);

    EnsuresEqual(value::toDouble(matrix(0, 100)), 100);
    EnsuresEqual(value::toDouble(matrix(1, 100)), 2550);
    EnsuresEqual(value::toInteger(matrix(2, 100)), 1);
}

void
test_gens_delete_connection()
{
    auto ctx = vle::utils::make_context();
    vle::utils::Path p(DEVS_TEST_DIR);
    vle::utils::Path::current_path(p);

    vpz::Vpz file(DEVS_TEST_DIR "/gensdelete.vpz");
    devs::RootCoordinator root(ctx);

    try {
        root.load(file);
        file.clear();
        root.init();

        while (root.run())
            ;
        std::unique_ptr<value::Map> out = root.outputs();
        Ensures(not out);
        root.finish();
    } catch (const std::exception& e) {
        Ensures(false);
    }
}

void
test_gens_ordereddeleter()
{
    auto ctx = vle::utils::make_context();
    vle::utils::Path p(DEVS_TEST_DIR);
    vle::utils::Path::current_path(p);

    for (int s = 0, es = 100; s != es; ++s) {
        vpz::Vpz file(DEVS_TEST_DIR "/ordereddeleter.vpz");
        devs::RootCoordinator root(ctx);

        root.load(file);
        file.clear();
        root.init();

        while (root.run())
            ;
        auto out = root.outputs();
        root.finish();

        /* begin check */
        Ensures(out);
        EnsuresEqual(out->size(), 1);

        /* get result of simulation */
        value::Matrix& matrix = out->getMatrix("view1");

        EnsuresEqual(matrix.columns(), (std::size_t)2);

        for (std::size_t i = 0, ei = 10; i != ei; ++i) {
            EnsuresEqual(value::toDouble(matrix(0, i)), i);
            EnsuresEqual(value::toDouble(matrix(1, i)), 0);
        }
    }
}

void
test_confluent_transition()
{
    auto ctx = vle::utils::make_context();
    vle::utils::Path p(DEVS_TEST_DIR);
    vle::utils::Path::current_path(p);

    vpz::Vpz file(DEVS_TEST_DIR "/confluent_transition.vpz");
    devs::RootCoordinator root(ctx);

    try {
        root.load(file);
        file.clear();
        root.init();

        while (root.run())
            ;
        std::unique_ptr<value::Map> out = root.outputs();
        Ensures(not out);
        root.finish();
    } catch (const std::exception& e) {
        Ensures(false);
    } catch (...) {
        Ensures(false);
    }
}

void
test_confluent_transition_2()
{
    auto ctx = vle::utils::make_context();
    vle::utils::Path p(DEVS_TEST_DIR);
    vle::utils::Path::current_path(p);

    vpz::Vpz file(DEVS_TEST_DIR "/confluent_transition.vpz");
    devs::RootCoordinator root(ctx);

    auto& dyna = file.project().dynamics().get("confluent_transitionA");
    dyna.setLibrary("dynamics_confluent_transitionC");
    auto& dynb = file.project().dynamics().get("confluent_transitionB");
    dynb.setLibrary("dynamics_confluent_transitionD");

    try {
        root.load(file);
        file.clear();
        root.init();

        while (root.run())
            ;
        std::unique_ptr<value::Map> out = root.outputs();
        Ensures(not out);
        root.finish();
    } catch (const std::exception& e) {
        Ensures(false);
    } catch (...) {
        Ensures(false);
    }
}

int
main()
{
    vle::Init app;

    test_normal_behaviour();
    test_confluent_transition();
    test_confluent_transition_2();
    test_gensvpz();
    test_gens_delete_connection();
    test_gens_ordereddeleter();

    return unit_test::report_errors();
}
