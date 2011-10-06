/*
 * @file examples/gens/GenExecutiveGraph.cpp
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


#include <vle/devs/ExecutiveDbg.hpp>
#include <vle/translator/GraphTranslator.hpp>
#include <vle/utils.hpp>
#include <vle/devs.hpp>
#include <vle/vpz.hpp>
#include <boost/numeric/conversion/cast.hpp>

namespace vle { namespace examples { namespace gens {

class GenExecutiveGraph : public devs::Executive
{
public:
    GenExecutiveGraph(const devs::ExecutiveInit& mdl,
                      const devs::InitEventList& events)
        : devs::Executive(mdl, events)
    {
    }

    virtual ~GenExecutiveGraph()
    {}

    virtual devs::Time init(const devs::Time& /* time */)
    {
        translator::GraphTranslator tr(*this);

        value::Map mp;
        mp.addString("prefix", "node");
        mp.addInt("number", 6);
        mp.addString("port", "in-out");
        mp.addString("adjacency matrix",
                     " 0 0 0 0 0 1"
                     " 0 0 1 0 0 1"
                     " 0 1 0 0 0 1"
                     " 0 0 0 1 0 1"
                     " 0 0 0 1 0 1"
                     " 0 0 0 0 0 0");
        mp.addString("classes", "beepbeep beepbeepbeep beepbeep beepbeepbeep "
                     "beepbeep counter");

        tr.translate(mp);

        return devs::Time::infinity;
    }
};

}}} // namespace vle examples gens

DECLARE_NAMED_EXECUTIVE_DBG(exegraph, vle::examples::gens::GenExecutiveGraph)
