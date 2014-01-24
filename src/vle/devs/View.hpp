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
#include <vle/devs/StreamWriter.hpp>
#include <vle/devs/Time.hpp>
#include <vle/value/Matrix.hpp>
#include <string>
#include <map>

namespace vle { namespace devs {

class Simulator;
class StreamWriter;
class View;

typedef std::multimap < Simulator*, std::string > ObservableList;
typedef std::map < std::string, View* > ViewList;

/**
 * @brief Represent a View on a devs::Simulator and a port name.
 *
 */
class VLE_API View
{
public:
    typedef ObservableList::iterator iterator;
    typedef ObservableList::const_iterator const_iterator;
    typedef ObservableList::size_type size_type;
    typedef ObservableList::value_type value_type;

    View(const std::string& name, StreamWriter* stream)
        : m_name(name), m_stream(stream), m_size(0)
    {}

    virtual ~View();

    void addObservable(Simulator* model,
                       const std::string& portName,
                       const Time& currenttime);

    void finish(const Time& time);

    virtual bool isEvent() const
    { return false; }

    virtual bool isTimed() const
    { return false; }

    virtual bool isFinish() const
    { return false; }

    void run(const Time& current);

    virtual Time getNextTime(const Time& current) const = 0;

    /**
     * Delete an observable for a specified Simulator. If model does not
     * exist, nothing is produce otherwise, the stream receives a message
     * that a observable is dead.
     * @param model delete observable attached to the specified
     * Simulator.
     */
    void removeObservable(Simulator* model);

    /**
     * @brief Test if a simulator is already connected with the same port
     * to the View.
     * @param simulator the simulator to observe.
     * @param portname the port of the simulator to observe.
     * @return true if simulator is already connected with the same port.
     */
    bool exist(Simulator* simulator, const std::string& portname) const;

    /**
     * @brief Test if a simulator is already connected to the View.
     * @param simulator the simulator to search.
     * @return true if simulator is already connected.
     */
    bool exist(Simulator* simulator) const;

    //
    // Get/Set functions
    //

    inline const std::string& getName() const
    { return m_name; }

    inline const ObservableList& getObservableList() const
    { return m_observableList; }

    inline unsigned int getSize() const
    { return m_size; }

    inline StreamWriter * getStream() const
    { return m_stream; }

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
    value::Matrix * matrix() const;

protected:
    ObservableList      m_observableList;
    std::string         m_name;
    StreamWriter*       m_stream;
    size_t              m_size;
};

/**
 * @brief Define a Timed View base on devs::View class. This class
 * build state event with timed clock.
 */
class VLE_API TimedView : public View
{
public:
    TimedView(const std::string& name, StreamWriter* stream,
              const Time& timestep)
        : View(name, stream), mTimestep(timestep)
    {}

    virtual ~TimedView()
    {}

    virtual bool isTimed() const
    { return true; }

    virtual Time getNextTime(const Time& current) const
    {
        return current + mTimestep;
    }

private:
    Time mTimestep;
};

/**
 * @brief Define a Event View base on devs::View class. This class
 * build state event when event are push.
 */
class VLE_API EventView : public View
{
public:
    EventView(const std::string& name, StreamWriter* stream)
        : View(name, stream)
    {}

    virtual ~EventView()
    {}

    virtual bool isEvent() const
    { return true; }

    virtual Time getNextTime(const Time& /*current*/) const
    {
        return infinity;
    }
};

/**
 * @brief Define a Finish view based on devs::View class. This class build
 * state event only at the end of the simulation.
 */
class VLE_API FinishView : public View
{
public:
    FinishView(const std::string& name, StreamWriter* stream)
        : View(name, stream)
    {}

    virtual ~FinishView()
    {}

    virtual bool isFinish() const
    { return true; }

    virtual Time getNextTime(const Time& /*current*/) const
    {
        return infinity;
    }
};

typedef std::map < std::string, FinishView* > FinishViewList;
typedef std::map < std::string, EventView* > EventViewList;
typedef std::map < std::string, TimedView* > TimedViewList;

}} // namespace vle devs

#endif
