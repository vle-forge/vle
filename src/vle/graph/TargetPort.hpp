/**
 * @file graph/TargetPort.hpp
 * @author The VLE Development Team.
 * @brief Define a couple graph::AtomicModel, port name to help
 * devs::Simulator to find destination models of external events.
 */

/*
 * Copyright (C) 2004, 05, 06 - The vle Development Team
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef VLE_GRAPH_TARGET_PORT_HPP
#define VLE_GRAPH_TARGET_PORT_HPP

#include <string>



namespace vle { namespace graph {

    class AtomicModel;

    /**
     * @brief Define a couple graph::AtomicModel, port name to help
     * graph::Simulator to find destination models of external events.
     *
     */
    class TargetPort
    {
    public:
	TargetPort(const std::string& portName,
		   AtomicModel* model) :
	    m_portName(portName),
	    m_model(model)
        { }

	inline const std::string& getPortName() const
        { return m_portName; }

        inline AtomicModel* getModel()
        { return m_model; }

    private:
	std::string   m_portName;
	AtomicModel*  m_model;
    };

}} // namespace vle graph

#endif
