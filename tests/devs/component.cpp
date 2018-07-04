/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2018 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2018 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2018 INRA http://www.inra.fr
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
#include <vle/devs/MultiComponent.hpp>
#include <vle/manager/Simulation.hpp>
#include <vle/oov/Plugin.hpp>
#include <vle/utils/Filesystem.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/unit-test.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/String.hpp>
#include <vle/vpz/Classes.hpp>
#include <vle/vpz/CoupledModel.hpp>
#include <vle/vpz/Dynamics.hpp>
#include <vle/vpz/Experiment.hpp>
#include <vle/vpz/Vpz.hpp>

#include "oov.hpp"

#include <chrono>
#include <numeric>

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

namespace package {

class Agent : public vle::devs::Dynamics
{
    struct position
    {
        position() = default;

        position(unsigned x_, unsigned y_)
          : x(x_)
          , y(y_)
        {}

        unsigned x = 0;
        unsigned y = 0;
    };

    struct task
    {
        task() = default;

        task(vle::devs::Time duration_,
             const std::initializer_list<position>& cells_)
          : duration(duration_)
          , cells(cells_)
        {}

        vle::devs::Time duration = { 0 };
        std::vector<position> cells;
    };

    std::vector<task> plan;
    std::size_t plan_id;
    unsigned m_width;
    unsigned m_height;

public:
    Agent(const vle::devs::DynamicsInit& model,
          const vle::devs::InitEventList& events)
      : vle::devs::Dynamics(model, events)
      , plan_id{ 0 }
      , m_width(static_cast<unsigned>(events.getInt("width")))
      , m_height(static_cast<unsigned>(events.getInt("height")))
    {
        Ensures(m_width > 5 and m_height > 5);

        plan.emplace_back();
        auto& vec = plan.back().cells;

        if (events.exist("oscillator")) {
            std::string oscillator = events.getString("oscillator");

            if (oscillator == "blinker") {
                vec.emplace_back(m_width / 2, m_height / 2);
                vec.emplace_back(m_width / 2, m_height / 2 - 1);
                vec.emplace_back(m_width / 2, m_height / 2 + 1);
            } else if (oscillator == "toad") {
                vec.emplace_back(m_width / 2 - 1, m_height / 2);
                vec.emplace_back(m_width / 2, m_height / 2);
                vec.emplace_back(m_width / 2 + 1, m_height / 2);
                vec.emplace_back(m_width / 2 - 2, m_height / 2 + 1);
                vec.emplace_back(m_width / 2 - 1, m_height / 2 + 1);
                vec.emplace_back(m_width / 2, m_height / 2 + 1);
            } else if (oscillator == "beacon") {
                vec.emplace_back(m_width / 2 - 2, m_height / 2 - 1);
                vec.emplace_back(m_width / 2 - 1, m_height / 2 - 1);
                vec.emplace_back(m_width / 2 - 2, m_height / 2);
                vec.emplace_back(m_width / 2 + 2, m_height / 2 + 1);
                vec.emplace_back(m_width / 2 + 1, m_height / 2 + 2);
                vec.emplace_back(m_width / 2 + 2, m_height / 2 + 2);
            } else if (oscillator == "block") {
                vec.emplace_back(m_width / 2, m_height / 2);
                vec.emplace_back(m_width / 2 + 1, m_height / 2);
                vec.emplace_back(m_width / 2, m_height / 2 + 1);
                vec.emplace_back(m_width / 2, m_height / 2 + 1);
            } else {
                vec.emplace_back(m_width / 2 - 1, m_height / 2);
                vec.emplace_back(m_width / 2 + 1, m_height / 2);
                vec.emplace_back(m_width / 2, m_height / 2 - 1);
                vec.emplace_back(m_width / 2, m_height / 2 + 1);
            }
        } else {
            for (unsigned j = 0; j != m_height / 4; ++j)
                for (unsigned i = 0; i != m_width; ++i)
                    vec.emplace_back(i, j);
        }
    }

    vle::devs::Time init(vle::devs::Time /* time */) override
    {
        return 0.0;
    }

    vle::devs::Time timeAdvance() const override
    {
        return plan_id < plan.size() ? plan[plan_id].duration
                                     : vle::devs::infinity;
    }

    void output(vle::devs::Time /* time */,
                vle::devs::ExternalEventList& output) const override
    {
        const task& t = plan[plan_id];

        for (auto pos : t.cells) {
            output.emplace_back("out");
            output.back().addInteger(pos.y * m_width + pos.x);
        }
    }

    void internalTransition(vle::devs::Time /*time*/) override
    {
        ++plan_id;
    }
};

enum class cell_status
{
    live,
    dead
};

struct cell_state
{
    cell_state() = default;

    cell_state(cell_status state_)
      : state(state_)
    {}

    cell_status state = cell_status::dead;
};

struct LifeGameCell
{
    std::unordered_map<unsigned, cell_state> m_neighborhood;
    vle::devs::Time m_sigma;
    unsigned m_x;
    unsigned m_y;
    cell_state m_state;

