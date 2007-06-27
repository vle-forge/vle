/**
 * @file graph/CoupledModel.cpp
 * @author The VLE Development Team.
 * @brief Represent the DEVS coupled model. This class have a list of children
 * models, three list of input, output and state connections.
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
#include <vle/graph/Model.hpp>
#include <vle/graph/Port.hpp>
#include <vle/graph/TargetPort.hpp>
#include <vle/graph/Connection.hpp>
#include <vle/utils/XML.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Tools.hpp>
#include <set>

using std::vector;
using std::string;
using std::list;
using namespace vle::utils::xml;



namespace vle { namespace graph {

CoupledModel::CoupledModel(const std::string& name, CoupledModel* parent) :
    Model(name, parent)
{
}

CoupledModel::CoupledModel(const CoupledModel& model) :
    Model(model)
{
    for (VectorModel::const_iterator it = model.m_modelList.begin();
         it != model.m_modelList.end();++it) {
        graph::Model* cloned = (*it).second->clone();
        cloned->setParent(this);
        addModel(cloned);
    }

    vector < Connection* >::const_iterator it;
    for (it = model.m_internalConnectionList.begin(); it !=
         model.m_internalConnectionList.end(); ++it) {
        Model* v_originModel = getModel((*it)->getOriginModel()->getName());
        Model* v_destinationModel =
            getModel((*it)->getDestinationModel()->getName());

        addInternalConnection(v_originModel,(*it)->getOriginPort()->getName(),
                              v_destinationModel,
                              (*it)->getDestinationPort()->getName());
    }

    for (it = model.m_inputConnectionList.begin(); it !=
         model.m_inputConnectionList.end(); ++it) {
        Model* v_destinationModel =
            getModel((*it)->getDestinationModel()->getName());

        addInputConnection(this,(*it)->getOriginPort()->getName(),
                           v_destinationModel,
                           (*it)->getDestinationPort()->getName());
    }
    
    for (it = model.m_outputConnectionList.begin(); it !=
         model.m_outputConnectionList.end(); ++it) {
        Model* v_originModel = getModel((*it)->getOriginModel()->getName());

	addOutputConnection(v_originModel,(*it)->getOriginPort()->getName(),
                            this,(*it)->getDestinationPort()->getName());
    }
}

CoupledModel::~CoupledModel()
{
    delAllConnection();

    for (VectorModel::iterator it = m_modelList.begin(); it !=
         m_modelList.end(); ++it) {
        delete (*it).second;
    }
}

Model* CoupledModel::clone() const
{
    return new CoupledModel(*this);
}



/********************************************************************
 *
 * GET FUNCTIONS
 *
 ********************************************************************/



const VectorConnection & CoupledModel::getInputConnectionList() const
{
    return m_inputConnectionList;
}



const VectorConnection & CoupledModel::getOutputConnectionList() const
{
    return m_outputConnectionList;
}


const VectorConnection & CoupledModel::getInternalConnectionList() const
{
    return m_internalConnectionList;
}



/**************************************************************
 *
 * CONNECTION
 *
 *************************************************************/



void CoupledModel::addInputConnection(Model * src, const std::string & portSrc,
				      Model * dst, const std::string & portDst)
{
    AssertI(src and dst);

    Port * psrc = src->getInPort(portSrc);
    Port * pdst = dst->getInPort(portDst);

    AssertI(psrc and pdst);
    AssertI(src == this and getModel(dst) != 0);
    AssertI(existInputPort(portSrc) and dst->existInputPort(portDst));

    m_inputConnectionList.push_back(
	new Connection(src, psrc, dst, pdst));
}


void CoupledModel::addOutputConnection(Model * src, const std::string & portSrc,
				       Model * dst, const std::string & portDst)
{
    AssertI(src and dst);

    Port * psrc = src->getOutPort(portSrc);
    Port * pdst = dst->getOutPort(portDst);

    AssertI(psrc and pdst);
    AssertI(getModel(src) != 0 and dst == this);
    AssertI(src->existOutputPort(portSrc) and
	   existOutputPort(portDst) == true);

    m_outputConnectionList.push_back(
	new Connection(src, psrc, dst, pdst));
}



