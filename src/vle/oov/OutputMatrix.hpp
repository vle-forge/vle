/*
 * @file vle/oov/OutputMatrix.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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

#include <vle/oov/DllDefines.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/value/Double.hpp>
#include <boost/utility.hpp>
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
    class VLE_OOV_EXPORT OutputMatrix
    {
    public:
        ///! @brief define a list of string to define list of port.
        typedef std::list < std::string > StringList;

        ///! @brief define a dictionary model name, ports list.
        typedef std::map < std::string, StringList > MapStringList;

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
                value::MatrixValue::index, pairCmp > MapPairIndex;

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
         * @brief Copy constructor make a clone of the value::Matrix.
         * @param outputmatrix The OutputMatrix to copy.
         */
        OutputMatrix(const OutputMatrix& outputmatrix) :
            m_values(value::toMatrixValue(outputmatrix.m_values->clone())),
            m_info(outputmatrix.m_info),
            m_modelNames(outputmatrix.m_modelNames),
            m_colAccess(outputmatrix.m_colAccess)
        {}

        /**
         * @brief Destroy the allocated value::Matrix.
         */
        virtual ~OutputMatrix();

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
        value::Value* serialize() const;

        /**
         * @brief Read the value::Value and rebuild the content of the
         * OutputMatrix.
         * @param vals A value::Value to rebuild the OutputMatrix.
         * @throw utils::ArgError if an error occurred in parsing.
         */
        void deserialize(const value::Value& vals);

        void resize(value::Matrix::size_type maxcols,
                    value::Matrix::size_type maxrows);

        /**
         * @brief Update the resize value in columns and rows.
         *
         * @param resizeColumns The number of columns to add.
         * @param resizeRows The number of rows to add.
         */
        void updateStep(value::Matrix::size_type resizeColumns,
                        value::Matrix::size_type resizeRows);

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
        value::Matrix::index addModel(const std::string& model,
                                      const std::string& port);

        /**
         * @brief Add a value to the specified couple model port.
         *
         * @param model the model name.
         * @param port the port name.
         * @param value the value clone and set.
         */
        void addValue(const std::string& model, const std::string& port,
                      const value::Value& value);

        /**
         * @brief Add a value to the specified couple model port.
         *
         * @param model the model name.
         * @param port the port name.
         * @param value the value to set.
         */
        void addValue(const std::string& model, const std::string& port,
                      value::Value* value);

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
        value::VectorView getTime()
        { return m_values->column(0); }

        /**
         * @brief Get the Time vector.
         *
         * @return A view on the Time column;
         */
        value::ConstVectorView getTime() const
        { return m_values->column(0); }

        /**
         * @brief Get the last observation date (ie. last value in the column
         * time (0)).
         * @return A real.
         */
        double getLastTime() const
        { return getTime()[m_values->rows() - 1]->toDouble().value(); }

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
        value::VectorView getValue(const std::string& model,
                                   const std::string& port);

        /**
         * @brief Retrive the vector or column of a specific index.
         *
         * @param idx the index of the vector.
         *
         * @return A Vector View.
         */
        value::VectorView getValue(value::Matrix::size_type idx);

        /**
         * @brief Retrieve the constant vector of a couple model, port.
         * @param model the model name.
         * @param port the port name.
         * @return A vector of value.
         * @throw utils::ArgError if the model or if the port's model does not
         * exist.
         */
        value::ConstVectorView getValue(const std::string& model,
                                        const std::string& port) const;

        /**
         * @brief Retrive the constant vector or column of a specific index.
         * @param idx the index of the vector.
         * @return A Vector View.
         */
        value::ConstVectorView getValue(value::Matrix::size_type idx) const;

        /**
         * @brief Return the size of a vector.
         * @return A integer.
         */
        value::Matrix::index getVectorSize() const
        { return m_values->rows(); }

        /**
         * @brief Return the Matrix of the values.
         * @return A constant reference to the Matrix.
         */
        inline value::ConstMatrixView values() const
        { return m_values->getConstValue(); }

        /**
         * @brief Return the Matrix of the values.
         * @return A constant reference to the Matrix.
         */
        inline value::MatrixView values()
        { return m_values->value(); }

        inline value::Matrix* matrix()
        { return m_values; }

        /**
         * @brief Return the index of the specified couple model port.
         * @param model Name of the model.
         * @param port Name of the port.
         * @throw utils::ArgError if couple model port does not exist.
         * @return A index.
         */
        value::Matrix::index column(const std::string& model,
                                    const std::string& port) const;

        /**
         * @brief Return the index (model, port) to matrix index.
         *
         * @return A constant reference to the index.
         */
        inline const MapPairIndex& index() const
        { return m_colAccess; }

    private:
        value::Matrix* m_values; ///! the matrix.
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
