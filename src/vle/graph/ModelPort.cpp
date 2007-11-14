/** 
 * @file graph/ModelPort.cpp
 * @brief Represent the relation between a Model and an input or output port
 * specified by a constant string.
 * @author The vle Development Team
 * @date lun, 02 jui 2007 11:45:50 +0200
 */

/*
 * Copyright (C) 2007 - The vle Development Team
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

#include <vle/graph/ModelPort.hpp>
#include <vle/graph/Model.hpp>
#include <vle/utils/Debug.hpp>



namespace vle { namespace graph {

ModelPort::ModelPort(Model* model, const std::string& portname) :
    m_model(model),
    m_port(portname)
{
    Assert(utils::DevsGraphError, model,
           "Build a ModelPort without model");

    Assert(utils::DevsGraphError, not portname.empty(),
           "Build a ModelPort without portname");

    Assert(utils::DevsGraphError, model->existInputPort(portname) or
           model->existOutputPort(portname), boost::format(
               "Build a ModelPort '(%1%, %2%)' and model does not have this port")
           % model->getName() % portname);
}


}} // namespace vle graph
