/**
 * @file graph/CoupledModel.cpp
 * @author The VLE Development Team.
 * @brief Represent the DEVS coupled model. This class have a list of children
 * models, three list of input, output and state connections.
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
#include <vle/utils/Tools.hpp>
#include <set>

using std::vector;
using std::string;
using std::list;


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
    AssertS(utils::DevsGraphError, dst);

    ModelPortList& outs(src->getOutPort(portSrc));
    outs.add(dst, portDst);

    ModelPortList& ins(dst->getOutPort(portDst));
    ins.add(src, portSrc);
}

void CoupledModel::addInputConnection(const std::string & portSrc,
				      Model* dst, const std::string& portDst)
{
    AssertS(utils::DevsGraphError, dst);
    AssertS(utils::DevsGraphError, dst != this);

    ModelPortList& outs(getInternalOutPort(portSrc));
    outs.add(dst, portDst);

    ModelPortList& ins(dst->getInPort(portDst));
    ins.add(this, portSrc);
}


void CoupledModel::addOutputConnection(Model* src, const std::string& portSrc,
				       const std::string& portDst)
{
    AssertS(utils::DevsGraphError, src);
    AssertS(utils::DevsGraphError, src != this);
    
    ModelPortList& outs(src->getOutPort(portSrc));
    outs.add(this, portDst);

    ModelPortList& ins(getInternalInPort(portDst));
    ins.add(src, portSrc);
}

void CoupledModel::addInternalConnection(Model* src,
                                         const std::string& portSrc,
                                         Model* dst,
                                         const
                                         std::string& portDst)
{
    AssertS(utils::DevsGraphError, src and dst);
    AssertS(utils::DevsGraphError, src != this and dst != this);
    addConnection(src, portSrc, dst, portDst);
}

bool CoupledModel::existInputConnection(const std::string& portsrc,
                                        const std::string& dst,
                                        const std::string& portdst) const
{
    Model* mdst = findModel(dst);

    if (mdst == 0)
        return false;

    if (not mdst->existInputPort(portdst))
        return false;

    if (not existInternalInputPort(portsrc))
        return false;

    const ModelPortList& mp_src = getInternalInPort(portsrc);
    const ModelPortList& mp_dst = mdst->getInPort(portdst);

    if (not mp_src.exist(mdst, portdst))
        return false;

    if (not mp_dst.exist(this, portsrc))
        return false;

    return true;
}

bool CoupledModel::existOutputConnection(const std::string& src,
                                         const std::string& portsrc,
                                         const std::string& portdst) const
{
    Model* msrc = findModel(src);

    if (msrc == 0)
        return false;

    if (not msrc->existOutputPort(portsrc))
        return false;

    if (not existInternalOutputPort(portdst))
        return false;

    const ModelPortList& mp_src = msrc->getOutPort(portsrc);
    const ModelPortList& mp_dst = getInternalOutPort(portdst);

    if (not mp_src.exist(this, portdst))
        return false;

    if (not mp_dst.exist(msrc, portsrc))
        return false;

    return true;
}

bool CoupledModel::existInternalConnection(const std::string& src,
                                           const std::string& portsrc,
                                           const std::string& dst,
                                           const std::string& portdst) const
{
    Model* msrc = findModel(src);
    Model* mdst = findModel(dst);

    if (msrc == 0 or mdst == 0)
        return false;

    if (not msrc->existOutputPort(portsrc))
        return false;
    
    if (not mdst->existInputPort(portdst))
        return false;

    const ModelPortList& mp_src = msrc->getOutPort(portsrc);
    const ModelPortList& mp_dst = mdst->getInPort(portdst);

    if (not mp_src.exist(mdst, portdst))
        return false;

    if (not mp_dst.exist(msrc, portsrc))
        return false;

    return true;
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
    AssertS(utils::DevsGraphError, dst);

    ModelPortList& outs(src->getOutPort(portSrc));
    outs.remove(dst, portDst);

    ModelPortList& ins(dst->getOutPort(portDst));
    ins.remove(src, portSrc);
}

void CoupledModel::delInputConnection(const std::string& portSrc,
                                      Model* dst, const std::string& portDst)
{
    AssertS(utils::DevsGraphError, dst);

    delConnection(this, portSrc, dst, portDst);
}

void CoupledModel::delOutputConnection(Model* src, const std::string & portSrc,
                                       const std::string & portDst)
{
    AssertS(utils::DevsGraphError, src);

    delConnection(src, portSrc, this, portDst);
}

void CoupledModel::delInternalConnection(Model* src, const std::string& portSrc,
                                         Model* dst, const std::string& portDst)
{
    AssertS(utils::DevsGraphError, src and dst);

    delConnection(src, portSrc, dst, portDst);
}

void CoupledModel::delAllConnection(Model* m)
{
    AssertS(utils::DevsGraphError, m);
    Throw(utils::NotYetImplented, "CoupledModel::delAllConnection");
}

void CoupledModel::delAllConnection()
{
    AssertS(utils::DevsGraphError, false);
}

void CoupledModel::replace(Model* oldmodel, Model* newmodel)
{
    Assert(utils::DevsGraphError, oldmodel, "Replace a null model ?");
    Assert(utils::DevsGraphError, newmodel, "Replace a model by null ?");

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
    Throw(utils::NotYetImplented, "CoupledModel::hasConnection");
    return false;
}

bool CoupledModel::hasConnectionProblem(const ModelList& /* lst */) const
{
    Throw(utils::NotYetImplented, "CoupledModel::hasConnectionProblem");
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
    AssertS(utils::DevsGraphError, not exist(model->getName()));

    model->setParent(this);
    m_modelList[model->getName()] = model;
}

