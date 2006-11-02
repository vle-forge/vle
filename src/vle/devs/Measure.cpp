/**
 * @file devs/Measure.cpp
 * @author The VLE Development Team.
 * @brief Represent an output, a type of measure: timed or event with a time
 * step. A measure is attached to a list of observer.
 */

/*
 * Copyright (c) 2005 The VLE Development Team.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include <vle/devs/Measure.hpp>

using std::string;
using std::vector;

namespace vle { namespace devs {

Measure::Measure(const std::string& name,
		 const std::string& type,
		 const std::string& outputName):
    m_name(name),
    m_type(type),
    m_outputName(outputName),
    m_timeStep()
{
}


Measure::Measure(const std::string& name,
		 const std::string& type,
		 const std::string& outputName,
		 const std::string& timeStep):
    m_name(name),
    m_type(type),
    m_outputName(outputName),
    m_timeStep(timeStep)
{
}

void Measure::addObservable(const std::string& modelName,
			    const std::string& portName,
			    const std::string& group,
			    int index)
{
    observable v_observable;

    v_observable.modelName = modelName;
    v_observable.portName = portName;
    v_observable.group = group;
    v_observable.index = index;
    m_observableList.push_back(v_observable);
}


const std::vector < observable >& Measure::getObservableList() const
{
    return m_observableList;
}
 
}} // namespace vle devs
