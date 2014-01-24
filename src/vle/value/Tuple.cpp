/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <vle/value/Tuple.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/lexical_cast.hpp>

namespace vle { namespace value {

void Tuple::writeFile(std::ostream& out) const
{
    for (const_iterator it = m_value.begin(); it != m_value.end(); ++it) {
        if (it != m_value.begin()) {
            out << " ";
        }
        out << *it;
    }
}

void Tuple::writeString(std::ostream& out) const
{
    out << "(";
    for (const_iterator it = m_value.begin(); it != m_value.end(); ++it) {
        if (it != m_value.begin()) {
            out << ",";
        }
        out << *it;
    }
    out << ")";
}

void Tuple::writeXml(std::ostream& out) const
{
    out << "<tuple>";
    for (const_iterator it = m_value.begin(); it != m_value.end(); ++it) {
        if (it != m_value.begin()) {
            out << " ";
        }
        out << *it;
    }
    out << "</tuple>";
}

void Tuple::fill(const std::string& str)
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
                    throw utils::ArgError(fmt(
                                "Can not convert string '%1%' into"
                                " double or long") % (*it));
                }
            }
        }
    }
}

}} // namespace vle value

