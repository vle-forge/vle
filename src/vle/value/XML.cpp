/**
 * @file vle/value/XML.cpp
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


#include <vle/value/XML.hpp>
#include <vle/utils/Debug.hpp>



namespace vle { namespace value {

XML XMLFactory::create(const std::string& value)
{
    return XML(new XMLFactory(value));
}

Value XMLFactory::clone() const
{
    return Value(new XMLFactory(m_value));
}

std::string XMLFactory::toXML() const
{
    std::string val("<xml>\n<![CDATA[\n");
    val += m_value;
    val += "\n]]>\n</xml>";
    return val;
}

XML toXmlValue(const Value& value)
{
    Assert(utils::ArgError, value->getType() == ValueBase::XMLTYPE,
           "Value is not a XML");
    return boost::static_pointer_cast < XMLFactory >(value);
}

const std::string& toXml(const Value& value)
{
    Assert(utils::ArgError, value->getType() == ValueBase::XMLTYPE,
           "Value is not a XML");
    return boost::static_pointer_cast < XMLFactory >(value)->stringValue();
}

}} // namespace vle value
