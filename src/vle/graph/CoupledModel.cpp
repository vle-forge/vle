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

CoupledModel::~CoupledModel()
{
    delAllConnection();

    for (ModelList::iterator it = m_modelList.begin(); it !=
         m_modelList.end(); ++it) {
        delete (*it).second;
    }
}



/**************************************************************
 *
 * CONNECTION
 *
 *************************************************************/


void CoupledModel::addConnection(Model* src, const std::string& portSrc,
                                 Model* dst, const std::string& portDst)
{
    AssertI(dst);

    ModelPortList& outs(src->getOutPort(portSrc));
    outs.add(dst, portDst);

    ModelPortList& ins(dst->getOutPort(portDst));
    ins.add(src, portSrc);
}

void CoupledModel::addInputConnection(const std::string & portSrc,
				      Model* dst, const std::string& portDst)
{
    AssertI(dst);
    AssertI(dst != this);

}


void CoupledModel::addOutputConnection(Model* src, const std::string& portSrc,
				       const std::string& portDst)
{
    AssertI(src);
    AssertI(src != this);
    AssertI(existOutputPort(portDst));
    ModelPortList& outs(getInternalOutPort(portDst));
    outs.add(this, portDst);

    ModelPortList& ins(dst->getOutPort(portDst));
    ins.add(src, portSrc);
}

void CoupledModel::addInternalConnection(Model* src,
                                         const std::string& portSrc,
                                         Model* dst,
                                         const
                                         std::string& portDst)
{
    AssertI(src and dst);
    AssertI(src != this and dst != this);
    addConnection(src, portSrc, dst, portDst);
}

void CoupledModel::addInputConnection(const std::string& portSrc,
                                      const std::string& dst,
                                      const std::string& portDst)
{
    addConnection(this, portSrc, getModel(dst), portDst);
}

void CoupledModel::addOutputConnection(const std::string& src,
                                       const std::string& portSrc,
                                       const std::string& portDst)
{
    addConnection(getModel(src), portSrc, this, portDst);
}

void CoupledModel::addInternalConnection(const std::string& src,
                                         const std::string& portSrc,
                                         const std::string& dst,
                                         const std::string& portDst)
{
    addConnection(getModel(src), portSrc, getModel(dst), portDst);
}


void CoupledModel::delConnection(Model* src, const std::string& portSrc,
                                 Model* dst, const std::string& portDst)
{
    AssertI(dst);

    ModelPortList& outs(src->getOutPort(portSrc));
    outs.remove(dst, portDst);

    ModelPortList& ins(dst->getOutPort(portDst));
    ins.remove(src, portSrc);
}

void CoupledModel::delInputConnection(const std::string& portSrc,
                                      Model* dst, const std::string& portDst)
{
    AssertI(dst);

    delConnection(this, portSrc, dst, portDst);
}

void CoupledModel::delOutputConnection(Model* src, const std::string & portSrc,
                                       const std::string & portDst)
{
    AssertI(src);

    delConnection(src, portSrc, this, portDst);
}

void CoupledModel::delInternalConnection(Model* src, const std::string& portSrc,
                                         Model* dst, const std::string& portDst)
{
    AssertI(src and dst);

    delConnection(src, portSrc, dst, portDst);
}

void CoupledModel::delAllConnection(Model* m)
{
    AssertI(m);
    // FIXME
}

void CoupledModel::delAllConnection()
{
    AssertI(false);
}

void CoupledModel::replace(Model* oldmodel, Model* newmodel)
{
    Assert(vle::utils::InternalError, oldmodel, "Replace a null model ?");
    Assert(vle::utils::InternalError, newmodel, "Replace a model by null ?");

    {
        ConnectionList& source(oldmodel->getInputPortList());
        for (ConnectionList::iterator it = source.begin(); it != source.end();
             ++it) {
            ModelPortList& lst(newmodel->addInputPort(it->first));
            lst.merge(it->second);
        }
    }

    {
        ConnectionList& source(oldmodel->getOutputPortList());
        for (ConnectionList::iterator it = source.begin(); it != source.end();
             ++it) {
            ModelPortList& lst(newmodel->addOutputPort(it->first));
            lst.merge(it->second);
        }
    }

    delModel(oldmodel);
    addModel(newmodel);
}

bool CoupledModel::hasConnection(Model* /* model */,
                                 const std::string& /* name */) const
{
    // FIXME
    return false;
}

bool CoupledModel::hasConnectionProblem(const ModelList& /* lst */) const
{
    return false;
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
    ModelList::const_iterator it = m_modelList.find(name);
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
    ModelList::iterator it = m_modelList.find(model->getName());
    if (it != m_modelList.end()) {
        delAllConnection(model);
        m_modelList.erase(it);
        delete model;
    }
}