    LifeGameCell(unsigned x_, unsigned y_, unsigned width, unsigned height)
      : m_sigma(vle::devs::infinity)
      , m_x{ x_ }
      , m_y{ y_ }
      , m_state{ cell_status::dead }
    {
        Ensures(x_ < width and y_ < height);

        const unsigned begin_i = m_x > 0 ? m_x - 1 : 0;
        const unsigned end_i = m_x < width - 1 ? m_x + 1 : width - 1;
        const unsigned begin_j = m_y > 0 ? m_y - 1 : 0;
        const unsigned end_j = m_y < height - 1 ? m_y + 1 : height - 1;

        for (unsigned i{ begin_i }; i <= end_i; ++i)
            for (unsigned j{ begin_j }; j <= end_j; ++j)
                if (not(i == x_ and j == y_))
                    m_neighborhood[j * width + i] = cell_status::dead;

        Ensures(m_neighborhood.size() >= std::size_t{ 3 } and
                m_neighborhood.size() <= std::size_t{ 8 });

        if (x_ == 0 and y_ == 0)
            Ensures(m_neighborhood.size() == 3);

        if (x_ == width / 2 and y_ == height / 2)
            Ensures(m_neighborhood.size() == 8);
    }

    vle::devs::Time init(vle::devs::Time /*time*/)
    {
        return vle::devs::infinity;
    }

    void output(vle::devs::Time, vle::devs::ExternalEventList&) const
    {}

    vle::devs::Time timeAdvance() const
    {
        return m_sigma;
    }

    bool update()
    {
        std::vector<std::pair<unsigned, cell_state>> ret;

        auto sum =
          std::accumulate(m_neighborhood.begin(),
                          m_neighborhood.end(),
                          0,
                          [](unsigned init, const auto& neighbour) {
                              if (neighbour.second.state == cell_status::live)
                                  return init + 1;

                              return init;
                          });

        if (m_state.state == cell_status::live and (sum < 2 or sum > 3)) {
            m_state.state = cell_status::dead;
            return true;
        } else if (m_state.state == cell_status::dead and sum == 3) {
            m_state.state = cell_status::live;
            return true;
        }

        return false;
    };

    auto react(const std::vector<std::pair<unsigned, cell_state>>& changes)
      -> void
    {
        bool may_change = false;

        for (const auto& elem : changes) {
            auto& neighbour = m_neighborhood[elem.first];
            if (neighbour.state != elem.second.state) {
                may_change = true;
                neighbour.state = elem.second.state;
            }
        }

        m_sigma = may_change ? 1.0 : vle::devs::infinity;
    }

    auto internalTransition(vle::devs::Time)
      -> std::vector<std::pair<unsigned, cell_state>>
    {
        std::vector<std::pair<unsigned, cell_state>> ret;

        if (update()) {
            for (const auto& elem : m_neighborhood)
                ret.emplace_back(elem.first, m_state.state);
            m_sigma = 1.0;
        } else {
            m_sigma = vle::devs::infinity;
        }

        return ret;
    }

    auto externalTransition(const vle::devs::ExternalEventList& events,
                            vle::devs::Time /*time*/)
      -> std::vector<std::pair<unsigned, cell_state>>
    {
        std::vector<std::pair<unsigned, cell_state>> ret;

        m_sigma = vle::devs::infinity;

        std::size_t need_live{ 0 }, need_dead{ 0 };

        for (const auto& event : events) {
            if (event.onPort("live"))
                ++need_live;
            else if (event.onPort("dead"))
                ++need_dead;
        }

        cell_status new_state =
          (need_live > need_dead) ? cell_status::live : cell_status::dead;

        if (new_state != m_state.state) {
            m_state.state = new_state;
            for (const auto& elem : m_neighborhood)
                ret.emplace_back(elem.first, new_state);
        }

        return ret;
    }

    auto confluentTransitions(const vle::devs::ExternalEventList& events,
                              vle::devs::Time time)
      -> std::vector<std::pair<unsigned, cell_state>>
    {
        std::vector<std::pair<unsigned, cell_state>> ret;
        cell_status old_state = m_state.state;

        (void)externalTransition(events, time);
        (void)internalTransition(time);

        if (old_state != m_state.state)
            for (const auto& elem : m_neighborhood)
                ret.emplace_back(elem.first, old_state);

        return ret;
    }
};

struct ModelList
{
    std::vector<LifeGameCell> m_cells;

    /** Map input port, componant */
    std::map<std::string, std::vector<unsigned>> influencers;

    /** Map output port, influenced */
    std::map<std::string, std::vector<unsigned>> influenced;

    const std::vector<unsigned> empty = {};
    const unsigned int m_width;
    const unsigned int m_height;

    ModelList(const vle::vpz::AtomicModel& /*atom*/,
              const vle::devs::InitEventList& events)
      : m_width(events.getInt("width"))
      , m_height(events.getInt("height"))
    {
        for (unsigned j{ 0 }; j != m_height; ++j)
            for (unsigned i{ 0 }; i != m_width; ++i)
                m_cells.emplace_back(static_cast<unsigned>(i),
                                     static_cast<unsigned>(j),
                                     static_cast<unsigned>(m_width),
                                     static_cast<unsigned>(m_height));
    }

