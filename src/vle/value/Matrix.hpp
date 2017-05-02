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

#ifndef VLE_VALUE_MATRIX_HPP
#define VLE_VALUE_MATRIX_HPP 1

#include <vector>
#include <vle/DllDefines.hpp>
#include <vle/value/Value.hpp>

namespace vle {
namespace value {

/// @brief Define a Matrix of value::Value object.
using MatrixValue = std::vector<std::unique_ptr<Value>>;

/**
 * @brief A Matrix Value. This class wraps an std::vector class and manage
 * size (rows() * columns()), allocated_size (rowmax * columnmax) to step
 * up resize or add column/row dynamically.
 */
class VLE_API Matrix : public Value
{
public:
    using index = MatrixValue::size_type;
    using size_type = MatrixValue::size_type;
    using iterator = MatrixValue::iterator;
    using const_iterator = MatrixValue::const_iterator;

    /**
     * @brief Build an empty Matrix but an allocated size of 256*1024 cells.
     */
    Matrix();

    /**
     * @brief Build an empty matrix of value of size [colums][row].
     * @param columns the initial number of columns.
     * @param rows the initial number of rows.
     * @param resizeColumns the number of columns to add when resize the
     * matrix.
     * @param resizeRows the number of rows to add when resize the matrix.
     */
    Matrix(index columns, index rows, index resizeColumns, index resizeRows);

    /**
     * @brief Build an empty buffered matrix of value of size [colums][rows] in
     * a matrix of [columnmax][rowmax].
     * @param columns the initial number of columns.
     * @param rows the initial number of rows.
     * @param columnmax The max number of columns.
     * @param rowmax The max number of rows.
     * @param resizeColumns the number of columns to add when resize the
     * matrix.
     * @param resizeRow the number of rows to add when resize the matrix.
     * @throw utils::ArgError if columns > columnmax or if rows > rowmax.
     */
    Matrix(index columns,
           index rows,
           index columnmax,
           index rowmax,
           index resizeColumns,
           index resizeRow);

    /**
     * @brief Build a new Matrix, all the value::Value from the
     * Matrix are cloned.
     * @param m the Matrix to copy.
     */
    Matrix(const Matrix& m);

    /**
     * @brief Delete all data.
     */
    virtual ~Matrix()
    {
    }

    /**
     * @brief Build a new Matrix.
     * @param columns Define the number of columns for the buffer.
     * @param rows Define the number of rows for the buffer.
     * @param resizeColumns Number of columns when resize buffer.
     * @param resizeRows Number of row when resize buffer.
     * @return A new allocated Matrix.
     */
    static std::unique_ptr<Value> create(index columns = 10,
                                         index rows = 10,
                                         index resizeColumns = 10,
                                         index resizeRows = 10)
    {
        return std::unique_ptr<Value>(
          new Matrix(columns, rows, resizeColumns, resizeRows));
    }

