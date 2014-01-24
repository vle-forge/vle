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


#include <vle/vpz/Output.hpp>
#include <vle/value/Value.hpp>

namespace vle { namespace vpz {

Output::Output()
    : m_format(LOCAL), m_data(0)
{
}

Output::Output(const Output& output)
    : Base(output), m_format(output.m_format), m_name(output.m_name),
    m_plugin(output.m_plugin), m_location(output.m_location),
    m_package(output.m_package)
{
    if (output.m_data) {
        m_data = output.m_data->clone();
    } else {
        m_data = 0;
    }
}

Output& Output::operator=(const Output& output)
{
    Output tmp(output);
    swap(tmp);
    return *this;
}

Output::~Output()
{
    clearData();
}

void Output::swap(Output& output)
{
    std::swap(m_format, output.m_format);
    std::swap(m_name, output.m_name);
    std::swap(m_plugin, output.m_plugin);
    std::swap(m_location, output.m_location);
    std::swap(m_package, output.m_package);
    std::swap(m_data, output.m_data);
}

void Output::write(std::ostream& out) const
{
    out << "<output name=\"" << m_name.c_str() << "\" "
        << "location=\"" << m_location.c_str()
        << "\" ";

    switch (m_format) {
    case Output::LOCAL:
        out << "format=\"local\"";
        break;
    case Output::DISTANT:
        out << "format=\"distant\"";
        break;
    }

    if (not m_package.empty()) {
        out << " package=\"" << m_package.c_str() << "\" ";
    }

    out << " plugin=\"" << m_plugin.c_str() << "\" ";

    if (m_data) {
        out << ">\n";
        m_data->writeXml(out);
        out << "</output>\n";
    } else {
        out << "/>\n";
    }
}

void Output::setLocalStream(const std::string& location,
                            const std::string& plugin,
                            const std::string& package)
{
    if (plugin.empty()) {
        throw utils::ArgError(fmt(
                _("Output '%1%' have not plugin defined")) % m_name);
    }

    m_location.assign(location);
    m_plugin.assign(plugin);
    m_package.assign(package);
    m_format = Output::LOCAL;

    clearData();
}

void Output::setLocalStreamLocation(const std::string& location)
{
    m_location.assign(location);
    m_format = Output::LOCAL;
}

void Output::setLocalStream(const std::string& location,
                            const std::string& plugin)
{
    if (plugin.empty()) {
        throw utils::ArgError(fmt(
                _("Output '%1%' have not plugin defined")) % m_name);
    }

    m_location.assign(location);
    m_plugin.assign(plugin);
    m_package.clear();
    m_format = Output::LOCAL;

    clearData();
}

void Output::setDistantStream(const std::string& location,
                              const std::string& plugin,
                              const std::string& package)
{
    if (plugin.empty()) {
        throw utils::ArgError(fmt(
                _("Output '%1%' have not plugin defined")) % m_name);
    }

    m_location.assign(location);
    m_plugin.assign(plugin);
    m_package.assign(package);
    m_format = Output::DISTANT;

    clearData();
}

void Output::setDistantStream(const std::string& location,
                              const std::string& plugin)
{
    if (plugin.empty()) {
        throw utils::ArgError(fmt(
                _("Output '%1%' have not plugin defined")) % m_name);
    }

    m_location.assign(location);
    m_plugin.assign(plugin);
    m_package.clear();
    m_format = Output::DISTANT;

    clearData();
}

void Output::setData(value::Value* value)
{
    clearData();
    m_data = value;
}

void Output::clearData()
{
    delete m_data;
    m_data = 0;
}

bool Output::operator==(const Output& output) const
{
    return m_format == output.format() and m_name == output.name()
        and m_plugin == output.plugin() and m_location == output.location()
        and m_package == output.package() and m_data == output.data();

}

}} // namespace vle vpz
