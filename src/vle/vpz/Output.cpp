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

#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/value/Value.hpp>
#include <vle/vpz/Output.hpp>

namespace vle {
namespace vpz {

Output::Output() = default;

Output::Output(const Output& output)
  : Base(output)
  , m_name(output.m_name)
  , m_plugin(output.m_plugin)
  , m_location(output.m_location)
  , m_package(output.m_package)
  , m_data()
{
    if (output.m_data)
        m_data = output.m_data->clone();
}

Output&
Output::operator=(const Output& output)
{
    Output tmp(output);
    swap(tmp);
    return *this;
}

Output::~Output()
{
    clearData();
}

void
Output::swap(Output& output)
{
    std::swap(m_name, output.m_name);
    std::swap(m_plugin, output.m_plugin);
    std::swap(m_location, output.m_location);
    std::swap(m_package, output.m_package);
    std::swap(m_data, output.m_data);
}

void
Output::write(std::ostream& out) const
{
    out << "<output name=\"" << m_name.c_str() << "\" "
        << "location=\"" << m_location.c_str() << "\" ";

    if (not m_package.empty())
        out << "package=\"" << m_package.c_str() << "\" ";

    out << "plugin=\"" << m_plugin.c_str() << "\" ";

    if (m_data) {
        out << ">\n";
        m_data->writeXml(out);
        out << "</output>\n";
    } else {
        out << "/>\n";
    }
}

void
Output::setStream(const std::string& location,
                  const std::string& plugin,
                  const std::string& package)
{
    if (plugin.empty()) {
        throw utils::ArgError(
          (fmt(_("Output '%1%' have not plugin defined")) % m_name).str());
    }

    m_location.assign(location);
    m_plugin.assign(plugin);
    m_package.assign(package);

    clearData();
}

void
Output::setStreamLocation(const std::string& location)
{
    m_location.assign(location);
}

void
Output::setStream(const std::string& location, const std::string& plugin)
{
    if (plugin.empty()) {
        throw utils::ArgError(
          (fmt(_("Output '%1%' have not plugin defined")) % m_name).str());
    }

    m_location.assign(location);
    m_plugin.assign(plugin);
    m_package.clear();

    clearData();
}

void
Output::setData(std::shared_ptr<value::Value> value)
{
    m_data = std::move(value);
}

void
Output::clearData()
{
    m_data.reset();
}

bool
Output::operator==(const Output& output) const
{
    return m_name == output.name() and m_plugin == output.plugin() and
           m_location == output.location() and
           m_package == output.package() and m_data == output.data();
}
}
} // namespace vle vpz
