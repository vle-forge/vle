/**
 * @file graph/Model.cpp
 * @author The VLE Development Team.
 * @brief The DEVS model base class.
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

#include <vle/graph/CoupledModel.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/graph/NoVLEModel.hpp>
#include <vle/graph/Port.hpp>
#include <vle/graph/Model.hpp>
#include <vle/utils/XML.hpp>
#include <vle/utils/Debug.hpp>

using std::string;
using std::map;
using std::vector;
using std::list;
using namespace vle::graph;
using namespace vle::utils::xml;



namespace vle { namespace graph {

Model::Model() :
    m_parent(0),
    m_x(0),
    m_y(0),
    m_width(0),
    m_height(0)
{
}

Model::Model(CoupledModel* parent) :
    m_parent(parent),
    m_x(0),
    m_y(0),
    m_width(0),
    m_height(0)
{
}

Model::Model(const std::string& name, CoupledModel* parent) :
    m_name(name),
    m_parent(parent),
    m_x(0),
    m_y(0),
    m_width(0),
    m_height(0)
{
    if (parent) {
        parent->addModel(this);
    }
}

Model::Model(const Model& mdl) :
    m_name(mdl.getName()),
    m_description(mdl.getDescription()),
    m_parent(mdl.getParent()),
    m_x(mdl.x()),
    m_y(mdl.y()),
    m_width(mdl.width()),
    m_height(mdl.height())
{
    for (MapStringPort::const_iterator it = mdl.m_initPortList.begin();
	 it != mdl.m_initPortList.end();++it)
	m_initPortList[it->first] = new Port(*it->second);

    for (MapStringPort::const_iterator it = mdl.m_inPortList.begin();
	 it != mdl.m_inPortList.end();++it)
	m_inPortList[it->first] = new Port(*it->second);

    for (MapStringPort::const_iterator it = mdl.m_outPortList.begin();
	 it != mdl.m_outPortList.end();++it)
	m_outPortList[it->first] = new Port(*it->second);
    
    for (MapStringPort::const_iterator it = mdl.m_statePortList.begin();
	 it != mdl.m_statePortList.end();++it)
	m_statePortList[it->first] = new Port(*it->second);
}

Model& Model::operator=(const Model& mdl)
{
    if (this != &mdl) {
        m_name = mdl.getName();
        m_description = mdl.getDescription();
        m_parent = mdl.getParent();

        clearStatePort();
        clearOutputPort();
        clearInputPort();
        clearInitPort();

        for (MapStringPort::const_iterator it = mdl.m_initPortList.begin();
             it != mdl.m_initPortList.end();++it)
            m_initPortList[it->first] = new Port(*it->second);

        for (MapStringPort::const_iterator it = mdl.m_inPortList.begin();
             it != mdl.m_inPortList.end();++it)
            m_inPortList[it->first] = new Port(*it->second);

        for (MapStringPort::const_iterator it = mdl.m_outPortList.begin();
             it != mdl.m_outPortList.end();++it)
            m_outPortList[it->first] = new Port(*it->second);

        for (MapStringPort::const_iterator it = mdl.m_statePortList.begin();
             it != mdl.m_statePortList.end();++it)
            m_statePortList[it->first] = new Port(*it->second);
    }
    return *this;
}

Model::~Model()
{
    clearStatePort();
    clearOutputPort();
    clearInputPort();
    clearInitPort();
}

int Model::getInitPortNumber() const
{
    return m_initPortList.size();
}

int Model::getInPortNumber() const
{
    return m_inPortList.size();
}

int Model::getOutPortNumber() const
{
    return m_outPortList.size();
}

int Model::getStatePortNumber() const
{
    return m_statePortList.size();
}

Port* Model::getPort(const std::string & p_name) const
{
    Port * port = getInitPort(p_name);

    if (port != NULL) return port;
    port = getInPort(p_name);
    if (port != NULL) return port;
    port = getOutPort(p_name);
    if (port != NULL) return port;
    port = getStatePort(p_name);
    if (port != NULL) return port;

    return NULL;
}

Port* Model::getInitPort(const std::string & p_name) const
{
    map<string,Port*>::const_iterator it = m_initPortList.find(p_name);
    return (it == m_initPortList.end()) ? NULL : (*it).second;
}

Port* Model::getInPort(const std::string & p_name) const
{
    map<string,Port*>::const_iterator it = m_inPortList.find(p_name);
    return (it == m_inPortList.end()) ? NULL : (*it).second;
}

Port* Model::getOutPort(const std::string & p_name) const
{
    map<string,Port*>::const_iterator it = m_outPortList.find(p_name);
    return (it == m_outPortList.end()) ? NULL : (*it).second;
}

Port* Model::getStatePort(const std::string & p_name) const
{
    map<string,Port*>::const_iterator it = m_statePortList.find(p_name);
    return (it == m_statePortList.end()) ? NULL : (*it).second;
}

int Model::getInitPortIndex(const std::string & name) const
{
    MapStringPort::const_iterator it = m_initPortList.begin();
    int r = 0;

    while (it != m_initPortList.end()) {
	if ((*it).first == name) {
            return r;
        }
	++it;
	++r;
    }

    AssertI(false);
    return -1;
}

int Model::getInputPortIndex(const std::string & name) const
{
    MapStringPort::const_iterator it = m_inPortList.begin();
    int r = 0;

    while (it != m_inPortList.end()) {
        if ((*it).first == name) {
            return r;
        }
	++it;
	++r;
    }
    AssertI(false);
    return -1;
}

int Model::getOutputPortIndex(const std::string & name) const
{
    MapStringPort::const_iterator it = m_outPortList.begin();
    int r = 0;

    while (it != m_outPortList.end()) {
	if ((*it).first == name) {
	    return r;
        }
        ++it;
	++r;
    }
    AssertI(false);
    return -1;
}

int Model::getStatePortIndex(const std::string & name) const
{
    MapStringPort::const_iterator it = m_statePortList.begin();
    int r = 0;

    while (it != m_statePortList.end()) {
	if ((*it).first == name) {
            return r;
        }
	++it;
	++r;
    }
    AssertI(false);
    return -1;
}

void Model::addInitPort(const std::string & name)
{
    m_initPortList[name] = new Port(this, name);
}

void Model::addInputPort(const std::string & name)
{
    m_inPortList[name] = new Port(this, name);
}

void Model::addOutputPort(const std::string & name)
{
    m_outPortList[name] = new Port(this, name);
}

void Model::addStatePort(const std::string & name)
{
    m_statePortList[name] = new Port(this, name);
}

bool Model::delInitPort(const std::string& name)
{
    MapStringPort::iterator it = m_initPortList.find(name);
    if (it != m_initPortList.end()) {
        delete (*it).second;
        m_initPortList.erase(it);
        return true;
    }
    return false;
}

bool Model::delInputPort(const std::string& name)
{
    graph::CoupledModel* parent = (m_parent) ? toCoupled(m_parent) : NULL;
    graph::CoupledModel* cpled = (isCoupled() ? Model::toCoupled(this) : NULL);

    if (parent == NULL) {
        if ((cpled and cpled->hasConnection(this, name) == false) or
            isAtomic()) {
            MapStringPort::iterator it = m_inPortList.find(name);
            if (it != m_inPortList.end()) {
                delete (*it).second;
                m_inPortList.erase(it);
                return true;
            }
        }
    } else {
        if (parent->hasConnection(this, name) == false) {
            if ((cpled and cpled->hasConnection(this, name) == false) or
                isAtomic()) {
                MapStringPort::iterator it = m_inPortList.find(name);
                if (it != m_inPortList.end()) {
                    delete (*it).second;
                    m_inPortList.erase(it);
                    return true;
                }
            }
        }
    }
    return false;
}

bool Model::delOutputPort(const std::string& name)
{
    graph::CoupledModel* parent = (m_parent) ? toCoupled(m_parent) : NULL;
    graph::CoupledModel* cpled = (isCoupled() ? Model::toCoupled(this) : NULL);

    if (parent == NULL) {
        if ((cpled and cpled->hasConnection(this, name) == false) or
            isAtomic()) {
            MapStringPort::iterator it = m_outPortList.find(name);
            if (it != m_outPortList.end()) {
                delete (*it).second;
                m_outPortList.erase(it);
                return true;
            }
        }
    } else {
        if (parent->hasConnection(this, name) == false) {
            if ((cpled and cpled->hasConnection(this, name) == false) or
                isAtomic()) {
                MapStringPort::iterator it = m_outPortList.find(name);
                if (it != m_outPortList.end()) {
                    delete (*it).second;
                    m_outPortList.erase(it);
                    return true;
                }
            }
        }
    }
    return false;
}

bool Model::delStatePort(const std::string& name)
{
    MapStringPort::iterator it = m_statePortList.find(name);
    if (it != m_statePortList.end()) {
        delete (*it).second;
        m_statePortList.erase(it);
        return true;
    }
    return false;
}

void Model::delConnectionOnPort(const std::string& name)
{
    graph::CoupledModel* parent = (m_parent) ? toCoupled(m_parent) : NULL;
    graph::CoupledModel* cpled = (isCoupled() ? Model::toCoupled(this) : NULL);
    graph::Connection* cnx = 0;

    if (parent == NULL) {
        cnx = cpled->hasConnection(this, name);
        cpled->delConnection(cnx);
    } else {
        cnx = parent->hasConnection(this, name);
        parent->delConnection(cnx);

        if (cpled) {
            cnx = cpled->hasConnection(this, name);
            cpled->delConnection(cnx);
        }
    }
}

void Model::delInputPortAndConnection(const std::string& name)
{
    delConnectionOnPort(name);
    delInputPort(name);
}

void Model::delOutputPortAndConnection(const std::string& name)
{
    delConnectionOnPort(name);
    delOutputPort(name);
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

void Model::clearInitPort()
{
    map < string, Port* >::iterator it1 = m_initPortList.begin();

    while (it1 != m_initPortList.end()) {
        delete(it1++)->second;
    }

    m_initPortList.clear();
}

void Model::clearInputPort()
{
    map < string, Port* >::iterator it2 = m_inPortList.begin();

    while (it2 != m_inPortList.end()) {
        delete(it2++)->second;
    }

    m_inPortList.clear();
}

void Model::clearOutputPort()
{
    map < string, Port* >::iterator it3 = m_outPortList.begin();

    while (it3 != m_outPortList.end()) {
        delete(it3++)->second;
    }

    m_outPortList.clear();
}

void Model::clearStatePort()
{
    map < string, Port* >::iterator it4 = m_statePortList.begin();

    while (it4 != m_statePortList.end()) {
        delete(it4++)->second;
    }

    m_statePortList.clear();
}

const MapStringPort & Model::getInitPortList() const
{
    return m_initPortList;
}

const MapStringPort & Model::getInputPortList() const
{
    return m_inPortList;
}

const MapStringPort & Model::getOutputPortList() const
{
    return m_outPortList;
}

const MapStringPort & Model::getStatePortList() const
{
    return m_statePortList;
}

void Model::mergePort(std::list < std::string >& inlist,
                      std::list < std::string >& outlist,
                      std::list < std::string >& statelist,
                      std::list < std::string >& initlist)
{
    MapStringPort::iterator it;
    std::list < std::string >::iterator jt;

    for (it = m_inPortList.begin(); it != m_inPortList.end(); ++it) {
        jt = std::find(inlist.begin(), inlist.end(), (*it).first);
        if (jt == inlist.end()) delInputPortAndConnection((*it).first);
        inlist.erase(jt);
    }
    for (jt = inlist.begin(); jt != inlist.end(); ++jt)
        addInputPort(*jt);

    for (it = m_outPortList.begin(); it != m_outPortList.end(); ++it) {
        jt = std::find(outlist.begin(), outlist.end(), (*it).first);
        if (jt == outlist.end())
            delOutputPortAndConnection((*it).first);
        else
            outlist.erase(jt);
    }
    for (jt = outlist.begin(); jt != outlist.end(); ++jt)
        addOutputPort(*jt);

    for (it = m_statePortList.begin(); it != m_statePortList.end(); ++it) {
        jt = std::find(statelist.begin(), statelist.end(), (*it).first);
        if (jt == statelist.end())
            delStatePort((*it).first);
        else
            statelist.erase(jt);
    }
    for (jt = statelist.begin(); jt != statelist.end(); ++jt)
        addStatePort(*jt);

    for (it = m_initPortList.begin(); it != m_initPortList.end(); ++it) {
        jt = std::find(initlist.begin(), initlist.end(), (*it).first);
        if (jt == initlist.end())
            delInitPort((*it).first);
        else
            initlist.erase(jt);
    }
    for (jt = initlist.begin(); jt != initlist.end(); ++jt)
        addInitPort(*jt);
}

Model* Model::parseXMLmodel(xmlpp::Element* modelNode,
                             CoupledModel* parent)
{
    Model *model = 0;
    string name = get_attribute(modelNode,"NAME");
    string type = get_attribute(modelNode,"TYPE");

    if (type == "atomic") {
	model = new AtomicModel(parent);
    } else if (type == "coupled") {
	model = new CoupledModel(parent);
    } else if (type == "no-vle") {
	model = new NoVLEModel(parent);
    } else {
        Throw(vle::utils::ParseError, boost::format(
                    "Unknow type tag [%1%] for model %2%\n") % type % name);
    }

    model->m_name = name;
    model->parseXML(modelNode, parent);

    return model;
}

bool Model::parseXMLports(xmlpp::Element* p_modelNode)
{
    xmlpp::Element* init = exist_children(p_modelNode,"INIT") ?
        get_children(p_modelNode,"INIT") : 0;

    if (init)
	parseXMLport(p_modelNode, init, m_initPortList);

    xmlpp::Element* in = exist_children(p_modelNode,"IN") ?
	get_children(p_modelNode,"IN") : 0;

    if (in)
	parseXMLport(p_modelNode, in, m_inPortList);

    xmlpp::Element* out = exist_children(p_modelNode,"OUT") ?
	get_children(p_modelNode,"OUT") : 0;

    if (out)
	parseXMLport(p_modelNode, out, m_outPortList);

    xmlpp::Element* state =exist_children(p_modelNode,"STATE") ?
	get_children(p_modelNode,"STATE") : 0;

    if (state)
	parseXMLport(p_modelNode, state, m_statePortList);

    return true;
}

/*
 XML syntax of port

  <PORT NAME="">
  <DATA NAME="">
  <METADATA>...</METADATA>
  <UNIT ... />
  <TYPE ... />
  <CONTENT>...</CONTENT>
  </DATA>
  </PORT>
*/