void CoupledModel::addInternalConnection(Model* src, const std::string&
                                         portSrc, Model* dst, const
                                         std::string& portDst) {
    AssertI(src and dst);

    Port * psrc = src->getOutPort(portSrc);
    Port * pdst = dst->getInPort(portDst);

    AssertI(psrc and pdst);
    AssertI(getModel(src) != 0 and getModel(dst) != 0);
    AssertI(src->existOutputPort(portSrc) == true and
	   dst->existInputPort(portDst) == true);

    m_internalConnectionList.push_back(
	new Connection(src, psrc, dst, pdst));
}

void CoupledModel::addInputConnection(const std::string& src,
                                      const std::string& portSrc,
                                      const std::string& dst,
                                      const std::string& portDst)
{
    AssertI(src == getName());
    Model* msrc = this;
    Model* mdst = getModel(dst);
    Port* psrc = msrc->getInPort(portSrc);
    Port* pdst = mdst->getInPort(portDst);

    AssertI(msrc);
    AssertI(mdst);
    AssertI(psrc);
    AssertI(pdst);

    m_inputConnectionList.push_back(
	new Connection(msrc, psrc, mdst, pdst));
}

void CoupledModel::addOutputConnection(const std::string& src,
                                       const std::string& portSrc,
                                       const std::string& dst,
                                       const std::string& portDst)
{
    AssertI(dst == getName());
    Model* msrc = getModel(src);
    AssertI(msrc);
    Model* mdst = this;
    AssertI(mdst);
    Port* psrc = msrc->getOutPort(portSrc);
    Port* pdst = mdst->getOutPort(portDst);
    AssertI(psrc);
    AssertI(pdst);

    m_outputConnectionList.push_back(
	new Connection(msrc, psrc, mdst, pdst));
}

void CoupledModel::addInternalConnection(const std::string& src,
                                         const std::string& portSrc,
                                         const std::string& dst,
                                         const std::string& portDst)
{
    Model* msrc = getModel(src);
    Model* mdst = getModel(dst);
    AssertI(msrc);
    AssertI(mdst);

    Port* psrc = msrc->getOutPort(portSrc);
    Port* pdst = mdst->getInPort(portDst);
    AssertI(psrc);
    AssertI(pdst);
    
    m_internalConnectionList.push_back(
	new Connection(msrc, psrc, mdst, pdst));
}


Connection* CoupledModel::getInputConnection(const std::string& portSrc,
                                             const std::string& dst,
                                             const std::string& portDst)
{
    Model* mdst = getModel(dst);
    AssertI(mdst);

    return getInputConnection(this, portSrc, mdst, portDst);
}


Connection *
CoupledModel::getInputConnection(Model * src, const std::string & portSrc,
                                 Model * dst, const std::string & portDst)
{
    AssertI(src and src);

    Port * psrc = src->getInPort(portSrc);
    Port * pdst = dst->getInPort(portDst);

    AssertI(psrc and pdst);

    VectorConnection::iterator it = m_inputConnectionList.begin();
    while (it != m_inputConnectionList.end())
    {
	if ((*it)->getOriginModel() == src and
	    (*it)->getOriginPort() == psrc and
	    (*it)->getDestinationModel() == dst and
	    (*it)->getDestinationPort() == pdst)
	    return (*it);
	it++;
    }
    return 0;
}


Connection* CoupledModel::getOutputConnection(const std::string& src,
                                              const std::string& portSrc,
                                              const std::string& portDst)
{
    Model* msrc = getModel(src);
    AssertI(msrc);

    return getOutputConnection(msrc, portSrc, this, portDst);
}

Connection *
CoupledModel::getOutputConnection(Model * src, const std::string & portSrc,
				  Model * dst, const std::string & portDst)
{
    AssertI(src and dst);

    Port * psrc = src->getOutPort(portSrc);
    Port * pdst = dst->getOutPort(portDst);

    AssertI(psrc and pdst);

    VectorConnection::iterator it = m_outputConnectionList.begin();
    while (it != m_outputConnectionList.end())
    {
	if ((*it)->getOriginModel() == src and
	    (*it)->getOriginPort() == psrc and
	    (*it)->getDestinationModel() == dst and
	    (*it)->getDestinationPort() == pdst)
	    return (*it);
	it++;
    }
    return 0;
}