AtomicModel* CoupledModel::addAtomicModel(const std::string& name)
{
    AssertS(utils::DevsGraphError, not exist(name));
    AtomicModel* x = new AtomicModel(name, this);
    m_modelList[name] = x;
    return x;
}

NoVLEModel* CoupledModel::addNoVLEModel(const std::string& name)
{
    AssertS(utils::DevsGraphError, not exist(name));
    NoVLEModel* x = new NoVLEModel(name, this);
    m_modelList[name] = x;
    return x;
}

CoupledModel* CoupledModel::addCoupledModel(const std::string& name)
{
    AssertS(utils::DevsGraphError, not exist(name));
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
    AssertS(utils::DevsGraphError, not exist(model->getName()));

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
        Throw(utils::DevsGraphError, (boost::format(
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

void CoupledModel::writeXML(std::ostream& out) const
{
    out << "<model name=\"" << getName() << "\" " << " type=\"coupled\" >\n";
    writePortListXML(out);
    out << "<submodels>\n";

    for (ModelList::const_iterator it = m_modelList.begin(); it !=
         m_modelList.end(); ++it) {
        it->second->writeXML(out);
    }

    out << "<connections>\n";
    writeConnections(out);
    out << "<connections>\n";
}

void CoupledModel::writeConnections(std::ostream& out) const
{
    for (ConnectionList::const_iterator it = m_internalOutputList.begin();
         it != m_internalOutputList.end(); ++it) {
        const std::string& port(it->first);
        const ModelPortList& lst(it->second);
        for (ModelPortList::const_iterator jt = lst.begin(); jt != lst.end();
             ++jt) {
            out << "<connection type=\"input\">\n"
                << " <origin model=\"" << getName() << "\" "
                << "port=\"" << port << "\" />\n"
                << " <destination model=\"" << (*jt)->model()->getName() << "\" "
                << "port=\"" << (*jt)->port() << "\" />\n"
                << "</connection>\n";
        }
    }

    for (ConnectionList::const_iterator it = m_internalInputList.begin();
         it != m_internalInputList.end(); ++it) {
        const std::string& port(it->first);
        const ModelPortList& lst(it->second);
        for (ModelPortList::const_iterator jt = lst.begin(); jt != lst.end();
             ++jt) {
            out << "<connection type=\"output\">\n"
                << " <origin model=\"" << (*jt)->model()->getName() << "\" "
                << "port=\"" << (*jt)->port() << "\" />\n"
                << " <destination model=\"" << getName() << "\" "
                << "port=\"" << port << "\" />\n"
                << "</connection>\n";
        }
    }

    for (ModelList::const_iterator it = m_modelList.begin(); it !=
         m_modelList.end(); ++it) {
        const ConnectionList& cnts((*it).second->getOutputPortList());
        for (ConnectionList::const_iterator jt = cnts.begin(); jt != cnts.end();
             ++jt) {
            for (ModelPortList::const_iterator kt = jt->second.begin();
                 kt != jt->second.end(); ++kt) {
                out << "<connection type=\"internal\">\n"
                    << " <origin model=\"" << (*it).second->getName() << "\" "
                    << "port=\"" << jt->first << "\" />\n"
                    << " <destination model=\""  << (*kt)->model()->getName()
                    << "\" port=\"" << (*kt)->port() << "\" />\n"
                    << "</connection>\n";
            }
        }
    }

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
        Throw(utils::DevsGraphError, boost::format(
                "Coupled model %1% have no input port %2%") % getName() % name);
    }

    return it->second;
}

const ModelPortList& CoupledModel::getInternalInPort(
    const std::string& name) const
{
    ConnectionList::const_iterator it = m_internalInputList.find(name);
    if (it == m_internalInputList.end()) {
        Throw(utils::DevsGraphError, boost::format(
                "Coupled model %1% have no input port %2%") % getName() % name);
    }

    return it->second;
}

ModelPortList& CoupledModel::getInternalOutPort(const std::string& name)
{
    ConnectionList::iterator it = m_internalOutputList.find(name);
    if (it == m_internalOutputList.end()) {
        Throw(utils::DevsGraphError, boost::format(
                "Coupled model %1% have no input port %2%") % getName() % name);
    }

    return it->second;
}

const ModelPortList& CoupledModel::getInternalOutPort(
    const std::string& name) const
{
    ConnectionList::const_iterator it = m_internalOutputList.find(name);
    if (it == m_internalOutputList.end()) {
        Throw(utils::DevsGraphError, boost::format(
                "Coupled model %1% have no input port %2%") % getName() % name);
    }

    return it->second;
}

}} // namespace vle graph
