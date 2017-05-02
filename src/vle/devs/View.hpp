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

#ifndef VLE_DEVS_VIEW_HPP
#define VLE_DEVS_VIEW_HPP 1

#include <map>
#include <string>
#include <vle/DllDefines.hpp>
#include <vle/devs/Time.hpp>
#include <vle/oov/Plugin.hpp>
#include <vle/utils/Context.hpp>
#include <vle/value/Matrix.hpp>

namespace vle {
namespace devs {

class Dynamics;
class View;

/**
 * A simple structure that stores observation values for a specific view
 * and portname tuple.
 */
struct Observation
{
    View* view = nullptr;
    std::string portname;
    std::unique_ptr<value::Value> value;
};

/**
 * @brief Represent a View on a devs::Dynamics and a port name.
 *
 */
class VLE_LOCAL View
{
public:
    View() = default;
    ~View() = default;

    /**
     * Initialize plugin with specified information.
     *
     * @param plugin the plugin's name.
     * @param package the plugin's package.
     * @param location where the plugin write data.
     * @param file name of the file.
     * @param parameters the value attached to the plug-in.
     * @param time the date when the plug-in was opened.
     */
    void open(utils::ContextPtr ctx,
              const std::string& name,
              const std::string& pluginname,
              const std::string& package,
              const std::string& location,
              const std::string& file,
              Time time,
              std::unique_ptr<value::Value> parameters);

    /**
     * Add new observable (\e Dynamics*, \e portname) into the View.
     *
     */
    void addObservable(Dynamics* model,
                       const std::string& portName,
                       Time currenttime);

    void run(Time current);

    void run(const Dynamics* dynamics, Time current, const std::string& port);

    void run(const Dynamics* dynamics,
             Time current,
             const std::string& port,
             std::unique_ptr<value::Value> value);

    /**
     * Delete an observable for a specified Dynamics. If model does not
     * exist, nothing is produce otherwise, the stream receives a message
     * that a observable is dead.
     * @param model delete observable attached to the specified
     * Dynamics.
     */
    void removeObservable(Dynamics* model);

    /**
     * @brief Test if a dynamics is already connected with the same port
     * to the View.
     * @param dynamics the dynamics to observe.
     * @param portname the port of the dynamics to observe.
     * @return true if dynamics is already connected with the same port.
     */
    bool exist(Dynamics* dynamics, const std::string& portname) const;

    /**
     * @brief Test if a dynamics is already connected to the View.
     * @param dynamics the dynamics to search.
     * @return true if dynamics is already connected.
     */
    bool exist(Dynamics* dynamics) const;

    /**
     * Return a pointer to the \c value::Matrix.
     *
     * If the plug-in does not manage \c value::Matrix, this function
     * returns NULL otherwise, this function return the \c
     * value::Matrix provided as a clone by the plugin.
     *
     */
    std::unique_ptr<value::Matrix> matrix() const;

    /**
     * Retrieves the name of this \e View.
     *
     * @return A name
     */
    std::string name() const
    {
        return m_name;
    }

    /**
     * Return a pointer to the \c value::Matrix managed by te plugin.
     *
     * @param current, the current time (finish time)
     */
    std::unique_ptr<value::Matrix> finish(Time current);

protected:
    using ObservableList = std::multimap<Dynamics*, std::string>;

    ObservableList m_observableList;
    std::string m_name;
    oov::PluginPtr m_plugin;
};
}
} // namespace vle devs

#endif
