/**
 * @file vle/value/Matrix.cpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <vle/value/Matrix.hpp>
#include <vle/value/Deleter.hpp>
#include <vle/utils/Debug.hpp>
#include <boost/utility.hpp>

namespace vle { namespace value {

Matrix::Matrix() :
    m_matrix(),
    m_nbcol(0),
    m_nbrow(0),
    m_stepcol(1),
    m_steprow(1),
    m_lastX(0),
    m_lastY(0)
{
}

Matrix::Matrix(index columns, index rows, index resizeColumns, index
               resizeRows) :
    m_matrix(m_extents[columns][rows]),
    m_nbcol(0),
    m_nbrow(0),
    m_stepcol(resizeColumns),
    m_steprow(resizeRows),
    m_lastX(0),
    m_lastY(0)
{
}

Matrix::Matrix(index columns, index rows, index columnmax, index rowmax, index
               resizeColumns, index resizeRows) :
    m_matrix(m_extents[columnmax][rowmax]),
    m_nbcol(columns),
    m_nbrow(rows),
    m_stepcol(resizeColumns),
    m_steprow(resizeRows),
    m_lastX(0),
    m_lastY(0)
{
    Assert(utils::ArgError, columns < columnmax, boost::format(
            "Number of columns error: %1% on %2%") % columns % columnmax);
    Assert(utils::ArgError, rows < rowmax, boost::format(
            "Number of row error: %1% on %2%") % rows % rowmax);
}


Matrix::Matrix(const Matrix& m) :
    Value(m),
    m_matrix(m.m_matrix),
    m_nbcol(m.m_nbcol),
    m_nbrow(m.m_nbrow),
    m_stepcol(m.m_stepcol),
    m_steprow(m.m_steprow)
{
    for (size_type i = 0; i < m_nbcol; ++i) {
        for (size_type j = 0; j < m_nbrow; ++j) {
            if( m_matrix[i][j]) {
                m_matrix[i][j] = m_matrix[i][j]->clone();
            } else {
                m_matrix[i][j] = 0;
            }
        }
    }
}

Matrix::~Matrix()
{
    clear();
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
    std::stack < Value* > composite;

    for (size_type j = 0; j < m_nbrow; ++j) {
        for (size_type i = 0; i < m_nbcol; ++i) {
            if (m_matrix[i][j]) {
                if (isComposite(m_matrix[i][j])) {
                    composite.push(m_matrix[i][j]);
                } else {
                    delete m_matrix[i][j];
                    m_matrix[i][j] = 0;
                }
            }
        }
    }

    deleter(composite);

    m_nbcol = 0;
    m_nbrow = 0;
    m_lastX = 0;
    m_lastY = 0;
}

void Matrix::resize(size_type columns, size_type rows)
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

void Matrix::add(size_type column, size_type row, value::Value* val)
{
    if (m_matrix[column][row]) {
        delete m_matrix[column][row];
    }
    m_matrix[column][row] = val;
}


void Matrix::add(size_type column, size_type row, const value::Value& val)
{
    if (m_matrix[column][row]) {
        delete m_matrix[column][row];
    }
    m_matrix[column][row] = val.clone();
}

void Matrix::addToLastCell(const value::Value& val)
{
    m_matrix[m_lastX][m_lastY] = val.clone();
}

void Matrix::addToLastCell(value::Value* val)
{
    assert(val);
    m_matrix[m_lastX][m_lastY] = val;
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

