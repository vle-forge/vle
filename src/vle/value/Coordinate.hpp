/** 
 * @file Coordinate.hpp
 * @brief 
 * @author The vle Development Team
 * @date mar, 06 déc 2005 18:21:51 +0100
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

#ifndef UTILS_VALUE_COORDINATE_HPP
#define UTILS_VALUE_COORDINATE_HPP

#include <utils/value/Value.hpp>
#include <utils/geometry/Point.hpp>

namespace vle { namespace value {

    template < size_t n, class Class >
        class Coordinate : public Value
    {
    public:
        Coordinate(Class x, Class y) :
            m_value(x, y)
        { }

        Coordinate(xmlpp::Element* root)
        { }

        virtual Value* clone() const
        {
            return new Coordinate(m_value);
        }

        virtual Value::type getType() const
        {
            return Value::COORDINATE;
        }

        inline utils::geometry::Point < n, Class >& coordinate()
        {
            return m_value;
        }

        inline const utils::geometry::Point < n, Class >& coordinate() const
        {
            return m_value;
        }

        virtual std::string toFile() const
        {
            std::string str;

            for (size_t sz = 0; sz < n; ++sz) {
                str += utils::to_string(m_value[sz]);
                if (sz + 1 <= n) {
                    str += ' ';
                }
            }

            return str;
        }

        virtual std::string toString() const
        {
            std::string str('(');

            for (size_t sz = 0; sz < n; ++sz) {
                str += utils::to_string(m_value[sz]);
                if (sz + 1 <= n) {
                    str += ", ";
                }
            }
            str += ')';

            return str;
        }

        virtual std::string toXML() const
        {
            std::string str("<COORDINATE><VAL>");
            for (size_t sz = 0; sz < n; ++sz) {
                str += utils::to_string(m_value[sz]);
                if (sz + 1 <= n) {
                    str += "</VAL><VAL>";
                }
            }
            str += "</VAL></Coordinate>";

            return str;
        }

    private:
        utils::geometry::Point < n, Class >     m_value;
    };

}} // namespace vle value
#endif
