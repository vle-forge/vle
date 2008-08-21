/**
 * @file vle/graph/CoupledModel.cpp
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
#include <vle/utils/XML.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Tools.hpp>
#include <set>



namespace vle { namespace graph {

CoupledModel::CoupledModel(const std::string& name, CoupledModel* parent) :
    Model(name, parent)
{
}

CoupledModel::CoupledModel(const CoupledModel& mdl) :
    Model(mdl),
    m_modelList(mdl.m_modelList),
    m_internalInputList(mdl.m_internalInputList),
    m_internalOutputList(mdl.m_internalOutputList)
{
    assert(mdl.getModelList().size() == getModelList().size());

    std::for_each(mdl.m_internalInputList.begin(),
                  mdl.m_internalInputList.end(),
                  CopyWithoutConnection(m_internalInputList));
    
    std::for_each(mdl.m_internalOutputList.begin(),
                  mdl.m_internalOutputList.end(),
                  CopyWithoutConnection(m_internalOutputList));

    std::for_each(m_modelList.begin(), m_modelList.end(), CloneModel(this));

    copyConnection(mdl.m_internalInputList, m_internalInputList);
    copyConnection(mdl.m_internalOutputList, m_internalOutputList);

    ModelList::const_iterator it = mdl.getModelList().begin();
    ModelList::iterator jt = m_modelList.begin();
    while (it != mdl.getModelList().end()) {
        const Model* src = it->second;
        Model* dst = jt->second;
        copyConnection(src->getInputPortList(), dst->getInputPortList());
        copyConnection(src->getOutputPortList(), dst->getOutputPortList());
        ++it;
        ++jt;
    }
}

CoupledModel& CoupledModel::operator=(const CoupledModel& mdl)
{
    CoupledModel m(mdl);
    swap(m);
    std::swap(m_internalInputList, m.m_internalInputList);
    std::swap(m_internalOutputList, m.m_internalOutputList);
    return *this;
}

CoupledModel::~CoupledModel()
{
    delAllModel();
}


/**************************************************************
 *
 * CONNECTION
 *
 *************************************************************/


void CoupledModel::addInputConnection(const std::string & portSrc,
				      Model* dst, const std::string& portDst)
{
    AssertS(utils::DevsGraphError, dst);
    AssertS(utils::DevsGraphError, dst != this);

    ModelPortList& outs(getInternalInPort(portSrc));
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

    ModelPortList& ins(getInternalOutPort(portDst));
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
    
    ModelPortList& outs(src->getOutPort(portSrc));
    outs.add(dst, portDst);

    ModelPortList& ins(dst->getInPort(portDst));
    ins.add(src, portSrc);
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
    addInputConnection(portSrc, findModel(dst), portDst);
}

void CoupledModel::addOutputConnection(const std::string& src,
                                       const std::string& portSrc,
                                       const std::string& portDst)
{
    addOutputConnection(findModel(src), portSrc, portDst);
}

void CoupledModel::addInternalConnection(const std::string& src,
                                         const std::string& portSrc,
                                         const std::string& dst,
                                         const std::string& portDst)
{
    addInternalConnection(findModel(src), portSrc, findModel(dst), portDst);
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
    AssertS(utils::DevsGraphError, dst != this);

    ModelPortList& outs(getInternalInPort(portSrc));
    outs.remove(dst, portDst);

    ModelPortList& ins(dst->getInPort(portDst));
    ins.remove(this, portSrc);
}

void CoupledModel::delOutputConnection(Model* src, const std::string & portSrc,
                                       const std::string & portDst)
{
    AssertS(utils::DevsGraphError, src);
    AssertS(utils::DevsGraphError, src != this);
    
    ModelPortList& outs(src->getOutPort(portSrc));
    outs.remove(this, portDst);

    ModelPortList& ins(getInternalOutPort(portDst));
    ins.remove(src, portSrc);
}

void CoupledModel::delInternalConnection(Model* src, const std::string& portSrc,
                                         Model* dst, const std::string& portDst)
{
    AssertS(utils::DevsGraphError, src and dst);
    AssertS(utils::DevsGraphError, src != this and dst != this);
    
    ModelPortList& outs(src->getOutPort(portSrc));
    outs.remove(dst, portDst);

    ModelPortList& ins(dst->getInPort(portDst));
    ins.remove(src, portSrc);
}

void CoupledModel::delInputConnection(const std::string& portSrc,
                                      const std::string& dst,
                                      const std::string& portDst)
{
    delInputConnection(portSrc, findModel(dst), portDst);
}

void CoupledModel::delOutputConnection(const std::string& src,
                                       const std::string& portSrc,
                                       const std::string& portDst)
{
    delOutputConnection(findModel(src), portSrc, portDst);
}

