/**
 * @file Port.cpp
 * @author The VLE Development Team.
 * @brief Represent a DEVS port for DEVS model. This class is member of the
 * DEVS graph.
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

#include <vle/graph/Port.hpp>
#include <vle/graph/Model.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/value/Value.hpp>



namespace vle { namespace graph {

Port::Port(Model* model, const std::string& name) :
    m_model(model),
    m_name(name)
{
    Assert(vle::utils::InternalError, model, boost::format(
            "Bad model address while building port %1%\n") % name);
}

const std::string& Port::getModelName() const
{
    return m_model->getName();
}

void Port::setStructure(const vle::value::Value* val)
{
    delete m_structure;
    m_structure = (val == 0) ? 0 : val->clone();
}
    
bool Port::isValidWithStructure(const vle::value::Value* val)
{
    return val == m_structure; // FIXME test de map.
}

}} // namespace vle graph
