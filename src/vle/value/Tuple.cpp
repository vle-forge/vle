/**
 * @file vle/value/Tuple.cpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <vle/value/Tuple.hpp>
#include <vle/utils/Debug.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>



namespace vle { namespace value {

TupleFactory::TupleFactory(const TupleFactory& setfactory) :
    ValueBase(setfactory),
    m_value(setfactory.m_value)
{ }

Tuple TupleFactory::create()
{
    return Tuple(new TupleFactory());
}

Value TupleFactory::clone() const
{
    return Value(new TupleFactory(*this));
}

void TupleFactory::fill(const std::string& str)
{
    std::string cpy(str);
    boost::algorithm::trim(cpy);

    std::vector < std::string > result;
    boost::algorithm::split(result, cpy, 
                            boost::algorithm::is_any_of(" \n\t\r"));

    for (std::vector < std::string >::iterator it = result.begin();
         it != result.end(); ++it) {
        boost::algorithm::trim(*it);
        if (not (*it).empty()) {
            try {
                m_value.push_back(boost::lexical_cast < double >(*it));
            } catch(const boost::bad_lexical_cast& e) {
                try {
                    m_value.push_back(boost::lexical_cast < long >(*it));
                } catch(const boost::bad_lexical_cast& e) {
                    Throw(utils::ArgError,
                          ((boost::format("Can not convert string \'%1%\' into"
                                    " double or long") %
                            (*it)).str()));
                }
            }
        }
    }
}

std::string TupleFactory::toFile() const
{
    std::string s;

    for (const_iterator it = m_value.begin(); it != m_value.end(); ++it) {
	s += boost::lexical_cast < std::string >(*it);
        if (it + 1 != m_value.end())
            s+= " ";
    }
    return s;
}

std::string TupleFactory::toString() const
{
    std::string s = "(";
    for (const_iterator it = m_value.begin(); it != m_value.end(); ++it) {
	s += boost::lexical_cast < std::string >(*it);
	if (it + 1 != m_value.end())
	    s += ",";
    }
    s += ")";
    return s;
}

std::string TupleFactory::toXML() const
{
    std::string s = "<tuple>";
    for (const_iterator it = m_value.begin(); it != m_value.end(); ++it) {
        s += boost::lexical_cast < std::string >(*it);
        if (it + 1 != m_value.end())
            s += " ";
    }
    s += "</tuple>";
    return s;
}

Tuple toTupleValue(const Value& value)
{
    Assert(utils::ArgError, value->getType() == ValueBase::TUPLE,
           "Value is not a Tuple");
    return boost::static_pointer_cast < TupleFactory >(value);
}

const TupleFactory::TupleValue& toTuple(const Value& value)
{
    Assert(utils::ArgError, value->getType() == ValueBase::TUPLE,
           "Value is not a Tuple");
    return boost::static_pointer_cast < TupleFactory >(value)->getValue();
}

}} // namespace vle value
