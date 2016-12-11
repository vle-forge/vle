/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2016 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2016 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2016 INRA http://www.inra.fr
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
#include <vle/DllDefines.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/value/Null.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Table.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/XML.hpp>

namespace vle {
namespace vpz {

/**
 * @brief The ValueStackSax is used to construct the value::Value class and
 * more particulary, the complex value::Value like value::Set, value::Map,
 * value::Matrix.
 */
class VLE_LOCAL ValueStackSax {
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
    void pushMapKey(const std::string &key);

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
    value::Value *topValue();

    /**
     * @brief Add to the lastest complex value (value::Set, value::Map,
     * value::Matrix, value::Tuple or value::Table), a new value.
     * @param val the value to add.
     */
    template <typename T, typename... Args>
    void pushOnVectorValue(Args &&... args)
    {
        vle::value::Value *pointer = nullptr;

        //
        // If the m_valuestack is not empty, we build a std::unique_ptr and
        // attach the value to the top of the m_valuestack stack.
        //
        if (not m_valuestack.empty()) {
            if (m_valuestack.top()->isSet()) {
                auto value = std::unique_ptr<vle::value::Value>(
                    new T(std::forward<Args>(args)...));

                pointer = value.get();
                m_valuestack.top()->toSet().add(std::move(value));
            }
            else if (m_valuestack.top()->isMap()) {
                auto value = std::unique_ptr<vle::value::Value>(
                    new T(std::forward<Args>(args)...));

                pointer = value.get();
                m_valuestack.top()->toMap().add(m_lastkey, std::move(value));
            }
            else if (m_valuestack.top()->isMatrix()) {
                auto value = std::unique_ptr<vle::value::Value>(
                    new T(std::forward<Args>(args)...));

                value::Matrix &mx(m_valuestack.top()->toMatrix());
                if (not value->isNull()) {
                    pointer = value.get();
                    mx.addToLastCell(std::move(value));
                }

                mx.moveLastCell();
            }
        }
        else {
            //
            // Otherwise we are reading a @c std::vector of @c shared_ptr.
            //
            auto value = std::shared_ptr<vle::value::Value>(
                new T(std::forward<Args>(args)...));

            pointer = value.get();
            m_result.push_back(value);
        }

        if (pointer and
            (pointer->isSet() or pointer->isMap() or pointer->isTuple() or
             pointer->isTable() or pointer->isMatrix()))
            m_valuestack.push(pointer);
    }

    /**
     * @brief Pop the current head. If stack is empty, do nothing.
     */
    inline void pop() { m_valuestack.pop(); }

    /**
     * @brief Return true if this sax parser stack is empty.
     *
     * @return true if empty, false otherwise.
     */
    inline bool empty() const { return m_valuestack.empty(); }

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
    void pushResult(std::shared_ptr<value::Value> val);

    /**
     * @brief Get the value::Value from the values'vector at a specified
     * index.
     * @param i The index in the result vector.
     * @throw utils::SaxParserError if the index is greater that the
     * vector'size.
     * @return A constant reference to the value::Value.
     */
    const std::shared_ptr<value::Value> &getResult(size_t i) const;

    /**
     * @brief Get latest value::Value pushed into the values'vector.
     * @throw utils::SaxParserError if the vector of result is empty.
     * @return A constant reference to the latest pushed value::Value.
     */
    const std::shared_ptr<value::Value> &getLastResult() const;

    /**
     * @brief Get the vector of value::Value.
     * @return A constant reference to the vector of value::Value.
     */
    const std::vector<std::shared_ptr<value::Value>> &getResults() const
    {
        return m_result;
    }

    /**
     * @brief Get the vector of value::Value.
     * @return A reference to the vector of value::Value.
     */
    std::vector<std::shared_ptr<value::Value>> &getResults()
    {
        return m_result;
    }

private:
    /**
     * @brief Test if top of values stack are a composite like Map, Set,
     * Matrix etc.
     * @return True if top of the stack is composite.
     */
    bool isCompositeParent() const;

    /**
     * Store the value stack, usefull for composite value, Map, Set, Matrix.
     * @c m_valuestack does not take ownership of value. Just reference. The
     * @c value::Value objects are stored in @c std::shared_ptr m_result or in
     * std::unique_ptr shared by composite value.
     */
    std::stack<value::Value *> m_valuestack;

    /**
     * @brief Store result of Values parsing from trame, simple value,
     * factor.
     */
    std::vector<std::shared_ptr<value::Value>> m_result;

    /**
     * @brief Last map key read.
     */

    std::string m_lastkey;
};
}
} // namespace vle vpz

#endif
