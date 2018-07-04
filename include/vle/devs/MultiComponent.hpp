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

#ifndef VLEPROJECT_ORG_VLE_DEVS_MULTICOMPONENT_HPP
#define VLEPROJECT_ORG_VLE_DEVS_MULTICOMPONENT_HPP

#include <vle/devs/Dynamics.hpp>

#include <algorithm>
#include <functional>
#include <unordered_set>
#include <vector>

namespace vle {
namespace devs {

using identifier = unsigned int;

namespace details {

struct event
{
    vle::devs::Time time{ 0 };
    identifier id{ 0 };

    event() = default;

    event(vle::devs::Time time_, identifier id_)
      : time(time_)
      , id(id_)
    {}

    bool operator==(const event& other) const
    {
        return time == other.time and id == other.id;
    }

    bool operator<(const event& other) const
    {
        return time > other.time;
    }
};

struct scheduler
{
    std::vector<event> data;
    std::unordered_set<identifier> imminent;

    void make_heap()
    {
        std::make_heap(data.begin(), data.end());
    }

    void make_imminent()
    {
        imminent.clear();

        if (data.empty())
            return;

        vle::devs::Time t = data.back().time;
        imminent.emplace(data.back().id);
        data.pop_back();

        while (not data.empty() and t == data.back().time) {
            imminent.emplace(data.back().id);
            data.pop_back();
        }
    }

    bool empty() const
    {
        return data.empty();
    }

    vle::devs::Time top() const
    {
        return empty() ? vle::devs::infinity : data.back().time;
    }

    void remove(identifier id)
    {
        auto it =
          std::find_if(data.begin(), data.end(), [id](const event& e) -> bool {
              return e.id == id;
          });

        if (it == data.end())
            return;

        if (it + 1 != data.end())
            *it = *(it + 1);

        data.pop_back();
    }

    template<typename T>
    void remove(const std::map<identifier, T>& models)
    {
        for (const auto& elem : models)
            remove(elem.first);

        make_heap();
    }
};

template<typename ModelList>
auto
getModelExternalEventList(const ModelList& components,
                          const vle::devs::ExternalEventList& events)
  -> std::map<identifier, vle::devs::ExternalEventList>
{
    std::map<identifier, vle::devs::ExternalEventList> ret;

    for (const auto& event : events) {
        const auto& models = components.getInfluenced(event);

        for (auto model : models)
            ret[model].emplace_back(event);
    }

    return ret;
};

} // namespace details

template<typename ChangeState, typename ModelList>
class MultiComponent : public vle::devs::Dynamics
{
public:
    using change_state = ChangeState;
    using modellist_type = ModelList;

private:
    mutable details::scheduler m_scheduler;
    mutable modellist_type m_components;
    mutable std::map<identifier,
                     std::vector<std::pair<identifier, change_state>>>
      m_changes;
    vle::devs::Time m_current;

    auto push_changes(
      identifier from,
      const std::vector<std::pair<identifier, change_state>>& changes) -> void
    {
        for (const auto& elem : changes)
            m_changes[elem.first].push_back(std::make_pair(from, elem.second));
    }

    auto process_changes() -> void
    {
        for (const auto& elem : m_changes) {
            m_components.react(elem.first, elem.second);

            m_scheduler.remove(elem.first);
            m_scheduler.imminent.emplace(elem.first);
        }
    }

public:
    MultiComponent(const vle::devs::DynamicsInit& init,
                   const vle::devs::InitEventList& events)
      : vle::devs::Dynamics(init, events)
      , m_components(getModel(), events)
    {}

    ~MultiComponent() final = default;

    /**
     * @brief Initialize children and returns the duration of the initial
     *     state.
     * @details Call the @c init function for each components and returns the
     *     minimal duration of the most urgent component.
     *
     * @param time Current simulation time.
     * @return The minimal duration of the most urgent component.
     */
    vle::devs::Time init(vle::devs::Time time) final
    {
        m_current = time;

        for (identifier i{ 0 }, e{ m_components.size() }; i != e; ++i) {
            auto tn = m_components.init(i, time);
            if (tn < 0.0)
                throw vle::utils::ModellingError(
                  "Negative init function from "
                  "component %u in MultiComponent  "
                  "'%s' (%f)",
                  i,
                  getModel().getName().c_str(),
                  tn);

            if (not vle::devs::isInfinity(tn))
                m_scheduler.data.emplace_back(m_current + tn, i);
        }

        m_scheduler.make_heap();

        return m_scheduler.top();
    }

    /**
     * @brief performs the output function for each imminent component.
     * @details This function is called before an @c internalTransition or a @c
     *     confluentTransition. After building the imminent component list with
     *     the help of the scheduler, output of imminent component are call.
     *
     * @param time Current simulation time.
     * @param [out] output Stores the @c ExternalEvent build by component.
     */
    void output(vle::devs::Time time,
                vle::devs::ExternalEventList& output) const final
    {
#ifndef NDEBUG
        // Precondition: output with an empty imminent list from scheduler.

        if (not m_scheduler.imminent.empty())
            throw vle::utils::InternalError("output with empty imminent");
#endif

        m_scheduler.make_imminent();

        for (const auto& elem : m_scheduler.imminent)
            m_components.output(elem, time, output);
    }

