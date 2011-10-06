/*
 * @file vle/oov/plugins/Storage.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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
#include <vle/value/Map.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/String.hpp>
#include <vle/value/XML.hpp>
#include <vle/value/Null.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/numeric/conversion/cast.hpp>

namespace vle { namespace oov { namespace plugin {

Storage::Storage(const std::string& location)
    : Plugin(location), m_matrix(2, 2, 1, 1), m_time(-1.0), m_isstart(false)
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
                          value::Value* parameters,
                          const double& /* time */)
{
    if (parameters) {
        if (not parameters->isMap()) {
            throw utils::ArgError(
                _("Storage: initialization failed, bad parameters"));
        }
        value::Map* init = dynamic_cast < value::Map* >(parameters);
        int columns = -1, rows = -1, rzcolumns = -1, rzrows = -1;

        if (init->exist("columns")) {
            columns = value::toInteger(init->get("columns"));
        }

        if (init->exist("rows")) {
            rows = value::toInteger(init->get("rows"));
        }

        if (init->exist("inc_columns")) {
            rzcolumns = value::toInteger(init->get("inc_columns"));
        }

        if (init->exist("inc_rows")) {
            rzrows = value::toInteger(init->get("inc_rows"));
        }

        m_matrix.resize(columns > 1 ? columns : 10,
                        rows > 1 ? rows : 10);
        m_matrix.updateStep(rzcolumns > 0 ? rzcolumns : 1,
                            rzrows > 0 ? rzrows : 1);

        delete parameters;
    }
}

void Storage::onNewObservable(const std::string& simulator,
                              const std::string& parent,
                              const std::string& port,
                              const std::string& /* view */,
                              const double& /* time */)
{
    std::string name(parent);
    name += ":";
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

    if (not simulator.empty()) {
        std::string name(parent);
        name += ":";
        name += simulator;
        m_matrix.addValue(name, port, value);
    }
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