    unsigned size() const noexcept
    {
        return static_cast<unsigned>(m_cells.size());
    }

    auto getInfluenced(const vle::devs::ExternalEvent& event) const
      -> std::vector<unsigned>
    {
        if (event.onPort("live") or event.onPort("dead")) {
            return { static_cast<unsigned>(event.getInteger().value()) };
        }

        return {};
    }

    void output(unsigned /* id */,
                vle::devs::Time /* time */,
                vle::devs::ExternalEventList& /* output */) const
    {}

    vle::devs::Time init(unsigned id, vle::devs::Time time)
    {
        return m_cells[id].init(time);
    }

    vle::devs::Time timeAdvance(unsigned id) const
    {
        return m_cells[id].timeAdvance();
    }

    auto internalTransition(unsigned id, vle::devs::Time time)
      -> std::vector<std::pair<unsigned, cell_state>>
    {
        return m_cells[id].internalTransition(time);
    }

    auto externalTransition(unsigned id,
                            const vle::devs::ExternalEventList& events,
                            vle::devs::Time time)
      -> std::vector<std::pair<unsigned, cell_state>>
    {
        return m_cells[id].externalTransition(events, time);
    }

    auto confluentTransitions(unsigned id,
                              vle::devs::Time time,
                              const vle::devs::ExternalEventList& events)
      -> std::vector<std::pair<unsigned, cell_state>>
    {
        return m_cells[id].confluentTransitions(events, time);
    }

    void react(unsigned id,
               const std::vector<std::pair<unsigned, cell_state>>& changes)
    {
        for (const auto& change : changes)
            Ensures(m_cells[id].m_neighborhood.find(change.first) !=
                    m_cells[id].m_neighborhood.end());

        m_cells[id].react(changes);
    }

    std::unique_ptr<vle::value::Value> observation(
      const vle::devs::ObservationEvent& /*event*/) const
    {
        std::string ret;

        for (unsigned j{ 0 }; j != m_height; ++j)
            for (unsigned i{ 0 }; i != m_width; ++i)
                ret +=
                  (m_cells[j * m_width + i].m_state.state == cell_status::live)
                    ? "x"
                    : ".";

        return vle::value::String::create(ret);
    }

    void finish(unsigned /*id*/)
    {}
};

using Model = vle::devs::MultiComponent<cell_state, ModelList>;

} // namespace package

DECLARE_DYNAMICS_SYMBOL(dynamics_component_a, package::Model);
DECLARE_DYNAMICS_SYMBOL(dynamics_agent, package::Agent);

extern "C" {

__attribute__((visibility("default")))
vle::oov::Plugin*
oov_plugin(const std::string& location)
{
    return new vletest::OutputPlugin(location);
}

}

extern
__attribute__((visibility("default")))
vle::oov::Plugin*
oov_plugin(const std::string& location);

static auto
run_simulation(vle::utils::ContextPtr ctx,
               std::unique_ptr<vle::vpz::Vpz> file)
  -> std::unique_ptr<vle::value::Map>
{
    using namespace std::chrono_literals;

    vle::manager::Simulation simulator(ctx,
                                       vle::manager::LOG_NONE,
                                       vle::manager::SIMULATION_NONE,
                                       0ms,
                                       &std::cerr);

    vle::manager::Error error;
    auto ret = simulator.run(std::move(file), &error);

    if (error.code)
        std::cerr << "Simulation failed with code " << error.code
                  << " : " << error.message << '\n';

    return ret;
}

void
test_component()
{
    auto ctx = vle::utils::make_context();
    ctx->set_log_priority(7);

    vle::utils::Path p(DEVS_TEST_DIR);
    vle::utils::Path::current_path(p);

    auto file = std::make_unique<vle::vpz::Vpz>(DEVS_TEST_DIR "/component.vpz");

    try {
        auto out = run_simulation(ctx, std::move(file));

        Ensures(out);
        EnsuresEqual(out->size(), 1);
        const auto& matrix = out->getMatrix("view1");
        EnsuresEqual(matrix.columns(), static_cast<std::size_t>(2));
        EnsuresEqual(matrix.rows(), static_cast<std::size_t>(5));

        EnsuresEqual(
          vle::value::toString(matrix(1, 0)),
          std::string(".......................xxx...xxx................."));

        EnsuresEqual(
          vle::value::toString(matrix(1, 1)),
          std::string(".................x....x..x...x..x....x..........."));

        EnsuresEqual(
          vle::value::toString(matrix(1, 2)),
          std::string(".......................xxx...xxx................."));

        EnsuresEqual(
          vle::value::toString(matrix(1, 3)),
          std::string(".................x....x..x...x..x....x..........."));

        EnsuresEqual(
          vle::value::toString(matrix(1, 4)),
          std::string(".......................xxx...xxx................."));

        out->writeXml(std::cout);

    } catch (const std::exception& e) {
        fprintf(stderr, "test_component: %s\n", e.what());
        Ensures(false);
    } catch (...) {
        Ensures(false);
    }
}

int
main()
{
    auto x = ::oov_plugin("test");
    if (x)
        test_component();

    return unit_test::report_errors();
}
