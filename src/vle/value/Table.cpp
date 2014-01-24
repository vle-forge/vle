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


#include <vle/value/Table.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/lexical_cast.hpp>

namespace vle { namespace value {

void Table::writeFile(std::ostream& out) const
{
    for (index j = 0; j < m_height; ++j) {
        for (index i = 0; i < m_width; ++i) {
            out << m_value[i][j] << " ";
        }
        out << "\n";
    }
}

void Table::writeString(std::ostream& out) const
{
    out << "(";

    for (index j = 0; j < m_height; ++j) {
        out << "(";
        for (index i = 0; i < m_width; ++i) {
            out << m_value[i][j];
            if (i + 1 < m_width) {
                out << ",";
            }
        }
        if (j + 1 < m_height) {
            out << "),";
        } else {
            out << ")";
        }
    }
    out << ")";
}

void Table::writeXml(std::ostream& out) const
{
    out << "<table width=\"" << m_width << "\" height=\"" << m_height << "\" >";
    for (index j = 0; j < m_height; ++j) {
        for (index i = 0; i < m_width; ++i) {
            out << m_value[i][j] << " ";
        }
    }
    out << "</table>";
}

void Table::fill(const std::string& str)
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
                    throw utils::ArgError(fmt(_(
                            "Can not convert string \'%1%\' into"
                            " double or long")) % (*it));
                }
            }

            m_value[i][j] = result;
            if (i + 1 >= m_width) {
                i = 0;
                if (j + 1 >= m_height) {
                    return;
                } else {
                    j++;
                }
            } else {
                i++;
            }
        }
    }
}

}} // namespace vle value

