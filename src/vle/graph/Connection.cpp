/**
 * @file graph/Connection.cpp
 * @author The VLE Development Team.
 * @brief Represent a DEVS connection between two models: internal, input or
 * output. This class is member of the DEVS Graph.
 */

/*
 * Copyright (C) 2006 - The vle Development Team
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

#include <vle/graph/Connection.hpp>
#include <vle/graph/Model.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Debug.hpp>



namespace vle { namespace graph {

Connection::Connection() :
    m_originModel(0),
    m_originPort(0),
    m_destinationModel(0),
    m_destinationPort(0)
{ }

Connection::Connection(Model* originModel,
                       Port* originPort,
                       Model* destinationModel,
                       Port* destinationPort) :
    m_originModel(originModel),
    m_originPort(originPort),
    m_destinationModel(destinationModel),
    m_destinationPort(destinationPort)
{
    Assert(vle::utils::ParseError, originModel,
           "Make connection with unknow origin model.");

    Assert(vle::utils::ParseError, destinationModel,
           "Make connection with unknow destination model.");

    Assert(vle::utils::ParseError, destinationPort,
           boost::format("Make connection with unknow destination port on "
                          "model '%1%'") % destinationModel->getName());

    Assert(vle::utils::ParseError, originPort,
           boost::format("Make connection with unknow origin port on model "
                          "'%1%") % originModel->getName());
}

}} // namespace vle graph
