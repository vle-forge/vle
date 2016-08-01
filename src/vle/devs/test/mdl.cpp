/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2016 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2016 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2016 INRA http://www.inra.fr
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
#define BOOST_TEST_MODULE devstime_mdl
#include <boost/test/included/unit_test.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/devs/DynamicsDbg.hpp>
#include <vle/devs/Executive.hpp>
#include <vle/devs/Coordinator.hpp>
#include <vle/devs/RootCoordinator.hpp>
#include <vle/vpz/CoupledModel.hpp>
#include <vle/vpz/Dynamics.hpp>
#include <vle/vpz/Experiment.hpp>
#include <vle/vpz/Classes.hpp>
#include <vle/oov/Plugin.hpp>
#include <vle/utils/Filesystem.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <stack>
#include <stdexcept>
#include <limits>
#include <fstream>
#include "oov.hpp"

using namespace vle;

#define xstringify(a) stringify(a)
#define stringify(a) #a

#define DECLARE_DYNAMICS_SYMBOL(symbol_, model_)        \
    extern "C" {                                        \
        VLE_MODULE vle::devs::Dynamics*                 \
        symbol_(const vle::devs::DynamicsInit& init,    \
                const vle::devs::InitEventList& events) \
        {                                               \
            return new model_(init, events);            \
        }                                               \
    }

#define DECLARE_EXECUTIVE_SYMBOL(symbol_, model_)                       \
    extern "C" {                                                        \
        VLE_MODULE vle::devs::Dynamics*                                 \
        symbol_(const vle::devs::ExecutiveInit& init,                   \
                const vle::devs::InitEventList& events)                 \
        {                                                               \
            std::string symbol_test(xstringify(symbol_));               \
            BOOST_REQUIRE(symbol_test.length() > 4);                    \
            BOOST_REQUIRE(symbol_test.compare(0, 4, "exe_") == 0);      \
            return new model_(init, events);                            \
        }                                                               \
    }

#define DECLARE_OOV_SYMBOL(symbol_, model_)     \
    extern "C" {                                \
        VLE_MODULE vle::oov::Plugin*            \
        symbol_(const std::string& location)    \
        {                                       \
            return new model_(location);        \
        }                                       \
    }



class MyBeep : public devs::Dynamics
{
public:
    MyBeep(const devs::DynamicsInit& model,
         const devs::InitEventList& events) :
        devs::Dynamics(model, events)
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
    Branch(const devs::ExecutiveInit& mdl, const devs::InitEventList& events) :
        devs::Executive(mdl, events)
    {}

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
        output.back().addString("branch") ;
    }
};

class Counter : public devs::Dynamics
{
public:
    Counter(const devs::DynamicsInit& model,
            const devs::InitEventList& events) :
        devs::Dynamics(model, events),
        m_counter(0),
        m_active(false)
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

    virtual std::unique_ptr<value::Value>
    observation(const devs::ObservationEvent& ev) const override
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
    {}

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
        devs::Time tmp = std::abs(std::min(time, mAlarm) -
                                  std::max(time, mAlarm));

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
              const devs::InitEventList& events) :
        devs::Dynamics(atom, events)
    {}

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

    virtual void externalTransition(
        const devs::ExternalEventList& events,
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

    virtual std::unique_ptr<value::Value>
    observation(const devs::ObservationEvent&) const override
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
              const devs::InitEventList& events) :
        devs::Dynamics(atom, events)
    {}

    virtual devs::Time init(devs::Time /* time */) override
    {
        return 0.0;
    }

    virtual devs::Time timeAdvance() const override
    {
        return devs::infinity;
    }

    virtual void internalTransition(devs::Time /* time */) override
    {
    }

    virtual void externalTransition(
        const devs::ExternalEventList& /*events*/,
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

    virtual std::unique_ptr<value::Value>
    observation(const devs::ObservationEvent&) const override
    {
        return 0;
    }
};

class Confluent_transitionB : public devs::Dynamics
{
public:
    Confluent_transitionB(const devs::DynamicsInit& atom,
              const devs::InitEventList& events) :
        devs::Dynamics(atom, events), state(0)
    {}

    virtual devs::Time init(devs::Time /* time */) override
    {
        return 0.0;
    }

    virtual devs::Time timeAdvance() const override
    {
        if (state<2) {
            return 0;
        } else {
            return devs::infinity;
        }
    }

    virtual void internalTransition(devs::Time /* time */) override
    {
        state++;
    }

    virtual void externalTransition(
        const devs::ExternalEventList& /*events*/,
        devs::Time /* time */) override
    {
        //should not have an external transition, rather a confluent
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

    virtual std::unique_ptr<value::Value>
    observation(const devs::ObservationEvent&) const override
    {
        return 0;
    }
    int state;
};

class GenExecutive : public devs::Executive
{
    enum state { INIT, IDLE, ADDMODEL, DELMODEL };

