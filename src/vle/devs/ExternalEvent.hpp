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


#ifndef VLE_DEVS_EXTERNALEVENT_HPP
#define VLE_DEVS_EXTERNALEVENT_HPP

#include <vle/DllDefines.hpp>
#include <vle/devs/Attribute.hpp>
#include <boost/shared_ptr.hpp>
#include <string>

namespace vle { namespace devs {

class Simulator;

/**
 * @brief External event based on the devs::Event class and are build by
 * graph::Model when output function are called.
 *
 */
class VLE_API ExternalEvent
{
public:
    ExternalEvent(const std::string& sourcePortName)
        : m_target(0),
        m_port(sourcePortName)
    {
    }

    ExternalEvent(ExternalEvent& event,
                  Simulator* target,
                  const std::string& targetPortName)
        : m_target(target),
        m_attributes(event.m_attributes),
        m_port(targetPortName)
    {
    }

    ~ExternalEvent()
    {
    }

    const std::string& getPortName() const
    { return m_port; }

    Simulator* getTarget()
    { return m_target; }

    bool onPort(const std::string& portName) const
    { return m_port == portName; }

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
     * ExternalEvent* evt = new ExternalEvent(
     *                              "free?", currenttime, getModel();
     * evt << attribute("x", 5) << attribute("y", 7.0)
     *     << attribute("msg", "hello world");
     * @endcode
     * @param event the event to put the attribute.
     * @param attr the attribute to put into event.
     * @return the current event.
     */
    friend ExternalEvent* operator<<(ExternalEvent* event,
            const Attribute& attr)
    {
        event->putAttribute(attr.first, attr.second);
        return event;
    }

    /**
     * Put an attribute on an event. The goal is to simplify building event.
     * @code
     * ExternalEvent evt("free?", currenttime, getModel();
     * evt << attribute("x", 5) << attribute("y", 7.0)
     *     << attribute("msg", "hello world");
     * @endcode
     * @param event the event to put the attribute.
     * @param attr the attribute to put into event.
     * @return the current event.
     */
    friend ExternalEvent& operator<<(ExternalEvent& event,
            const Attribute& attr)
    {
        event.putAttribute(attr.first, attr.second);
        return event;
    }

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
    { return m_attributes.get(); }

    value::Map& attributes()
    {
        if (m_attributes.get() == 0) {
            m_attributes = boost::shared_ptr < value::Map >(new value::Map());
        }
        return *m_attributes;
    }

    const value::Map& attributes() const
    {
        if (m_attributes.get() == 0) {
            throw utils::ArgError(_("No attribute in this event"));
        }
        return *m_attributes;
    }

private:
    ExternalEvent();
    ExternalEvent(const ExternalEvent& other);
    ExternalEvent& operator=(const ExternalEvent& other);

    Simulator                        *m_target;
    boost::shared_ptr < value::Map >  m_attributes;
    std::string                       m_port;
};

}} // namespace vle devs

#endif
