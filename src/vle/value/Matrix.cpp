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

namespace vle { namespace value {

Matrix::Matrix(index columns, index rows, index columnmax, index rowmax, index
               resizeColumns, index resizeRows)
    : m_matrix(m_extents[columnmax][rowmax]), m_nbcol(columns), m_nbrow(rows),
    m_stepcol(resizeColumns), m_steprow(resizeRows), m_lastX(0), m_lastY(0)
{
    if (columns > columnmax) {
        throw utils::ArgError(fmt(_(
                "Number of columns error: %1% on %2%")) % columns % columnmax);
    }

    if (rows > rowmax) {
        throw utils::ArgError(fmt(_(
                "Number of row error: %1% on %2%")) % rows % rowmax);
    }
}

Matrix::Matrix(const Matrix& m)
    : Value(m), m_nbcol(m.m_nbcol), m_nbrow(m.m_nbrow), m_stepcol(m.m_stepcol),
    m_steprow(m.m_steprow), m_lastX(0), m_lastY(0)
{
    m_matrix.resize(m_extents[m_nbcol][m_nbrow]);

    for (size_type i = 0; i < m_nbcol; ++i) {
        for (size_type j = 0; j < m_nbrow; ++j) {
            if (m.m_matrix[i][j]) {
                m_matrix[i][j] = m.m_matrix[i][j]->clone();
            } else {
                m_matrix[i][j] = 0;
            }
        }
    }
}

void Matrix::writeFile(std::ostream& out) const
{
    for (size_type j = 0; j < m_nbrow; ++j) {
        for (size_type i = 0; i < m_nbcol; ++i) {
            if (m_matrix[i][j]) {
                m_matrix[i][j]->writeFile(out);
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
    for (size_type j = 0; j < m_nbrow; ++j) {
        for (size_type i = 0; i < m_nbcol; ++i) {
            if (m_matrix[i][j]) {
                m_matrix[i][j]->writeString(out);
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
        << "columnmax=\"" << m_matrix.shape()[0] << "\" "
        << "rowmax=\"" << m_matrix.shape()[1] << "\" "
        << "columnstep=\"" << m_stepcol << "\" "
        << "rowstep=\"" << m_steprow << "\" >";

    for (size_type j = 0; j < m_nbrow; ++j) {
        for (size_type i = 0; i < m_nbcol; ++i) {
            if (m_matrix[i][j]) {
                m_matrix[i][j]->writeXml(out);
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
    for (size_type j = 0; j < m_nbrow; ++j) {
        for (size_type i = 0; i < m_nbcol; ++i) {
            delete m_matrix[i][j];
            m_matrix[i][j] = 0;
        }
    }

    m_lastX = 0;
    m_lastY = 0;
}

Set& Matrix::addSet(const size_type& column, const size_type& row)
{
    value::Set* tmp = new value::Set();
    add(column, row, tmp);
    return *tmp;
}

Map& Matrix::addMap(const size_type& column, const size_type& row)
{
    value::Map* tmp = new value::Map();
    add(column, row, tmp);
    return *tmp;
}

Matrix& Matrix::addMatrix(const size_type& column, const size_type& row)
{
    value::Matrix* tmp = new value::Matrix();
    add(column, row, tmp);
    return *tmp;
}

Set& Matrix::getSet(const size_type& column, const size_type& row)
{
    return value::toSetValue(value::reference(get(column, row)));
}

Map& Matrix::getMap(const size_type& column, const size_type& row)
{
    return value::toMapValue(value::reference(get(column, row)));
}

Matrix& Matrix::getMatrix(const size_type& column, const size_type& row)
{
    return value::toMatrixValue(value::reference(get(column, row)));
}

const Set& Matrix::getSet(const size_type& column, const size_type& row) const
{
    return value::toSetValue(value::reference(get(column, row)));
}

const Map& Matrix::getMap(const size_type& column, const size_type& row) const
{
    return value::toMapValue(value::reference(get(column, row)));
}

const Matrix& Matrix::getMatrix(const size_type& column,
                                const size_type& row) const
{
    return value::toMatrixValue(value::reference(get(column, row)));
}

void Matrix::resize(const size_type& columns, const size_type& rows)
{
    m_matrix.resize(m_extents[columns][rows]);
}

void Matrix::addColumn()
{
    if (m_nbcol + 1 >= m_matrix.shape()[0]) {
        m_matrix.resize(
            m_extents[m_matrix.shape()[0] + m_stepcol]
            [m_matrix.shape()[1]]);
    }
    ++m_nbcol;
}

void Matrix::addRow()
{
    if (m_nbrow + 1 >= m_matrix.shape()[1]) {
        m_matrix.resize(
            m_extents[m_matrix.shape()[0]]
            [m_matrix.shape()[1] + m_steprow]);
    }
    ++m_nbrow;
}

void Matrix::moveLastCell()
{
    ++m_lastX;
    if (m_lastX >= columns()) {
        m_lastX = 0;
        ++m_lastY;
        if (m_lastY >= rows()) {
            m_lastY = 0;
        }
    }
}

}} // namespace vle value

