/**
 * @file src/vle/graph/Model.cpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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




#include <vle/graph/CoupledModel.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/graph/Model.hpp>
#include <vle/utils/Debug.hpp>
#include <boost/tuple/tuple.hpp>
#include <stack>

namespace vle { namespace graph {

Model::Model(const std::string& name, CoupledModel* parent) :
    m_parent(parent),
    m_x(-1),
    m_y(-1),
    m_width(-1),
    m_height(-1),
    m_name(name)
{
    if (parent) {
        parent->addModel(this);
    }
}

Model::Model(const Model& mdl) :
    m_parent(0),
    m_inPortList(mdl.m_inPortList),
    m_outPortList(mdl.m_outPortList),
    m_x(mdl.m_x),
    m_y(mdl.m_y),
    m_width(mdl.m_width),
    m_height(mdl.m_height),
    m_name(mdl.m_name)
{
    std::for_each(mdl.m_inPortList.begin(), mdl.m_inPortList.end(),
                  CopyWithoutConnection(m_inPortList));

    std::for_each(mdl.m_outPortList.begin(), mdl.m_outPortList.end(),
                  CopyWithoutConnection(m_outPortList));
}

void Model::swap(Model& mdl)
{
    std::swap(m_parent, mdl.m_parent);
    std::swap(m_inPortList, mdl.m_inPortList);
    std::swap(m_outPortList, mdl.m_outPortList);
    std::swap(m_x, mdl.m_x);
    std::swap(m_y, mdl.m_y);
    std::swap(m_width, mdl.m_width);
    std::swap(m_height, mdl.m_height);
    std::swap(m_name, mdl.m_name);
}

void Model::getTargetPortList(const std::string& portname,
                              TargetModelList& out)
{
    typedef boost::tuple < CoupledModel*, std::string, bool > DataTarget;
    std::stack < DataTarget > stk;

    ModelPortList& outs(getOutPort(portname));
    for (ModelPortList::iterator it = outs.begin(); it != outs.end(); ++it) {
        if (it->first->isAtomic()) {
            out.push_back(ModelPort(it->first, it->second));
        } else if (it->first->isCoupled()) {
            CoupledModel* cpl = static_cast < CoupledModel* >(it->first);
            if (it->first == getParent()) {
                stk.push(DataTarget(cpl, it->second, false));
            } else {
                stk.push(DataTarget(cpl, it->second, true));
            }
        }
    }

    while (not stk.empty()) {
        DataTarget tmp(stk.top());
        stk.pop();

        ModelPortList* outs = 0;
        if (tmp.get<2>())
            outs = &(tmp.get<0>()->getInternalInPort(tmp.get<1>()));
        else
            outs = &(tmp.get<0>()->getOutPort(tmp.get<1>()));

        for (ModelPortList::iterator it = outs->begin(); it != outs->end();
             ++it) {
            if (it->first->isAtomic()) {
                out.push_back(ModelPort(it->first, it->second));
            } else if (it->first->isCoupled()) {
                CoupledModel* cpl = static_cast < CoupledModel* >(it->first);
                if (it->first == tmp.get<0>()->getParent()) {
                    stk.push(DataTarget(cpl, it->second, false));
                } else {
                    stk.push(DataTarget(cpl, it->second, true));
                }
            }
        }
    }
}

void Model::rename(Model* mdl, const std::string& newname)
{
    AssertI(mdl);

    CoupledModel* parent = mdl->getParent();
    if (parent) {
        ModelList::iterator it = parent->getModelList().find(mdl->getName());
        AssertI(it != parent->getModelList().end());

        mdl->m_name.assign(newname);
        parent->getModelList().erase(it);
        parent->addModel(mdl);
    } else {
        mdl->m_name.assign(newname);
    }
}

std::string Model::getParentName() const
{
    std::list < std::string > lst;
    CoupledModel* parent = m_parent;

    while (parent) {
        lst.push_front(parent->getName());
        parent = parent->getParent();
    }

    std::string result;
    while (not lst.empty()) {
        result += lst.front();
        lst.pop_front();
        if (not result.empty()) {
            result += ',';
        }
    }
    return result;
}

void Model::getParents(CoupledModelVector& parents) const
{
    CoupledModel* parent = m_parent;

    while (parent) {
        parents.push_back(parent);
        parent = parent->getParent();
    }
}

Model* Model::getModel(const CoupledModelVector& lst,
                              const std::string& name)
{
    if (lst.empty()) {
        return findModel(name);
    } else {
        Assert(utils::DevsGraphError, isCoupled(),
               "Bad use of getModel from a list");
        CoupledModelVector::const_reverse_iterator it = lst.rbegin();
        CoupledModel* top = static_cast < CoupledModel* >(this);
        CoupledModel* other = *it;

        Assert(utils::DevsGraphError, other->getName() == top->getName(),
               boost::format("Get model have not the same name '%1%' and '%2%'")
               % top->getName() % other->getName());

        it++;

        while (it != lst.rend()) {
            other = *it;
            Model* tmp = top->findModel(other->getName());

            Assert(utils::DevsGraphError, tmp, boost::format(
                    "Model %1% not found") % other->getName());

            Assert(utils::DevsGraphError, tmp->isCoupled(), boost::format(
                    "Model %1% is not a coupled model") % other->getName());

            Assert(utils::DevsGraphError, tmp->getName() == other->getName(),
                   boost::format( "Model %1% have not the same parent") %
                   other->getName());

            top = static_cast < CoupledModel* >(tmp);
            ++it;
        }

        return top->findModel(name);
    }
}

ModelPortList& Model::addInputPort(const std::string& name)
{
    ConnectionList::iterator it(m_inPortList.find(name));
    if (it == m_inPortList.end()) {
        if (isCoupled()) {
            CoupledModel* cpl = static_cast < CoupledModel* >(this);
            cpl->getInternalInputPortList().insert(
                std::make_pair < std::string, ModelPortList >(
                    name, ModelPortList()));
        }
        return (*m_inPortList.insert(
                std::make_pair < std::string, ModelPortList >(
                    name, ModelPortList())).first).second;
    } else {
        return it->second;
    }
}

ModelPortList& Model::addOutputPort(const std::string& name)
{
    ConnectionList::iterator it(m_outPortList.find(name));
    if (it == m_outPortList.end()) {
        if (isCoupled()) {
            CoupledModel* cpl = static_cast < CoupledModel* >(this);
            cpl->getInternalOutputPortList().insert(
                std::make_pair < std::string, ModelPortList >(
                    name, ModelPortList()));
        }
        return (*m_outPortList.insert(
                std::make_pair < std::string, ModelPortList >(
                    name, ModelPortList())).first).second;
    } else {
        return it->second;
    }
}

void Model::delInputPort(const std::string& name)
{
    ModelPortList& lst(getInPort(name));
    for (ModelPortList::iterator it = lst.begin(); it != lst.end(); ++it) {
        if (it->first != m_parent) {
            ModelPortList& toclean(it->first->getOutPort(it->second));
            toclean.erase(this);
        } else {
            ModelPortList& toclean(m_parent->getInternalInPort(it->second));
            toclean.erase(this);
        }

        if (isCoupled()) {
            CoupledModel* tmp(toCoupled(this));
            ModelPortList& intern(tmp->getInternalInPort(name));
            ModelPortList::iterator jt = intern.begin();
            while(jt != intern.end()) {
                jt->first->getInPort(jt->second).erase(this);
                ++jt;
            }
            intern.clear();
        }
    }
    lst.remove_all();
    m_inPortList.erase(name);
}

void Model::delOutputPort(const std::string & name)
{
    ModelPortList& lst(getOutPort(name));
    for(ModelPortList::iterator it = lst.begin(); it != lst.end(); ++it) {
        if (it->first != m_parent) {
            ModelPortList& toclean(it->first->getInPort(it->second));
            toclean.erase(this);
        } else {
            ModelPortList& toclean(m_parent->getInternalOutPort(it->second));
            toclean.erase(this);
        }

        if (isCoupled()) {
            CoupledModel* tmp(toCoupled(this));
            ModelPortList& intern(tmp->getInternalOutPort(name));
            ModelPortList::iterator jt = intern.begin();
            while(jt != intern.end()) {
                jt->first->getOutPort(jt->second).erase(this);
                ++jt;
            }
            intern.clear();
        }
    }
    lst.remove_all();
    m_outPortList.erase(name);
}

void Model::addInputPort(const std::list < std::string > & lst)
{
    std::for_each(lst.begin(), lst.end(), AddInputPort(*this));
}

void Model::addOutputPort(const std::list < std::string > & lst)
{
    std::for_each(lst.begin(), lst.end(), AddOutputPort(*this));
}

const ModelPortList& Model::getInPort(const std::string& name) const
{
    ConnectionList::const_iterator it = m_inPortList.find(name);
    if (it == m_inPortList.end()) {
        Throw(utils::DevsGraphError, boost::format(
                "Coupled model %1% have no input port %2%") % getName() % name);
    }

    return it->second;
}

const ModelPortList& Model::getOutPort(const std::string& name) const
{
    ConnectionList::const_iterator it = m_outPortList.find(name);
    if (it == m_outPortList.end()) {
        Throw(utils::DevsGraphError, boost::format(
                "Coupled model %1% have no output port %2%") % getName() % name);
    }

    return it->second;
}

ModelPortList& Model::getInPort(const std::string& name)
{
    ConnectionList::iterator it = m_inPortList.find(name);
    if (it == m_inPortList.end()) {
        Throw(utils::DevsGraphError, boost::format(
                "Model %1% have no input port %2%") % getName() % name);
    }

    return it->second;
}

ModelPortList& Model::getOutPort(const std::string& name)
{
    ConnectionList::iterator it = m_outPortList.find(name);
    if (it == m_outPortList.end()) {
        Throw(utils::DevsGraphError, boost::format(
                "Model %1% have no output port %2%") % getName() % name);
    }

    return it->second;
}

bool Model::existInputPort(const std::string & name)
{
    return m_inPortList.find(name) != m_inPortList.end();
}

bool Model::existOutputPort(const std::string & name)
{
    return m_outPortList.find(name) != m_outPortList.end();
}

int Model::getInputPortIndex(const std::string& name) const
{
    ConnectionList::const_iterator it = m_inPortList.find(name);
    Assert(utils::DevsGraphError, it != m_inPortList.end(), boost::format(
            "Input port %1% not exist in model %2%") % name % getName());

    return std::distance(m_inPortList.begin(), it);
}

int Model::getOutputPortIndex(const std::string& name) const
{
    ConnectionList::const_iterator it = m_outPortList.find(name);
    Assert(utils::DevsGraphError, it != m_outPortList.end(), boost::format(
            "Output port %1% not exist in model %2%") % name % getName());

    return std::distance(m_outPortList.begin(), it);
}

void Model::writePortListXML(std::ostream& out) const
{
    if (not m_inPortList.empty()) {
        out << "<in>\n";

        ConnectionList::const_iterator it = m_inPortList.begin();
        for (;it != m_inPortList.end(); ++it) {
            out << "<port name=\"" << (*it).first << "\" />\n";
        }
        out << "</in>\n";
    }

    if (not m_outPortList.empty()) {
        out << "<out>\n";
	ConnectionList::const_iterator it = m_outPortList.begin();
        for(; it != m_outPortList.end(); ++it) {
            out << "<port name=\"" << (*it).first << "\" />\n";
	}
        out << "</out>\n";
    }
}

AtomicModel* Model::toAtomic(Model* model)
{
    if (model and model->isAtomic()) {
        return static_cast<AtomicModel*>(model);
    }
    return 0;
}

CoupledModel* Model::toCoupled(Model* model)
{
    if (model and model->isCoupled()) {
        return static_cast<CoupledModel*>(model);
    }
    return 0;
}

void Model::getAtomicModelList(Model* model,
                               AtomicModelVector& list)
{
    if (model->isAtomic()) {
        list.push_back((AtomicModel*)model);
    } else {
	std::list < CoupledModel* > coupledModelList;
	coupledModelList.push_back((CoupledModel*)model);
        while (!coupledModelList.empty()) {
            CoupledModel* m = coupledModelList.front();
            ModelList& v(m->getModelList());

            for (ModelList::iterator it = v.begin(); it != v.end(); ++it) {
		Model* n = it->second;
                if (n->isAtomic()) {
                    list.push_back(static_cast < AtomicModel*>(n));
                } else {
                    coupledModelList.push_back(static_cast < CoupledModel*>(n));
                }
	    }
	    coupledModelList.pop_front();
	}
    }
}

bool Model::isInList(const ModelList& lst, graph::Model* m)
{
    return lst.find(m->getName()) != lst.end();
}

Model::Model() :
    m_parent(0),
    m_x(-1),
    m_y(-1),
    m_width(-1),
    m_height(-1)
{
    Throw(utils::NotYetImplemented, "Model::Model not developed");
}

Model& Model::operator=(const Model& /* mdl */)
{
    Throw(utils::NotYetImplemented, "Model::operator= not developed");
}

}} // namespace vle graph
