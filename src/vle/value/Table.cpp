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
#include <vle/value/Table.hpp>

namespace {

inline double&
pp_get(vle::value::TableValue& t,
       std::size_t x,
       std::size_t y,
       std::size_t width)
{
    auto i = y * width + x;

    if (i > t.size())
        throw vle::utils::ArgError(
          (vle::fmt(_("Table: bad accessor %1%x%2%")) % x % y).str());

    return t[i];
}

inline double
pp_get(const vle::value::TableValue& t,
       std::size_t x,
       std::size_t y,
       std::size_t width)
{
    auto i = y * width + x;

    if (i > t.size())
        throw vle::utils::ArgError(
          (vle::fmt(_("Table: bad accessor %1%x%2%")) % x % y).str());

    return t[i];
}
}

namespace vle {
namespace value {

Table::Table()
  : m_value(1, 0.0)
  , m_width(1)
  , m_height(1)
{
}

Table::Table(std::size_t width, std::size_t height)
  : m_value(width * height)
  , m_width(width)
  , m_height(height)
{
    if (width * height <= 0)
        throw utils::ArgError(
          (fmt(_("Table: bad constructor initialization %1%x%2%")) % width %
           height)
            .str());
}

Value::type
Table::getType() const
{
    return Value::TABLE;
}

void
Table::writeFile(std::ostream& out) const
{
    for (index j = 0; j < m_height; ++j) {
        for (index i = 0; i < m_width; ++i) {
            out << get(i, j) << " ";
        }
        out << "\n";
    }
}

void
Table::writeString(std::ostream& out) const
{
    out << "(";

    for (index j = 0; j < m_height; ++j) {
        out << "(";
        for (index i = 0; i < m_width; ++i) {
            out << get(i, j);
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

void
Table::writeXml(std::ostream& out) const
{
    out << "<table width=\"" << m_width << "\" height=\"" << m_height
        << "\" >";
    for (index j = 0; j < m_height; ++j) {
        for (index i = 0; i < m_width; ++i) {
            out << get(i, j) << " ";
        }
    }
    out << "</table>";
}

void
Table::resize(std::size_t width, std::size_t height)
{
    if (width * height <= 0)
        throw utils::ArgError(
          (fmt(_("Table: bad constructor initialization %1%x%2%")) % width %
           height)
            .str());

    if (width == m_width and height == m_height)
        return;

    std::vector<double> tmp(width * height);

    auto min_r = std::min(m_width, width);
    auto min_c = std::min(m_height, height);

    for (std::size_t r = 0; r < min_r; ++r)
        for (std::size_t c = 0; c < min_c; ++c)
            tmp[c * width + r] = m_value[c * m_width + r];

    std::swap(tmp, m_value);

    m_width = width;
    m_height = height;
}

double
Table::operator()(std::size_t x, std::size_t y) const
{
    return ::pp_get(m_value, x, y, m_width);
}

double&
Table::operator()(std::size_t x, std::size_t y)
{
    return ::pp_get(m_value, x, y, m_width);
}

double
Table::get(std::size_t x, std::size_t y) const
{
    return ::pp_get(m_value, x, y, m_width);
}

double&
Table::get(std::size_t x, std::size_t y)
{
    return ::pp_get(m_value, x, y, m_width);
}

void
Table::fill(const std::string& str)
{
    std::string cpy(str);
    boost::algorithm::trim(cpy);

    std::vector<std::string> result;
    boost::algorithm::split(
      result, cpy, boost::algorithm::is_any_of(" \n\t\r"));

    index i = 0;
    index j = 0;
    for (auto& elem : result) {
        boost::algorithm::trim(elem);
        if (not(elem).empty()) {
            double result;
            try {
                result = boost::lexical_cast<double>(elem);
            } catch (const boost::bad_lexical_cast& e) {
                try {
                    result = boost::lexical_cast<long>(elem);
                } catch (const boost::bad_lexical_cast& e) {
                    throw vle::utils::ArgError(
                      (fmt(_("Can not convert string \'%1%\' into"
                             " double or long")) %
                       (elem))
                        .str());
                }
            }

            get(i, j) = result;
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
}
} // namespace vle value
