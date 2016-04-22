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
#include <vle/value/Map.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Deprecated.hpp>
#include <memory>
#include <string>

namespace vle { namespace devs {

class Simulator;

/**
 * \e ExternalEvent are simple object to store {Simulator, Port, Value}
 * for event build in \e vle::devs::Dynamics::output() function. The same
 * object is use into the \e vle::devs::Dynamics::externalTransition()
 * function.
 *
 */
class VLE_API ExternalEvent
{
public:
    ExternalEvent() = delete;
    ExternalEvent(const ExternalEvent& other) = delete;
    ExternalEvent& operator=(const ExternalEvent& other) = delete;

    ExternalEvent(const std::string& port)
        : m_target(0)
        , m_attributes(0)
        , m_port(port)
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

    ~ExternalEvent() = default;

    const std::string& getPortName() const
    {
        return m_port;
    }

    Simulator* getTarget()
    { return m_target; }

    bool onPort(const std::string& port) const
    { return m_port == port; }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * Return the map attached to the event.
     *
     * \return a reference to the attached map.
     *
     * \exception can throw \e utils::ArgError if \e attributes() is empty
     * or if \e attributes() is not a \e value::Boolean.
     */
    const value::Boolean& getBoolean() const;

    /**
     * Return the map attached to the event.
     *
     * \return a reference to the attached map.
     *
     * \exception can throw \e utils::ArgError if \e attributes() is empty
     * or if \e attributes() is not a \e value::Boolean.
     */
    value::Boolean& getBoolean();

        /**
     * Return the map attached to the event.
     *
     * \return a reference to the attached map.
     *
     * \exception can throw \e utils::ArgError if \e attributes() is empty
     * or if \e attributes() is not a \e value::Double.
     */
    const value::Double& getDouble() const;

    /**
     * Return the map attached to the event.
     *
     * \return a reference to the attached map.
     *
     * \exception can throw \e utils::ArgError if \e attributes() is empty
     * or if \e attributes() is not a \e value::Double.
     */
    value::Double& getDouble();

        /**
     * Return the map attached to the event.
     *
     * \return a reference to the attached map.
     *
     * \exception can throw \e utils::ArgError if \e attributes() is empty
     * or if \e attributes() is not a \e value::Integer.
     */
    const value::Integer& getInteger() const;

    /**
     * Return the map attached to the event.
     *
     * \return a reference to the attached map.
     *
     * \exception can throw \e utils::ArgError if \e attributes() is empty
     * or if \e attributes() is not a \e value::Integer.
     */
    value::Integer& getInteger();

        /**
     * Return the map attached to the event.
     *
     * \return a reference to the attached map.
     *
     * \exception can throw \e utils::ArgError if \e attributes() is empty
     * or if \e attributes() is not a \e value::String.
     */
    const value::String& getString() const;

    /**
     * Return the map attached to the event.
     *
     * \return a reference to the attached map.
     *
     * \exception can throw \e utils::ArgError if \e attributes() is empty
     * or if \e attributes() is not a \e value::String.
     */
    value::String& getString();

        /**
     * Return the map attached to the event.
     *
     * \return a reference to the attached map.
     *
     * \exception can throw \e utils::ArgError if \e attributes() is empty
     * or if \e attributes() is not a \e value::Xml.
     */
    const value::Xml& getXml() const;

    /**
     * Return the map attached to the event.
     *
     * \return a reference to the attached map.
     *
     * \exception can throw \e utils::ArgError if \e attributes() is empty
     * or if \e attributes() is not a \e value::Xml.
     */
    value::Xml& getXml();

        /**
     * Return the map attached to the event.
     *
     * \return a reference to the attached map.
     *
     * \exception can throw \e utils::ArgError if \e attributes() is empty
     * or if \e attributes() is not a \e value::Tuple.
     */
    const value::Tuple& getTuple() const;

    /**
     * Return the map attached to the event.
     *
     * \return a reference to the attached map.
     *
     * \exception can throw \e utils::ArgError if \e attributes() is empty
     * or if \e attributes() is not a \e value::Tuple.
     */
    value::Tuple& getTuple();

        /**
     * Return the map attached to the event.
     *
     * \return a reference to the attached map.
     *
     * \exception can throw \e utils::ArgError if \e attributes() is empty
     * or if \e attributes() is not a \e value::Table.
     */
    const value::Table& getTable() const;

    /**
     * Return the map attached to the event.
     *
     * \return a reference to the attached map.
     *
     * \exception can throw \e utils::ArgError if \e attributes() is empty
     * or if \e attributes() is not a \e value::Table.
     */
    value::Table& getTable();

