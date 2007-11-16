/**
 * @file graph/Model.cpp
 * @author The VLE Development Team.
 * @brief The DEVS model base class.
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

#include <vle/graph/CoupledModel.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/graph/NoVLEModel.hpp>
#include <vle/graph/Model.hpp>
#include <vle/utils/Debug.hpp>
#include <boost/tuple/tuple.hpp>
#include <stack>

namespace vle { namespace graph {

Model::Model(const std::string& name, CoupledModel* parent) :
    m_parent(parent),
    m_x(0),
    m_y(0),
    m_width(0),
    m_height(0),
    m_name(name)
{
    if (parent) {
        parent->addModel(this);
    }
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

        for (ModelPortList::iterator it = outs->begin(); it != outs->end(); ++it) {
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

void Model::delInputPort(ModelPortList& ins)
{
    for (ModelPortList::iterator jt = ins.begin(); jt != ins.end(); ++jt) {
        if (jt->first == this) {
            if (isCoupled()) {
                CoupledModel* tmp = toCoupled(this);
                tmp->getInternalInPort(jt->second).remove(this, getName());
            }
        } else {
            jt->first->getOutPort(jt->second).remove(this, getName());
        }
    }
    ins.remove_all();
}

void Model::delInputPort(const std::string & name)
{
    delInputPort(getInPort(name));
}

void Model::delOutputPort(ModelPortList& ins)
{
    for (ModelPortList::iterator jt = ins.begin(); jt != ins.end(); ++jt) {
        if (jt->first == this) {
            if (isCoupled()) {
                CoupledModel* tmp = toCoupled(this);
                tmp->getInternalOutPort(jt->second).remove(this, getName());
            }
        } else {
            jt->first->getInPort(jt->second).remove(this, getName());
        }
    }
    ins.remove_all();
}

void Model::delOutputPort(const std::string & name)
{
    delOutputPort(getOutPort(name));
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
    
    return std::distance(it, m_inPortList.begin());
}

int Model::getOutputPortIndex(const std::string& name) const
{
    ConnectionList::const_iterator it = m_outPortList.find(name);
    Assert(utils::DevsGraphError, it != m_outPortList.end(), boost::format(
            "Output port %1% not exist in model %2%") % name % getName());
    
    return std::distance(it, m_outPortList.begin());
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

NoVLEModel* Model::toNoVLE(Model* model)
{
    if (model and model->isNoVLE()) {
        return static_cast<NoVLEModel*>(model);
    }
    return 0;
}

void Model::getAtomicModelList(Model* model,
                               std::vector < AtomicModel* >& list)
{
    if (model->isAtomic()) {
        list.push_back((AtomicModel*)model);
    } else {
	std::vector < CoupledModel* > coupledModelList;
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
	    coupledModelList.erase(coupledModelList.begin());
	}
    }
}

bool Model::isInList(const ModelList& lst, graph::Model* m)
{
    return lst.find(m->getName()) != lst.end();
}

}} // namespace vle graph
