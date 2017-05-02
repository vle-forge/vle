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

#include <cassert>
#include <vle/devs/Dynamics.hpp>
#include <vle/devs/View.hpp>
#include <vle/utils/Algo.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/vpz/CoupledModel.hpp>

namespace vle {
namespace devs {

void
View::open(utils::ContextPtr ctx,
           const std::string& name,
           const std::string& pluginname,
           const std::string& package,
           const std::string& location,
           const std::string& file,
           Time time,
           std::unique_ptr<value::Value> parameters)
{
    m_name = name;

    try {
        void* symbol = nullptr;

        /* If \e package is not empty we assume that library is the shared
         * library. Otherwise, we load the global symbol stores in \e
         * library/executable and we cast it into a \e
         * vle::oov::OovPluginSlot... Only useful for unit test or to
         * build executable with plugins.
         */
        if (not package.empty())
            symbol = ctx->get_symbol(package,
                                     pluginname,
                                     utils::Context::ModuleType::MODULE_OOV,
                                     nullptr);
        else
            symbol = ctx->get_symbol(pluginname);

        oov::OovPluginSlot fct(
          utils::functionCast<oov::OovPluginSlot>(symbol));
        m_plugin = std::unique_ptr<oov::Plugin>(fct(location));
    } catch (const std::exception& e) {
        throw utils::FileError(
          _("View: Can not open the plug-in `%s' in package `%s'"
            ": %s"),
          pluginname.c_str(),
          package.c_str(),
          e.what());
    }

    m_plugin->onParameter(
      pluginname, location, file, std::move(parameters), time);
}

void
View::addObservable(Dynamics* dynamics,
                    const std::string& portname,
                    Time currenttime)
{
    assert(dynamics);
    assert(not exist(dynamics, portname));
    assert(m_plugin);

    m_observableList.emplace(dynamics, portname);

    m_plugin->onNewObservable(dynamics->getModel().getName(),
                              dynamics->getModel().getParentName(),
                              portname,
                              m_name,
                              currenttime);
}

void
View::removeObservable(Dynamics* dynamics)
{
    assert(dynamics);
    assert(m_plugin);

    auto result = m_observableList.equal_range(dynamics);

    for (auto it = result.first; it != result.second; ++it)
        m_plugin->onDelObservable(it->first->getModel().getName(),
                                  it->first->getModel().getParentName(),
                                  it->second,
                                  m_name,
                                  0.0);

    m_observableList.erase(result.first, result.second);
}

bool
View::exist(Dynamics* dynamics, const std::string& portname) const
{
    auto result = m_observableList.equal_range(dynamics);

    for (auto it = result.first; it != result.second; ++it)
        if (it->second == portname)
            return true;

    return false;
}

bool
View::exist(Dynamics* dynamics) const
{
    return m_observableList.find(dynamics) != m_observableList.end();
}

void
View::run(Time time)
{
    if (not m_observableList.empty()) {
        for (auto& elem : m_observableList) {
            ObservationEvent event(time, m_name, elem.second);
            auto val = elem.first->observation(event);
            m_plugin->onValue(elem.first->getModel().getName(),
                              elem.first->getModel().getParentName(),
                              elem.second,
                              m_name,
                              time,
                              std::move(val));
        }
    } else {
        //
        // Strange behavior.
        //
        m_plugin->onValue(
          std::string(), std::string(), std::string(), m_name, time, nullptr);
    }
}

void
View::run(const Dynamics* dynamics, Time current, const std::string& port)
{
    ObservationEvent event(current, m_name, port);
    auto val = dynamics->observation(event);

    m_plugin->onValue(dynamics->getModel().getName(),
                      dynamics->getModel().getParentName(),
                      port,
                      m_name,
                      current,
                      std::move(val));
}

void
View::run(const Dynamics* dynamics,
          Time current,
          const std::string& port,
          std::unique_ptr<value::Value> value)
{
    m_plugin->onValue(dynamics->getModel().getName(),
                      dynamics->getModel().getParentName(),
                      port,
                      m_name,
                      current,
                      std::move(value));
}

std::unique_ptr<value::Matrix>
View::matrix() const
{
    return m_plugin->matrix();
}

std::unique_ptr<value::Matrix>
View::finish(Time current)
{
    return m_plugin->finish(current);
}
}
} // namespace vle devs