    /**
     * @brief Build a new Matrix with a specified views.
     * @param columns Define the number of columns for the buffer.
     * @param rows Define the number of rows for the buffer.
     * @param columnmax The max number of columns.
     * @param rowmax The max number of rows.
     * @param resizeColumns Number of columns when resize buffer.
     * @param resizeRows Number of row when resize buffer.
     * @return A new allocated Matrix.
     */
    static std::unique_ptr<Value> create(index columns,
                                         index rows,
                                         index columnmax,
                                         index rowmax,
                                         index resizeColumns,
                                         index resizeRows)
    {
        return std::unique_ptr<Value>(new Matrix(
          columns, rows, columnmax, rowmax, resizeColumns, resizeRows));
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
      * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Clone the Matrix and all value::Value.
     * @return A new Matrix.
     */
    virtual std::unique_ptr<Value> clone() const override
    {
        return std::unique_ptr<Value>(new Matrix(*this));
    }

    /**
     * @brief Get the type of this class.
     * @return Value::MATRIX.
     */
    inline virtual Value::type getType() const override;

    /**
     * @brief Push all Value from the MatrixValue, recursively and space
     * separated.
     * @code
     * 1 2 3 4 5 NA 7
     * 8 9 1 2 3  4 5
     * @endcode
     * @param out The output stream.
     */
    virtual void writeFile(std::ostream& out) const override;

    /**
     * @brief Push all Value from the MatrixValue, recursively and space
     * separated.
     * @code
     * 1 2 3 4 5 NA 7
     * 8 tutu 1 2 3  4 5
     * @endcode
     * @param out The output stream.
     */
    virtual void writeString(std::ostream& out) const override;

    /**
     * @brief Push all Value from the MatrixValue recursively in an XML
     * representation.
     * @code
     * <matrix rows="2" columns="7" columnmax="100" rowmax="2000"
     *         columnstep="1" rowstep="12">
     *  <integer>1</integer>
     *  <integer>2</integer>
     *  <integer>3</integer>
     *  <integer>4</integer>
     *  <integer>5</integer>
     *  <null />
     *  <integer>7</integer>
     *  <string>tutu</string>
     *  <integer>9</integer>
     *  <integer>1</integer>
     *  <integer>2</integer>
     *  <integer>3</integer>
     *  <integer>4</integer>
     *  <integer>5</integer>
     * </matrix>
     * @endcode
     * @param out The output stream.
     */
    virtual void writeXml(std::ostream& out) const override;

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
      * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    iterator begin()
    {
        return m_matrix.begin();
    }

    iterator end()
    {
        return m_matrix.end();
    }

    const_iterator begin() const
    {
        return m_matrix.begin();
    }

    const_iterator end() const
    {
        return m_matrix.end();
    }

    size_type size() const
    {
        return columns() * rows();
    }

    /**
     * @brief Delete all element from the matrix. All value are deleted.
     */
    void clear();

    /**
     * Reserve memory for the matrix. If \e columnmax or \e rowmax are
     * lower than columm and row do nothing.
     * @param columnmax The number of columns of the matrix.
     * @param rowmax The number of rows of the matrix.
     */
    void reserve(size_type columnmax, size_type rowmax);

    /**
     * @brief Resize the current matrix.
     * @param columns The number of columns of the matrix.
     * @param rows The number of rows of the matrix.
     */
    void resize(size_type columns, size_type rows);

    /**
     * @brief Resize the current matrix.
     * @param columns The number of columns of the matrix.
     * @param rows The number of rows of the matrix.
     * @parma value Default value to be assigned in each new cell.
     */
    void resize(size_type columns,
                size_type rows,
                const std::unique_ptr<Value>& value);

    /**
     * @brief Resize only the size of allocated matrix . Content of the matrix
     * is not modified.
     * @param columns The number of columns of the allocated matrix. Has to be
     * greater the m_nbcol and m_matrix.shape[0], otherwise nothing happens.
     * @param rows The number of rows of the matrix. Has to be
     * greater the m_nbrow and m_matrix.shape[1], otherwise nothing happens.
     */
    void increaseAllocation(size_type columns, size_type rows);

    /**
     * @brief Add a new column to the Matrix.
     */
    void addColumn();

    /**
     * @brief Add a new row to the Matrix.
     */
    void addRow();

    /**
     * @brief Get am access to the underlying std::vector.
     * @return a reference to the std::vector.
     */
    inline MatrixValue& value()
    {
        return m_matrix;
    }

    /**
     * @brief Get a constant access to the underlying std::vector.
     * @return a reference to the std::vector.
     */
    inline const MatrixValue& value() const
    {
        return m_matrix;
    }

    /**
     * @brief Set the cell at (column, row) to the specified value. Be careful,
     * the value is managed by the Matrix.
     * @param column index of the cell's column.
     * @param row index of the cell's row.
     * @param value the value to set.
     */
    void add(index column, index row, std::unique_ptr<Value> val);

    /**
     * @brief Set the cell at (column, row) to the specified value. Be careful,
     * the value is managed by the Matrix.
     * @param column index of the cell's column.
     * @param row index of the cell's row.
     * @param value the value to set.
     */
    void set(index column, index row, std::unique_ptr<Value> val);

    /**
     * @brief Get a pointer from a cell of the matrix.
     * @param column The column.
     * @param row The row.
     * @return A constant pointer to the Value.
     * @throw utils::ArgError if bad access to the matrix, in debug mode only.
     */
    const std::unique_ptr<Value>& get(index column, index row) const;

    /**
     * Get an ownership pointer to the specified cell. The \e Value is
     * removed from the Matrix.
     * @param column The column.
     * @param row The row.
     * @return A ownership pointer to the Value.
     */
    std::unique_ptr<Value> give(index column, index row);

    /**
     * @brief Get a pointer from a cell of the matrix.
     * @param column The column.
     * @param row The row.
     * @return A constant pointer to the Value.
     * @throw utils::ArgError if bad access to the matrix, in debug mode only.
     */
    const std::unique_ptr<Value>& operator()(index column, index row) const;

    /**
     * @brief Set the last cell to the specificed value. The value is
     * cloned.
     * @param val the value to clone and assign.
     */
    void addToLastCell(std::unique_ptr<Value> val);

    /**
     * @brief Move the last cell to the nearest Cell in column or row. If
     * the last cell equal the number of column, column is set to 0, row to
     * row + 1. If the row equal the number of row, row is set to 0.
     */
    void moveLastCell();

    /**
     * @brief Get a constant reference to the complete matrix.
     * @return A constant reference to the complete matrix.
     */
    inline const MatrixValue& matrix() const
    {
        return m_matrix;
    }

    /**
     * @brief Return the number of valid column data.
     * @return the number of valid column.
     */
    inline size_type columns() const
    {
        return m_nbcol;
    }

    /**
     * @brief Return the number of allocatede column data.
     * @return the number of valid column.
     */
    inline size_type columns_max() const
    {
        return m_nbcolmax;
    }

    /**
     * @brief Return the number of valid row data.
     * @return the number of valid row.
     */
    inline size_type rows() const
    {
        return m_nbrow;
    }

    /**
     * @brief Return the number allocated row data.
     * @return the number of valid row.
     */
    inline size_type rows_max() const
    {
        return m_nbrowmax;
    }

    /**
     * @brief Return the number of column to add.
     * @return the number of column to add.
     */
    inline size_type resizeColumn() const
    {
        return m_stepcol;
    }

    /**
     * @brief Update the column to add factor.
     * @param colstep The number of column to add.
     */
    inline void setResizeColumn(size_type colstep)
    {
        m_stepcol = (colstep <= 0) ? m_stepcol : colstep;
    }

    /**
     * @brief Return the number of row to add.
     * @return the number of row to add.
     */
    inline size_type resizeRow() const
    {
        return m_steprow;
    }

    /**
     * @brief Update the row to add factor.
     * @param colrow The number of row to add.
     */
    inline void setResizeRow(size_type colrow)
    {
        m_steprow = (colrow <= 0) ? m_steprow : colrow;
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
      * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Add a null value into the matrix.
     * @param column The column.
     * @param row The row.
     */
    Null& addNull(index column, index row);

    /**
     * @brief Add a boolean into the matrix.
     * @param column The column.
     * @param row The row.
     * @param value The value of the boolean.
     */
    Boolean& addBoolean(index column, index row, bool value);

    /**
     * @brief Get a boolean from the matrix.
     * @param column The column.
     * @param row The row.
     * @return The boolean readed from the matrix.
     */
    bool getBoolean(index column, index row) const;

    /**
     * @brief Get a boolean from the matrix.
     * @param column The column.
     * @param row The row.
     * @return The boolean readed from the matrix.
     */
    bool& getBoolean(index column, index row);

    /**
     * @brief Add a double into the matrix.
     * @param column The column.
     * @param row The row.
     * @param value The value of the double.
     */
    Double& addDouble(index column, index row, double value);

    /**
     * @brief Get a double from the matrix.
     * @param column The column.
     * @param row The row.
     * @return The double readed from the matrix.
     */
    double getDouble(index column, index row) const;

    /**
     * @brief Get a double from the matrix.
     * @param column The column.
     * @param row The row.
     * @return The double readed from the matrix.
     */
    double& getDouble(index column, index row);

    /**
     * @brief Add an integer into the matrix.
     * @param column The column.
     * @param row The row.
     * @param value The value of the int.
     */
    Integer& addInt(index column, index row, int32_t value);

    /**
     * @brief Get an integer from the matrix.
     * @param column The column.
     * @param row The row.
     * @return The integer readed from the matrix.
     */
    int32_t getInt(index column, index row) const;

    /**
     * @brief Get an integer from the matrix.
     * @param column The column.
     * @param row The row.
     * @return The integer readed from the matrix.
     */
    int32_t& getInt(index column, index row);

    /**
     * @brief Add a string into the matrix.
     * @param column The column.
     * @param row The row.
     * @param value The value of the string.
     */
    String& addString(index column, index row, const std::string& value);

    /**
     * @brief Get a string from the matrix.
     * @param column The column.
     * @param row The row.
     * @return The integer readed from the matrix.
     */
    const std::string& getString(index column, index row) const;

    /**
     * @brief Get a string from the matrix.
     * @param column The column.
     * @param row The row.
     * @return The integer readed from the matrix.
     */
    std::string& getString(index column, index row);

    /**
     * @brief Add an xml into the matrix.
     * @param column The column.
     * @param row The row.
     * @param value The value of the xml.
     */
    Xml& addXml(index column, index row, const std::string& value);

    /**
     * @brief Get an xml from the matrix.
     * @param column The column.
     * @param row The row.
     * @return The integer readed from the matrix.
     */
    const std::string& getXml(index column, index row) const;

    /**
     * @brief Get an xml from the matrix.
     * @param column The column.
     * @param row The row.
     * @return The integer readed from the matrix.
     */
    std::string& getXml(index column, index row);

    /**
     * @brief Add a Tuple into the Matrix.
     * @param column The column.
     * @param row The row.
     * @param width The width of the newly allocated Tuple.
     * @param value The default value of cells in the Tuple.
     * @return A reference to the allocated Tuple.
     */
    Tuple& addTuple(index column,
                    index row,
                    std::size_t width = 0,
                    double value = 0.0);

    /**
     * @brief Get a Tuple from the matrix.
     * @param column The column.
     * @param row The row.
     * @return The Tuple readed from the matrix.
     */
    const Tuple& getTuple(index column, index row) const;

    /**
     * @brief Get a Tuple from the matrix.
     * @param column The column.
     * @param row The row.
     * @return The Tuple readed from the matrix.
     */
    Tuple& getTuple(index column, index row);

    /**
     * @brief Add a Table into the Matrix.
     * @param column The column.
     * @param row The row.
     * @param width The width of the newly allocated Table.
     * @param height The height of the newly allocated Table.
     * @return A reference to the allocated Table.
     */
    Table& addTable(index column,
                    index row,
                    std::size_t width = 0,
                    std::size_t height = 0);

    /**
     * @brief Get a Table from the matrix.
     * @param column The column.
     * @param row The row.
     * @return The Table readed from the matrix.
     */
    const Table& getTable(index column, index row) const;

    /**
     * @brief Get a Table from the matrix.
     * @param column The column.
     * @param row The row.
     * @return The Table readed from the matrix.
     */
    Table& getTable(index column, index row);

    /**
     * @brief Add a Set at the end of the Set.
     * @param column The column.
     * @param row The row.
     * @return A reference to the newly allocated Set.
     */
    Set& addSet(index column, index row);

    /**
     * @brief Add a Map at the end of the Set.
     * @param column The column.
     * @param row The row.
     * @return A reference to the newly allocated Map.
     */
    Map& addMap(index column, index row);

    /**
     * @brief Add a Matrix at the end of the Set.
     * @param column The column.
     * @param row The row.
     * @return A reference to the newly allocated Set.
     */
    Matrix& addMatrix(index column, index row);

    /**
     * @brief Get a Set from the specified index.
     * @param column The column.
     * @param row The row.
     * @return A Set.
     * @throw utils::ArgError if the index 'column' or 'row' are to big (in
     * debug mode) or if the value at (column, row) is null or is not a Set.
     */
    Set& getSet(index column, index row);

    /**
     * @brief Get a Map from the specified index.
     * @param column The column.
     * @param row The row.
     * @return A Map.
     * @throw utils::ArgError if the index 'column' or 'row' are to big (in
     * debug mode) or if the value at (column, row) is null or is not a Set.
     */
    Map& getMap(index column, index row);

    /**
     * @brief Get a Matrix from the specified index.
     * @param column The column.
     * @param row The row.
     * @return A Matrix.
     * @throw utils::ArgError if the index 'column' or 'row' are to big (in
     * debug mode) or if the value at (column, row) is null or is not a Map.
     */
    Matrix& getMatrix(index column, index row);

    /**
     * @brief Get a constant Set from the specified index.
     * @param column The column.
     * @param row The row.
     * @return A Set.
     * @throw utils::ArgError if the index 'column' or 'row' are to big (in
     * debug mode) or if the value at (column, row) is null or is not a Map.
     */
    const Set& getSet(index column, index row) const;

    /**
     * @brief Get a constant Map from the specified index.
     * @param column The column.
     * @param row The row.
     * @return A Map.
     * @throw utils::ArgError if the index 'column' or 'row' are to big (in
     * debug mode) or if the value at (column, row) is null or is not an
     * Matrix.
     */
    const Map& getMap(index column, index row) const;

    /**
     * @brief Get a constant Matrix from the specified index.
     * @param column The column.
     * @param row The row.
     * @return A Matrix.
     * @throw utils::ArgError if the index 'column' or 'row' are to big (in
     * debug mode) or if the value at (column, row) is null or is not an
     * Matrix.
     */
    const Matrix& getMatrix(index column, index row) const;

private:
    MatrixValue m_matrix; /// @brief to store the values.
    size_type m_nbcol;    /// @brief to store the column number.
    size_type m_nbrow;    /// @brief to store the row number.
    size_type m_nbcolmax; /// @brief to store the column number.
    size_type m_nbrowmax; /// @brief to store the row number.
    size_type m_stepcol;  /// @brief the column when resize.
    size_type m_steprow;  /// @brief the row when resize.
    index m_lastX;        /// @brief the last columns set.
    index m_lastY;        /// @brief the last row set.
};

inline const Matrix&
toMatrixValue(std::shared_ptr<Value> value)
{
    return value::reference(value).toMatrix();
}

inline const Matrix&
toMatrixValue(std::shared_ptr<const Value> value)
{
    return value::reference(value).toMatrix();
}

inline const Matrix&
toMatrixValue(const std::unique_ptr<Value>& value)
{
    return value::reference(value).toMatrix();
}

inline const Matrix&
toMatrixValue(const Value& value)
{
    return value.toMatrix();
}

inline Matrix&
toMatrixValue(Value& value)
{
    return value.toMatrix();
}

inline const MatrixValue&
toMatrix(std::shared_ptr<Value> value)
{
    return value::reference(value).toMatrix().value();
}

inline const MatrixValue&
toMatrix(std::shared_ptr<const Value> value)
{
    return value::reference(value).toMatrix().value();
}

inline const MatrixValue&
toMatrix(const std::unique_ptr<Value>& value)
{
    return value::reference(value).toMatrix().value();
}

inline const MatrixValue&
toMatrix(const Value& value)
{
    return value.toMatrix().value();
}

inline MatrixValue&
toMatrix(Value& value)
{
    return value.toMatrix().value();
}
}
} // namespace vle value

#endif
