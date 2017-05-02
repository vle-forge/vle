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

#ifndef VLE_DEVS_EXTERNALEVENT_HPP
#define VLE_DEVS_EXTERNALEVENT_HPP

#include <memory>
#include <string>
#include <vle/DllDefines.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/value/Map.hpp>

namespace vle {
namespace devs {

/**
 * \e ExternalEvent are simple object to store two data \e port and \e value
 * for event build in \e vle::devs::Dynamics::output() function. The same
 * object is use into the \e vle::devs::Dynamics::externalTransition()
 * function.
 *
 */
class VLE_API ExternalEvent
{
public:
    ExternalEvent() = default;
    ExternalEvent(const ExternalEvent& other) = default;
    ExternalEvent& operator=(const ExternalEvent& other) = default;
    ExternalEvent(ExternalEvent&& other) = default;
    ExternalEvent& operator=(ExternalEvent&& other) = default;
    ~ExternalEvent() = default;

    ExternalEvent(const std::string& port)
      : m_port(port)
    {
    }

    ExternalEvent(const std::shared_ptr<value::Value>& attributes,
                  const std::string& port)
      : m_attributes(attributes)
      , m_port(port)
    {
    }

    const std::string& getPortName() const
    {
        return m_port;
    }

    bool onPort(const std::string& port) const
    {
        return m_port == port;
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * Initialize the \e attributes with a Boolean.
     *
     * \param value default value.
     * \return a reference to the newly allocated \e attributes.
     */
    value::Boolean& addBoolean(bool value = true);

    /**
     * Initialize the \e attributes with a Double.
     *
     * \param value default value.
     * \return a reference to the newly allocated \e attributes.
     */
    value::Double& addDouble(double value = 0.0);

    /**
     * Initialize the \e attributes with a Integer.
     *
     * \param value default value.
     * \return a reference to the newly allocated \e attributes.
     */
    value::Integer& addInteger(int32_t value = 0);

    /**
     * Initialize the \e attributes with a String.
     *
     * \param value default value.
     * \return a reference to the newly allocated \e attributes.
     */
    value::String& addString(const std::string& value = std::string());

    /**
     * Initialize the \e attributes with a Xml.
     *
     * \param value default value.
     * \return a reference to the newly allocated \e attributes.
     */
    value::Xml& addXml(const std::string& value = std::string());

    /**
     * Initialize the \e attributes with a Tuple.
     *
     * \param size size of the tuple.
     * \param value default value.
     * \return a reference to the newly allocated \e attributes.
     */
    value::Tuple& addTuple(std::size_t size, double value);

    /**
     * Initialize the \e attributes with a Table.
     *
     * \param width Default width of the Table.
     * \param height Default height of the Table.
     * \return a reference to the newly allocated \e attributes.
     */
    value::Table& addTable(std::size_t width, std::size_t height);

    /**
     * Initialize the \e attributes with a Map.
     *
     * \return a reference to the newly allocated \e attributes.
     */
    value::Map& addMap();

    /**
     * Initialize the \e attributes with a Set.
     *
     * \return a reference to the newly allocated \e attributes.
     */
    value::Set& addSet();

    /**
     * Initialize the \e attributes with a Matrix.
     *
     * \param columns the initial number of columns.
     * \param rows the initial number of rows.
     * \param columnmax The max number of columns.
     * \param rowmax The max number of rows.
     * \param resizeColumns the number of columns to add when resize the
     * matrix.
     * \param resizeRow the number of rows to add when resize the matrix.
     * \return a reference to the newly allocated \e attributes.
     */
    value::Matrix& addMatrix(std::size_t columns,
                             std::size_t rows,
                             std::size_t columnmax,
                             std::size_t rowmax,
                             std::size_t resizeColumns,
                             std::size_t resizeRows);

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
    std::shared_ptr<value::Value> m_attributes;
    std::string m_port;

    template <typename T, typename... Args>
    T& pp_add(Args&&... args)
    {
        auto value = std::make_shared<T>(std::forward<Args>(args)...);
        auto ret = value.get();
        m_attributes = value;
        return *ret;
    }
};
}
} // namespace vle devs

#endif
