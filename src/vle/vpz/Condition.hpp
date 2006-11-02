/** 
 * @file vpz/Condition.hpp
 * @brief 
 * @author The vle Development Team
 * @date mar, 31 jan 2006 17:19:48 +0100
 */

/*
 * Copyright (C) 2006 - The vle Development Team
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef VLE_VPZ_CONDITION_HPP
#define VLE_VPZ_CONDITION_HPP

#include <vector>
#include <string>
#include <vle/vpz/Base.hpp>

namespace vle { namespace value {

    class Value;

}} // namespace vle value

namespace vle { namespace vpz {

    /** 
     * @brief A condition define a couple model name, port name and a Value.
     * This class allow loading and writing a condition.
     */
    class Condition : public Base
    {
    public:
        Condition();

        Condition(const Condition& cond);

        Condition& operator=(const Condition& cond);

        virtual ~Condition();

        virtual void init(xmlpp::Element* elt);

        virtual void write(xmlpp::Element* elt) const;

        /** 
         * @brief Set the information for this Condition.
         * 
         * @param modelname a new model name.
         * @param portname a new port name.
         */
        void setCondition(const std::string& modelname,
                          const std::string& portname);

        const std::string& modelname() const
        { return m_modelname; }

        const std::string& portname() const
        { return m_portname; }

        const std::vector < vle::value::Value* >& value() const
        { return m_value; }

        /** 
         * @brief Build a clone of the first condition from value list.
         * 
         * @return a cloned Value.
         *
         * @throw Exception::Internal if value list is empty.
         */
        vle::value::Value* firstValue() const;

        /** 
         * @brief Build a clone of the nth condition from value list.
         * 
         * @param i the value to clone.
         * 
         * @return a cloned Value.
         *
         * @throw Exception::Internal if value list have no nth value.
         */
        vle::value::Value* nValue(size_t i) const;
        
        /** 
         * @brief Add the value into the vector. Be carrefull, the value is not
         * cloned.
         * 
         * @param val the value to add.
         */
        void addValue(vle::value::Value* val);

        /** 
         * @brief Delete all value from condition.
         */
        void clearValue();


    private:
        std::string                         m_modelname;
        std::string                         m_portname;
        std::vector < vle::value::Value* >  m_value;
    };

}} // namespace vle vpz

#endif