Connection* CoupledModel::getInternalConnection(const std::string& src,
                                                const std::string& portSrc,
                                                const std::string& dst,
                                                const std::string& portDst)
{
    Model* msrc = getModel(src);
    Model* mdst = getModel(dst);
    AssertI(msrc);
    AssertI(mdst);

    return getInternalConnection(msrc, portSrc, mdst, portDst);
}

Connection *
CoupledModel::getInternalConnection(Model * src, const std::string & portSrc,
				    Model * dst, const std::string & portDst)
{
    AssertI(src and dst);

    Port * psrc = src->getOutPort(portSrc);
    Port * pdst = dst->getInPort(portDst);

    AssertI(psrc and pdst);

    VectorConnection::iterator it = m_internalConnectionList.begin();
    while (it != m_internalConnectionList.end())
    {
	if ((*it)->getOriginModel() == src and
	    (*it)->getOriginPort() == psrc and
	    (*it)->getDestinationModel() == dst and
	    (*it)->getDestinationPort() == pdst)
	    return (*it);
	it++;
    }
    return 0;
}



void CoupledModel::delInputConnection(Model * src, const std::string & portSrc,
                                      Model * dst, const std::string & portDst)
{
    AssertI(src and dst);
    AssertI(src == this and dst != this);

    Port * psrc = src->getInPort(portSrc);
    Port * pdst = dst->getInPort(portDst);

    AssertI(psrc and pdst);

    VectorConnection::iterator it = m_inputConnectionList.begin();
    while (it != m_inputConnectionList.end())
    {
	if ((*it)->getOriginModel() == src and
	    (*it)->getOriginPort() == psrc and
	    (*it)->getOriginModel() == dst and
	    (*it)->getOriginPort() == pdst)
	{
	    delete (*it);
	    m_inputConnectionList.erase(it);
	    return;
	}
	++it;
    }
}



void CoupledModel::delOutputConnection(Model * src, const std::string & portSrc,
                                       Model * dst, const std::string & portDst)
{
    AssertI(src and dst);
    AssertI(dst == this and src != this);

    Port * psrc = src->getOutPort(portSrc);
    Port * pdst = dst->getOutPort(portDst);

    AssertI(psrc and pdst);

    VectorConnection::iterator it = m_outputConnectionList.begin();
    while (it != m_outputConnectionList.end())
    {
	if ((*it)->getDestinationModel() == src and
	    (*it)->getDestinationPort() == psrc and
	    (*it)->getDestinationModel() == dst and
	    (*it)->getDestinationPort() == pdst)
	{
	    delete (*it);
	    m_outputConnectionList.erase(it);
	    return;
	}
	++it;
    }
}



void CoupledModel::delInternalConnection(Model * src, const std::string & portSrc,
                                         Model * dst, const std::string & portDst)
{
    AssertI(src and dst);
    AssertI(dst != this and src != this);

    Port * psrc = src->getOutPort(portSrc);
    Port * pdst = dst->getInPort(portDst);

    AssertI(psrc and pdst);

    VectorConnection::iterator it = m_internalConnectionList.begin();
    while (it != m_internalConnectionList.end())
    {
	if ((*it)->getOriginModel() == src and
	    (*it)->getOriginPort() == psrc and
	    (*it)->getDestinationModel() == dst and
	    (*it)->getDestinationPort() == pdst)
	{
	    delete (*it);
	    m_internalConnectionList.erase(it);
	    return;
	}
	++it;
    }
}



/**
 * delete input connection connect from input connection list
 *
 * @param connect connection to delete
 */
void CoupledModel::delInputConnection(Connection * connect)
{
    AssertI(connect and connect->getOriginModel() == this);

    VectorConnection::iterator it = m_inputConnectionList.begin();
    while (it != m_inputConnectionList.end())
    {
	if ((*it) == connect)
	{
	    delete (*it);
	    m_inputConnectionList.erase(it);
	    return;
	}
	++it;
    }

    AssertI(false);
}



/**
 * delete output connection connect from output connection list
 *
 * @param connect connection to delete
 */
void CoupledModel::delOutputConnection(Connection * connect)
{
    AssertI(connect and connect->getDestinationModel() == this);

    VectorConnection::iterator it = m_outputConnectionList.begin();
    while (it != m_outputConnectionList.end())
    {
	if ((*it) == connect)
	{
	    delete (*it);
	    m_outputConnectionList.erase(it);
	    return;
	}
	++it;
    }

    AssertI(false);
}



