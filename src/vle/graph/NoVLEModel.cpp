/**
 * @file graph/NoVLEModel.cpp
 * @author The VLE Development Team.
 * @brief Define no-vle model ie. model that use a different notation.
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

#include <vle/graph/NoVLEModel.hpp>
#include <vle/utils/XML.hpp>



namespace vle { namespace graph {

NoVLEModel::NoVLEModel(const std::string& name, CoupledModel* parent) :
    Model(name, parent)
{
}

NoVLEModel::NoVLEModel(const NoVLEModel& model) :
    Model(model)
{
}

Model* NoVLEModel::clone() const
{
    return new NoVLEModel(*this);
}

void NoVLEModel::writeXML(std::ostream& out) const
{
    out << "<model name=\"" << getName() << "\" type=\"novle\" />";
}

bool NoVLEModel::isAtomic() const
{
    return false;
}

bool NoVLEModel::isCoupled() const
{
    return false;
}

bool NoVLEModel::isNoVLE() const
{
    return true;
}

Model* NoVLEModel::findModel(const std::string& name) const
{
    if (getName() == name)
        return (Model*)this;
    else
        return 0;
}

}} // namespace vle graph
