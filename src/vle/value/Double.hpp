/**
 * @file Double.hpp
 * @author The VLE Development Team.
 * @brief Double Value.
 */

/*
 * Copyright (c) 2004, 2005 The vle Development Team
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

#ifndef UTILS_VALUE_DOUBLE_HPP
#define UTILS_VALUE_DOUBLE_HPP

#include <vle/value/Value.hpp>

namespace vle { namespace value {

    /**
     * @brief Double Value.
     */
    class Double : public Value
    {
    public:
        Double(double value = 0.0) :
            m_value(value)
        { }

        Double(xmlpp::Element* root);

        virtual Value* clone() const
        { return new Double(m_value); }

        virtual Value::type getType() const
        { return Value::DOUBLE; }

        inline double doubleValue() const
        { return m_value; }

        virtual std::string toFile() const;

        virtual std::string toString() const;

        virtual std::string toXML() const;

    private:
        double m_value;
    };

}} // namespace vle value
#endif