/**
 * delete internal connection connect from internal connection list
 *
 * @param connect connection to delete
 */
void CoupledModel::delInternalConnection(Connection * connect)
{
    AssertI(connect);
    AssertI(connect->getDestinationModel() != this);
    AssertI(connect->getOriginModel() != this);

    VectorConnection::iterator it = m_internalConnectionList.begin();
    while (it != m_internalConnectionList.end())
    {
	if ((*it) == connect)
	{
	    delete (*it);
	    m_internalConnectionList.erase(it);
	    return;
	}
	++it;
    }

    AssertI(false);
}



/**
 * delete connection connect. Input, output and internal connection are tested
 * to find connect and destroy them.
 *
 * @param connect connection to delete
 */
void CoupledModel::delConnection(Connection * connect)
{
    if (connect->getOriginModel() == this)
	delInputConnection(connect);
    else if (connect->getDestinationModel() == this)
	delOutputConnection(connect);
    else
	delInternalConnection(connect);
}

void CoupledModel::delAllConnection(Model* m)
{
    VectorConnection::iterator it;

    it = m_inputConnectionList.begin();
    while (it != m_inputConnectionList.end()) {
        if ((*it)->getDestinationModel() == m) {
	    delete (*it);
	    m_inputConnectionList.erase(it);
	    it = m_inputConnectionList.begin();
        } else {
            ++it;
        }
    }

    it = m_outputConnectionList.begin();
    while (it != m_outputConnectionList.end()) {
        if ((*it)->getOriginModel() == m) {
	    delete (*it);
	    m_outputConnectionList.erase(it);
	    it = m_outputConnectionList.begin();
        } else {
            ++it;
        }
    }

    it = m_internalConnectionList.begin();
    while (it != m_internalConnectionList.end()) {
        if (((*it)->getOriginModel() == m) or 
            ((*it)->getDestinationModel() == m)) {
	    delete (*it);
	    m_internalConnectionList.erase(it);
	    it = m_internalConnectionList.begin();
        } else {
            ++it;
        }
    }
}

void CoupledModel::delAllConnection()
{
    VectorConnection::iterator it;

    it = m_inputConnectionList.begin();
    while (it != m_inputConnectionList.end()) {
        delete (*it);
        ++it;
    }

    it = m_outputConnectionList.begin();
    while (it != m_outputConnectionList.end()) {
        delete (*it);
        ++it;
    }

    it = m_internalConnectionList.begin();
    while (it != m_internalConnectionList.end()) {
        delete (*it);
        ++it;
    }

    m_inputConnectionList.erase(m_inputConnectionList.begin(),
                                m_inputConnectionList.end());
    m_outputConnectionList.erase(m_outputConnectionList.begin(),
                                 m_outputConnectionList.end());
    m_internalConnectionList.erase(m_internalConnectionList.begin(),
                                   m_internalConnectionList.end());
}

void CoupledModel::replace(Model* old, Model* mdl)
{
    Assert(vle::utils::InternalError, old, "Replace a null model ?");
    Assert(vle::utils::InternalError, mdl, "Replace a model by null ?");

    VectorConnection::iterator it;

    for (it = m_inputConnectionList.begin(); it != m_inputConnectionList.end();
         ++it) {
        if ((*it)->getDestinationModel() == old) {
            (*it)->setDestinationModel(mdl);
            (*it)->setDestinationPort(
                mdl->getInPort((*it)->getDestinationPort()->getName()));
        }
    }

    for (it = m_outputConnectionList.begin();
         it != m_outputConnectionList.end(); ++it) {
        if ((*it)->getOriginModel() == old) {
            (*it)->setOriginModel(mdl);
            (*it)->setOriginPort(
                mdl->getInPort((*it)->getOriginPort()->getName()));
        }
    }

    for (it = m_internalConnectionList.begin();
         it != m_internalConnectionList.end(); ++it) {
        if ((*it)->getDestinationModel() == old) {
            (*it)->setDestinationModel(mdl);
            (*it)->setDestinationPort(
                mdl->getInPort((*it)->getDestinationPort()->getName()));
        }
        if ((*it)->getOriginModel() == old) {
            (*it)->setOriginModel(mdl);
            (*it)->setOriginPort(
                mdl->getInPort((*it)->getOriginPort()->getName()));
        }
    }

    delModel(old);
    addModel(mdl);
}

