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


#ifndef VLE_DEVS_VIEW_HPP
#define VLE_DEVS_VIEW_HPP 1

#include <vle/DllDefines.hpp>
#include <vle/devs/Time.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/devs/StreamWriter.hpp>
#include <string>
#include <map>

namespace vle { namespace devs {

class Dynamics;
class View;

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
     * Initialize the View: a name and a stream writer.
     *
     */
    void open(const std::string& name, std::unique_ptr<StreamWriter> stream);

    void addObservable(Dynamics* model,
                       const std::string& portName,
                       const Time& currenttime);

    void run(const Time& current);

    void run(const Dynamics *dynamics, Time current, const std::string& port);

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
     * value::Matrix manager by the plug-in.
     *
     * @attention You are in charge of freeing the value::Matrix after
     * the end of the simulation.
     */
    std::unique_ptr<value::Matrix> matrix() const;

protected:
    using ObservableList = std::multimap<Dynamics*, std::string>;

    ObservableList                m_observableList;
    std::string                   m_name;
    std::unique_ptr<StreamWriter> m_stream;
};

}} // namespace vle devs

#endif
