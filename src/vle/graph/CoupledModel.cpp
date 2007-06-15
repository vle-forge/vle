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

CoupledModel::CoupledModel(CoupledModel* parent) :
    Model(parent)
{
}

CoupledModel::CoupledModel(const std::string& name, CoupledModel* parent) :
    Model(name, parent)
{
}

CoupledModel::CoupledModel(const CoupledModel& model) :
    Model(model)
{
    for (VectorModel::const_iterator it = model.m_modelList.begin();
         it != model.m_modelList.end();++it) {
        graph::Model* cloned = (*it)->clone();
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
        delete (*it);
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
    Model* mdst = this;
    Port* psrc = msrc->getOutPort(portSrc);
    Port* pdst = mdst->getOutPort(portDst);

    AssertI(msrc);
    AssertI(mdst);
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
    Port* psrc = msrc->getOutPort(portSrc);
    Port* pdst = mdst->getInPort(portDst);

    AssertI(msrc);
    AssertI(mdst);
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
	    PModel m = c->getDestinationModel();

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
	    PModel m = c->getDestinationModel();

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

Model* CoupledModel::findModel(const std::string & name) const
{
    VectorModel::const_iterator it = m_modelList.begin();

    if (getName() == name) {
	return (Model *)this;
    } else {
	while (it != m_modelList.end()) {
	    Model * found = (*it)->findModel(name);
	    if (found != 0)
		return found;
	    ++it;
	}
    }
    return 0;
}

Model* CoupledModel::getModel(const std::string& p_modelName)
{
    if (getName() == p_modelName)
	return this;
    else {
	Model * m = NULL;
	vector < Model* >::iterator it = m_modelList.begin();
	bool v_found = false;

	while (it != m_modelList.end() && !v_found) {
	    m = (*it);
	    v_found = m->getName() == p_modelName;
	    ++it;
	}
	if (v_found)
	    return m;
	else
	    return NULL;
    }
}

Model * CoupledModel::getModel(Model * model)
{
    vector < Model * >::iterator it = m_modelList.begin();
    while (it != m_modelList.end()) {
	if (model == (*it))
	    return model;
	++it;
    }

    return 0;
}

void CoupledModel::addModel(Model * model)
{
    AssertI(model and getModel(model) == 0);

    model->setParent(this);
    m_modelList.push_back(model);
}

AtomicModel* CoupledModel::addAtomicModel(const std::string& name)
{
    AssertI(not exist(name));
    AtomicModel* x = new AtomicModel(name, this);
    m_modelList.push_back(x);
    return x;
}

NoVLEModel* CoupledModel::addNoVLEModel(const std::string& name)
{
    AssertI(not exist(name));
    NoVLEModel* x = new NoVLEModel(name, this);
    m_modelList.push_back(x);
    return x;
}

CoupledModel* CoupledModel::addCoupledModel(const std::string& name)
{
    AssertI(not exist(name));
    CoupledModel* x = new CoupledModel(name, this);
    m_modelList.push_back(x);
    return x;
}

void CoupledModel::delModel(Model* model)
{
    AssertI(model);

    delAllConnection(model);

    VectorModel::iterator it = m_modelList.begin();
    while (it != m_modelList.end()) {
        if ((*it) == model) {
	    m_modelList.erase(it);
	    delete model;
	    break;
	}
	++it;
    }
}

void CoupledModel::delAllModel()
{
    VectorModel::iterator it = m_modelList.begin();
    while (it != m_modelList.end()) {
        delModel(*it);
	++it;
    }
}



void CoupledModel::attachModel(Model* model)
{
    AssertI(model);

    if (model->getParent()) {
        model->getParent()->detachModel(model);
    }

    m_modelList.push_back(model);
    model->setParent(this);
}

void CoupledModel::attachModels(VectorModel& models)
{
    for (VectorModel::iterator it = models.begin(); it != models.end(); ++it) {
        attachModel(*it);
    }
}

void CoupledModel::detachModel(Model * model)
{
    AssertI(model and getModel(model) != 0);

    VectorModel::iterator it = m_modelList.begin();
    while (it != m_modelList.end()) {
        if ((*it) == model) {
	    (*it)->setParent(0);
	    m_modelList.erase(it);
	    break;
	}
	++it;
    }
}

void CoupledModel::detachModels(const VectorModel& models)
{
    VectorModel::const_iterator lst = models.begin();
    while (lst != models.end()) {
	detachModel(*lst);
	++lst;
    }
}

void CoupledModel::detachInternalConnection(const VectorModel& lst,
                                            std::list < Connection *> & lc)
{
    lc.clear();

    VectorModel::const_iterator it = lst.begin();
    while (it != lst.end()) {
	VectorConnection::iterator internal = m_internalConnectionList.begin();
        while (internal != m_internalConnectionList.end()) {
            if ((*internal)->getOriginModel() == (*it) and
                (std::find(lst.begin(), lst.end(),
                      (*internal)->getDestinationModel()) != lst.end())) {
		VectorConnection::iterator del = internal;
		++internal;
		lc.push_back(*del);
		m_internalConnectionList.erase(del);
		break;
            } else if ((*internal)->getDestinationModel() == (*it) and
                       (std::find(lst.begin(), lst.end(),
                                  (*internal)->getOriginModel()) != lst.end()))
            {
		VectorConnection::iterator del = internal;
		++internal;
		lc.push_back(*del);
		m_internalConnectionList.erase(del);
		break;
	    }
	    else
		++internal;
	}
	++it;
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



/***************************/
/* XML syntax of model     */
/***************************/
/*
  <MODEL NAME="Copepod" TYPE="atomic|coupled" USEALONE="yes|no"
  URL="location" PROTOCOL="JavaClass|JavaRMI|DynamicLibrary|SOAP|...">
  <INIT> .. </INIT>
  <IN> ... </IN>
  <OUT> ... </OUT>
  <STATE> ... </STATE>
  <SUBMODELS TYPE="extension|comprehension">
  <MODEL NAME="A" TYPE="atomic|coupled" GENERIC="yes|no"
  MULTIPLICITY="*|n|n..m|n..*|*,*|n,n|n..m,n..m|..."> ... </MODEL>
  <MODEL NAME="B" TYPE="atomic|coupled"> ... </MODEL>
  </SUBMODELS>
  <CONNECTIONS TYPE="extension|comprehension">\\
  <CONNECTION TYPE="internal|input|output">
  <ORIGIN MODEL="A" PORT="out">
  <DESTINATION MODEL="B" PORT="in">
  </CONNECTION>
  </CONNECTIONS>\\
  </MODEL>

  par defaut, les sous-modeles et les connexions sont exprimes en extension
  les attributs TYPE ne sont alors pas specifies.

  l'attribut GENERIC est par defaut egal a "no" si absent.
  si l'attribut GENERIC est egal a "yes" alors le modele est un type de
  modeles => l'attribut MULTIPLICITY est alors present
  * : plusieurs
  n : un nombre donné et fixe
  n..m : un nombre entre les constantes n et m
  n..* : au moins n
  plusieurs elements de multiplicite peuvent apparaitre separes par des
  virgules ; chaque element qualifie une dimension
  la notion de dimension permet de definir de maniere generique les
  connexions entre les modeles generiques

  le nommage des modeles repose sur l'expression de la multiplicite.
  si la multiplicite est par exemple 2,2 et que le modele generique se
  nomme A alors les modeles auront pour nom A_1_1, A_1_2, A_2_1 et A_2_2.

  si la multiplicite n'est pas consistante alors l'instance d'experiences
  doivent preciser la multiplicite.

  Expression des connexions generiques
  ------------------------------------

  les connexions de type "input" ou "output" sont simples.

  les connexions internes ("internal").

  * structure reguliere :
  - ligne :
  <CONNECTION TYPE="internal">
  <ORIGIN MODEL="A" SET="in(1..n-1)" PORT="out">
  <DESTINATION MODEL="A" SET="i+1" PORT="L">
  </CONNECTION>
  <CONNECTION TYPE="internal">
  <ORIGIN MODEL="A" SET="in(2..n)" PORT="out">
  <DESTINATION MODEL="A" SET="i-1" PORT="R">
  </CONNECTION>


*/
/*************************/

bool CoupledModel::parseXML(xmlpp::Element* p_modelNode,
			    CoupledModel*)
{
    parseXMLports(p_modelNode);

    xmlpp::Element* v_submodels = get_children(p_modelNode, "SUBMODELS");
    xmlpp::Node::NodeList lst = v_submodels->get_children("MODEL");
    xmlpp::Node::NodeList::iterator it = lst.begin();
    while (it != lst.end()) {
	xmlpp::Element * v_modelNode = (xmlpp::Element*)(*it);
	PModel v_model = Model::parseXMLmodel(v_modelNode, this);
	if (v_model) m_modelList.push_back(v_model);
	++it;
    }

    xmlpp::Element* v_connections = get_children(p_modelNode, "CONNECTIONS");
    xmlpp::Node::NodeList lst2 = v_connections->get_children("CONNECTION");
    xmlpp::Node::NodeList::iterator it2 = lst2.begin();
    while (it2 != lst2.end()) {
	xmlpp::Element * connection = (xmlpp::Element*)(*it2);
	string v_type = get_attribute(connection,"TYPE");
	xmlpp::Element* origin = get_children(connection,"ORIGIN");
	string v_originModelName = get_attribute(origin,"MODEL");
	string v_originPortName = get_attribute(origin,"PORT");
	PModel v_originModel = getModel(v_originModelName);
	xmlpp::Element* destination = get_children(connection,"DESTINATION");
	string v_destinationModelName = get_attribute(destination,"MODEL");
	string v_destinationPortName = get_attribute(destination,"PORT");
	PModel v_destinationModel = getModel(v_destinationModelName);
	Connection* v_connection;

        try {
            if (v_type == "internal") {
                v_connection =
                    new Connection(v_originModel,
                                   v_originModel->getOutPort(v_originPortName),
                                   v_destinationModel,
                                   v_destinationModel->
                                   getInPort(v_destinationPortName));
                m_internalConnectionList.push_back(v_connection);
            }
            else if (v_type == "input") {
                v_connection =
                    new Connection(v_originModel,
                                   v_originModel->getInPort(v_originPortName),
                                   v_destinationModel,
                                   v_destinationModel->
                                   getInPort(v_destinationPortName));
                m_inputConnectionList.push_back(v_connection);
            } else if (v_type == "output") {
                v_connection =
                    new Connection(v_originModel,
                                   v_originModel->getOutPort(v_originPortName),
                                   v_destinationModel,
                                   v_destinationModel->
                                   getOutPort(v_destinationPortName));
                m_outputConnectionList.push_back(v_connection);
            }
        } catch(const std::exception& e) {
            Assert(vle::utils::ParseError, false, boost::format(
                    "Error occured during coupled model '%1%' allocation with "
                    "message : %2%\n") % getName() % e.what());
        }
	++it2;
    }
    return true;
}



void CoupledModel::writeXML(std::ostream& out) const
{
    out << "<model name=\"" << getName() << "\" " << " type=\"coupled\" >";
    writePortListXML(out);
    out << "<submodels>";

    const size_t szModel = m_modelList.size();
    const size_t szInConnection = m_inputConnectionList.size();
    const size_t szOutConnection = m_outputConnectionList.size();
    const size_t szInternalConnection = m_internalConnectionList.size();

    for (size_t i = 0; i < szModel; ++i) {
        m_modelList[i]->writeXML(out);
    }

    out << "<connections>";
    for (size_t i = 0; i < szInConnection; ++i) {
        out << "<connection type=\"input\">"
            << " <origin model=\""
            << m_inputConnectionList[i]->getOriginModel()->getName()
            << "\" port=\""
            << m_inputConnectionList[i]->getOriginPort()->getName()
            << "\" />"
            << " <destination model=\""
            << m_inputConnectionList[i]->getDestinationModel()->getName()
            << "\" port=\""
            << m_inputConnectionList[i]->getDestinationPort()->getName()
            << "\" />"
            << "</connection>";
    }

    for (size_t i = 0; i < szOutConnection; ++i) {
        out << "<connection type=\"output\">"
            << " <origin model=\""
            << m_outputConnectionList[i]->getOriginModel()->getName()
            << "\" port=\""
            << m_outputConnectionList[i]->getOriginPort()->getName()
            << "\" />"
            << " <destination model=\""
            << m_outputConnectionList[i]->getDestinationModel()->getName()
            << "\" port=\""
            << m_outputConnectionList[i]->getDestinationPort()->getName()
            << "\" />"
            << "</connection>";
    }

    for (size_t i = 0; i < szInternalConnection; ++i) {
        out << "<connection type=\"internal\">"
            << " <origin model=\""
            << m_internalConnectionList[i]->getOriginModel()->getName()
            << "\" port=\""
            << m_internalConnectionList[i]->getOriginPort()->getName()
            << "\" />"
            << " <destination model=\""
            << m_internalConnectionList[i]->getDestinationModel()->getName()
            << "\" port=\""
            << m_internalConnectionList[i]->getDestinationPort()->getName()
            << "\" />"
            << "</connection>";
    }
    out << "</connections>";
}

Model* CoupledModel::find(const std::string& name) const
{
    VectorModel::const_iterator it;
    for (it = m_modelList.begin(); it != m_modelList.end(); ++it) {
        if ((*it)->getName() == name) {
            return (*it);
        }
    }
    return 0;
}

Model* CoupledModel::find(int x, int y) const
{
    VectorModel::const_iterator it;
    for (it = m_modelList.begin(); it != m_modelList.end(); ++it) {
        if ((*it)->x() <= x and x <= (*it)->x() + (*it)->width() and
            (*it)->y() <= y and y <= (*it)->y() + (*it)->height()) {
            return (*it);
        }
    }
    return 0;
}

std::string CoupledModel::buildNewName(const std::string& prefix) const
{
    std::set < std::string > names;
    std::string name, newname;

    for (VectorModel::const_iterator it = m_modelList.begin();
         it != m_modelList.end(); ++it)
        names.insert((*it)->getName());

    if (prefix.empty())
        name.assign("runtimebuilding");
    else
        name.assign(prefix);

    int i = 0;
    do {
        newname.assign(name);
        newname += '-';
        newname += utils::to_string(i);
        ++i;
    } while (names.find(newname) != names.end());

    return newname;
}

bool CoupledModel::exist(const std::string& name) const
{
    for (VectorModel::const_iterator it = m_modelList.begin();
         it != m_modelList.end(); ++it) {
        if ((*it)->getName() == name) {
            return true;
        }
    }
    return false;
}

}} // namespace vle graph
