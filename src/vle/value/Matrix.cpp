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


#include <vle/value/Matrix.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Map.hpp>
#include <cassert>

namespace vle { namespace value {

Matrix::Matrix()
    : m_matrix(256 * 1024), m_nbcol(0), m_nbrow(0),
      m_nbcolmax(256), m_nbrowmax(1024),
      m_stepcol(1), m_steprow(1),
      m_lastX(0), m_lastY(0)
{
}

Matrix::Matrix(index columns, index rows, index resizeColumns, index resizeRows)
    : m_matrix(columns * rows), m_nbcol(columns), m_nbrow(rows),
      m_nbcolmax(columns), m_nbrowmax(rows),
      m_stepcol(resizeColumns), m_steprow(resizeRows), m_lastX(0), m_lastY(0)
{
    if (columns * rows <= 0)
        throw utils::ArgError(
            fmt(_("Matrix: bad constructor initialization %1%x%2%"))
            % columns % rows);
}

Matrix::Matrix(index columns, index rows, index columnmax, index rowmax, index
               resizeColumns, index resizeRows)
    : m_matrix(columnmax * rowmax), m_nbcol(columns), m_nbrow(rows),
      m_nbcolmax(columnmax), m_nbrowmax(rowmax),
      m_stepcol(resizeColumns), m_steprow(resizeRows), m_lastX(0), m_lastY(0)
{
    if (columnmax * rowmax <= 0)
        throw utils::ArgError(
            fmt(_("Matrix: bad constructor initialization %1%x%2%"))
            % columns % rows);

    if (columns > columnmax)
        throw utils::ArgError(
            fmt(_("Matrix: Number of columns error: %1% on %2%"))
            % columns % columnmax);

    if (rows > rowmax)
        throw utils::ArgError(
            fmt(_("Matrix: Number of row error: %1% on %2%")) % rows % rowmax);
}

Matrix::Matrix(const Matrix& m)
    : Value(m), m_nbcol(m.m_nbcol), m_nbrow(m.m_nbrow),
      m_nbcolmax(m.m_nbcolmax), m_nbrowmax(m.m_nbrowmax),
      m_stepcol(m.m_stepcol), m_steprow(m.m_steprow),
      m_lastX(0), m_lastY(0)
{
    assert(m.matrix.size() == m_nbcolmax * m_nbrowmax);

    m_matrix.reserve(m.m_matrix.size());

    for (const auto & elem : m.matrix())
        m_matrix.emplace_back(elem.get() ? elem->clone() : nullptr);
}

void Matrix::writeFile(std::ostream& out) const
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

void Matrix::writeString(std::ostream& out) const
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

void Matrix::writeXml(std::ostream& out) const
{
    out << "<matrix "
        << "rows=\"" << m_nbrow  << "\" "
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

void Matrix::clear()
{
    for (auto & elem : m_matrix)
        elem.reset(nullptr);

    m_lastX = 0;
    m_lastY = 0;
}

Set& Matrix::addSet(index column, index row)
{
    return pp_add<Set>(column, row);
}

Map& Matrix::addMap(index column, index row)
{
    return pp_add<Map>(column, row);
}

Matrix& Matrix::addMatrix(index column, index row)
{
    return pp_add<Matrix>(column, row);
}

Set& Matrix::getSet(index column, index row)
{
    return pp_get_value(column, row).toSet();
}

Map& Matrix::getMap(index column, index row)
{
    return pp_get_value(column, row).toMap();
}

Matrix& Matrix::getMatrix(index column, index row)
{
    return pp_get_value(column, row).toMatrix();
}

const Set& Matrix::getSet(index column, index row) const
{
    return pp_get_value(column, row).toSet();
}

const Map& Matrix::getMap(index column, index row) const
{
    return pp_get_value(column, row).toMap();
}

const Matrix& Matrix::getMatrix(index column, index row) const
{
    return pp_get_value(column, row).toMatrix();
}

void Matrix::reserve(size_type columnmax, size_type rowmax)
{
    if (columnmax * rowmax <= 0)
        throw utils::ArgError(
            fmt(_("Matrix: bad reserve operation %1%x%2%"))
            % columnmax % rowmax);

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

void Matrix::resize(size_type columns, size_type rows)
{
    if (columns * rows <= 0)
        throw utils::ArgError(
            fmt(_("Matrix: bad resize operation %1%x%2%"))
            % columns % rows);

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

void Matrix::resize(size_type columns, size_type rows,
                    std::unique_ptr<Value> value)
{
    if (columns * rows <= 0)
        throw utils::ArgError(
            fmt(_("Matrix: bad resize operation %1%x%2%"))
            % columns % rows);

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

void Matrix::increaseAllocation(size_type columns, size_type rows)
{
    resize(std::max(m_nbcol, columns), std::max(m_nbrow, rows));
}

void Matrix::addColumn()
{
    resize(m_nbcol + 1, m_nbrow);
}

void Matrix::addRow()
{
    resize(m_nbcol, m_nbrow + 1);
}

void Matrix::moveLastCell()
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

}} // namespace vle value
