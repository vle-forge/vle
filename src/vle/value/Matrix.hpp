/**
 * @file src/vle/value/Matrix.hpp
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



#ifndef VLE_VALUE_MATRIX_HPP
#define VLE_VALUE_MATRIX_HPP

#include <vle/value/Value.hpp>
#include <boost/multi_array.hpp>

namespace vle { namespace value {

    /** 
     * @brief A Matrix Value. This class wraps a boost::multi_array from the
     * Boost library (http://www.boost.org) class of two dimension of
     * value::Value.
     */
    class MatrixFactory : public ValueBase
    {
    public:
        ///! @brief Define a Matrix of value::Value object.
        typedef boost::multi_array<value::Value, 2 >  MatrixValue;

        ///! @brief Define a Vector View  of the Matrix.
        typedef MatrixValue::array_view < 1 >::type VectorView;

        ///! @brief Define a constant Vector View  of the Matrix.
        typedef MatrixValue::const_array_view < 1 >::type ConstVectorView;

        ///! @brief Define a Matrix View of the Matrix.
        typedef MatrixValue::array_view < 2 >::type MatrixView;

        ///! @brief Define a constant Matrix View of the Matrix.
        typedef MatrixValue::const_array_view < 2 >::type ConstMatrixView;

        ///! Define indices of Matrix.
        typedef MatrixValue::index_gen Indices;

        ///! Define a Range of Matrix.
        typedef MatrixValue::index_range Range;

        ///! Define a Extents of Matrix.
        typedef MatrixValue::extent_gen Extents;

        ///! Define an index of Matrix.
        typedef MatrixValue::index index;

        ///! Define a dimension size of Matrix.
        typedef MatrixValue::size_type size_type;


        /** 
         * @brief Delete all data.
         */
        virtual ~MatrixFactory()
        { }

        /** 
         * @brief Build a new Matrix.
         * 
         * @param columns Define the number of columns for the buffer.
         * @param rows Define the number of rows for the buffer.
         * @param resizeColumns Number of columns when resize buffer.
         * @param resizeRows Number of row when resize buffer.
         * 
         * @return A new allocated Matrix.
         */
        static Matrix create(MatrixValue::index columns = 10,
                             MatrixValue::index rows = 10,
                             MatrixValue::index resizeColumns = 10,
                             MatrixValue::index resizeRows = 10);

        /** 
         * @brief Build a new Matrix with a specified views.
         * 
         * @param columns Define the number of columns for the buffer.
         * @param rows Define the number of rows for the buffer.
         * @param columnmax The max number of columns.
         * @param rowmax The max number of rows.
         * @param resizeColumns Number of columns when resize buffer.
         * @param resizeRows Number of row when resize buffer.
         * 
         * @return A new allocated Matrix.
         */
        static Matrix create(MatrixValue::index columns,
                             MatrixValue::index rows,
                             MatrixValue::index columnmax,
                             MatrixValue::index rowmax,
                             MatrixValue::index resizeColumns,
                             MatrixValue::index resizeRows);

        /** 
         * @brief Clone the Matrix and all value::Value.
         * 
         * @return A new Matrix.
         */
        virtual Value clone() const;

        inline virtual ValueBase::type getType() const
        { return ValueBase::MATRIX; }

        virtual std::string toFile() const;

        virtual std::string toString() const;

        virtual std::string toXML() const;

        //
        // Matrix specific functions
        //

        /** 
         * @brief Resize the current matrix.
         * 
         * @param columns The number of columns of the matrix.
         * @param rows The number of rows of the matrix.
         * @throw utils::ArgError if data are loss.
         */
        void resize(MatrixValue::size_type columns,
                    MatrixValue::size_type rows);

        /** 
         * @brief Add a new column to the Matrix.
         */
        void addColumn();

        /** 
         * @brief Add a new row to the Matrix.
         */
        void addRow();

        /** 
         * @brief Set the cell at (column, row) to the specified value.
         *
         * @param column index of the cell's column.
         * @param row index of the cell's row.
         * @param value the value to set.
         */
        void addValue(MatrixValue::size_type column,
                      MatrixValue::size_type row,
                      const value::Value& val);

        /** 
         * @brief Set the last cell to the specificed value.
         * 
         * @param val the value to set.
         */
        void addValueToLastCell(const value::Value& val);

        /** 
         * @brief Move the last cell to the nearest Cell in column or row. If
         * the last cell equal the number of column, column is set to 0, row to
         * row + 1. If the row equal the number of row, row is set to 0.
         * 
         * @param column 
         * @param row 
         */
        void moveLastCell();

        /** 
         * @brief Get the correct subset of the Matrix define in:
         * [0, columns()][0, rows()]. Not the global Matrix:
         * [0, matrix.shape()[0][0, matrix.shape()[1]];
         * 
         * @return A view of the data.
         */
        inline MatrixView getValue()
        { return m_matrix[m_indices [Range(0, m_nbcol)][Range(0, m_nbrow)]]; }

        /** 
         * @brief Get the correct subset of the Matrx define in:
         * [0, columns()][0, rows()]. Not the global Matrix:
         * [0, matrix.shape()[0][0, matrix.shape()[1]];
         * 
         * @return A constant view of the data.
         */
        inline ConstMatrixView getConstValue() const
        { return m_matrix[m_indices [Range(0, m_nbcol)][Range(0, m_nbrow)]]; }

        /** 
         * @brief Get a constant reference to the complete matrix.
         * 
         * @return A constant reference to the complete matrix.
         */
        inline const MatrixValue& matrix() const
        { return m_matrix; }

        /** 
         * @brief Return the number of valid column data.
         * 
         * @return the number of valid column.
         */
        inline MatrixValue::size_type columns() const
        { return m_nbcol; }

        /** 
         * @brief Return a vector from the Matrix.
         * 
         * @param index the index of the vector.
         * 
         * @return A view on the column of the Matrix.
         */
        inline VectorView column(MatrixValue::index index)
        { return m_matrix[boost::indices[index][Range(0, m_nbrow)]]; }

        /** 
         * @brief Return a vector from the Matrix.
         * 
         * @param index the index of the vector.
         * 
         * @return A view on the column of the Matrix.
         */
        inline ConstVectorView column(MatrixValue::index index) const
        { return m_matrix[boost::indices[index][Range(0, m_nbrow)]]; }

        /** 
         * @brief Return the number of valid row data.
         * 
         * @return the number of valid row.
         */
        inline MatrixValue::size_type rows() const
        { return m_nbrow; }

        /** 
         * @brief Return a vector from the Matrix.
         * 
         * @param index the index of the vector.
         * 
         * @return A view on the row of the Matrix.
         */
        inline VectorView row(MatrixValue::index index)
        { return m_matrix[boost::indices[Range(0, m_nbcol)][index]]; }

        /** 
         * @brief Return a constant vector from the Matrix.
         * 
         * @param index the index of the vector.
         * 
         * @return A constant view on the row of the Matrix.
         */
        inline ConstVectorView row(MatrixValue::index index) const
        { return m_matrix[boost::indices[Range(0, m_nbcol)][index]]; }

        /** 
         * @brief Return the number of column to add.
         * 
         * @return the number of column to add.
         */
        inline MatrixValue::size_type resizeColumn() const
        { return m_stepcol; }

        /** 
         * @brief Update the column to add factor.
         * 
         * @param colstep The number of column to add.
         */
        inline void setResizeColumn(MatrixValue::size_type colstep)
        { m_stepcol = (colstep <= 0) ? m_stepcol : colstep; }

        /** 
         * @brief Return the number of row to add.
         * 
         * @return the number of row to add.
         */
        inline MatrixValue::size_type resizeRow() const
        { return m_steprow; }

        /** 
         * @brief Update the row to add factor.
         * 
         * @param colrow The number of row to add.
         */
        inline void setResizeRow(MatrixValue::size_type colrow)
        { m_steprow = (colrow <= 0) ? m_steprow : colrow; }

    private:
        /** 
         * @brief Build an empty matrix of value of size [colums][row].
         * 
         * @param columns the initial number of columns.
         * @param rows the initial number of rows.
         * @param resizeColumns the number of columns to add when resize the
         * matrix.
         * @param resizeRow the number of rows to add when resize the matrix.
         */
        MatrixFactory(MatrixValue::index columns,
                      MatrixValue::index rows,
                      MatrixValue::index resizeColumns,
                      MatrixValue::index resizeRow);

        /** 
         * @brief Build an empty matrix of value of size [colums][row].
         * 
         * @param columns the initial number of columns.
         * @param rows the initial number of rows.
         * @param columnmax The max number of columns.
         * @param rowmax The max number of rows.
         * @param resizeColumns the number of columns to add when resize the
         * matrix.
         * @param resizeRow the number of rows to add when resize the matrix.
         */
        MatrixFactory(MatrixValue::index columns,
                      MatrixValue::index rows,
                      MatrixValue::index columnmax,
                      MatrixValue::index rowmax,
                      MatrixValue::index resizeColumns,
                      MatrixValue::index resizeRow);

        /** 
         * @brief Build a new MatrixFactory, all the value::Value from the
         * Matrix are cloned.
         * 
         * @param m the Matrix to copy. 
         */
        MatrixFactory(const MatrixFactory& m);


        MatrixValue m_matrix; ///! @brief to store the values.
        MatrixValue::index_gen m_indices; ///! @brief indices for the matrix.
        MatrixValue::extent_gen m_extents; ///! @brief to extents matrix.
        MatrixValue::size_type m_nbcol;  ///! @brief to store the column number.
        MatrixValue::size_type m_nbrow;  ///! @brief to store the row number.
        MatrixValue::size_type m_stepcol; ///! @brief the column when resize.
        MatrixValue::size_type m_steprow; ///! @brief the row when resize.

        MatrixValue::size_type m_lastX; ///! @brief the last columns set.
        MatrixValue::size_type m_lastY; ///! @brief the last row set.
    };

    /** 
     * @brief A functor to test is a Value is a Matrix. To use with algorithms
     * of test.
     */
    struct IsMatrixValue
    {
        bool operator()(const value::Value& value) const
        { return value.get() and value->getType() == ValueBase::MATRIX; }
    };
    
    Matrix toMatrixValue(const Value& value);

    MatrixFactory::MatrixView toMatrix(const Value& value);

}} // namespace vle value

#endif
