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
#define BOOST_TEST_MODULE devstime_mdl
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <stack>
#include <stdexcept>
#include <limits>
#include <fstream>
#include <vle/devs/Dynamics.hpp>
#include <vle/devs/DynamicsDbg.hpp>
#include <vle/devs/Executive.hpp>
#include <vle/devs/ExecutiveDbg.hpp>
#include <vle/devs/Coordinator.hpp>
#include <vle/devs/RootCoordinator.hpp>
#include <vle/vpz/CoupledModel.hpp>
#include <vle/vpz/Dynamics.hpp>
#include <vle/vpz/Experiment.hpp>
#include <vle/vpz/Classes.hpp>
#include <vle/oov/Plugin.hpp>
#include <vle/utils/ModuleManager.hpp>
#include <iostream>

#if defined(__unix__)
#include <unistd.h>
#elif defined(__WIN32__)
#include <io.h>
#include <stdio.h>
#endif

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



class Beep : public devs::Dynamics
{
public:
    Beep(const devs::DynamicsInit& model,
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
        if (m_active) {
            return devs::Time(0.0);
        }
        else {
            return devs::infinity;
        }
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
            const devs::ExternalEventList& /*extEventlist*/)
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
            const devs::ExternalEventList& /*extEventlist*/)
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


class TimedObs : public devs::Dynamics
{
public:
    TimedObs(const devs::DynamicsInit& atom,
              const devs::InitEventList& events) :
        devs::Dynamics(atom, events)
    {}

    virtual devs::Time init(devs::Time /* time */) override
    {
        return devs::infinity;
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
            const devs::ExternalEventList& /*extEventlist*/)
    {
    }

    virtual void output(devs::Time /*time*/,
                        devs::ExternalEventList& /*output*/) const override
    {
    }

