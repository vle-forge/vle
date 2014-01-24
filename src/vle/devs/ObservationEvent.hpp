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


#ifndef VLE_DEVS_OBSERVATION_EVENT_HPP
#define VLE_DEVS_OBSERVATION_EVENT_HPP 1

#include <vle/DllDefines.hpp>
#include <vle/devs/Attribute.hpp>
#include <vle/devs/Time.hpp>
#include <vector>

namespace vle { namespace devs {

class Simulator;

/**
 * @brief State event use to get information from graph::Model using
 * TimedView or EventView.
 *
 */
class VLE_API ObservationEvent
{
public:
    ObservationEvent(const Time& time,
                     Simulator* model,
                     const std::string& viewname,
                     const std::string& portName) :
        m_model(model),
        m_attributes(0),
        m_time(time),
        m_viewName(viewname),
        m_portName(portName)
    {
    }

    ~ObservationEvent()
    {
        delete m_attributes;
    }

    const std::string& getViewName() const
    { return m_viewName; }

    const std::string& getPortName() const
    { return m_portName; }

    bool onPort(std::string const& portName) const
    { return m_portName == portName; }

    /**
     * @return arrived time.
     */
    const Time& getTime() const
    { return m_time; }

    void putAttributes(const value::Map& map);

    /**
     * Put an attribute on this Event.
     * @param name std::string name of Value to add.
     * @param value Value to add, not clone.
     */
    void putAttribute(const std::string& name, value::Value* value)
    { attributes().add(name, value); }

    /**
     * Put an attribute on an event. The goal is to simplify building event.
     * @code
     * ObservationEvent* evt = new ObservationEvent(
     *                              "free?", currenttime, getModel();
     * evt << attribute("x", 5) << attribute("y", 7.0)
     *     << attribute("msg", "hello world");
     * @endcode
     * @param event the event to put the attribute.
     * @param attr the attribute to put into event.
     * @return the current event.
     */
    friend ObservationEvent* operator<<(ObservationEvent* event, const Attribute& attr)
    { event->putAttribute(attr.first, attr.second); return event;}

    /**
     * Put an attribute on an event. The goal is to simplify building event.
     * @code
     * ObservationEvent evt("free?", currenttime, getModel();
     * evt << attribute("x", 5) << attribute("y", 7.0)
     *     << attribute("msg", "hello world");
     * @endcode
     * @param event the event to put the attribute.
     * @param attr the attribute to put into event.
     * @return the current event.
     */
    friend ObservationEvent& operator<<(ObservationEvent& event, const Attribute& attr)
    { event.putAttribute(attr.first, attr.second); return event;}

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Test if the map have a Value with specified name.
     * @param name the name of value to find.
     * @return true if Value exist, false otherwise.
     */
    bool existAttributeValue(const std::string& name) const
    { return haveAttributes() ? attributes().exist(name) : false; }

    /**
     * Get an attribute from this Event.
     * @param name std::string name of Value to get.
     * @return a reference to Value.
     */
    const value::Value& getAttributeValue(
        const std::string& name) const
    { return value::reference(attributes().get(name)); }

    /**
     * Get a double attribute from this Event.
     * @param name std::string name of double to get.
     * @return a double.
     */
    double getDoubleAttributeValue(const std::string& name) const
    { return attributes().getDouble(name); }

    /**
     * Get an integer attribute from this Event.
     * @param name std::string name of integer to get.
     * @return an integer.
     */
    int32_t getIntegerAttributeValue(const std::string& name) const
    { return attributes().getInt(name); }

    /**
     * Get a boolean attribute from this Event.
     * @param name std::string name of boolean to get.
     * @return a boolean.
     */
    bool getBooleanAttributeValue(const std::string& name) const
    { return attributes().getBoolean(name); }

    /**
     * Get a string attribute from this Event.
     * @param name std::string name of string to get.
     * @return a string.
     */
    const std::string& getStringAttributeValue(
        const std::string& name) const
    { return attributes().getString(name); }

    /**
     * @brief Get a Set attribute from this event.
     * @param name std::string name of Set to get.
     * @return a Set
     */
    const value::Set& getSetAttributeValue(
        const std::string& name) const
    { return attributes().getSet(name); }

    /**
     * @brief Get a Map attribute from this event.
     * @param name std::string name of Map to get.
     * @return a Map.
     */
    const value::Map& getMapAttributeValue(
        const std::string& name) const
    { return attributes().getMap(name); }

    /**
     * @brief Return the map attached to the event.
     * @return a reference to the attached map.
     */
    const value::Map& getAttributes() const
    { return attributes(); }

    /**
     * @brief Return the map attached to the event.
     * @return a reference to the attached map.
     */
    value::Map& getAttributes()
    { return attributes(); }

    /**
     * @brief Check if attributes is present in the attributes lists.
     * @return True if the attributes lists exists, false otherwise.
     */
    bool haveAttributes() const
    { return m_attributes; }

    value::Map& attributes()
    {
        if (m_attributes == 0) {
            m_attributes = new value::Map();
        }
        return *m_attributes;
    }

    const value::Map& attributes() const
    {
        if (m_attributes == 0) {
            throw utils::ArgError(_("No attribute in this event"));
        }
        return *m_attributes;
    }

private:
    ObservationEvent();
    ObservationEvent(const ObservationEvent& other);
    ObservationEvent& operator=(const ObservationEvent& other);

    Simulator  *m_model;
    value::Map *m_attributes;
    Time        m_time;
    std::string m_viewName;
    std::string m_portName;
};

/**
 * @brief Define a vector pointer of ObservationEvent.
 */
typedef std::vector < ObservationEvent* > ObservationEventList;

}} // namespace vle devs

#endif