bool Model::parseXMLport(xmlpp::Element*,
			 xmlpp::Element* p_portNode,
			 std::map < std::string , Port* > &p_portList)
{
    xmlpp::Node::NodeList lst = p_portNode->get_children("PORT");
    xmlpp::Node::NodeList::iterator it = lst.begin();

    while ( it != lst.end() ) {
	xmlpp::Element * portNode = ( xmlpp::Element* )( *it );
	string name = get_attribute(portNode,"NAME");

	p_portList[name] = new Port(this, name);
	++it;
    }
    return true;
}

bool Model::parseXMLspace(xmlpp::Element*, xmlpp::Element*)
{
    return true;
}

bool Model::parseXMLtime(xmlpp::Element*, xmlpp::Element*)
{
    return true;
}

void Model::writePortListXML(std::ostream& out) const
{
    if (not m_initPortList.empty()) {
    out << "<init>\n";
	MapStringPort::const_iterator it = m_initPortList.begin();
        for (;it != m_initPortList.end(); ++it) {
            out << " <port name=\"" << (*it).first << "\" />\n";
        }
        out << "</init>\n";
    }

    if (not m_inPortList.empty()) {
        out << "<in>\n";
        MapStringPort::const_iterator it = m_inPortList.begin();
        for (;it != m_inPortList.end(); ++it) {
            out << "<port name=\"" << (*it).first << "\" />\n";
        }
        out << "</in>\n";
    }

    if (not m_outPortList.empty()) {
        out << "<out>\n";
	MapStringPort::const_iterator it = m_outPortList.begin();
        for(; it != m_outPortList.end(); ++it) {
            out << "<port name=\"" << (*it).first << "\" />\n";
	}
        out << "</out>\n";
    }

    if (not m_statePortList.empty()) {
        out << "<state>\n";
	MapStringPort::const_iterator it = m_statePortList.begin();
        for (; it != m_statePortList.end(); ++it) {
            out << "<port name=\"" << (*it).first << "\" />\n";
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

void Model::getAtomicModelList(Model* p_model,
                               std::vector < AtomicModel* >& p_list)
{
    if (p_model->isAtomic()) {
        p_list.push_back((AtomicModel*)p_model);
    } else {
	vector < CoupledModel* > coupledModelList;
	coupledModelList.push_back((CoupledModel*)p_model);
        while (!coupledModelList.empty()) {
	    CoupledModel* m = coupledModelList.front();
	    vector < Model* > v = m->getModelList();
	    vector < Model* >::iterator it = v.begin();

            while (it != v.end()) {
		PModel n = *it;
		if (n->isAtomic()) p_list.push_back((AtomicModel*)n);
		else coupledModelList.push_back((CoupledModel*)n);
		++it;
	    }
	    coupledModelList.erase(coupledModelList.begin());
	}
    }
}

bool Model::isInList(const VectorModel& lst, graph::Model* m)
{
    return std::find(lst.begin(), lst.end(), m) != lst.end();
}

}} // namespace vle graph
