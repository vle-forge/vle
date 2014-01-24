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


#ifndef VLE_SAX_STACK_VALUE_HPP
#define VLE_SAX_STACK_VALUE_HPP

#include <stack>
#include <vle/value/Value.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/value/Table.hpp>
#include <vle/DllDefines.hpp>

namespace vle { namespace vpz {

    /**
     * @brief The ValueStackSax is used to construct the value::Value class and
     * more particulary, the complex value::Value like value::Set, value::Map,
     * value::Matrix.
     */
    class VLE_API ValueStackSax
    {
    public:
        /**
         * @brief Add an integer to the stack.
         */
        void pushInteger();

        /**
         * @brief Add a boolean to the stack.
         */
        void pushBoolean();

        /**
         * @brief Add a string to the stack.
         */
        void pushString();

        /**
         * @brief Add a double to the stack.
         */
        void pushDouble();

        /**
         * @brief Add a value::Map to the stack.
         */
        void pushMap();

        /**
         * @brief Add a key of a value::Map the to stack.
         * @param key the name of the key.
         */
        void pushMapKey(const std::string& key);

        /**
         * @brief Add a value::Set to the stack.
         */
        void pushSet();

        /**
         * @brief Add a value::Matrix to the stack.
         * @param col number of columns.
         * @param row number of rows.
         * @param colmax buffer of columns.
         * @param rowmax buffer of rows.
         * @param colstep number of columns to add when resize the matrix'columns.
         * @param rowstep number of rows to add when resize the matrix'rows.
         */
        void pushMatrix(value::Matrix::index col,
                        value::Matrix::index row,
                        value::Matrix::index colmax,
                        value::Matrix::index rowmax,
                        value::Matrix::index colstep,
                        value::Matrix::index rowstep);

        /**
         * @brief Add a value::Tuple to the stack.
         */
        void pushTuple();

        /**
         * @brief Add a value::Table to the stack.
         * @param width the width of the value::Table.
         * @param height the height of the value::Table.
         */
        void pushTable(const size_t width, const size_t height);

        /**
         * @brief Add a value::XML to the stack.
         */
        void pushXml();

        /**
         * @brief Add a value::Null to the stack.
         */
        void pushNull();

        /**
         * @brief Pop the latest pushed value. If the stack is empty, nothing is
         * deleted.
         */
        void popValue();

        /**
         * @brief Get the latest value pushed into the stack.
         * @return the latest value.
         * @throw utils::SaxParserError if the stack is empty.
         */
        value::Value* topValue();

        /**
         * @brief Add to the lastest complex value (value::Set, value::Map,
         * value::Matrix, value::Tuple or value::Table), a new value.
         * @param val the value to add.
         */
        void pushOnVectorValue(value::Value* val);

        /**
         * @brief Pop the current head. If stack is empty, do nothing.
         */
        inline void pop()
        { m_valuestack.pop(); }

        /**
         * @brief Return true if this sax parser stack is empty.
         *
         * @return true if empty, false otherwise.
         */
        inline bool empty() const
        { return m_valuestack.empty(); }

        /**
         * @brief Clear stack value and result stack value.
         */
        void clear();

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Get/Set functions
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Add a value::Value to the values'vector.
         * @param val The value::Value to add.
         */
        inline void pushResult(value::Value* val)
        { m_result.push_back(val); }

        /**
         * @brief Get the value::Value from the values'vector at a specified
         * index.
         * @param i The index in the result vector.
         * @throw utils::SaxParserError if the index is greater that the
         * vector'size.
         * @return A constant reference to the value::Value.
         */
        inline value::Value* getResult(size_t i) const
        {
            if (m_result.size() < i) {
                throw utils::SaxParserError(fmt(
                        _("Get result value with to big index %1%.")) % i);
            }

            return m_result[i];
        }

        /**
         * @brief Get latest value::Value pushed into the values'vector.
         * @throw utils::SaxParserError if the vector of result is empty.
         * @return A constant reference to the latest pushed value::Value.
         */
        inline value::Value* getLastResult() const
        {
            if (m_result.empty()) {
                throw utils::SaxParserError(
                    _("Get last result value with empty result vector"));
            }

            return m_result[m_result.size() - 1];
        }

        /**
         * @brief Get the vector of value::Value.
         * @return A constant reference to the vector of value::Value.
         */
        const std::vector < value::Value* >& getResults() const
        { return m_result; }

        /**
         * @brief Get the vector of value::Value.
         * @return A reference to the vector of value::Value.
         */
        std::vector < value::Value* >& getResults()
        { return m_result; }

    private:
        /**
         * @brief Test if top of values stack are a composite like Map, Set,
         * Matrix etc.
         * @return True if top of the stack is composite.
         */
        bool isCompositeParent() const;

        /**
         * @brief Store the value stack, usefull for composite value, Map, Set,
         * Matrix.
         */
        std::stack < value::Value* >  m_valuestack;

        /**
         * @brief Store result of Values parsing from trame, simple value,
         * factor.
         */
        std::vector < value::Value* > m_result;

        /**
         * @brief Last map key read.
         */
        std::string m_lastkey;
    };


}} // namespace vle vpz

#endif
