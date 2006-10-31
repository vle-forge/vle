/**
 * @file sCoupledModel.cpp
 * @author The VLE Development Team.
 * @brief
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

#include <vle/devs/sCoupledModel.hpp>
#include <vle/devs/Coordinator.hpp>
#include <vle/devs/Simulator.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/graph/TargetPort.hpp>
#include <vle/graph/Connection.hpp>

namespace vle { namespace devs {

sCoupledModel::sCoupledModel(graph::CoupledModel* c, Simulator* simulator) :
    sModel(simulator),
    m_coupledModel(c)
{
}

std::vector < graph::Model* > sCoupledModel::getModelList() const
{
    return m_coupledModel->getModelList();
}

bool sCoupledModel::isAtomic() const
{
    return false;
}

bool sCoupledModel::isCoupled() const
{
    return true;
}

graph::Model* sCoupledModel::findModel(const std::string & name) const
{
    return m_coupledModel->findModel(name);
}

graph::Model* sCoupledModel::getModel(const std::string& modelName)
{
    return m_coupledModel->getModel(modelName);
}

graph::CoupledModel* sCoupledModel::getStructure() const
{
    return m_coupledModel;
}

bool sCoupledModel::parseXML(xmlNodePtr, Simulator*, graph::CoupledModel*)
{
    return true;
}

void sCoupledModel::processInitEvent(InitEvent*)
{
}

StateEvent* sCoupledModel::processStateEvent(StateEvent*) const
{
    return NULL;
}

sCoupledModel::~sCoupledModel()
{
    delete m_coupledModel;
}

}} // namespace vle devs