bool CoupledModel::hasConnection(Model* model, const std::string& name) const
{
    VectorConnection::const_iterator it;

    for (it = m_inputConnectionList.begin(); it !=
         m_inputConnectionList.end(); ++it) {
        const Connection* cnx = (*it);
        if (cnx->getOriginModel() == model and
            cnx->getOriginPort()->getName() == name) {
            return true;
        }
        if (cnx->getDestinationModel()  == model and
            cnx->getDestinationPort()->getName() == name) {
            return true;
        }
    }
    for (it = m_outputConnectionList.begin(); it !=
         m_outputConnectionList.end(); ++it) {
        const Connection* cnx = (*it);
        if (cnx->getOriginModel() == model and
            cnx->getOriginPort()->getName() == name) {
            return true;
        }
        if (cnx->getDestinationModel() == model and
            cnx->getDestinationPort()->getName() == name) {
            return true;
        }
    }
    for (it = m_internalConnectionList.begin(); it !=
         m_internalConnectionList.end(); ++it) {
        const Connection* cnx = (*it);
        if (cnx->getOriginModel() == model and
            cnx->getOriginPort()->getName() == name) {
            return true;
        }
        if (cnx->getDestinationModel() == model and
            cnx->getDestinationPort()->getName() == name) {
            return true;
        }
    }
    return false;
}

Connection* CoupledModel::hasConnection(Model* model, const std::string& name)
{
    VectorConnection::const_iterator it;

    for (it = m_inputConnectionList.begin(); it !=
         m_inputConnectionList.end(); ++it) {
        Connection* cnx = (*it);
        if (cnx->getOriginModel() == model and
            cnx->getOriginPort()->getName() == name) {
            return cnx;
        }
        if (cnx->getDestinationModel()  == model and
            cnx->getDestinationPort()->getName() == name) {
            return cnx;
        }
    }
    for (it = m_outputConnectionList.begin(); it !=
         m_outputConnectionList.end(); ++it) {
        Connection* cnx = (*it);
        if (cnx->getOriginModel() == model and
            cnx->getOriginPort()->getName() == name) {
            return cnx;
        }
        if (cnx->getDestinationModel() == model and
            cnx->getDestinationPort()->getName() == name) {
            return cnx;
        }
    }
    for (it = m_internalConnectionList.begin(); it !=
         m_internalConnectionList.end(); ++it) {
        Connection* cnx = (*it);
        if (cnx->getOriginModel() == model and
            cnx->getOriginPort()->getName() == name) {
            return cnx;
        }
        if (cnx->getDestinationModel() == model and
            cnx->getDestinationPort()->getName() == name) {
            return cnx;
        }
    }
    return 0;
}

bool CoupledModel::hasConnectionProblem(const VectorModel& lst) const
{
    const graph::VectorConnection& in = getInputConnectionList();
    const graph::VectorConnection& out = getOutputConnectionList();
    const graph::VectorConnection& inter = getInternalConnectionList();

    graph::VectorConnection::const_iterator it = in.begin();
    while(it != in.end()) {
        graph::Model* org =(*it)->getOriginModel();
        graph::Model* dst =(*it)->getDestinationModel();
        AssertI(org == this);

        if(isInList(lst, dst) == true)
            return true;

        ++it;
    }

    it = out.begin();
    while(it != out.end()){
        graph::Model* org =(*it)->getOriginModel();
        graph::Model* dst =(*it)->getDestinationModel();
        AssertI(dst == this);

        if(isInList(lst, org) == true)
            return true;

        ++it;
    }

    it = inter.begin();
    while(it != inter.end()) {
        graph::Model* org =(*it)->getOriginModel();
        graph::Model* dst =(*it)->getDestinationModel();

        if((isInList(lst, org) == true and isInList(lst, dst) == false) or
           (isInList(lst, org) == false and isInList(lst, dst) == true))
            return true;

        ++it;
    }

    return false;
}


