/*
 * @file vle/vpz/AtomicModels.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
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


#include <vle/vpz/AtomicModels.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/utils/Exception.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <stack>

namespace vle { namespace vpz {

AtomicModel::AtomicModel(const std::string& conditions,
                         const std::string& dynamics,
                         const std::string& observables)
    : m_dynamics(dynamics), m_observables(observables)
{
    std::string conditionList(conditions);
    boost::trim(conditionList);

    if (not conditionList.empty()) {
        boost::split(m_conditions, conditionList, boost::is_any_of(","),
                     boost::algorithm::token_compress_on);
        if (m_conditions.front().empty()) {
            m_conditions.pop_back();
        }
    }
}

void AtomicModel::delCondition(const std::string& str)
{
    std::vector < std::string >::iterator itfind =
	std::find(m_conditions.begin(), m_conditions.end(), str);

    m_conditions.erase(itfind);
}

void AtomicModelList::add(const AtomicModelList& atoms)
{
    for (const_iterator it = atoms.atomicmodels().begin();
         it != atoms.atomicmodels().end(); ++it) {
        add(it->first, it->second);
    }
}

AtomicModel& AtomicModelList::add(graph::Model* mdl,
                                  const AtomicModel& atom)
{
    const_iterator it = m_lst.find(mdl);

    if (it != end()) {
        throw utils::ArgError(fmt(
                _("The model '%1%' already have external information")) %
            mdl->getName());
    }

    return (*m_lst.insert(value_type(mdl, atom)).first).second;
}

void AtomicModelList::del(graph::Model* mdl)
{
    iterator it = m_lst.find(mdl);

    if (it == end()) {
        throw utils::ArgError(fmt(
                _("The model [%1%] have not external information")) %
            mdl->getName());
    }

    m_lst.erase(it);
}

const AtomicModel& AtomicModelList::get(graph::Model* atom) const
{
    const_iterator it = m_lst.find(atom);
    if (it == end()) {
        throw(utils::ArgError(utils::ArgError(fmt(_(
		 "The information about atomic model [%1%] does not exist")) %
            atom->getName())));
    }
    return it->second;
}

AtomicModel& AtomicModelList::get(graph::Model* atom)
{
    iterator it = m_lst.find(atom);
    if (it == end()) {
        throw(utils::ArgError(fmt(_(
                "The information about atomic model [%1%] does not exist")) %
                    atom->getName()));
    }
    return it->second;
}

const AtomicModel& AtomicModelList::get(const graph::Model* atom) const
{
    const_iterator it = m_lst.find(const_cast < graph::Model* >(atom));
    if (it == end()) {
        throw(utils::ArgError(fmt(_(
		"The information about atomic model [%1%] does not exist")) %
            atom->getName()));
    }
    return it->second;
}

AtomicModel& AtomicModelList::get(const graph::Model* atom)
{
    iterator it = m_lst.find(const_cast < graph::Model* >(atom));
    if (it == end()) {
        throw(utils::ArgError(fmt(_(
                "The information about atomic model [%1%] does not exist")) %
                    atom->getName()));
    }
    return it->second;
}

void AtomicModelList::updateCondition(const std::string& oldname,
				      const std::string& newname)
{
    for (iterator it = m_lst.begin(); it != m_lst.end(); ++it) {
        const std::vector < std::string >& lstconds(it->second.conditions());

	if (std::find(lstconds.begin(), lstconds.end(), newname)
                != lstconds.end()) {
            throw(utils::ArgError(fmt(_(
                    "Condition [%1%] already exist")) %
                        newname));
        }

        std::vector < std::string >::const_iterator itfind =
                std::find(lstconds.begin(), lstconds.end(), oldname);

        if (itfind != lstconds.end()) {
            it->second.delCondition(oldname);
            it->second.addCondition(newname);
        }
    }
}

void AtomicModelList::updateDynamics(const std::string& oldname,
				     const std::string& newname)
{
    for (iterator it = m_lst.begin(); it != m_lst.end(); ++it) {
	if (it->second.dynamics() == oldname)
	    it->second.setDynamics(newname);
    }
}

void AtomicModelList::updateObservable(const std::string& oldname,
				      const std::string& newname)
{
    for (iterator it = m_lst.begin(); it != m_lst.end(); ++it) {
	if (it->second.observables() == oldname)
	    it->second.setObservables(newname);
    }
}

//
///
//// Write graph information to stream.
///
//

void AtomicModelList::writeModel(const graph::Model* mdl, std::ostream& out) const
{
    if (mdl) {
        if (mdl->isAtomic()) {
            writeAtomic(out, static_cast < const graph::AtomicModel* >(mdl));
        } else {
            writeCoupled(out, static_cast < const graph::CoupledModel* > (mdl));
        }
    }
}

void AtomicModelList::writeCoupled(std::ostream& out, const graph::CoupledModel* mdl) const
{
    typedef std::stack < const graph::CoupledModel* > CoupledModelList;
    typedef std::stack < bool > IsWritedCoupledModel;

    CoupledModelList cms;
    IsWritedCoupledModel writed;

    cms.push(mdl);
    writed.push(false);

    while (not cms.empty()) {
        const graph::CoupledModel* top(cms.top());

        if (not writed.top()) {
            writed.top() = true;
            out << "<model name=\"" << top->getName().c_str() << "\" "
                << "type=\"coupled\" ";
            writeGraphics(out, top);
            out << " >\n";
            writePort(out, top);
            out << "<submodels>\n";

            const graph::ModelList& childs(top->getModelList());
            for (graph::ModelList::const_iterator it = childs.begin();
                 it != childs.end(); ++it) {
                if (it->second->isCoupled()) {
                    cms.push(static_cast < graph::CoupledModel* >(it->second));
                    writed.push(false);
                } else if (it->second->isAtomic()) {
                    writeAtomic(out, static_cast < graph::AtomicModel* >
                                (it->second));
                }
            }
        } else {
            out << "</submodels>\n";
            writeConnection(out, top);
            cms.pop();
            writed.pop();
            out << "</model>\n";
        }
    }
}

void AtomicModelList::writeAtomic(std::ostream& out, const graph::AtomicModel* mdl) const
{
    const AtomicModel& vpzatom(get(mdl));

    out << "<model name=\"" << mdl->getName().c_str() << "\" "
        << "type=\"atomic\" ";

    if (not vpzatom.conditions().empty()) {
	out << "conditions=\"";

        std::vector < std::string >::const_iterator it =
            vpzatom.conditions().begin();
        while (it != vpzatom.conditions().end()) {
            out << it->c_str();
            ++it;
            if (it != vpzatom.conditions().end()) {
                out << ",";
            }
        }

        out << "\" ";
    }
    out << "dynamics=\"" << vpzatom.dynamics().c_str() << "\" ";

    if (not vpzatom.observables().empty()) {
        out << "observables=\"" << vpzatom.observables().c_str() << "\" ";
    }

    writeGraphics(out, mdl);

    out << ">\n";

    writePort(out, mdl);

    out << "</model>\n";
}

void AtomicModelList::writeGraphics(std::ostream& out, const graph::Model* mdl) const
{
    if (mdl->x() >= 0) {
        out << "x=\"" << mdl->x() << "\" ";
    }
    if (mdl->y() >= 0) {
        out << "y=\"" << mdl->y() << "\" ";
    }
    if (mdl->width() >= 0) {
        out << "width=\"" << mdl->width() << "\" ";
    }
    if (mdl->height() >= 0) {
        out << "height=\"" << mdl->height() << "\" ";
    }
}

void AtomicModelList::writePort(std::ostream& out, const graph::Model* mdl) const
{
    const graph::ConnectionList& ins(mdl->getInputPortList());
    if (not ins.empty()) {
        out << "<in>\n";
        for (graph::ConnectionList::const_iterator it = ins.begin();
             it != ins.end(); ++it) {
            out << " <port name=\"" << it->first.c_str() << "\" />\n";
        }
        out << "</in>\n";
    }

    const graph::ConnectionList& outs(mdl->getOutputPortList());
    if (not outs.empty()) {
        out << "<out>\n";
        for (graph::ConnectionList::const_iterator it = outs.begin();
             it != outs.end(); ++it) {
            out << " <port name=\"" << it->first.c_str() << "\" />\n";
        }
        out << "</out>\n";
    }

}

void AtomicModelList::writeConnection(std::ostream& out, const graph::CoupledModel* mdl) const
{
    out << "<connections>\n";
    mdl->writeConnections(out);
    out << "</connections>\n";
}

}} // namespace vle vpz
