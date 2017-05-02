/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2017 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2017 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2017 INRA http://www.inra.fr
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

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/value/Tuple.hpp>

namespace {

inline double
pp_get(const vle::value::TupleValue& t, size_t i)
{
    if (i > t.size())
        throw vle::utils::ArgError(
          (vle::fmt(_("Tuple: bad accessor %1%")) % i).str());

    return t[i];
}

inline double&
pp_get(vle::value::TupleValue& t, size_t i)
{
    if (i > t.size())
        throw vle::utils::ArgError(
          (vle::fmt(_("Tuple: bad accessor %1%")) % i).str());

    return t[i];
}
}

namespace vle {
namespace value {

Tuple::Tuple(size_type n, double value)
  : m_value(n, value)
{
}

Value::type
Tuple::getType() const
{
    return Value::TUPLE;
}

void
Tuple::writeFile(std::ostream& out) const
{
    for (auto it = m_value.begin(); it != m_value.end(); ++it) {
        if (it != m_value.begin()) {
            out << " ";
        }
        out << *it;
    }
}

void
Tuple::writeString(std::ostream& out) const
{
    out << "(";
    for (auto it = m_value.begin(); it != m_value.end(); ++it) {
        if (it != m_value.begin()) {
            out << ",";
        }
        out << *it;
    }
    out << ")";
}

void
Tuple::writeXml(std::ostream& out) const
{
    out << "<tuple>";
    for (auto it = m_value.begin(); it != m_value.end(); ++it) {
        if (it != m_value.begin()) {
            out << " ";
        }
        out << *it;
    }
    out << "</tuple>";
}

double Tuple::operator[](size_type i) const
{
    return ::pp_get(m_value, i);
}

double& Tuple::operator[](size_type i)
{
    return ::pp_get(m_value, i);
}

double
Tuple::operator()(size_type i) const
{
    return ::pp_get(m_value, i);
}

double&
Tuple::operator()(size_type i)
{
    return ::pp_get(m_value, i);
}

double
Tuple::get(size_type i) const
{
    return ::pp_get(m_value, i);
}

double&
Tuple::get(size_type i)
{
    return ::pp_get(m_value, i);
}

double
Tuple::at(size_type i) const
{
    return ::pp_get(m_value, i);
}

double&
Tuple::at(size_type i)
{
    return ::pp_get(m_value, i);
}

void
Tuple::fill(const std::string& str)
{
    std::string cpy(str);
    boost::algorithm::trim(cpy);

    std::vector<std::string> result;
    boost::algorithm::split(
      result, cpy, boost::algorithm::is_any_of(" \n\t\r"));

    for (auto& elem : result) {
        boost::algorithm::trim(elem);
        if (not(elem).empty()) {
            try {
                m_value.push_back(boost::lexical_cast<double>(elem));
            } catch (const boost::bad_lexical_cast& e) {
                try {
                    m_value.push_back(boost::lexical_cast<long>(elem));
                } catch (const boost::bad_lexical_cast& e) {
                    throw vle::utils::ArgError(
                      (fmt("Can not convert string '%1%' into"
                           " double or long") %
                       (elem))
                        .str());
                }
            }
        }
    }
}

void
Tuple::remove(size_type i)
{
    m_value.erase(m_value.begin() + i);
}

void
Tuple::resize(size_type n, double value)
{
    m_value.resize(n, value);
}
}
} // namespace vle value
