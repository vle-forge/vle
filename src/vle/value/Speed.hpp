/** 
 * @file value/Speed.hpp
 * @brief 
 * @author The vle Development Team
 * @date mar, 06 déc 2005 11:28:06 +0100
 */

/*
 * Copyright (C) 2005 - The vle Development Team
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

#ifndef UTILS_VALUE_SPEED_HPP
#define UTILS_VALUE_SPEED_HPP

#include <utils/value/Value.hpp>

namespace vle { namespace value {

    template < class Class >
        class Speed : public Value
    {
    public:
        Speed(Class x) :
            m_value(x)
        { }

        Speed(xmlpp::Element* root)
        { }

        virtual Value* clone() const
        {
            return new Speed(m_value);
        }

        virtual Value::type getType() const
        {
            return Value::SPEED;
        }

        inline Class speed() const
        {
            return m_value;
        }

        inline std::string toFile() const
        {
            return utils::to_string(m_value);
        }

        inline std::string toString() const
        {
            return utils::to_string(m_value);
        }

        inline std::string toXML() const
        {
            std::string str("<SPEED>");
            str += utils::to_string(m_value);
            str += "</SPEED>";

            return str;
        }

    private:
        Class       m_value;    
    };

}} // namespace vle value
#endif
