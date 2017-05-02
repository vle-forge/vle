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

#ifndef VLE_VALUE_TABLE_HPP
#define VLE_VALUE_TABLE_HPP 1

#include <vector>
#include <vle/DllDefines.hpp>
#include <vle/value/Value.hpp>

namespace vle {
namespace value {

/**
 * @brief Define an array of two dimensions of real.
 */
typedef std::vector<double> TableValue;

/**
 * @brief A table is a container for double value into an
 * boost::multi_array < double, 2 >. The XML format is:
 */
class VLE_API Table : public Value
{
public:
    using index = TableValue::size_type;
    using size_type = TableValue::size_type;
    using iterator = TableValue::iterator;
    using const_iterator = TableValue::const_iterator;

    /**
     * @brief Build a Table object with a default value to empty TableValue.
     */
    Table();

    /**
     * @brief Build a Table object wi a specified size.
     *
     * @param width The number of columns.
     * @param height The number of rows.
     */
    Table(std::size_t width, std::size_t height);

    /**
     * @brief Copy constructor.
     *
     * @param value The value to copy.
     */
    Table(const Table& value) = default;

    /**
     * @brief Nothing to delete.
     */
    virtual ~Table()
    {
    }

    ///
    ////
    ///

    /**
     * @brief Build a Table.
     *
     * @return A new empty Table.
     */
    static std::unique_ptr<value::Value> create()
    {
        return std::unique_ptr<value::Value>(new Table());
    }

    /**
     * @brief Build a Table with a specified size.
     *
     * @param width The width of the TableValue.
     * @param height The height of the TableValue.
     *
     * @return A new Table with the specified size, all real are initialized
     * with 0.0.
     */
    static std::unique_ptr<value::Value> create(index width, index height)
    {
        return std::unique_ptr<value::Value>(new Table(width, height));
    }

    ///
    ////
    ///

    /**
     * @brief Clone the current Table with the same TableValue datas.
     * @return A new Table.
     */
    virtual std::unique_ptr<Value> clone() const override
    {
        return std::unique_ptr<Value>(new Table(*this));
    }

    /**
     * @brief Get the type of this class.
     *
     * @return Value::TABLE.
     */
    virtual Value::type getType() const override;

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
    virtual void writeFile(std::ostream& out) const override;

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
    virtual void writeString(std::ostream& out) const override;

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
    virtual void writeXml(std::ostream& out) const override;

    ///
    ////
    ///

    /**
     * @brief Get a reference to the TableValue.
     *
     * @return A reference to the TableValue.
     */
    inline TableValue& value()
    {
        return m_value;
    }

    /**
     * @brief Get a constant reference to the TableValue.
     *
     * @return A constant reference to the TableValue.
     */
    inline const TableValue& value() const
    {
        return m_value;
    }

    /**
     * @brief Check if the TableValue is empty.
     *
     * @return True if TableValue is empty, false otherwise.
     */
    inline bool empty() const
    {
        return m_value.empty();
    }

    /**
     * @brief Get the width of the TableValue.
     *
     * @return The width.
     */
    inline std::size_t width() const
    {
        return m_width;
    }

    /**
     * @brief Get the height of the TableValue.
     *
     * @return The height.
     */
    inline std::size_t height() const
    {
        return m_height;
    }

    /**
     * @brief Resize the Table with a new size. Be carrefull, data may be
     * deleted.
     *
     * @param width The width of the TableValue.
     * @param height The height of the TableValue.
     */
    void resize(std::size_t width, std::size_t height);

    /**
     * @brief get a constant reference to the value at the specified index.
     *
     * @param x the index of the column.
     * @param y the index of the row.
     *
     * @return a constant reference to the real.
     */
    double operator()(std::size_t x, std::size_t y) const;

    /**
     * @brief get a reference to the value at the specified index.
     *
     * @param x the index of the column.
     * @param y the index of the row.
     *
     * @return a reference to the real.
     */
    double& operator()(std::size_t x, std::size_t y);

    /**
     * @brief get a constant reference to the value at the specified index.
     *
     * @param x the index of the column.
     * @param y the index of the row.
     *
     * @return a constant reference to the real.
     */
    double get(std::size_t x, std::size_t y) const;

    /**
     * @brief get a reference to the value at the specified index.
     *
     * @param x the index of the column.
     * @param y the index of the row.
     *
     * @return a reference to the real.
     */
    double& get(std::size_t x, std::size_t y);

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
    TableValue m_value;
    index m_width;
    index m_height;
};

inline const Table&
toTableValue(std::shared_ptr<Value> value)
{
    return value::reference(value).toTable();
}

inline const Table&
toTableValue(std::shared_ptr<const Value> value)
{
    return value::reference(value).toTable();
}

inline const Table&
toTableValue(const std::unique_ptr<Value>& value)
{
    return value::reference(value).toTable();
}

inline const Table&
toTableValue(const Value& value)
{
    return value.toTable();
}

inline Table&
toTableValue(Value& value)
{
    return value.toTable();
}

inline const TableValue&
toTable(std::shared_ptr<Value> value)
{
    return value::reference(value).toTable().value();
}

inline const TableValue&
toTable(std::shared_ptr<const Value> value)
{
    return value::reference(value).toTable().value();
}

inline const TableValue&
toTable(const std::unique_ptr<Value>& value)
{
    return value::reference(value).toTable().value();
}

inline const TableValue&
toTable(const Value& value)
{
    return value.toTable().value();
}

inline TableValue&
toTable(Value& value)
{
    return value.toTable().value();
}
}
} // namespace vle value

#endif