void CoupledModel::delInternalConnection(const std::string& src,
                                         const std::string& portSrc,
                                         const std::string& dst,
                                         const std::string& portDst)
{
    delInternalConnection(findModel(src), portSrc, findModel(dst), portDst);
}

void CoupledModel::delAllConnection(Model* m)
{
    AssertS(utils::DevsGraphError, m);

    for (ConnectionList::iterator it = m->getInputPortList().begin(); it !=
         m->getInputPortList().end(); ++it) {
        ModelPortList& ins = it->second;
        for (ModelPortList::iterator jt = ins.begin(); jt != ins.end(); ++jt) {
            if (jt->first == this) {
                getInternalInPort(jt->second).remove(m, it->first);
            } else {
                jt->first->getOutPort(jt->second).remove(m, it->first);
            }
        }
        ins.removeAll();
    }

    for (ConnectionList::iterator it = m->getOutputPortList().begin(); it !=
         m->getOutputPortList().end(); ++it) {
        ModelPortList& ins = it->second;
        for (ModelPortList::iterator jt = ins.begin(); jt != ins.end(); ++jt) {
            if (jt->first == this) {
                getInternalOutPort(jt->second).remove(m, it->first);
            } else {
                jt->first->getInPort(jt->second).remove(m, it->first);
            }
        }
        ins.removeAll();
    }
}

