/** 
 * @file value/ConstPointer.hpp
 * @brief Constant pointer value.
 * @author The vle Development Team
 * @date dim, 04 déc 2005 12:49:20 +0100
 */

/*
 * Copyright (c) 2005 The vle Development Team
 *
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifndef UTILS_VALUE_CONSTPOINTER_HPP
#define UTILS_VALUE_CONSTPOINTER_HPP

#include <vle/value/Value.hpp>
#include <vle/utils/Tools.hpp>

namespace vle { namespace value {

    /**
     * @brief Pointer Value. This class is base on a reference to a constant
     * pointer. This technics allows a quick message manipulation. Be carefull,
     * you must use this reference immediatly and do not store this reference.
     * The reference can be destroyed or displaced.
     */
    class ConstPointer : public Value
    {
    public:
        ConstPointer(const void* pointer) :
            m_value(pointer)
        {
            Assert(utils::InternalError, pointer,
                   "Pointer Value must have a correct reference.");
        }

        virtual ~ConstPointer()
        { }

        virtual Value* clone() const
        { return new ConstPointer(m_value); }

        virtual Value::type getType() const
        { return Value::POINTER; }

        virtual std::string toFile() const
        { return utils::to_string(m_value); }

        virtual std::string toString() const
        { return utils::to_string(m_value); }

        virtual std::string toXML() const
        { return utils::to_string(m_value); }

        const void* ref() const
        { return m_value; }

    protected:
        const void* m_value;
    };

}} // namespace vle value
#endif