    /**
     * @brief computes the the duration of the current state.
     * @details For each component, after an @c internalTransition, @c
     *     externalTransition, a @c confluentTransition or a @c
     *     reactTransition, the @c timeAdvance function is called, fill the
     *     scheduler and return the minimal duration of the current state.
     *
     * @param time Current simulation time.
     * @return The minimal duration of the most urgent component.
     */
    vle::devs::Time timeAdvance() const final
    {
#ifndef NDEBUG
        {
            // Precondition: the scheduler data must not contains any
            // identifier from the scheduler imminent list.

            for (auto id : m_scheduler.imminent)
                for (auto elem : m_scheduler.data)
                    if (id == elem.id)
                        throw vle::utils::InternalError(
                          "Scheduler data must not contains any identifier "
                          "from imminent list");
        }
#endif

        // For each component in imminent list, fill the scheduler with
        // all duration.

        for (auto elem : m_scheduler.imminent) {
            auto tn = m_components.timeAdvance(elem);
            if (tn < 0.0)
                throw vle::utils::ModellingError(
                  "Negative ta function from "
                  "component %u in MultiComponent  "
                  "'%s' (%f)",
                  elem,
                  getModel().getName().c_str(),
                  tn);

            if (not vle::devs::isInfinity(tn))
                m_scheduler.data.emplace_back(m_current + tn, elem);
        }

        m_scheduler.make_heap();
        m_scheduler.imminent.clear();

        return m_scheduler.top() - m_current;
    }

    /**
     * @brief performs the internalTransition for each imminent component.
     * @details Use the scheduler imminent list produced by the @c output
     *     function and call the internalTransition for each component.
     *
     * @param time Current simulation time.
     */
    void internalTransition(vle::devs::Time time) final
    {
        m_current = time;

        printf("internalTransition: %f\n", time);

        // m_scheduler.imminent is filled by the previous call to the output
        // function that fills the imminent list.

        m_changes.clear();
        for (const auto& elem : m_scheduler.imminent)
            push_changes(elem, m_components.internalTransition(elem, time));

        process_changes();
    }

    /**
     * @brief performs the externalTransition for each component.
     * @details First, need to remove influenced component from scheduler
     *     then, build a list of event per component to perform an
     *     externalTransition with the correct bag.
     *
     * @param events The list of external events.
     * @param time Current simulation time
     */
    void externalTransition(const vle::devs::ExternalEventList& events,
                            vle::devs::Time time) final
    {
        m_current = time;

        printf("MC externalTransition: %f\n", time);

        // Get the perturbed component from the @c ModelList.

        auto map = details::getModelExternalEventList(m_components, events);

        // Remove these models from the scheduler data and prepare the time
        // advance function call be filling the scheduler's imminent list.

        m_scheduler.remove(map);
        m_scheduler.imminent.clear();
        m_scheduler.imminent.reserve(map.size());

        m_changes.clear();
        for (const auto& model : map) {
            push_changes(model.first,
                         m_components.externalTransition(
                           model.first, model.second, time));
            m_scheduler.imminent.emplace(model.first);
        }

        process_changes();
        printf("MC externalTransition finished\n");
    }

    /**
     * @brief performs the confluentTransition.
     * @details This function performs a mix between @c internalTransition and
     *     @c externalTransition. First, need to get perturbed components, get
     *         imminent component then call correctly the @c internaTransition
     *         for the components that appear only in the imminent list, @c
     *         externalTransition that appear only in the perturbed component
     *         and @c confluentTransition from component that appear in both
     *         imminent and perturbed list.
     *
     * @param time Current simulation time
     * @param events The list of external events.
     */
    void confluentTransitions(vle::devs::Time time,
                              const vle::devs::ExternalEventList& events) final
    {
        m_current = time;

        printf("confluentTransitions: %f\n", time);

        // m_scheduler.imminent is filled by the previous call to the output
        // function that fills the imminent list. Get the perturbed component
        // from the @c ModelList and remove these models from the scheduler
        // data.

        auto map = details::getModelExternalEventList(m_components, events);
        m_scheduler.remove(map);

        // At least one component is in conflict (internal and external event
        // occurred at the same time). Try to found this or these components
        // and call the internal, confluent or external transition.

        m_changes.clear();
        for (auto id : m_scheduler.imminent) {
            auto it = map.find(id);

            // The component @c id appears both in the imminent and the
            // perturbed list, call confluent transition otherwise, this
            // component must made an internal transition

            if (it != map.end()) {
                push_changes(
                  id, m_components.confluentTransitions(id, time, events));

                // Remove this element from the imminent list to avoid multiple
                // id in imminent list.

                map.erase(it);
            } else {
                push_changes(id, m_components.internalTransition(id, time));
            }
        }

        // If the perturbed component list is not empty, these component must
        // perform an external transition.

        for (const auto& model : map) {
            push_changes(model.first,
                         m_components.externalTransition(
                           model.first, model.second, time));
            m_scheduler.imminent.emplace(model.first);
        }

        process_changes();
    }

    /**
     * @brief Process an observation event.
     * @details Relays the observation event to the @c ModelList::observation()
     *     function.
     *
     * @param event The observation that embeds observation port and view.
     * @return A value related to the observation event.
     */
    std::unique_ptr<vle::value::Value> observation(
      const vle::devs::ObservationEvent& event) const final
    {
        return m_components.observation(event);
    }

    /**
     * @brief Call finish for all component
     * @details End of simulation, time to clean-up.
     */
    void finish() final
    {
        for (identifier i{ 0 }, e{ m_components.size() }; i != e; ++i)
            m_components.finish(i);
    }
};
}
} // namespace vle devs

#endif
