/**
 * @file vle/oov/plugins/Storage.cpp
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


#include <vle/oov/plugins/Storage.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/String.hpp>
#include <vle/value/XML.hpp>
#include <vle/value/Null.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/numeric/conversion/cast.hpp>

namespace vle { namespace oov { namespace plugin {

Storage::Storage(const std::string& location) :
    Plugin(location),
    m_matrix(2, 2, 1, 1),
    m_time(-1.0),
    m_isstart(false)
{
}

Storage::~Storage()
{
}

bool Storage::isSerializable() const
{
    return true;
}

value::Value* Storage::serialize() const
{
    return m_matrix.serialize();
}

void Storage::deserialize(const value::Value& vals)
{
    m_matrix.deserialize(vals);
}

std::string Storage::name() const
{
    return std::string("storage");
}

void Storage::onParameter(const std::string& /* plugin */,
                          const std::string& /* location */,
                          const std::string& /* file */,
                          const std::string& parameters,
                          const double& /* time */)
{
    std::istringstream param(parameters);
    int columns = -1, rows = -1, rzcolumns = -1, rzrows = -1;

    if (param) {
        param >> columns;
        if (param) {
            param >> rows;
            if (param) {
                param >> rzcolumns;
                if (param) {
                    param >> rzrows;
                }
            }
        }
    }
    m_matrix.resize(columns > 1 ? columns : 10, rows > 1 ? rows : 10);
    m_matrix.updateStep(rzcolumns > 0 ? rzcolumns : 1, rzrows > 0 ? rzrows : 1);
}

void Storage::onNewObservable(const std::string& simulator,
                              const std::string& parent,
                              const std::string& port,
                              const std::string& /* view */,
                              const double& /* time */)
{
    std::string name(parent);
    name += simulator;
    m_matrix.addModel(name, port);
}

void Storage::onDelObservable(const std::string& /* simulator */,
                              const std::string& /* parent */,
                              const std::string& /* port */,
                              const std::string& /* view */,
                              const double& /* time */)
{
}

void Storage::onValue(const std::string& simulator,
                      const std::string& parent,
                      const std::string& port,
                      const std::string& /* view */,
                      const double& time,
                      value::Value* value)
{
    if (m_isstart) {
        nextTime(time);
    } else {
        if (m_time < .0) {
            m_time = time;
        } else {
            nextTime(time);
            m_isstart = true;
        }
    }

    std::string name(parent);
    name += simulator;
    m_matrix.addValue(name, port, value);
}

void Storage::close(const double& /* time */)
{
    m_matrix.setLastTime(m_time);
}

value::VectorView Storage::getTime()
{
    return m_matrix.getTime();
}

void Storage::nextTime(double trame_time)
{
    if (trame_time != m_time) {
        m_matrix.setLastTime(m_time);
	m_time = trame_time;
    }
}

}}} // namespace vle oov plugin
