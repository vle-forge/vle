/**
 * @file devs/Measure.hpp
 * @author The VLE Development Team.
 * @brief Represent an output, a type of measure: timed or event with a time
 * step. A measure is attached to a list of observer.
 */

/*
 * Copyright (c) 2004, 2005 The VLE Development Team.
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

#ifndef DEVS_MEASURE_HPP
#define DEVS_MEASURE_HPP

#include <vector>
#include <string>

namespace vle { namespace devs {

    struct observable {
	std::string modelName;
	std::string portName;
	std::string group;
	int index;
    };


    /**
     * @brief Represent an output, a type of measure: timed or event with a
     * time step. A measure is attached to a list of observer.
     *
     */
    class Measure
    {
    public:
	Measure(const std::string& name,
		const std::string& type,
		const std::string& outputName);

	Measure(const std::string& name,
		const std::string& type,
		const std::string& outputName,
		const std::string& timeStep);

	void addObservable(const std::string& modelName,
			   const std::string& portName,
			   const std::string& group,
			   int index);

	std::vector < observable > const & getObservableList() const;

	const std::string& getName() const { return m_name; }

	const std::string& getOutputName() const { return m_outputName; }

	const std::string& getTimeStep() const { return m_timeStep; }

	const std::string& getType() const { return m_type; }

    private:
	std::vector < observable > m_observableList;
	std::string m_name;
	std::string m_type;
	std::string m_outputName;
	std::string m_timeStep;
    };

}} // namespace vle devs

#endif