void CoupledModel::delAllConnection()
{
    for (ModelList::iterator it = m_modelList.begin(); it != m_modelList.end();
         ++it) {
        delAllConnection(it->second);
    }
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

std::vector < std::string > CoupledModel::getBasicConnections(
                const ModelList& models) const
{
    std::vector < std::string > storecnts;

    for (ModelList::const_iterator it = models.begin(); it != models.end();
         ++it) {

        Assert(utils::DevsGraphError, it->second->getParent() == this,
               boost::format("The model %1% is not the child of %2%") %
               it->second->getName() % getName());

        const ConnectionList& cnts((*it).second->getOutputPortList());
        for (ConnectionList::const_iterator jt = cnts.begin(); jt != cnts.end();
             ++jt) {
            for (ModelPortList::const_iterator kt = jt->second.begin();
                 kt != jt->second.end(); ++kt) {
                storecnts.push_back((*it).second->getName());
                storecnts.push_back(jt->first);
                storecnts.push_back(kt->first->getName());
                storecnts.push_back(kt->second);
            }
        }
    }

    return storecnts;
}

void CoupledModel::setBasicConnections(const std::vector < std::string >& lst)
{
    Assert(utils::DevsGraphError, lst.size() % 4 == 0, 
           "The basic connections list is malformed.");

    for (std::vector < std::string >::const_iterator it = lst.begin();
         it != lst.end(); it += 4) {
        const std::string& source(*(it));
        const std::string& portsource(*(it + 1));
        const std::string& destination(*(it + 2));
        const std::string& portdestination(*(it + 3));

        try {
            addInternalConnection(source, portsource, destination,
                                  portdestination);
        } catch(const utils::DevsGraphError& e) {
            for (std::vector < std::string >::const_iterator jt = lst.begin();
                 jt != it; jt += 4) {
                delInternalConnection(*jt, *(jt + 1), *(jt + 2), *(jt + 3));
            }
            Throw(utils::DevsGraphError, boost::format(
                    "Cannot displace the connection %1% %2% to %3% %4%") %
                source % portsource % destination % portdestination);
        }
    }
}

void CoupledModel::displace(ModelList& models, CoupledModel* destination)
{
    Assert(utils::DevsGraphError, not hasConnectionProblem(models),
           "One or more models are connected to another model");

    std::vector < std::string > cnts(getBasicConnections(models));
    detachModels(models);
    destination->attachModels(models);
    destination->setBasicConnections(cnts);
}

bool CoupledModel::hasConnectionProblem(const ModelList& lst) const
{
    for (ModelList::const_iterator it = lst.begin(); it != lst.end(); ++it) {
        const ConnectionList& ins(it->second->getInputPortList());
        if (haveConnectionWithOtherModel(ins, lst))
            return true;

        const ConnectionList& outs(it->second->getOutputPortList());
        if (haveConnectionWithOtherModel(outs, lst))
            return true;
    }
    return false;
}

bool CoupledModel::haveConnectionWithOtherModel(const ConnectionList& cnts,
                                                const ModelList& mdls) const
{
    for (ConnectionList::const_iterator it = cnts.begin(); it != cnts.end();
         ++it) {
        const ModelPortList& lst(it->second);
        for (ModelPortList::const_iterator jt = lst.begin(); jt != lst.end();
             ++jt) {

            if (mdls.count(jt->first->getName()) == 0) {
                return true;
            }
        }
    }
    return false;
}

Model* CoupledModel::findModel(const std::string& name) const
{
    ModelList::const_iterator it = m_modelList.find(name);
    return (it == m_modelList.end()) ? 0 : it->second;
}

Model* CoupledModel::findModelRecursively(const std::string& modelname) const
{
    if (getName() == modelname) {
        return const_cast < Model* >(
            reinterpret_cast < const Model* >(this));
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

void CoupledModel::addModel(Model* model, const std::string& name)
{
    AssertS(utils::DevsGraphError, not exist(name));
    Model::rename(model, name);
    model->setParent(this);
    addModel(model);
}

AtomicModel* CoupledModel::addAtomicModel(const std::string& name)
{
    AssertS(utils::DevsGraphError, not exist(name));
    AtomicModel* x = new AtomicModel(name, this);
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
    std::for_each(m_modelList.begin(), m_modelList.end(), DeleteModel(this));
    m_modelList.clear();
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
    std::for_each(models.begin(), models.end(), AttachModel(this));
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
    std::for_each(models.begin(), models.end(), DetachModel(this));
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
    out << "</submodels>\n";

    out << "<connections>\n";
    writeConnections(out);
    out << "</connections>\n";
    out << "</model>\n";
}

void CoupledModel::writeConnections(std::ostream& out) const
{
    for (ConnectionList::const_iterator it = m_internalOutputList.begin();
         it != m_internalOutputList.end(); ++it) {
        const std::string& port(it->first);
        const ModelPortList& lst(it->second);
        for (ModelPortList::const_iterator jt = lst.begin(); jt != lst.end();
             ++jt) {
            out << "<connection type=\"output\">\n"
                << " <origin model=\"" << jt->first->getName() << "\" "
                << "port=\"" << jt->second << "\" />\n"
                << " <destination model=\"" << getName() << "\" "
                << "port=\"" << port << "\" />\n"
                << "</connection>\n";
        }
    }

    for (ConnectionList::const_iterator it = m_internalInputList.begin();
         it != m_internalInputList.end(); ++it) {
        const std::string& port(it->first);
        const ModelPortList& lst(it->second);
        for (ModelPortList::const_iterator jt = lst.begin(); jt != lst.end();
             ++jt) {
            out << "<connection type=\"input\">\n"
                << " <origin model=\"" << getName() << "\" "
                << "port=\"" << port << "\" />\n"
                << " <destination model=\"" << jt->first->getName() << "\" "
                << "port=\"" << jt->second << "\" />\n"
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
                if (kt->first != this) {
                    out << "<connection type=\"internal\">\n"
                        << " <origin model=\"" << (*it).second->getName() << "\" "
                        << "port=\"" << jt->first << "\" />\n"
                        << " <destination model=\""  << kt->first->getName()
                        << "\" port=\"" << kt->second << "\" />\n"
                        << "</connection>\n";
                }
            }
        }
    }

}

Model* CoupledModel::find(int x, int y) const
{
    ModelList::const_iterator it = std::find_if(
        m_modelList.begin(), m_modelList.end(), IsInModelList(x, y));
    return (it == m_modelList.end()) ? 0 : it->second;
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
                "Coupled model %1% have no output port %2%") % getName() % name);
    }

    return it->second;
}

const ModelPortList& CoupledModel::getInternalOutPort(
    const std::string& name) const
{
    ConnectionList::const_iterator it = m_internalOutputList.find(name);
    if (it == m_internalOutputList.end()) {
        Throw(utils::DevsGraphError, boost::format(
                "Coupled model %1% have no output port %2%") % getName() %
            name);
    }

    return it->second;
}

void CoupledModel::copyConnection(const ConnectionList& src,
                                  ConnectionList& dst)
{
    assert(src.size() == dst.size());

    ConnectionList::const_iterator it = src.begin();
    ConnectionList::iterator jt = dst.begin();

    while (it != src.end()) {
        copyPort(it->second, jt->second);
        ++it;
        ++jt;
    }
}

void CoupledModel::copyPort(const ModelPortList& src, ModelPortList& dst)
{
    for (ModelPortList::const_iterator it = src.begin();
         it != src.end(); ++it) {
        const std::string& srcmodelname(it->first->getName());

        Model* dstmodel(findModel(srcmodelname));

        if (dstmodel == 0) {
            dstmodel = this;
        }

        Assert(utils::DevsGraphError, dstmodel, boost::format(
                "Destination model %1% not found in copy port") % srcmodelname);

        dst.add(dstmodel, it->second);
    }
}

}} // namespace vle graph
