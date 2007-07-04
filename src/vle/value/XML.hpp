/**
 * @file value/XML.hpp
 * @author The VLE Development Team.
 * @brief A XML Value.
 */

/*
 * Copyright (c) 2007 The vle Development Team
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

#ifndef UTILS_VALUE_XML_HPP
#define UTILS_VALUE_XML_HPP

#include <vle/value/Value.hpp>

namespace vle { namespace value {

    /**
     * @brief A XML Value.
     */
    class XMLFactory : public ValueBase
    {
    private:
        XMLFactory(const std::string& value) :
            m_value(value)
        { }

    public:
        static XML create(const std::string& value = std::string());

        virtual Value clone() const;

        virtual ValueBase::type getType() const
        { return ValueBase::XMLTYPE; }

        inline const std::string& stringValue() const
        { return m_value; }

        virtual std::string toFile() const
        { return m_value; }

        virtual std::string toString() const
        { return m_value; }

        virtual std::string toXML() const;

    private:
        std::string     m_value;
    };

}} // namespace vle value
#endif
