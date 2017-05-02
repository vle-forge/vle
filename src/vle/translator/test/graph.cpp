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

#include <vle/devs/Dynamics.hpp>
#include <vle/devs/Executive.hpp>
#include <vle/devs/RootCoordinator.hpp>
#include <vle/translator/GraphTranslator.hpp>
#include <vle/translator/MatrixTranslator.hpp>
#include <vle/utils/unit-test.hpp>
#include <vle/vpz/Vpz.hpp>

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

class Beep : public devs::Dynamics
{
public:
    Beep(const devs::DynamicsInit& model, const devs::InitEventList& events)
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

class GraphGenerator : public devs::Executive
{
    std::mt19937 generator;
    std::string generator_type;

    translator::graph_generator make_gg()
    {
        translator::graph_generator ret(
          { &GraphGenerator::maker,
            translator::graph_generator::connectivity::IN_OUT,
            false });

        return ret;
    }

    translator::regular_graph_generator make_rgg()
    {
        translator::regular_graph_generator ret(
          { &GraphGenerator::regular_maker,
            translator::regular_graph_generator::connectivity::NAMED });

        return ret;
    }

public:
    GraphGenerator(const devs::ExecutiveInit& mdl,
                   const devs::InitEventList& events)
      : devs::Executive(mdl, events)
      , generator(123)
      , generator_type(events.getString("generator"))
    {
    }

    static void maker(const translator::graph_generator::node_metrics& metrics,
                      std::string& name,
                      std::string& classname)
    {
        name = std::to_string(metrics.id);
        classname = "nothing";
    }

    static void regular_maker(
      const translator::regular_graph_generator::node_metrics& metrics,
      std::string& name,
      std::string& classname)
    {
        name = std::to_string(metrics.x);
        if (metrics.y != -1)
            name += '-' + std::to_string(metrics.y);

        classname = "nothing";
    }

