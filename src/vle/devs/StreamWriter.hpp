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


#ifndef VLE_DEVS_STREAM_HPP
#define VLE_DEVS_STREAM_HPP

#include <vle/DllDefines.hpp>
#include <vle/devs/Time.hpp>
#include <vle/value/Value.hpp>
#include <vle/oov/Plugin.hpp>
#include <vle/utils/Context.hpp>

namespace vle { namespace devs {

class Observable;

class VLE_LOCAL StreamWriter
{
public:
    StreamWriter(utils::ContextPtr ctx)
        : m_context(ctx)
    {}

    ~StreamWriter() = default;
    StreamWriter(const StreamWriter& rhs) = delete;
    StreamWriter& operator=(const StreamWriter& rhs) = delete;

    ///
    ////
    ///

    /**
     * @brief Initialise plugin with specified information.
     * @param plugin the plugin's name.
     * @param package the plugin's package.
     * @param location where the plugin write data.
     * @param file name of the file.
     * @param parameters the value attached to the plug-in.
     * @param time the date when the plug-in was opened.
     */
    void open(const std::string& plugin,
              const std::string& package,
              const std::string& location,
              const std::string& file,
              std::unique_ptr<value::Value> parameters,
              const devs::Time& time);

    void processNewObservable(const std::string& name,
                              const std::string& parent,
                              const std::string& portname,
                              const devs::Time& time,
                              const std::string& view);

    void processRemoveObservable(const std::string& name,
                                 const std::string& parent,
                                 const std::string& portname,
                                 const devs::Time& time,
                                 const std::string& view);

    /**
     * @brief Process the devs::ObservationEvent and write it to the
     * Stream.
     * @param event the devs::ObservationEvent to write.
     */
    void process(const std::string& name,
                 const std::string& parent,
                 const std::string& portname,
                 const devs::Time& time,
                 const std::string& view,
                 std::unique_ptr<value::Value> value);

    /**
     * Close the output stream.
     *
     * @return A reference to the oov::Plugin if the plugin is
     * serializable.
     */
    void close(const devs::Time& time);

    /**
     * Return a pointer to the \c value::Matrix.
     *
     * If the plug-in does not manage \c value::Matrix, this function
     * returns NULL otherwise, this function return the \c
     * value::Matrix manager by the plug-in.
     *
     * @attention You are in charge of freeing the value::Matrix after
     * the end of the simulation.
     */
    std::unique_ptr<value::Matrix> matrix() const;

    ///
    ////
    ///

    const oov::PluginPtr& plugin() const;

private:
    utils::ContextPtr m_context;
    oov::PluginPtr    m_plugin;
};

}} // Namespace vle devs

#endif
