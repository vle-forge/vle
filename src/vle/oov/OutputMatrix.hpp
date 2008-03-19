/**
 * @file src/vle/oov/OutputMatrix.hpp
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




#ifndef VLE_OOV_OUTPUTMATRIX_HPP
#define VLE_OOV_OUTPUTMATRIX_HPP

#include <vle/value/Matrix.hpp>
#include <list>
#include <string>
#include <map>



namespace vle { namespace oov {

    /** 
     * @brief This class use a value::Matrix to store all result from a
     * simulation. Each column are referenced by a couple (model's name, port's
     * name) stored into a std::map to get the index of a value. The column 0 is
     * use for store a representation of the Time.
     */
    class OutputMatrix
    {
    public:
        ///! @brief define a list of string to define list of port.
        typedef std::list < std::string > StringList;

        ///! @brief define a dictionary model name, ports list.
        typedef std::map < std::string, StringList > MapStringList;

        /** 
         * @brief Build an empty matrix of value of size [colums][row].
         * 
         * @param columns the initial number of columns.
         * @param rows the initial number of rows.
         * @param resizeColumns the number of columns to add when resize the
         * matrix.
         * @param resizeRow the number of rows to add when resize the matrix.
         */
        OutputMatrix(int columns = 100,
                     int rows = 1000,
                     int resizeColumns = 100,
                     int resizeRow = 1000);

        /** 
         * @brief Destroy
         */
        virtual ~OutputMatrix()
        { }

        /** 
         * @brief Serialize the data's to an output of value::Value object. Be
         * careful, the data (from the matrix) is not cloned. The
         * serialization is defined like:
         * @code
         * <set>
         *  <matrix row="x" column="y" columnmax="" rowmax=""
         *          columnstep="" rowstep="" >
         *   ... <!-- x * y values -->
         *  </matrix>
         *  <set>                        <!-- x set to represents -->
         *   <string>model</string>      <!-- model name -->
         *   <string>port</string>       <!-- port name -->
         *   <integer>index</integer>    <!-- index in the data array -->
         *  </set>
         * </set>
         * @endcode
         * @return The serialized representation of this Object.
         */
        value::Value serialize() const;

        /** 
         * @brief Read the value::Value and rebuild the content of the
         * OutputMatrix.
         * @param vals A value::Value to rebuild the OutputMatrix.
         * @throw utils::ArgError if an error occurred in parsing.
         */
        void deserialize(value::Value vals);

        void resize(value::MatrixFactory::size_type maxcols,
                    value::MatrixFactory::size_type maxrows);

        /** 
         * @brief Update the resize value in columns and rows.
         * 
         * @param resizeColumns The number of columns to add.
         * @param resizeRows The number of rows to add.
         */
        void updateStep(value::MatrixFactory::size_type resizeColumns,
                        value::MatrixFactory::size_type resizeRows);

        /** 
         * @brief Add a new column into the output matrix. If the number of
         * column is equal to the size of the matrix, the matrix is resize in
         * column and row by the resizeColumns and resizeRow of the constructor.
         *
         * @param model the model name.
         * @param port the port name.
         *
         * @return the index of the model, port in the matrix.
         */
        value::MatrixFactory::index addModel(const std::string& model,
                                             const std::string& port);

        /** 
         * @brief Add a value to the specified couple model port.
         * 
         * @param model the model name.
         * @param port the port name.
         * @param value the value to set.
         */
        void addValue(const std::string& model, const std::string& port,
                      const value::Value& value);

        /** 
         * @brief Set the last time (column 0, last row) to the specified
         * value and add a new row in the matrix. If the number of row equal the
         * number of row of the Matrix, the matrix is resized in row by the
         * resizeRow of the constructor.
         *
         * @param time the time to set.
         */
        void setLastTime(double time);

        /** 
         * @brief Get the Time vector.
         * 
         * @return A view on the Time column;
         */
        value::MatrixFactory::VectorView getTime()
        { return m_values->row(0); }

        /** 
         * @brief Return the list of model name.
         * @return A const reference on a vector of string.
         */
        const OutputMatrix::StringList& getModelList() const
        { return m_modelNames; }

        /** 
         * @brief Return the list of port for a specific model.
         * @param model The model name to get the list of ports.
         * @return A const reference on a vector of string.
         */
        const OutputMatrix::StringList&
            getPortList(const std::string& model) const;

        /** 
         * @brief Retrieve the vector of a couple model, port.
         * @param model the model name.
         * @param port the port name.
         * @return A vector of value.
         * @throw utils::ArgError if the model or if the port's model does not
         * exist.
         */
        value::MatrixFactory::VectorView getValue(const std::string& model,
                                                  const std::string& port);

        /** 
         * @brief Return the size of a vector.
         * @return A integer.
         */
        value::MatrixFactory::MatrixValue::index getVectorSize() const
        { return m_values->rows(); }

        /** 
         * @brief Return the Matrix of the values.
         * @return A constant reference to the Matrix.
         */
        inline value::MatrixFactory::ConstMatrixView values() const
        { return m_values->getConstValue(); }

        /** 
         * @brief Return the Matrix of the values.
         * @return A constant reference to the Matrix.
         */
        inline value::MatrixFactory::MatrixView values()
        { return m_values->getValue(); }

        /** 
         * @brief Return the index of the specified couple model port.
         * @param model Name of the model.
         * @param port Name of the port.
         * @throw utils::ArgError if couple model port does not exist.
         * @return A index.
         */
        value::MatrixFactory::MatrixValue::index
            column(const std::string& model, const std::string& port) const;

    private:
        ///! @brief define a pair model, port names.
        typedef std::pair < std::string, std::string > PairString;

        /** 
         * @brief A functor to test the difference between two couple model,
         * port.
         * To use with, find, compare etc.
         */
        struct pairCmp
        {
            inline bool
                operator()(const PairString& ps1, const PairString& ps2) const
                { return (ps1.first + ps1.second < ps2.first + ps2.second); }
        };

        ///! @brief define a dictionary model-port name to index in matrix.
        typedef std::map < PairString,
                value::MatrixFactory::MatrixValue::index, pairCmp > 
                    MapPairIndex;


        value::Matrix m_values; ///! the matrix.
        MapStringList m_info; ///! store for each model, list of port.
        StringList m_modelNames; ///! store the list of model's name.
        MapPairIndex m_colAccess; ///! from (model, port) to matrix index.
    };


    /** 
     * @brief This typedef is used to defined the dictionnary of key view name
     * and oov::PluginPtr.
     */
    typedef std::map < std::string, OutputMatrix > OutputMatrixViewList;

}} // namespace vle oov

#endif