    std::stack < std::string >  m_stacknames;
    state                       m_state;

public:
    GenExecutive(const devs::ExecutiveInit& mdl,
                 const devs::InitEventList& events) :
        devs::Executive(mdl, events)
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
            }
            else {
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

    virtual std::unique_ptr<value::Value>
    observation(const devs::ObservationEvent& ev) const override
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
            if(get_nb_model() > 0 and ev.getTime() < 50.0){
                set->toSet().add(value::String::create("add"));
                std::string name = (boost::format("MyBeep_%1%") %
                                    m_stacknames.size()).str();
                set->toSet().add(value::String::create(name));
                set->toSet().add(value::String::create("2"));
                std::string edge =  name + std::string(" counter ");
                set->toSet().add(value::String::create(edge));
            } else if(get_nb_model() > 0){
                set->toSet().add(value::String::create("delete"));
                std::string name = (boost::format("MyBeep_%1%")
                                    % (get_nb_model())).str();
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
        std::string name((boost::format("MyBeep_%1%")
                          % m_stacknames.size()).str());

        std::vector < std::string > outputs;
        outputs.push_back("out");

        createModel(name, std::vector < std::string >(), outputs, "gensMyBeep");
        addConnection(name, "out", "counter", "in");

        m_stacknames.push(name);
    }

    void del_first_model()
    {
        if (m_stacknames.empty()) {
            throw utils::InternalError(boost::format(
                    "Cannot delete any model, the executive have no "
                    "element.").str());
        }

        delModel(m_stacknames.top());
        m_stacknames.pop();
    }

    int get_nb_model() const
    {
        auto size = coupledmodel().getModelList().size();
        BOOST_REQUIRE(size > 0);
        BOOST_REQUIRE(size <= std::numeric_limits<int>::max());

        return static_cast<int>(size - 1);
    }
};

class Leaf : public devs::Executive
{
public:
    Leaf(const devs::ExecutiveInit& mdl, const devs::InitEventList& events) :
        devs::Executive(mdl, events)
    {}

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
    {}

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
                    //and it->first != "counter")
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
DECLARE_EXECUTIVE_SYMBOL(exe_branch, Branch)
DECLARE_EXECUTIVE_SYMBOL(exe_deleteconnection, DeleteConnection)
DECLARE_EXECUTIVE_SYMBOL(exe_genexecutive, GenExecutive)
DECLARE_EXECUTIVE_SYMBOL(exe_leaf, Leaf)
DECLARE_EXECUTIVE_SYMBOL(exe_root, Root)
DECLARE_OOV_SYMBOL(oov_plugin, vletest::OutputPlugin)

 BOOST_AUTO_TEST_CASE(test_gensvpz)
 {
     auto ctx = vle::utils::make_context();
     vle::utils::Path p(DEVS_TEST_DIR);
     vle::utils::Path::current_path(p);

     vpz::Vpz file(DEVS_TEST_DIR "/gens.vpz");
     devs::RootCoordinator root(ctx);

     root.load(file);
     file.clear();
     root.init();

     while (root.run());
     std::unique_ptr<value::Map> out = root.outputs();
     root.finish();


     BOOST_REQUIRE(out);
     BOOST_REQUIRE_EQUAL(out->size(), 2);

     /* get result of simulation */
     value::Matrix &matrix = out->getMatrix("view1");
     std::cout << matrix << '\n';

     BOOST_REQUIRE_EQUAL(matrix.rows(), (std::size_t)101);
     BOOST_REQUIRE_EQUAL(matrix.columns(), (std::size_t)3);

     BOOST_REQUIRE_EQUAL(value::toDouble(matrix(0, 0)), 0);
     BOOST_REQUIRE_EQUAL(value::toDouble(matrix(1, 0)), 0);
     BOOST_REQUIRE_EQUAL(value::toInteger(matrix(2, 0)), 1);

     BOOST_REQUIRE_EQUAL(value::toDouble(matrix(0, 10)), 10);
     BOOST_REQUIRE_EQUAL(value::toDouble(matrix(1, 10)), 55);
     BOOST_REQUIRE_EQUAL(value::toInteger(matrix(2, 10)), 11);

     BOOST_REQUIRE_EQUAL(value::toDouble(matrix(0, 14)), 14);
     BOOST_REQUIRE(not matrix(1, 14));
     BOOST_REQUIRE_EQUAL(value::toInteger(matrix(2, 14)), 15);

     BOOST_REQUIRE_EQUAL(value::toDouble(matrix(0, 31)), 31);
     BOOST_REQUIRE(not matrix(1, 31));
     BOOST_REQUIRE_EQUAL(value::toInteger(matrix(2, 31)), 32);

     BOOST_REQUIRE_EQUAL(value::toDouble(matrix(0, 100)), 100);
     BOOST_REQUIRE_EQUAL(value::toDouble(matrix(1, 100)), 2550);
     BOOST_REQUIRE_EQUAL(value::toInteger(matrix(2, 100)), 1);
 }

 BOOST_AUTO_TEST_CASE(test_gens_delete_connection)
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

         while (root.run());
         std::unique_ptr<value::Map> out = root.outputs();
         BOOST_REQUIRE(not out);
         root.finish();
     } catch (const std::exception& e) {
         BOOST_REQUIRE(false);
     }
 }

BOOST_AUTO_TEST_CASE(test_gens_ordereddeleter)
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

        while (root.run());
        auto out = root.outputs();
        root.finish();

        /* begin check */
        BOOST_REQUIRE(out);
        BOOST_REQUIRE_EQUAL(out->size(), 1);

        /* get result of simulation */
        value::Matrix &matrix = out->getMatrix("view1");

        BOOST_REQUIRE_EQUAL(matrix.columns(), (std::size_t)2);

        for (std::size_t i = 0, ei = 10; i != ei; ++i) {
            BOOST_REQUIRE_EQUAL(value::toDouble(matrix(0, i)), i);
            BOOST_REQUIRE_EQUAL(value::toDouble(matrix(1, i)), 0);
        }
    }
}

 BOOST_AUTO_TEST_CASE(test_confluent_transition)
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

         while (root.run());
         std::unique_ptr<value::Map> out = root.outputs();
         BOOST_REQUIRE(not out);
         root.finish();
     } catch (const std::exception& e) {
         BOOST_REQUIRE(false);
     } catch (...) {
         BOOST_REQUIRE(false);
     }
 }

