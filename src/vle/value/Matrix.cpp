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

#include <cassert>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/value/Null.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Table.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/XML.hpp>

namespace {

inline void
pp_check_index(const vle::value::Matrix& m,
               vle::value::Matrix::index column,
               vle::value::Matrix::index row)
{
#ifndef VLE_FULL_OPTIMIZATION
    if (not(column < m.columns() and row <= m.rows()))
        throw vle::utils::ArgError(
          (vle::fmt(_("Matrix: bad access %1% %2% for %3%x%4% matrix")) %
           column % row % m.columns() % m.rows())
            .str());
#else
    (void)m;
    (void)column;
    (void)row;
#endif
}

inline vle::value::Value&
pp_get_value(vle::value::Matrix& m,
             vle::value::Matrix::index column,
             vle::value::Matrix::index row)
{
    pp_check_index(m, column, row);

    auto pointer = m.value()[row * m.columns_max() + column].get();
    if (not pointer)
        throw vle::utils::ArgError(
          (vle::fmt(_("Matrix: empty or null value at %1% %2% for %3%x%4% "
                      "matrix")) %
           column % row % m.columns() % m.rows())
            .str());

    return *pointer;
}

inline const vle::value::Value&
pp_get_value(const vle::value::Matrix& m,
             vle::value::Matrix::index column,
             vle::value::Matrix::index row)
{
    pp_check_index(m, column, row);

    auto pointer = m.value()[row * m.columns_max() + column].get();
    if (not pointer)
        throw vle::utils::ArgError(
          (vle::fmt(_("Matrix: empty or null value at %1% %2% for %3%x%4% "
                      "matrix")) %
           column % row % m.columns() % m.rows())
            .str());

    return *pointer;
}

template <typename T, typename... Args>
T&
pp_add(vle::value::Matrix& m,
       vle::value::Matrix::index column,
       vle::value::Matrix::index row,
       Args&&... args)
{
    auto value =
      std::unique_ptr<vle::value::Value>(new T(std::forward<Args>(args)...));
    auto* ret = static_cast<T*>(value.get());

    pp_check_index(m, column, row);
    m.value()[row * m.columns_max() + column] = std::move(value);

    return *ret;
}
}

