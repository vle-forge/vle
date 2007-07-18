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
#include <vle/utils/XML.hpp>
#include <vle/utils/Debug.hpp>
#include <boost/tuple/tuple.hpp>
#include <stack>

using std::string;
using std::map;
using std::vector;
using std::list;
using namespace vle::graph;
using namespace vle::utils::xml;



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

Model::~Model()
{
}

void Model::getTargetPortList(const std::string& portname,
                              TargetModelList& out)
{
    typedef boost::tuple < CoupledModel*, string, bool > DataTarget;
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

void Model::delInputPortAndConnection(const std::string& /* name */)
{
    Throw(utils::NotYetImplented, "Model::delInputPortAndConnection");
}

void Model::delOutputPortAndConnection(const std::string& /* name */)
{
    Throw(utils::NotYetImplented, "Model::delInputPortAndConnection");
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

void Model::delInitPort(const std::string & name)
{
    m_initPortList.erase(name);
}

void Model::delInputPort(const std::string & name)
{
    Throw(utils::DevsGraphError, boost::format(
            "delInputPort %1% not yet implemented") % name);
}

void Model::delOutputPort(const std::string & name)
{
    Throw(utils::DevsGraphError, boost::format(
            "delOutputPort %1% not yet implemented") % name);
}

void Model::delStatePort(const std::string & name)
{
    m_statePortList.erase(name);
}

void Model::addInitPort(const std::string& name)
{
    m_initPortList.insert(name);
}

void Model::addStatePort(const std::string& name)
{
    m_statePortList.insert(name);
}

void Model::addInitPort(const std::list < std::string > & lst)
{
    list < string >::const_iterator it = lst.begin();
    while (it != lst.end()) {
	addInitPort(*it);
	++it;
    }
}

void Model::addInputPort(const std::list < std::string > & lst)
{
    list < string>::const_iterator it = lst.begin();
    while (it != lst.end()) {
	addInputPort(*it);
	++it;
    }
}

void Model::addOutputPort(const std::list < std::string > & lst)
{
    list < string >::const_iterator it = lst.begin();
    while (it != lst.end()) {
	addOutputPort(*it);
	++it;
    }
}

void Model::addStatePort(const std::list < std::string > & lst)
{
    list < string >::const_iterator it = lst.begin();
    while (it != lst.end()) {
	addStatePort(*it);
	++it;
    }
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

bool Model::existInitPort(const std::string & name)
{
    return m_initPortList.find(name) != m_initPortList.end();
}

bool Model::existInputPort(const std::string & name)
{
    return m_inPortList.find(name) != m_inPortList.end();
}

bool Model::existOutputPort(const std::string & name)
{
    return m_outPortList.find(name) != m_outPortList.end();
}

bool Model::existStatePort(const std::string & name)
{
    return m_statePortList.find(name) != m_statePortList.end();
}

//void Model::clearInitPort()
//{
//map < string, Port* >::iterator it1 = m_initPortList.begin();
//
//while (it1 != m_initPortList.end()) {
//delete(it1++)->second;
//}
//
//m_initPortList.clear();
//}
//
//void Model::clearInputPort()
//{
//map < string, Port* >::iterator it2 = m_inPortList.begin();
//
//while (it2 != m_inPortList.end()) {
//delete(it2++)->second;
//}
//
//m_inPortList.clear();
//}
//
//void Model::clearOutputPort()
//{
//map < string, Port* >::iterator it3 = m_outPortList.begin();
//
//while (it3 != m_outPortList.end()) {
//delete(it3++)->second;
//}
//
//m_outPortList.clear();
//}
//
//void Model::clearStatePort()
//{
//map < string, Port* >::iterator it4 = m_statePortList.begin();
//
//while (it4 != m_statePortList.end()) {
//delete(it4++)->second;
//}
//
//m_statePortList.clear();
//}

//void Model::mergePort(std::list < std::string >& inlist,
//std::list < std::string >& outlist,
//std::list < std::string >& statelist,
//std::list < std::string >& initlist)
//{
//ConnectionList::iterator it;
//std::list < std::string >::iterator jt;
//
//for (it = m_inPortList.begin(); it != m_inPortList.end(); ++it) {
//jt = std::find(inlist.begin(), inlist.end(), (*it).first);
//if (jt == inlist.end()) delInputPortAndConnection((*it).first);
//inlist.erase(jt);
//}
//for (jt = inlist.begin(); jt != inlist.end(); ++jt)
//addInputPort(*jt);
//
//for (it = m_outPortList.begin(); it != m_outPortList.end(); ++it) {
//jt = std::find(outlist.begin(), outlist.end(), (*it).first);
//if (jt == outlist.end())
//delOutputPortAndConnection((*it).first);
//else
//outlist.erase(jt);
//}
//for (jt = outlist.begin(); jt != outlist.end(); ++jt)
//addOutputPort(*jt);
//
//for (it = m_statePortList.begin(); it != m_statePortList.end(); ++it) {
//jt = std::find(statelist.begin(), statelist.end(), (*it).first);
//if (jt == statelist.end())
//delStatePort((*it).first);
//else
//statelist.erase(jt);
//}
//for (jt = statelist.begin(); jt != statelist.end(); ++jt)
//addStatePort(*jt);
//
//for (it = m_initPortList.begin(); it != m_initPortList.end(); ++it) {
//jt = std::find(initlist.begin(), initlist.end(), (*it).first);
//if (jt == initlist.end())
//delInitPort((*it).first);
//else
//initlist.erase(jt);
//}
//for (jt = initlist.begin(); jt != initlist.end(); ++jt)
//addInitPort(*jt);
//}
//
void Model::writePortListXML(std::ostream& out) const
{
    if (not m_initPortList.empty()) {
    out << "<init>\n";
	PortList::const_iterator it = m_initPortList.begin();
        for (;it != m_initPortList.end(); ++it) {
            out << " <port name=\"" << (*it) << "\" />\n";
        }
        out << "</init>\n";
    }

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

    if (not m_statePortList.empty()) {
        out << "<state>\n";
	PortList::const_iterator it = m_statePortList.begin();
        for (; it != m_statePortList.end(); ++it) {
            out << "<port name=\"" << (*it) << "\" />\n";
	}
        out << "</state>\n";
    }
}

AtomicModel* Model::toAtomic(Model* model)
{
    if (model != NULL and model->isAtomic()) {
        return static_cast<AtomicModel*>(model);
    }
    return NULL;
}

CoupledModel* Model::toCoupled(Model* model)
{
    if (model != NULL and model->isCoupled()) {
        return static_cast<CoupledModel*>(model);
    }
    return NULL;
}

NoVLEModel* Model::toNoVLE(Model* model)
{
    if (model != NULL and model->isNoVLE()) {
        return static_cast<NoVLEModel*>(model);
    }
    return NULL;
}

void Model::getAtomicModelList(Model* model,
                               std::vector < AtomicModel* >& list)
{
    if (model->isAtomic()) {
        list.push_back((AtomicModel*)model);
    } else {
	vector < CoupledModel* > coupledModelList;
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