        /**
     * Return the map attached to the event.
     *
     * \return a reference to the attached map.
     *
     * \exception can throw \e utils::ArgError if \e attributes() is empty
     * or if \e attributes() is not a \e value::Set.
     */
    const value::Set& getSet() const;

    /**
     * Return the map attached to the event.
     *
     * \return a reference to the attached map.
     *
     * \exception can throw \e utils::ArgError if \e attributes() is empty
     * or if \e attributes() is not a \e value::Set.
     */
    value::Set& getSet();

    /**
     * Return the map attached to the event.
     *
     * \return a reference to the attached map.
     *
     * \exception can throw \e utils::ArgError if \e attributes() is empty
     * or if \e attributes() is not a \e value::Map.
     */
    const value::Map& getMap() const;

    /**
     * Return the map attached to the event.
     *
     * \return a reference to the attached map.
     *
     * \exception can throw \e utils::ArgError if \e attributes() is empty
     * or if \e attributes() is not a \e value::Map.
     */
    value::Map& getMap();

        /**
     * Return the map attached to the event.
     *
     * \return a reference to the attached map.
     *
     * \exception can throw \e utils::ArgError if \e attributes() is empty
     * or if \e attributes() is not a \e value::Matrix.
     */
    const value::Matrix& getMatrix() const;

    /**
     * Return the map attached to the event.
     *
     * \return a reference to the attached map.
     *
     * \exception can throw \e utils::ArgError if \e attributes() is empty
     * or if \e attributes() is not a \e value::Matrix.
     */
    value::Matrix& getMatrix();

    //
    // Deprecated part.
    //

    /**
     * Return the map attached to the event or throw exception.
     *
     * \return a reference to the attached map.
     *
     * \deprecated
     *
     * \exception can throw \e utils::ArgError if \e attributes() is empty
     * or if \e attributes() is not a \e value::Map.
     */
    const value::Map& getAttributes() const DEPRECATED
    {
        if (m_attributes.get() == nullptr)
            throw utils::ArgError(
                _("ExternalEvent: getAttributes returns null pointer"));

        if (m_attributes->isMap())
            return m_attributes->toMap();

        throw utils::ArgError(
            _("ExternalEvent: getAttributes can not return another"
              " type than value::Map"));
    }

    /**
     * Return the map attached to the event or, if \e attributes() is
     * empty, returns a newly allocated value::Map.
     *
     * \return a reference to the attached map.
     *
     * \deprecated
     *
     * \exception can throw \e utils::ArgError if \e attributes() is not a
     * \e value::Map.
     */
    value::Map& getAttributes() DEPRECATED
    {
        if (m_attributes.get() == nullptr) {
            auto ptr = std::make_shared<value::Map>();
            m_attributes = ptr;
            return *ptr;
        }

        if (m_attributes->isMap())
            return m_attributes->toMap();

        throw utils::ArgError(
            _("ExternalEvent: getAttributes is filled with another "
              "type than value::Map"));
    }

    /**
     * Fill the \e m_attributes with a clone of the map.
     *
     * \deprecated
     */
    void putAttributes(const value::Map& map) DEPRECATED;

    /**
     * Put an attribute to the \e m_attributes if and only if the \e
     * m_attributes is a value::Map or m_attributes equal nullptr.
     *
     * \deprecated
     *
     * \param name std::string name of value::Value to add (the key of the
     * value::Map).
     * \param value Value to add, not clone.
     */
    void putAttribute(const std::string& name,
                      std::unique_ptr<value::Value> value);

    //
    // End of the deprecated part.
    //

    /**
     * Test if the attributes is present and is a value::Map.
     *
     * \return true if m_attributes.get() != nullptr and if
     * m_attribute.isMap() is true.
     */
    bool haveAttributes() const
    {
        return m_attributes.get() != nullptr;
    }

    /**
     * Get direct access to the underlying attributes (value::Value).
     *
     * \return a std::shared_ptr<value::Value> without or without values.
     */
    std::shared_ptr<value::Value>& attributes()
    {
        return m_attributes;
    }

    /**
     * Get direct access to the underlying attributes (value::Value).
     *
     * \return a std::shared_ptr<value::Value> without or without values.
     */
    const std::shared_ptr<value::Value>& attributes() const
    {
        return m_attributes;
    }

private:
    Simulator                      *m_target;
    std::shared_ptr <value::Value>  m_attributes;
    std::string                     m_port;
};

}} // namespace vle devs

#endif
