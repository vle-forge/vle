/**
 * @file graph/AtomicModel.cpp
 * @author The VLE Development Team.
 * @brief Represent the Atomic Model in DEVS formalism. This class just
 * represent the graph not the DEVS Simulator.
 */

/*
 * Copyright (C) 2004, 05, 06, 07 - The vle Development Team
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

#include <vle/graph/AtomicModel.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <vle/graph/Model.hpp>
#include <vle/utils/XML.hpp>



namespace vle { namespace graph {

AtomicModel::AtomicModel(const std::string& name, CoupledModel* parent) :
    Model(name, parent)
{
}

AtomicModel::~AtomicModel()
{
}

Model* AtomicModel::findModel(const std::string& name) const
{
    if (getName() == name)
	return (Model*)this;
    else
	return 0;
}

void AtomicModel::writeXML(std::ostream& out) const
{
    out << "<model"
        << " name=\"" << getName() << "\""
        << " type=\"atomic\""
        << ">\n";

    writePortListXML(out);

    out << "</model>\n";
}

}} // namespace vle graph