    virtual std::unique_ptr<value::Value>
    observation(const devs::ObservationEvent&) const override
    {
        return 0;
    }
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
        vpz::Dynamic dyn("gensbeep");
        dyn.setPackage("");
        dyn.setLibrary("dynamics_beep");
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
                std::string name = (fmt("beep_%1%") % m_stacknames.size()).str();
                set->toSet().add(value::String::create(name));
                set->toSet().add(value::String::create("2"));
                std::string edge =  name + std::string(" counter ");
                set->toSet().add(value::String::create(edge));
            } else if(get_nb_model() > 0){
                set->toSet().add(value::String::create("delete"));
                std::string name = (fmt(
                        "beep_%1%") % (get_nb_model())).str();
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
        std::string name((fmt("beep_%1%") % m_stacknames.size()).str());

        std::vector < std::string > outputs;
        outputs.push_back("out");

        createModel(name, std::vector < std::string >(), outputs, "gensbeep");
        addConnection(name, "out", "counter", "in");

        m_stacknames.push(name);
    }

    void del_first_model()
    {
        if (m_stacknames.empty()) {
            throw utils::InternalError(fmt(
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


std::string make_id(const std::string& view,
                    const std::string& simulator,
                    const std::string& parent,
                    const std::string& port)
{
    std::string ret;
    ret.reserve(view.size() + simulator.size() +
                parent.size() + port.size() + 4);

    ret += view;
    ret += '.';
    ret += parent;
    ret += '.';
    ret += simulator;
    ret += '.';
    ret += port;

    return ret;
}


class OutputPlugin : public vle::oov::Plugin
{
    using data_type = std::map<
        std::string,
        std::vector<
            std::pair<
                double, std::unique_ptr<vle::value::Value>>>>;
    
    data_type ppD;

public:
    OutputPlugin(const std::string& location)
        : vle::oov::Plugin(location)
    {   
    }

    virtual ~OutputPlugin() = default;

    virtual std::unique_ptr<value::Matrix> matrix() const override
    {
        BOOST_REQUIRE(not ppD.empty());
        const size_t columns = ppD.size() + 1; // colums = number of
                                               // observation + a time column.
        size_t rows = 0;

        for (auto& elem : ppD)
            for (std::size_t i = 0, e = elem.second.size(); i != e; ++i)
                rows = std::max(rows,
                                static_cast<std::size_t>(
                                    std::floor(elem.second[i].first)));

        auto matrix = std::unique_ptr<value::Matrix>(
            new value::Matrix{columns, rows + 1, 100, 100});

        size_t col = 1;
        for (auto& elem : ppD) {
            for (std::size_t i = 0, e = elem.second.size(); i != e; ++i) {
                std::size_t row = static_cast<std::size_t>(
                    std::floor(elem.second[i].first));

                if (elem.second[i].second)
                    matrix->set(col, row, elem.second[i].second->clone());

                matrix->set(0, row, value::Double::create(elem.second[i].first));
            }
            col++;
        }
    
        return matrix;
    }

    virtual std::string name() const override
    {
        return "OutputPlugin";
    }

    virtual bool isCairo() const override
    {
        return false;
    }

    virtual void onParameter(const std::string& plugin,
                             const std::string& location,
                             const std::string& file,
                             std::unique_ptr<value::Value> parameters,
                             const double& time) override
    {
        (void)plugin;
        (void)location;
        (void)file;
        (void)parameters;
        (void)time;            
    }

    virtual void onNewObservable(const std::string& simulator,
                                 const std::string& parent,
                                 const std::string& port,
                                 const std::string& view,
                                 const double& time) override
    {
        (void)time;

        std::string id = make_id(view, parent, simulator, port);
        BOOST_REQUIRE(ppD.find(id) == ppD.cend());

        ppD[id].reserve(100);
    }

    virtual void onDelObservable(const std::string& simulator,
                                 const std::string& parent,
                                 const std::string& port,
                                 const std::string& view,
                                 const double& time) override
    {
        (void)time;

        std::string id = make_id(view, parent, simulator, port);

        BOOST_REQUIRE(ppD.find(id) != ppD.cend());
    }

    virtual void onValue(const std::string& simulator,
                         const std::string& parent,
                         const std::string& port,
                         const std::string& view,
                         const double& time,
                         std::unique_ptr<value::Value> value) override
    {
        if (simulator.empty()) /** TODO this is a strange
                                   behaviour. Sending value withtout
                                   simulator ?!?. */
            return;
        
        std::string id = make_id(view, parent, simulator, port);

        BOOST_REQUIRE(ppD.find(id) != ppD.cend());

        ppD[id].emplace_back(time, std::move(value));
    }

    virtual void close(const double& /* time */) override
    {
    }
};

DECLARE_DYNAMICS_SYMBOL(dynamics_beep, ::Beep)
DECLARE_DYNAMICS_SYMBOL(dynamics_counter, ::Counter)
DECLARE_DYNAMICS_SYMBOL(dynamics_transform, ::Transform)
DECLARE_DYNAMICS_SYMBOL(dynamics_confluent_transitionA, ::Confluent_transitionA)
DECLARE_DYNAMICS_SYMBOL(dynamics_confluent_transitionB, ::Confluent_transitionB)
DECLARE_DYNAMICS_SYMBOL(dynamics_timed_obs, ::TimedObs)
DECLARE_EXECUTIVE_SYMBOL(exe_branch, ::Branch)
DECLARE_EXECUTIVE_SYMBOL(exe_deleteconnection, ::DeleteConnection)
DECLARE_EXECUTIVE_SYMBOL(exe_genexecutive, ::GenExecutive)
DECLARE_EXECUTIVE_SYMBOL(exe_leaf, ::Leaf)
DECLARE_EXECUTIVE_SYMBOL(exe_root, ::Root)
DECLARE_OOV_SYMBOL(oov_plugin, ::OutputPlugin)

BOOST_AUTO_TEST_CASE(test_gensvpz)
{
    int ret = ::chdir(DEVS_TEST_DIR);
    BOOST_REQUIRE(ret == 0);

    vpz::Vpz file(DEVS_TEST_DIR "/gens.vpz");
    utils::ModuleManager modules;
    devs::RootCoordinator root(modules);

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
    int ret = ::chdir(DEVS_TEST_DIR);
    BOOST_REQUIRE(ret == 0);

    vpz::Vpz file(DEVS_TEST_DIR "/gensdelete.vpz");
    utils::ModuleManager modules;
    devs::RootCoordinator root(modules);

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
    int ret = ::chdir(DEVS_TEST_DIR);
    BOOST_REQUIRE(ret == 0);

    for (int s = 0, es = 100; s != es; ++s) {
        vpz::Vpz file(DEVS_TEST_DIR "/ordereddeleter.vpz");
        utils::ModuleManager modules;
        devs::RootCoordinator root(modules);

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
    int ret = ::chdir(DEVS_TEST_DIR);
    BOOST_REQUIRE(ret == 0);

    vpz::Vpz file(DEVS_TEST_DIR "/confluent_transition.vpz");
    utils::ModuleManager modules;
    devs::RootCoordinator root(modules);

    try {
        root.load(file);
        file.clear();
        root.init();

        while (root.run());
        std::unique_ptr<value::Map> out = root.outputs();
        BOOST_REQUIRE(out);
        root.finish();
    } catch (const std::exception& e) {
        BOOST_REQUIRE(false);
    }
}


BOOST_AUTO_TEST_CASE(test_timed_obs)
{
    int ret = ::chdir(DEVS_TEST_DIR);
    BOOST_REQUIRE(ret == 0);

    vpz::Vpz file(DEVS_TEST_DIR "/timed_obs.vpz");
    utils::ModuleManager modules;
    devs::RootCoordinator root(modules);

    try {
        root.load(file);
        file.clear();
        root.init();

        while (root.run());
        std::unique_ptr<value::Map> out = root.outputs();
        BOOST_REQUIRE(out);
        root.finish();
    } catch (const std::exception& e) {
        BOOST_REQUIRE(false);
    }
}