void CoupledModel::delAllModel()
{
    for (ModelList::iterator it = m_modelList.begin(); it !=
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

void CoupledModel::attachModels(ModelList& models)
{
    for (ModelList::iterator it = models.begin(); it != models.end(); ++it) {
        attachModel(it->second);
    }
}

void CoupledModel::detachModel(Model* model)
{
    ModelList::iterator it = m_modelList.find(model->getName());
    if (it != m_modelList.end()) {
        it->second->setParent(0);
        m_modelList.erase(it);
    } else {
        Throw(utils::InternalError, (boost::format(
                    "Model %1% is not attached to the coupled model %2%") %
                model->getName() % getName()));
    }
}

void CoupledModel::detachModels(const ModelList& models)
{
    for (ModelList::const_iterator it = models.begin(); it != models.end();
         ++it) {
	detachModel(it->second);
    }
}

void CoupledModel::getTargetPortList(Model* model, const std::string& portname,
                                     TargetModelList& out)
{
    // FIXME
    AssertI(false);
    AssertI(model and portname.empty() and out.empty());
}

void CoupledModel::writeXML(std::ostream& out) const
{
    out << "<model name=\"" << getName() << "\" " << " type=\"coupled\" >\n";
    writePortListXML(out);
    out << "<submodels>\n";

    //const size_t szInConnection = m_inputConnectionList.size();
    //const size_t szOutConnection = m_outputConnectionList.size();
    //const size_t szInternalConnection = m_internalConnectionList.size();
    //
    //for (ModelList::const_iterator it = m_modelList.begin(); it !=
    //m_modelList.end(); ++it) {
    //it->second->writeXML(out);
    //}
    //
    //out << "<connections>\n";
    //for (size_t i = 0; i < szInConnection; ++i) {
    //out << "<connection type=\"input\">\n"
    //<< " <origin model=\""
    //<< m_inputConnectionList[i]->getOriginModel()->getName()
    //<< "\" port=\""
    //<< m_inputConnectionList[i]->getOriginPort()->getName()
    //<< "\" />\n"
    //<< " <destination model=\""
    //<< m_inputConnectionList[i]->getDestinationModel()->getName()
    //<< "\" port=\""
    //<< m_inputConnectionList[i]->getDestinationPort()->getName()
    //<< "\" />\n"
    //<< "</connection>\n";
    //}
    //
    //for (size_t i = 0; i < szOutConnection; ++i) {
    //out << "<connection type=\"output\">\n"
    //<< " <origin model=\""
    //<< m_outputConnectionList[i]->getOriginModel()->getName()
    //<< "\" port=\""
    //<< m_outputConnectionList[i]->getOriginPort()->getName()
    //<< "\" />\n"
    //<< " <destination model=\""
    //<< m_outputConnectionList[i]->getDestinationModel()->getName()
    //<< "\" port=\""
    //<< m_outputConnectionList[i]->getDestinationPort()->getName()
    //<< "\" />\n"
    //<< "</connection>\n";
    //}
    //
    //for (size_t i = 0; i < szInternalConnection; ++i) {
    //out << "<connection type=\"internal\">\n"
    //<< " <origin model=\""
    //<< m_internalConnectionList[i]->getOriginModel()->getName()
    //<< "\" port=\""
    //<< m_internalConnectionList[i]->getOriginPort()->getName()
    //<< "\" />\n"
    //<< " <destination model=\""
    //<< m_internalConnectionList[i]->getDestinationModel()->getName()
    //<< "\" port=\""
    //<< m_internalConnectionList[i]->getDestinationPort()->getName()
    //<< "\" />\n"
    //<< "</connection>\n";
    //}
    //out << "</connections>\n";
}

Model* CoupledModel::find(const std::string& name) const
{
    ModelList::const_iterator it = m_modelList.find(name);
    if (it == m_modelList.end()) {
        return 0;
    } else {
        return it->second;
    }
}

Model* CoupledModel::find(int x, int y) const
{
    ModelList::const_iterator it;
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

ModelPortList& CoupledModel::getInternalInPort(const std::string& name)
{
    ConnectionList::iterator it = m_internalInputList.find(name);
    if (it == m_internalInputList.end()) {
        Throw(utils::InternalError, boost::format(
                "Coupled model %1% have no input port %2%") % getName() % name);
    }

    return it->second;
}

const ModelPortList& CoupledModel::getInternalInPort(
    const std::string& name) const
{
    ConnectionList::const_iterator it = m_internalInputList.find(name);
    if (it == m_internalInputList.end()) {
        Throw(utils::InternalError, boost::format(
                "Coupled model %1% have no input port %2%") % getName() % name);
    }

    return it->second;
}

ModelPortList& CoupledModel::getInternalOutPort(const std::string& name)
{
    ConnectionList::const_iterator it = m_internalOutputList.find(name);
    if (it == m_internalOutputList.end()) {
        Throw(utils::InternalError, boost::format(
                "Coupled model %1% have no input port %2%") % getName() % name);
    }

    return it->second;
}

const ModelPortList& CoupledModel::getInternalOutPort(
    const std::string& name) const
{
    ConnectionList::const_iterator it = m_internalOutputList.find(name);
    if (it == m_internalOutputList.end()) {
        Throw(utils::InternalError, boost::format(
                "Coupled model %1% have no input port %2%") % getName() % name);
    }

    return it->second;
}

}} // namespace vle graph
