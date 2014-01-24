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


#ifndef VLE_VALUE_TABLE_HPP
#define VLE_VALUE_TABLE_HPP 1

#include <vle/value/Value.hpp>
#include <vle/DllDefines.hpp>
#include <boost/multi_array.hpp>

namespace vle { namespace value {

/**
 * @brief Define an array of two dimensions of real.
 */
typedef boost::multi_array < double, 2 > TableValue;

/**
 * @brief A table is a container for double value into an
 * boost::multi_array < double, 2 >. The XML format is:
 */
class VLE_API Table : public Value
{
public:
    typedef TableValue::index index;
    typedef TableValue::size_type size_type;
    typedef TableValue::iterator iterator;
    typedef TableValue::const_iterator const_iterator;

    /**
     * @brief Build a Table object with a default value to empty TableValue.
     */
    Table()
        : m_width(0), m_height(0)
    {}

    /**
     * @brief Build a Table object wi a specified size.
     *
     * @param width The number of columns.
     * @param height The number of rows.
     */
    Table(const index& width, const index& height)
        : m_value(boost::extents[width][height]), m_width(width),
        m_height(height)
    {}

    /**
     * @brief Copy constructor.
     *
     * @param value The value to copy.
     */
    Table(const Table& value)
        : Value(value), m_value(value.m_value), m_width(value.m_width),
        m_height(value.m_height)
    {}

    /**
     * @brief Nothing to delete.
     */
    virtual ~Table()
    {}

    ///
    ////
    ///

    /**
     * @brief Build a Table.
     *
     * @return A new empty Table.
     */
    static Table* create()
    { return new Table(); }

    /**
     * @brief Build a Table with a specified size.
     *
     * @param width The width of the TableValue.
     * @param height The height of the TableValue.
     *
     * @return A new Table with the specified size, all real are initialized
     * with 0.0.
     */
    static Table* create(const index& width, const index& height)
    { return new Table(width, height); }

    ///
    ////
    ///

    /**
     * @brief Clone the current Table with the same TableValue datas.
     * @return A new Table.
     */
    virtual Value* clone() const
    { return new Table(*this); }

    /**
     * @brief Get the type of this class.
     *
     * @return Value::TABLE.
     */
    virtual Value::type getType() const
    { return Value::TABLE; }

    /**
     * @brief Push all real from the TableValue separated by space for columns
     * and end line for line.
     * @code
     * 1 2 3 4
     * 5 6 7 8
     * @endcode
     *
     * @param out The output stream.
     */
    virtual void writeFile(std::ostream& out) const;

    /**
     * @brief Push all real from the TableValue separated by colon for
     * columns and parenthesis for line.
     *
     * @code
     * ((1,2,3,4),(5,6,7,8))
     * @endcode
     *
     * @param out The output stream.
     */
    virtual void writeString(std::ostream& out) const;

    /**
     * @brief Push all real from the TableValue into an XML representation.
     * @code
     * <table width="4" height="2">
     * 1 2 3 4 5 6 7 8
     * </table>
     * @endcode
     *
     * @param out The output stream.
     */
    virtual void writeXml(std::ostream& out) const;

    ///
    ////
    ///

    /**
     * @brief Get a reference to the TableValue.
     *
     * @return A reference to the TableValue.
     */
    inline TableValue& value()
    { return m_value; }

    /**
     * @brief Get a constant reference to the TableValue.
     *
     * @return A constant reference to the TableValue.
     */
    inline const TableValue& value() const
    { return m_value; }

    /**
     * @brief Check if the TableValue is empty.
     *
     * @return True if TableValue is empty, false otherwise.
     */
    inline bool empty() const
    { return m_value.empty(); }

    /**
     * @brief Get the width of the TableValue.
     *
     * @return The width.
     */
    inline const index& width() const
    { return m_width; }

    /**
     * @brief Get the height of the TableValue.
     *
     * @return The height.
     */
    inline const index& height() const
    { return m_height; }

    /**
     * @brief Resize the Table with a new size. Be carrefull, data may be
     * deleted.
     *
     * @param width The width of the TableValue.
     * @param height The height of the TableValue.
     */
    inline void resize(const index& width, const index& height)
    {
        m_value.resize((boost::extents[width][height]));
        m_width = width;
        m_height = height;
    }

    /**
     * @brief get a constant reference to the value at the specified index.
     *
     * @param x the index of the column.
     * @param y the index of the row.
     *
     * @return a constant reference to the real.
     */
    inline const double& get(const index& x, const index& y) const
    { return m_value[x][y]; }

    /**
     * @brief get a reference to the value at the specified index.
     *
     * @param x the index of the column.
     * @param y the index of the row.
     *
     * @return a reference to the real.
     */
    inline double& get(const index& x, const index& y)
    { return m_value[x][y]; }

    /**
     * @brief Fill the current table with multiple reals read from a string.
     * The number of real must be equal to width x height.
     *
     * @param str A string with [0.. x] real.
     *
     * @throw utils::ArgError if string have problem like bad number of
     * real.
     */
    void fill(const std::string& str);

private:
    TableValue      m_value;
    index           m_width;
    index           m_height;
};

inline const Table& toTableValue(const Value& value)
{ return value.toTable(); }

inline const Table* toTableValue(const Value* value)
{ return value ? &value->toTable() : 0; }

inline Table& toTableValue(Value& value)
{ return value.toTable(); }

inline Table* toTableValue(Value* value)
{ return value ? &value->toTable() : 0; }

inline const TableValue& toTable(const Value& value)
{ return value.toTable().value(); }

inline TableValue& toTable(Value& value)
{ return value.toTable().value(); }

inline const TableValue& toTable(const Value* value)
{ return value::reference(value).toTable().value(); }

inline TableValue& toTable(Value* value)
{ return value::reference(value).toTable().value(); }

}} // namespace vle value

#endif
