/**
 * @file value/Tuple.hpp
 * @author The VLE Development Team.
 * @brief Tuple Value a container to Value class.
 */

/*
 * Copyright (C) 2003-2007 - The vle Development Team
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

#ifndef UTILS_VALUE_TUPLE_HPP
#define UTILS_VALUE_TUPLE_HPP

#include <vle/value/Value.hpp>
#include <vector>



namespace vle { namespace value {

    /**
     * @brief A Tuple Value is a container to store a list of double value into
     * an std::vector standard container. The XML format is:
     * @code
     * <tuple>0.1 0.2 0.3 0.4 0.5 0.6 0.7
     * 0.8 0.9 1.0</tuple>
     * @endcode
     *
     * @author The VLE Development Team.
     */
    class TupleFactory : public ValueBase
    {
    private:
        TupleFactory()
        { }

        TupleFactory(const TupleFactory& setfactory);

    public:
        typedef std::vector < double > TupleValue;
        typedef std::vector < double >::iterator iterator;
        typedef std::vector < double >::const_iterator const_iterator;


        virtual ~TupleFactory()
        { }

        static Tuple create();
            
        virtual Value clone() const;

        virtual ValueBase::type getType() const
        { return ValueBase::TUPLE; }

        /** 
         * @brief Push a value at the end of the std::vector.
         * 
         * @param value the value to push.
         */
        inline void addValue(const double value)
        { m_value.push_back(value); }

        inline TupleValue& getValue()
        { return m_value; }

        inline const TupleValue& getValue() const
        { return m_value; }

        inline bool empty() const
        { return m_value.empty(); }

        inline size_t size() const
        { return m_value.size(); }

        inline double operator[](const size_t i) const
        { return m_value[i]; }

        inline double& operator[](const size_t i)
        { return m_value[i]; }

        /** 
         * @brief Fill the current tuple with multiple reals read from a string.
         * 
         * @param str A string with [0.. x] real.
         * 
         * @throw utils::ArgError if string have problem.
         */
        void fill(const std::string& str);

        virtual std::string toFile() const;

        virtual std::string toString() const;

        virtual std::string toXML() const;

    private:
        TupleValue          m_value;
    };

    Tuple toTupleValue(const Value& value);

    const TupleFactory::TupleValue& toTuple(const Value& value);

}} // namespace vle value
#endif
