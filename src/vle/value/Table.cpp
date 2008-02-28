/**
 * @file src/vle/value/Table.cpp
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




#include <vle/value/Table.hpp>
#include <vle/utils/Debug.hpp>



namespace vle { namespace value {

TableFactory::TableFactory(const int width, const int height) :
    m_value(boost::extents[width][height]),
    m_width(width),
    m_height(height)
{
}

TableFactory::TableFactory(const TableFactory& setfactory) :
    ValueBase(setfactory),
    m_value(setfactory.m_value),
    m_width(setfactory.m_width),
    m_height(setfactory.m_height)
{
}

Table TableFactory::create(const int width, const int height)
{
    return Table(new TableFactory(width, height));
}

Value TableFactory::clone() const
{
    return Value(new TableFactory(*this));
}

void TableFactory::fill(const std::string& str)
{
    std::string cpy(str);
    boost::algorithm::trim(cpy);

    std::vector < std::string > result;
    boost::algorithm::split(result, cpy, 
                            boost::algorithm::is_any_of(" \n\t\r"));

    index i = 0;
    index j = 0;
    for (std::vector < std::string >::iterator it = result.begin();
         it != result.end(); ++it) {
        boost::algorithm::trim(*it);
        if (not (*it).empty()) {
            double result;
            try {
                result = boost::lexical_cast < double >(*it);
            } catch(const boost::bad_lexical_cast& e) {
                try {
                    result = boost::lexical_cast < long >(*it);
                } catch(const boost::bad_lexical_cast& e) {
                    Throw(utils::ArgError,
                          ((boost::format("Can not convert string \'%1%\' into"
                                    " double or long") %
                            (*it)).str()));
                }
            }
            m_value[i][j] = result;
            if (j + 1 >= m_height) {
                j = 0;
                if (i + 1 >= m_width) {
                    return;
                } else {
                    i++;
                }
            } else {
                j++;
            }
        }
    }
}

std::string TableFactory::toFile() const
{
    std::string s;

    for (index i = 0; i < m_width; ++i) {
        for (index j = 0; j < m_height; ++j) {
	    s += boost::lexical_cast < std::string >(m_value[i][j]);
            s += " ";
        }
        s += "\n";
    }
    return s;
}

std::string TableFactory::toString() const
{
    std::string s = "(";
    
    for (index i = 0; i < m_width; ++i) {
        s += "(";
        for (index j = 0; j < m_height; ++j) {
	    s += boost::lexical_cast < std::string >(m_value[i][j]);
            s += ",";
        }
        s += ")";
    }
    s += ")";
    return s;
}

std::string TableFactory::toXML() const
{
    std::string s = (boost::format("<table width=\"%1%1\" heigth=\"%2%\">") %
                     m_width % m_height).str();
    for (index i = 0; i < m_width; ++i) {
        for (index j = 0; j < m_height; ++j) {
	    s += boost::lexical_cast < std::string >(m_value[i][j]);
            s += " ";
        }
    }
    s += "</table>";
    return s;
}

Table toTableValue(const Value& value)
{
    Assert(utils::ArgError, value->getType() == ValueBase::TABLE,
           "Value is not a Table");
    return boost::static_pointer_cast < TableFactory >(value);
}

const TableFactory::TableValue& toTable(const Value& value)
{
    Assert(utils::ArgError, value->getType() == ValueBase::TABLE,
           "Value is not a Table");
    return boost::static_pointer_cast < TableFactory >(value)->getValue();
}

}} // namespace vle value