vector < TargetPort* > CoupledModel::getTargetPortList(Port* p_port)
{
    vector < TargetPort* > v_list;

    // Internal connections
    vector < Connection* >::iterator it3 =
	m_internalConnectionList.begin();

    while (it3 != m_internalConnectionList.end()) {
	Connection* c = *it3;
	if (c->getOriginPort() == p_port) {
	    Model* m = c->getDestinationModel();

	    if (m != this) {
		if (m->isAtomic()) {
		    v_list.push_back(
			new TargetPort(c->getDestinationPort()->getName(),
				       (AtomicModel*)m));
		} else {
		    vector < TargetPort* > v =
			((CoupledModel*)m)->getTargetPortList(
			    c->getDestinationPort());
		    v_list.insert(v_list.end(), v.begin(), v.end());
		}
	    }
	}
	++it3;
    }

    // Input connections
    vector < Connection* >::iterator it1 = m_inputConnectionList.begin();

    while (it1 != m_inputConnectionList.end()) {
	Connection* c = *it1;

	if (c->getOriginPort() == p_port) {
	    Model* m = c->getDestinationModel();

	    if (m != this) {
		if (m->isAtomic()) {
		    v_list.push_back(
			new TargetPort(c->getDestinationPort()->getName(),
				       (AtomicModel*)m));
		} else {
		    vector < TargetPort* > v =
			((CoupledModel*) m)->getTargetPortList(
			    c->getDestinationPort());
		    v_list.insert(v_list.end(), v.begin(), v.end());
		}
	    }
	}
	++it1;
    }

    // Output connections
    vector < Connection* >::iterator it2 = m_outputConnectionList.begin();
    while (it2 != m_outputConnectionList.end()) {
	Connection* c = *it2;

	if (c->getOriginPort() == p_port) {
	    if (getParent() != NULL) {
		vector < TargetPort* > v =
		    getParent()->getTargetPortList(
			c->getDestinationPort());

		v_list.insert(v_list.end(), v.begin(), v.end());
	    }
	}
	++it2;
    }

    return v_list;
}

bool CoupledModel::isAtomic() const
{
    return false;
}

bool CoupledModel::isCoupled() const
{
    return true;
}

bool CoupledModel::isNoVLE() const
{
    return false;
}

Model* CoupledModel::findModel(const std::string& name) const
{
    VectorModel::const_iterator it = m_modelList.find(name);
    if (it == m_modelList.end()) {
        return 0;
    } else {
        return it->second;
    }
}

Model* CoupledModel::getModel(const std::string& modelname)
{
    if (getName() == modelname) {
	return this;
    } else {
        return findModel(modelname);
    }
}

Model* CoupledModel::getModel(Model* model)
{
    for (VectorModel::iterator it = m_modelList.begin(); it !=
         m_modelList.end(); ++it) {
        if (model == (*it).second)
	    return model;
    }

    return 0;
}

void CoupledModel::addModel(Model* model)
{
    AssertI(not exist(model->getName()));

    model->setParent(this);
    m_modelList[model->getName()] = model;
}

AtomicModel* CoupledModel::addAtomicModel(const std::string& name)
{
    AssertI(not exist(name));
    AtomicModel* x = new AtomicModel(name, this);
    m_modelList[name] = x;
    return x;
}

NoVLEModel* CoupledModel::addNoVLEModel(const std::string& name)
{
    AssertI(not exist(name));
    NoVLEModel* x = new NoVLEModel(name, this);
    m_modelList[name] = x;
    return x;
}

CoupledModel* CoupledModel::addCoupledModel(const std::string& name)
{
    AssertI(not exist(name));
    CoupledModel* x = new CoupledModel(name, this);
    m_modelList[name] = x;
    return x;
}

void CoupledModel::delModel(Model* model)
{
    VectorModel::iterator it = m_modelList.find(model->getName());
    if (it != m_modelList.end()) {
        delAllConnection(model);
        m_modelList.erase(it);
        delete model;
    }
}

void CoupledModel::delAllModel()
{
    for (VectorModel::iterator it = m_modelList.begin(); it !=
         m_modelList.end(); ++it) {
        delModel(it->second);
        ++it;
    }
}

void CoupledModel::attachModel(Model* model)
{
    AssertI(not exist(model->getName()));

    if (model->getParent()) {
        model->getParent()->detachModel(model);
    }

    m_modelList[model->getName()] = model;
    model->setParent(this);
}

void CoupledModel::attachModels(VectorModel& models)
{
    for (VectorModel::iterator it = models.begin(); it != models.end(); ++it) {
        attachModel(it->second);
    }
}