    virtual devs::Time init(devs::Time time) override
    {
        if (generator_type == "smallworld") {
            auto gg = make_gg();
            gg.make_smallworld(*this, generator, 1000, 7, 0.03, false);

            auto metrics = gg.metrics();

            Ensures(metrics.vertices == 1000);
            Ensures(metrics.edges > 0);
            Ensures(metrics.bandwidth > 0);
        } else if (generator_type == "scalefree") {
            auto gg = make_gg();
            gg.make_scalefree(*this, generator, 1000, 2.5, 10000, false);

            auto metrics = gg.metrics();

            Ensures(metrics.vertices == 1000);
            Ensures(metrics.edges > 0);
            Ensures(metrics.bandwidth > 0);
        } else if (generator_type == "sortederdosrenyi") {
            auto gg = make_gg();
            gg.make_sorted_erdos_renyi(*this, generator, 1000, 0.01, false);

            auto metrics = gg.metrics();

            Ensures(metrics.vertices == 1000);
            Ensures(metrics.edges > 0);
            Ensures(metrics.bandwidth > 0);
        } else if (generator_type == "1d") {
            auto rgg = make_rgg();
            std::vector<std::string> mask{ "left", "", "right" };
            rgg.make_1d(*this, 1000, false, mask, 1);

            auto metrics = rgg.metrics();
            Ensures(metrics.vertices == 1000);
        } else if (generator_type == "2d") {
            auto rgg = make_rgg();

            std::array<int, 2> length{ { 50, 20 } };
            std::array<bool, 2> wrap{ { false, false } };
            utils::Array<std::string> mask(3, 3);
            mask(1, 0) = "N";
            mask(0, 1) = "W";
            mask(2, 1) = "E";
            mask(1, 2) = "S";

            rgg.make_2d(*this, length, wrap, mask, 1, 1);

            auto metrics = rgg.metrics();
            Ensures(metrics.vertices == 1000);
        } else if (generator_type == "1dwrap") {
            auto rgg = make_rgg();
            std::vector<std::string> mask{ "left", "", "right" };
            rgg.make_1d(*this, 1000, true, mask, 1);

            auto metrics = rgg.metrics();
            Ensures(metrics.vertices == 1000);
        } else if (generator_type == "2dwrap") {
            auto rgg = make_rgg();

            std::array<int, 2> length{ { 50, 20 } };
            std::array<bool, 2> wrap{ { true, true } };
            utils::Array<std::string> mask(3, 3);
            mask(1, 0) = "N";
            mask(0, 1) = "W";
            mask(2, 1) = "E";
            mask(1, 2) = "S";

            rgg.make_2d(*this, length, wrap, mask, 1, 1);

            auto metrics = rgg.metrics();
            Ensures(metrics.vertices == 1000);
        } else {
            EnsuresNotReached();
        }

        Ensures(coupledmodel().getModelList().size() == 1001);

        return devs::Executive::init(time);
    }
};

DECLARE_DYNAMICS_SYMBOL(dynamics_Beep, Beep)
DECLARE_DYNAMICS_SYMBOL(dynamics_transform, Transform)
DECLARE_EXECUTIVE_SYMBOL(exe_graph, GraphGenerator)

void
test_smallworld()
{
    auto ctx = vle::utils::make_context();
    vle::utils::Path p(TRANSLATOR_TEST_DIR);
    vle::utils::Path::current_path(p);

    vpz::Vpz file(TRANSLATOR_TEST_DIR "/graph.vpz");
    devs::RootCoordinator root(ctx);

    auto& cond = file.project()
                   .experiment()
                   .conditions()
                   .get("cond")
                   .getSetValues("generator")[0]
                   ->toString()
                   .value();

    cond = "smallworld";

    root.load(file);
    file.clear();
    root.init();

    while (root.run())
        ;

    auto out = root.outputs();
    root.finish();

    Ensures(not out);
}

void
test_scalefree()
{
    auto ctx = vle::utils::make_context();
    vle::utils::Path p(TRANSLATOR_TEST_DIR);
    vle::utils::Path::current_path(p);

    vpz::Vpz file(TRANSLATOR_TEST_DIR "/graph.vpz");
    devs::RootCoordinator root(ctx);

    auto& cond = file.project()
                   .experiment()
                   .conditions()
                   .get("cond")
                   .getSetValues("generator")[0]
                   ->toString()
                   .value();

    cond = "scalefree";

    root.load(file);
    file.clear();
    root.init();

    while (root.run())
        ;

    auto out = root.outputs();
    root.finish();

    Ensures(not out);
}

void
test_sorted_erdos_renyi()
{
    auto ctx = vle::utils::make_context();
    vle::utils::Path p(TRANSLATOR_TEST_DIR);
    vle::utils::Path::current_path(p);

    vpz::Vpz file(TRANSLATOR_TEST_DIR "/graph.vpz");
    devs::RootCoordinator root(ctx);

    auto& cond = file.project()
                   .experiment()
                   .conditions()
                   .get("cond")
                   .getSetValues("generator")[0]
                   ->toString()
                   .value();

    cond = "sortederdosrenyi";

    root.load(file);
    file.clear();
    root.init();

    while (root.run())
        ;

    auto out = root.outputs();
    root.finish();

    Ensures(not out);
}

void
test_regular_1d()
{
    auto ctx = vle::utils::make_context();
    vle::utils::Path p(TRANSLATOR_TEST_DIR);
    vle::utils::Path::current_path(p);

    vpz::Vpz file(TRANSLATOR_TEST_DIR "/graph.vpz");
    devs::RootCoordinator root(ctx);

    auto& cond = file.project()
                   .experiment()
                   .conditions()
                   .get("cond")
                   .getSetValues("generator")[0]
                   ->toString()
                   .value();

    cond = "1d";

    root.load(file);
    file.clear();
    root.init();

    while (root.run())
        ;

    auto out = root.outputs();
    root.finish();

    Ensures(not out);
}

void
test_regular_2d()
{
    auto ctx = vle::utils::make_context();
    vle::utils::Path p(TRANSLATOR_TEST_DIR);
    vle::utils::Path::current_path(p);

    vpz::Vpz file(TRANSLATOR_TEST_DIR "/graph.vpz");
    devs::RootCoordinator root(ctx);

    auto& cond = file.project()
                   .experiment()
                   .conditions()
                   .get("cond")
                   .getSetValues("generator")[0]
                   ->toString()
                   .value();

    cond = "2d";

    root.load(file);
    file.clear();
    root.init();

    while (root.run())
        ;

    auto out = root.outputs();
    root.finish();

    Ensures(not out);
}

void
test_regular_1d_wrap()
{
    auto ctx = vle::utils::make_context();
    vle::utils::Path p(TRANSLATOR_TEST_DIR);
    vle::utils::Path::current_path(p);

    vpz::Vpz file(TRANSLATOR_TEST_DIR "/graph.vpz");
    devs::RootCoordinator root(ctx);

    auto& cond = file.project()
                   .experiment()
                   .conditions()
                   .get("cond")
                   .getSetValues("generator")[0]
                   ->toString()
                   .value();

    cond = "1dwrap";

    root.load(file);
    file.clear();
    root.init();

    while (root.run())
        ;

    auto out = root.outputs();
    root.finish();

    Ensures(not out);
}

void
test_regular_2d_wrap()
{
    auto ctx = vle::utils::make_context();
    vle::utils::Path p(TRANSLATOR_TEST_DIR);
    vle::utils::Path::current_path(p);

    vpz::Vpz file(TRANSLATOR_TEST_DIR "/graph.vpz");
    devs::RootCoordinator root(ctx);

    auto& cond = file.project()
                   .experiment()
                   .conditions()
                   .get("cond")
                   .getSetValues("generator")[0]
                   ->toString()
                   .value();

    cond = "2dwrap";

    root.load(file);
    file.clear();
    root.init();

    while (root.run())
        ;

    auto out = root.outputs();
    root.finish();

    Ensures(not out);
}

int
main()
{
    vle::Init app;

    test_smallworld();
    test_scalefree();
    test_sorted_erdos_renyi();

    test_regular_1d();
    test_regular_2d();
    test_regular_1d_wrap();
    test_regular_2d_wrap();

    return unit_test::report_errors();
}
