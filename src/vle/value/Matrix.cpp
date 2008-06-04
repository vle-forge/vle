/**
 * @file src/vle/value/Matrix.cpp
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
#include <vle/utils/Debug.hpp>

namespace vle { namespace value {

Matrix MatrixFactory::create(MatrixValue::index columns,
                             MatrixValue::index rows,
                             MatrixValue::index resizeColumns,
                             MatrixValue::index resizeRows)
{
    return Matrix(new MatrixFactory(columns, rows, resizeColumns,
                                    resizeRows));
}

Matrix MatrixFactory::create(MatrixValue::index columns,
                             MatrixValue::index rows,
                             MatrixValue::index columnmax,
                             MatrixValue::index rowmax,
                             MatrixValue::index resizeColumns,
                             MatrixValue::index resizeRows)
{
    Assert(utils::ArgError, columns < columnmax, boost::format(
            "Number of columns error: %1% on %2%") % columns % columnmax);
    Assert(utils::ArgError, rows < rowmax, boost::format(
            "Number of row error: %1% on %2%") % rows % rowmax);

    return Matrix(new MatrixFactory(columns, rows, columnmax, rowmax,
                                    resizeColumns, resizeRows));
}


Value MatrixFactory::clone() const
{
    return Value(new MatrixFactory(*this));
}

std::string MatrixFactory::toFile() const
{
    std::ostringstream o;

    for (MatrixValue::size_type j = 0; j < m_nbrow; ++j) {
        for (MatrixValue::size_type i = 0; i < m_nbcol; ++i) {
            if (m_matrix[i][j].get()) {
                o << m_matrix[i][j]->toFile();
            } else {
                o << "NA";
            }
            o << " ";
        }
        o << "\n";
    }

    o << "]";

    return o.str();
}

std::string MatrixFactory::toString() const
{
    std::ostringstream o;

    for (MatrixValue::size_type j = 0; j < m_nbrow; ++j) {
        for (MatrixValue::size_type i = 0; i < m_nbcol; ++i) {
            if (m_matrix[i][j].get()) {
                o << m_matrix[i][j]->toString();
            } else {
                o << "NA";
            }
            o << " ";
        }
        o << "\n";
    }
    return o.str();
}

std::string MatrixFactory::toXML() const
{
    std::ostringstream o;

    o << "<matrix "
        << "rows=\"" << m_nbrow  << "\" "
        << "columns=\"" << m_nbcol << "\" "
        << "columnmax=\"" << m_matrix.shape()[0] << "\" "
        << "rowmax=\"" << m_matrix.shape()[1] << "\" "
        << "columnstep=\"" << m_stepcol << "\" "
        << "rowstep=\"" << m_steprow << "\" >";

    for (MatrixValue::size_type j = 0; j < m_nbrow; ++j) {
        for (MatrixValue::size_type i = 0; i < m_nbcol; ++i) {
            if (m_matrix[i][j].get()) {
                o << m_matrix[i][j]->toXML();
            } else {
                o << "</ null>";
            }
            o << " ";
        }
        o << "\n";
    }

    o << "</matrix>";

    return o.str();
}

void MatrixFactory::resize(MatrixValue::size_type columns,
                           MatrixValue::size_type rows)
{
    m_matrix.resize(m_extents[columns][rows]);
}

void MatrixFactory::addColumn()
{
    if (m_nbcol - 1 >= m_matrix.shape()[0]) {
        m_matrix.resize(
            m_extents[m_matrix.shape()[0] + m_stepcol]
            [m_matrix.shape()[1]]);
    }
    ++m_nbcol;
}

void MatrixFactory::addRow()
{
    if (m_nbrow + 1 >= m_matrix.shape()[1]) {
        m_matrix.resize(
            m_extents[m_matrix.shape()[0]]
            [m_matrix.shape()[1] + m_steprow]);
    }
    ++m_nbrow;
}

void MatrixFactory::addValue(MatrixValue::size_type column,
                             MatrixValue::size_type row,
                             const value::Value& val)
{
    m_matrix[column][row] = val;
}

void MatrixFactory::addValueToLastCell(const value::Value& val)
{
    m_matrix[m_lastX][m_lastY] = val;
}

void MatrixFactory::moveLastCell()
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

Matrix toMatrixValue(const Value& value)
{
    Assert(utils::ArgError, value->getType() == ValueBase::MATRIX,
           "Value is not a Matrix");
    return boost::static_pointer_cast < MatrixFactory >(value);
}

MatrixFactory::MatrixView toMatrix(const Value& value)
{
    Assert(utils::ArgError, value->getType() == ValueBase::MATRIX,
           "Value is not a Matrix");
    return boost::static_pointer_cast < MatrixFactory >(value)->getValue();
}

MatrixFactory::MatrixFactory(MatrixValue::index columns,
                             MatrixValue::index rows,
                             MatrixValue::index resizeColumns,
                             MatrixValue::index resizeRows) :
    m_matrix(m_extents[columns][rows]),
    m_nbcol(0),
    m_nbrow(0),
    m_stepcol(resizeColumns),
    m_steprow(resizeRows),
    m_lastX(0),
    m_lastY(0)
{
}

MatrixFactory::MatrixFactory(MatrixValue::index columns,
                             MatrixValue::index rows,
                             MatrixValue::index columnmax,
                             MatrixValue::index rowmax,
                             MatrixValue::index resizeColumns,
                             MatrixValue::index resizeRows) :
    m_matrix(m_extents[columnmax][rowmax]),
    m_nbcol(columns),
    m_nbrow(rows),
    m_stepcol(resizeColumns),
    m_steprow(resizeRows),
    m_lastX(0),
    m_lastY(0)
{
}


MatrixFactory::MatrixFactory(const MatrixFactory& m) :
    ValueBase(m),
    m_matrix(m.m_matrix),
    m_nbcol(m.m_nbcol),
    m_nbrow(m.m_nbrow),
    m_stepcol(m.m_stepcol),
    m_steprow(m.m_steprow)
{
    for (MatrixValue::size_type i = 0; i < m_nbcol; ++i) {
        for (MatrixValue::size_type j = 0; j < m_nbrow; ++j) {
            if( m_matrix[i][j].get() != 0) {
                m_matrix[i][j] = m_matrix[i][j]->clone();
            } else {
                m_matrix[i][j] = Value();
            }
        }
    }
}

}} // namespace vle value


