/**
 * @file value/Table.hpp
 * @author The VLE Development Team.
 * @brief array Value a container to Value class.
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

#ifndef UTILS_VALUE_TABLE_HPP
#define UTILS_VALUE_TABLE_HPP

#include <vle/value/Value.hpp>
#include <boost/multi_array.hpp>

namespace vle { namespace value {

    /**
     * @brief A table is a container for double value into an
     * boost::multi_array < double, 2 >. The XML format is:
     * @code
     * <table row="2" width="3">
     * 1 2 3 4 5 6
     * </table>
     * @endcode
     *
     * @author The VLE Development Team.
     */
    class TableFactory : public ValueBase
    {
    private:
        TableFactory(const size_t width, const size_t height);

        TableFactory(const TableFactory& setfactory);

    public:
        typedef boost::array < double, 2 > TableValue;

        virtual ~TableFactory()
        { }

        static Table create(const size_t width, const size_t height);
            
        virtual Value clone() const;

        virtual ValueBase::type getType() const
        { return ValueBase::TABLE; }

        inline TableValue& getValue()
        { return m_value; }

        inline const TableValue& getValue() const
        { return m_value; }

        inline bool empty() const
        { return m_value.empty(); }

        virtual std::string toFile() const;

        virtual std::string toString() const;

        virtual std::string toXML() const;

    private:
        TableValue      m_value;
        size_t          m_width;
        size_t          m_height;
    };

}} // namespace vle value
#endif