void CoupledModel::detachModel(Model* model)
{
    VectorModel::iterator it = m_modelList.find(model->getName());
    if (it != m_modelList.end()) {
        it->second->setParent(0);
        m_modelList.erase(it);
    } else {
        Throw(utils::InternalError, (boost::format(
                    "Model %1% is not attached to the coupled model %2%") %
                model->getName() % getName()));
    }
}

void CoupledModel::detachModels(const VectorModel& models)
{
    for (VectorModel::const_iterator it = models.begin(); it != models.end();
         ++it) {
	detachModel(it->second);
    }
}

void CoupledModel::attachInternalConnection(
                const std::list < Connection * > & c)
{
    std::list < Connection * >::const_iterator it;

    it = c.begin();

    while (it != c.end())
    {
	m_internalConnectionList.push_back(*it);
	++it;
    }
}

void CoupledModel::writeXML(std::ostream& out) const
{
    out << "<model name=\"" << getName() << "\" " << " type=\"coupled\" >\n";
    writePortListXML(out);
    out << "<submodels>\n";

    const size_t szInConnection = m_inputConnectionList.size();
    const size_t szOutConnection = m_outputConnectionList.size();
    const size_t szInternalConnection = m_internalConnectionList.size();

    for (VectorModel::const_iterator it = m_modelList.begin(); it !=
         m_modelList.end(); ++it) {
        it->second->writeXML(out);
    }

    out << "<connections>\n";
    for (size_t i = 0; i < szInConnection; ++i) {
        out << "<connection type=\"input\">\n"
            << " <origin model=\""
            << m_inputConnectionList[i]->getOriginModel()->getName()
            << "\" port=\""
            << m_inputConnectionList[i]->getOriginPort()->getName()
            << "\" />\n"
            << " <destination model=\""
            << m_inputConnectionList[i]->getDestinationModel()->getName()
            << "\" port=\""
            << m_inputConnectionList[i]->getDestinationPort()->getName()
            << "\" />\n"
            << "</connection>\n";
    }

    for (size_t i = 0; i < szOutConnection; ++i) {
        out << "<connection type=\"output\">\n"
            << " <origin model=\""
            << m_outputConnectionList[i]->getOriginModel()->getName()
            << "\" port=\""
            << m_outputConnectionList[i]->getOriginPort()->getName()
            << "\" />\n"
            << " <destination model=\""
            << m_outputConnectionList[i]->getDestinationModel()->getName()
            << "\" port=\""
            << m_outputConnectionList[i]->getDestinationPort()->getName()
            << "\" />\n"
            << "</connection>\n";
    }

    for (size_t i = 0; i < szInternalConnection; ++i) {
        out << "<connection type=\"internal\">\n"
            << " <origin model=\""
            << m_internalConnectionList[i]->getOriginModel()->getName()
            << "\" port=\""
            << m_internalConnectionList[i]->getOriginPort()->getName()
            << "\" />\n"
            << " <destination model=\""
            << m_internalConnectionList[i]->getDestinationModel()->getName()
            << "\" port=\""
            << m_internalConnectionList[i]->getDestinationPort()->getName()
            << "\" />\n"
            << "</connection>\n";
    }
    out << "</connections>\n";
}

Model* CoupledModel::find(const std::string& name) const
{
    VectorModel::const_iterator it = m_modelList.find(name);
    if (it == m_modelList.end()) {
        return 0;
    } else {
        return it->second;
    }
}

Model* CoupledModel::find(int x, int y) const
{
    VectorModel::const_iterator it;
    for (it = m_modelList.begin(); it != m_modelList.end(); ++it) {
        if ((*it).second->x() <= x and x <= (*it).second->x() + (*it).second->width() and
            (*it).second->y() <= y and y <= (*it).second->y() + (*it).second->height()) {
            return (*it).second;
        }
    }
    return 0;
}

std::string CoupledModel::buildNewName(const std::string& prefix) const
{
    std::string name;
    if (prefix.empty()) {
        name.assign("runtimebuilding");
    } else {
        name.assign(prefix);
    }

    int i = 0;
    std::string newname;
    do {
        newname.assign((boost::format("%1%-%2%") % name % i).str());
        ++i;
    } while (exist(name));

    return newname;
}

}} // namespace vle graph
