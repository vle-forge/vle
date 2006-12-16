/** 
 * @file value/Direction.hpp
 * @brief 
 * @author The vle Development Team
 * @date mar, 06 déc 2005 18:26:32 +0100
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

#ifndef UTILS_VALUE_DIRECTION_HPP
#define UTILS_VALUE_DIRECTION_HPP

#include <utils/value/Value.hpp>
#include <utils/geometry/Point.hpp>

namespace vle { namespace value {

    template < size_t n, class Class >
    class DirectionFactory : public ValueBase
    {
    private:
        DirectionFactory(Class x, Class y) :
            m_value(x, y)
        { }

    public:
        virtual ~DirectionFactory()
        { }

        static Direction create(Class x, Class y)
        {
            return Direction(new DirectionFactory(x, y));
        }

        virtual Value clone() const
        {
            return Direction(new DirectionFactory(m_value));
        }

        virtual ValueBase::type getType() const
        {
            return ValueBase::DIRECTION;
        }

        inline utils::geometry::Point < n, Class >& direction()
        {
            return m_value;
        }

        inline const utils::geometry::Point < n, Class >& direction() const
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
            std::string str("<DIRECTION><VAL>");
            for (size_t sz = 0; sz < n; ++sz) {
                str += utils::to_string(m_value[sz]);
                if (sz + 1 <= n) {
                    str += "</VAL><VAL>";
                }
            }
            str += "</VAL></DIRECTION>";

            return str;
        }

    private:
        utils::geometry::Point < n, Class >     m_value;
    };

}} // namespace vle value
#endif