namespace vle {
namespace value {

Matrix::Matrix()
  : m_matrix(256 * 1024)
  , m_nbcol(0)
  , m_nbrow(0)
  , m_nbcolmax(256)
  , m_nbrowmax(1024)
  , m_stepcol(1)
  , m_steprow(1)
  , m_lastX(0)
  , m_lastY(0)
{
}

Matrix::Matrix(index columns,
               index rows,
               index resizeColumns,
               index resizeRows)
  : m_matrix(columns * rows)
  , m_nbcol(columns)
  , m_nbrow(rows)
  , m_nbcolmax(columns)
  , m_nbrowmax(rows)
  , m_stepcol(resizeColumns)
  , m_steprow(resizeRows)
  , m_lastX(0)
  , m_lastY(0)
{
}

Matrix::Matrix(index columns,
               index rows,
               index columnmax,
               index rowmax,
               index resizeColumns,
               index resizeRows)
  : m_matrix(columnmax * rowmax)
  , m_nbcol(columns)
  , m_nbrow(rows)
  , m_nbcolmax(columnmax)
  , m_nbrowmax(rowmax)
  , m_stepcol(resizeColumns)
  , m_steprow(resizeRows)
  , m_lastX(0)
  , m_lastY(0)
{
    if (columnmax * rowmax <= 0)
        throw utils::ArgError(
          (fmt(_("Matrix: bad constructor initialization %1%x%2%")) % columns %
           rows)
            .str());

    if (columns > columnmax)
        throw utils::ArgError(
          (fmt(_("Matrix: Number of columns error: %1% on %2%")) % columns %
           columnmax)
            .str());

    if (rows > rowmax)
        throw utils::ArgError(
          (fmt(_("Matrix: Number of row error: %1% on %2%")) % rows % rowmax)
            .str());
}

Value::type
Matrix::getType() const
{
    return Value::MATRIX;
}

Matrix::Matrix(const Matrix& m)
  : Value(m)
  , m_nbcol(m.m_nbcol)
  , m_nbrow(m.m_nbrow)
  , m_nbcolmax(m.m_nbcolmax)
  , m_nbrowmax(m.m_nbrowmax)
  , m_stepcol(m.m_stepcol)
  , m_steprow(m.m_steprow)
  , m_lastX(0)
  , m_lastY(0)
{
    assert(m.m_matrix.size() == m_nbcolmax * m_nbrowmax);

    m_matrix.reserve(m.m_matrix.size());

    for (const auto& elem : m.matrix())
        m_matrix.emplace_back(elem.get() ? elem->clone() : nullptr);
}

void
Matrix::writeFile(std::ostream& out) const
{
    for (size_type r = 0; r < m_nbrow; ++r) {
        for (size_type c = 0; c < m_nbcol; ++c) {
            if (get(c, r)) {
                get(c, r)->writeFile(out);
            } else {
                out << "NA";
            }
            out << " ";
        }
        out << "\n";
    }
}

void
Matrix::writeString(std::ostream& out) const
{
    for (size_type r = 0; r < m_nbrow; ++r) {
        for (size_type c = 0; c < m_nbcol; ++c) {
            if (get(c, r)) {
                get(c, r)->writeString(out);
            } else {
                out << "NA";
            }
            out << " ";
        }
        out << "\n";
    }
}

void
Matrix::writeXml(std::ostream& out) const
{
    out << "<matrix "
        << "rows=\"" << m_nbrow << "\" "
        << "columns=\"" << m_nbcol << "\" "
        << "columnmax=\"" << m_nbcolmax << "\" "
        << "rowmax=\"" << m_nbrowmax << "\" "
        << "columnstep=\"" << m_stepcol << "\" "
        << "rowstep=\"" << m_steprow << "\" >";

    for (size_type r = 0; r < m_nbrow; ++r) {
        for (size_type c = 0; c < m_nbcol; ++c) {
            if (get(c, r)) {
                get(c, r)->writeXml(out);
            } else {
                out << "<null />";
            }
            out << " ";
        }
        out << "\n";
    }
    out << "</matrix>";
}

void
Matrix::clear()
{
    for (auto& elem : m_matrix)
        elem.reset(nullptr);

    m_lastX = 0;
    m_lastY = 0;
}

Null&
Matrix::addNull(index column, index row)
{
    return ::pp_add<Null>(*this, column, row);
}

Boolean&
Matrix::addBoolean(index column, index row, bool value)
{
    return ::pp_add<Boolean>(*this, column, row, value);
}

bool
Matrix::getBoolean(index column, index row) const
{
    return ::pp_get_value(*this, column, row).toBoolean().value();
}

bool&
Matrix::getBoolean(index column, index row)
{
    return ::pp_get_value(*this, column, row).toBoolean().value();
}

Double&
Matrix::addDouble(index column, index row, double value)
{
    return ::pp_add<Double>(*this, column, row, value);
}

double
Matrix::getDouble(index column, index row) const
{
    return ::pp_get_value(*this, column, row).toDouble().value();
}

double&
Matrix::getDouble(index column, index row)
{
    return ::pp_get_value(*this, column, row).toDouble().value();
}

Integer&
Matrix::addInt(index column, index row, int32_t value)
{
    return ::pp_add<Integer>(*this, column, row, value);
}

int32_t
Matrix::getInt(index column, index row) const
{
    return ::pp_get_value(*this, column, row).toInteger().value();
}

int32_t&
Matrix::getInt(index column, index row)
{
    return ::pp_get_value(*this, column, row).toInteger().value();
}

String&
Matrix::addString(index column, index row, const std::string& value)
{
    return ::pp_add<String>(*this, column, row, value);
}

const std::string&
Matrix::getString(index column, index row) const
{
    return ::pp_get_value(*this, column, row).toString().value();
}

std::string&
Matrix::getString(index column, index row)
{
    return ::pp_get_value(*this, column, row).toString().value();
}

Xml&
Matrix::addXml(index column, index row, const std::string& value)
{
    return ::pp_add<Xml>(*this, column, row, value);
}

const std::string&
Matrix::getXml(index column, index row) const
{
    return ::pp_get_value(*this, column, row).toXml().value();
}

std::string&
Matrix::getXml(index column, index row)
{
    return ::pp_get_value(*this, column, row).toXml().value();
}

Tuple&
Matrix::addTuple(index column, index row, std::size_t width, double value)
{
    return ::pp_add<Tuple>(*this, column, row, width, value);
}

const Tuple&
Matrix::getTuple(index column, index row) const
{
    return ::pp_get_value(*this, column, row).toTuple();
}

Tuple&
Matrix::getTuple(index column, index row)
{
    return ::pp_get_value(*this, column, row).toTuple();
}

Table&
Matrix::addTable(index column,
                 index row,
                 std::size_t width,
                 std::size_t height)
{
    return ::pp_add<Table>(*this, column, row, width, height);
}

const Table&
Matrix::getTable(index column, index row) const
{
    return ::pp_get_value(*this, column, row).toTable();
}

Table&
Matrix::getTable(index column, index row)
{
    return ::pp_get_value(*this, column, row).toTable();
}

Set&
Matrix::addSet(index column, index row)
{
    return ::pp_add<Set>(*this, column, row);
}

Map&
Matrix::addMap(index column, index row)
{
    return ::pp_add<Map>(*this, column, row);
}

Matrix&
Matrix::addMatrix(index column, index row)
{
    return ::pp_add<Matrix>(*this, column, row);
}

Set&
Matrix::getSet(index column, index row)
{
    return ::pp_get_value(*this, column, row).toSet();
}

Map&
Matrix::getMap(index column, index row)
{
    return ::pp_get_value(*this, column, row).toMap();
}

Matrix&
Matrix::getMatrix(index column, index row)
{
    return ::pp_get_value(*this, column, row).toMatrix();
}

const Set&
Matrix::getSet(index column, index row) const
{
    return ::pp_get_value(*this, column, row).toSet();
}

const Map&
Matrix::getMap(index column, index row) const
{
    return ::pp_get_value(*this, column, row).toMap();
}

const Matrix&
Matrix::getMatrix(index column, index row) const
{
    return ::pp_get_value(*this, column, row).toMatrix();
}

void
Matrix::reserve(size_type columnmax, size_type rowmax)
{
    if (columnmax * rowmax <= 0)
        throw utils::ArgError(
          (fmt(_("Matrix: bad reserve operation %1%x%2%")) % columnmax %
           rowmax)
            .str());

    if (columnmax <= m_nbcolmax and rowmax <= m_nbrowmax)
        return;

    MatrixValue tmp(columnmax * rowmax);

    for (std::size_t r = 0; r < m_nbrow; ++r)
        for (std::size_t c = 0; c < m_nbcol; ++c)
            tmp[r * columnmax + c] = std::move(m_matrix[r * m_nbcolmax + c]);

    std::swap(tmp, m_matrix);

    m_nbcolmax = columnmax;
    m_nbrowmax = rowmax;
}

void
Matrix::resize(size_type columns, size_type rows)
{
    if (columns * rows <= 0) {
        m_nbcol = columns;
        m_nbrow = rows;
        return;
    }

    if (columns >= m_nbcolmax or rows >= m_nbrowmax)
        reserve(columns + m_stepcol, rows + m_steprow);

    // No reallocation necessary, just move the m_nbcol and m_nbrow values
    // with columns and rows parameters.

    auto range_r = std::minmax(m_nbrow, rows);
    auto range_c = std::minmax(m_nbcol, columns);

    for (std::size_t r = range_r.first; r < range_r.second; ++r)
        for (std::size_t c = 0; c < range_c.second; ++c)
            m_matrix[r * m_nbcolmax + c].reset(nullptr);

    for (std::size_t r = 0; r < range_r.second; ++r)
        for (std::size_t c = range_c.first; c < range_c.second; ++c)
            m_matrix[r * m_nbcolmax + c].reset(nullptr);

    m_nbcol = columns;
    m_nbrow = rows;
}

void
Matrix::resize(size_type columns,
               size_type rows,
               const std::unique_ptr<Value>& value)
{
    if (columns * rows <= 0) {
        m_nbcol = columns;
        m_nbrow = rows;
        return;
    }

    if (columns >= m_nbcolmax or rows >= m_nbrowmax)
        reserve(columns + m_stepcol, rows + m_steprow);

    // No reallocation necessary, just move the m_nbcol and m_nbrow values
    // with columns and rows parameters.

    auto range_r = std::minmax(m_nbrow, rows);
    auto range_c = std::minmax(m_nbcol, columns);

    for (std::size_t r = range_r.first; r <= range_r.second; ++r)
        for (std::size_t c = 0; c <= range_c.second; ++c)
            m_matrix[r * m_nbcolmax + c] = value->clone();

    for (std::size_t r = 0; r <= range_r.second; ++r)
        for (std::size_t c = range_c.first; c <= range_c.second; ++c)
            m_matrix[r * m_nbcolmax + c] = value->clone();

    m_nbcol = columns;
    m_nbrow = rows;
}

void
Matrix::increaseAllocation(size_type columns, size_type rows)
{
    resize(std::max(m_nbcol, columns), std::max(m_nbrow, rows));
}

void
Matrix::addColumn()
{
    resize(m_nbcol + 1, m_nbrow);
}

void
Matrix::addRow()
{
    resize(m_nbcol, m_nbrow + 1);
}

void
Matrix::moveLastCell()
{
    ++m_lastX;
    if (m_lastX >= static_cast<index>(columns())) {
        m_lastX = 0;
        ++m_lastY;
        if (m_lastY >= static_cast<index>(rows())) {
            m_lastY = 0;
        }
    }
}

void
Matrix::add(index column, index row, std::unique_ptr<Value> val)
{
    ::pp_check_index(*this, column, row);

    m_matrix[row * m_nbcolmax + column] = std::move(val);
}

void
Matrix::set(index column, index row, std::unique_ptr<Value> val)
{
    ::pp_check_index(*this, column, row);

    m_matrix[row * m_nbcolmax + column] = std::move(val);
}

const std::unique_ptr<Value>&
Matrix::get(index column, index row) const
{
    ::pp_check_index(*this, column, row);
    return m_matrix[row * m_nbcolmax + column];
}

std::unique_ptr<Value>
Matrix::give(index column, index row)
{
    ::pp_check_index(*this, column, row);
    return std::move(m_matrix[row * m_nbcolmax + column]);
}

const std::unique_ptr<Value>&
Matrix::operator()(index column, index row) const
{
    ::pp_check_index(*this, column, row);
    return m_matrix[row * m_nbcolmax + column];
}

void
Matrix::addToLastCell(std::unique_ptr<Value> val)
{
    m_matrix[m_lastY * m_nbcolmax + m_lastX] = std::move(val);
}
}
} // namespace vle value
