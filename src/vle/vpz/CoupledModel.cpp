/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2016 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2016 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2016 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <vle/vpz/CoupledModel.hpp>
#include <vle/vpz/AtomicModel.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>
#include <cmath>
#include <set>
#include <cassert>
#include <algorithm>
#include <stack>

namespace vle { namespace vpz {

CoupledModel::CoupledModel(const std::string& name, CoupledModel* parent) :
    BaseModel(name, parent)
{
}

CoupledModel::CoupledModel(const CoupledModel& mdl) :
    BaseModel(mdl),
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
        const BaseModel* src = it->second;
        BaseModel* dst = jt->second;
        copyInternalConnection(src->getInputPortList(),
                               dst->getInputPortList(), mdl, *this);
        copyInternalConnection(src->getOutputPortList(),
                               dst->getOutputPortList(), mdl, *this);
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
                                      BaseModel* dst, const std::string& portDst)
{
    if (not dst) {
        throw utils::DevsGraphError(
            _("Can not add input connection with unknown destination"));
    }

    if (dst == this) {
        throw utils::DevsGraphError(
            _("Can not add input connection if destination is this coupled "
              "model"));
    }

    ModelPortList& outs(getInternalInPort(portSrc));
    outs.add(dst, portDst);

    ModelPortList& ins(dst->getInPort(portDst));
    ins.add(this, portSrc);
}


void CoupledModel::addOutputConnection(BaseModel* src, const std::string& portSrc,
                                       const std::string& portDst)
{
    if (not src) {
        throw utils::DevsGraphError(
            _("Can not add output connection with unknown origin"));
    }

    if (src == this) {
        throw utils::DevsGraphError(
            _("Can not add output connection if source is this coupled model"));
    }

    ModelPortList& outs(src->getOutPort(portSrc));
    outs.add(this, portDst);

    ModelPortList& ins(getInternalOutPort(portDst));
    ins.add(src, portSrc);
}

void CoupledModel::addInternalConnection(BaseModel* src,
                                         const std::string& portSrc,
                                         BaseModel* dst,
                                         const
                                         std::string& portDst)
{
    if (not src) {
        throw utils::DevsGraphError(
            _("Cannot add internal connection with unknown origin"));
    }

    if (not dst) {
        throw utils::DevsGraphError(
            _("Cannot add internal connection with unknown destination"));
    }

    if (src == this) {
        throw utils::DevsGraphError(
            _("Cannot add internal connection if source is this coupled "
              "model"));
    }

    if (dst == this) {
        throw utils::DevsGraphError(
            _("Cannot add internal connection if destination is this coupled "
              "model"));
    }

    ModelPortList& outs(src->getOutPort(portSrc));
    outs.add(dst, portDst);

    ModelPortList& ins(dst->getInPort(portDst));
    ins.add(src, portSrc);
}

bool CoupledModel::existInputConnection(const std::string& portsrc,
                                        const std::string& dst,
                                        const std::string& portdst) const
{
    BaseModel* mdst = findModel(dst);

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
    BaseModel* msrc = findModel(src);

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
    BaseModel* msrc = findModel(src);
    BaseModel* mdst = findModel(dst);

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

int CoupledModel::nbInputConnection(const std::string& portsrc,
                                    const std::string& dst,
                                    const std::string& portdst)
{
    int nbConnections=0;

    BaseModel* mdst = findModel(dst);

    if (mdst == 0) {
        return false;
    }

    if (not mdst->existInputPort(portdst)) {
        return false;
    }

    if (not existInternalInputPort(portsrc)) {
        return false;
    }

    const ModelPortList& mp_src = getInternalInPort(portsrc);
    const ModelPortList& mp_dst = mdst->getInPort(portdst);

    if (not mp_src.exist(mdst, portdst)) {
        return false;
    }

    if (not mp_dst.exist(this, portsrc)) {
        return false;
    }

    return true;

    ConnectionList::iterator iter = mdst->getInputPortList().find(portdst);
    ModelPortList model(iter->second);
    for (ModelPortList::iterator it = model.begin(); it != model.end(); ++it) {
        if (it->first == this and it->second == portdst) {
            nbConnections++;
        }
    }

    return nbConnections;
}

int CoupledModel::nbOutputConnection(const std::string& src,
                                     const std::string& portsrc,
                                     const std::string& portdst)
{
    BaseModel* msrc = findModel(src);
    int nbConnections=0;

    if (msrc == 0) {
        return 0;
    }

    if (not msrc->existOutputPort(portsrc)) {
        return 0;
    }

    if (not existInternalOutputPort(portdst)) {
        return 0;
    }

    const ModelPortList& mp_src = msrc->getOutPort(portsrc);
    const ModelPortList& mp_dst = getInternalOutPort(portdst);

    if (not mp_src.exist(this, portdst)) {
        return 0;
    }


    if (not mp_dst.exist(msrc, portsrc)) {
        return 0;
    }

    ConnectionList::iterator iter = msrc->getOutputPortList().find(portsrc);
    ModelPortList model(iter->second);
    for (ModelPortList::iterator it = model.begin(); it != model.end(); ++it) {
        if (it->first == this and it->second == portdst) {
            nbConnections++;
        }
    }

    return nbConnections;
}

int CoupledModel::nbInternalConnection(const std::string& src,
                                       const std::string& portsrc,
                                       const std::string& dst,
                                       const std::string& portdst)
{
    BaseModel* msrc = findModel(src);
    BaseModel* mdst = findModel(dst);
    int nbConnections=0;

    if (msrc == 0 or mdst == 0) {
        return 0;
    }

    if (not msrc->existOutputPort(portsrc)) {
        return 0;
    }

    if (not mdst->existInputPort(portdst)) {
        return 0;
    }

    const ModelPortList& mp_src = msrc->getOutPort(portsrc);
    const ModelPortList& mp_dst = mdst->getInPort(portdst);

    if (not mp_src.exist(mdst, portdst)) {
        return 0;
    }


    if (not mp_dst.exist(msrc, portsrc)) {
        return 0;
    }

    ConnectionList::iterator iter = msrc->getOutputPortList().find(portsrc);
    ModelPortList model(iter->second);
    for (ModelPortList::iterator it = model.begin(); it != model.end(); ++it) {
        if (it->first == mdst and it->second == portdst) {
            nbConnections++;
        }
    }

    return nbConnections;
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

void CoupledModel::delConnection(BaseModel* src, const std::string& portSrc,
                                 BaseModel* dst, const std::string& portDst)
{
    if (not dst) {
        throw utils::DevsGraphError(
            _("Cannot delete connection with unknown destination"));
    }

    ModelPortList& outs(src->getOutPort(portSrc));
    outs.remove(dst, portDst);

    ModelPortList& ins(dst->getOutPort(portDst));
    ins.remove(src, portSrc);
}

void CoupledModel::delInputConnection(const std::string& portSrc,
                                      BaseModel* dst, const std::string& portDst)
{
    if (not dst) {
        throw utils::DevsGraphError(
            _("Cannot delete input connection with unknown destination"));
    }

    if (dst == this) {
        throw utils::DevsGraphError(
            _("Cannot delete input connection if destination is coupled "
              "model"));
    }

    ModelPortList& outs(getInternalInPort(portSrc));
    outs.remove(dst, portDst);

    ModelPortList& ins(dst->getInPort(portDst));
    ins.remove(this, portSrc);
}

void CoupledModel::delOutputConnection(BaseModel* src, const std::string & portSrc,
                                       const std::string & portDst)
{
    if (not src) {
        throw utils::DevsGraphError(
            _("Cannot delete output connection with unknown source"));
    }

    if (src == this) {
        throw utils::DevsGraphError(
            _("Cannot delete output connection if source is coupled model"));
    }

    ModelPortList& outs(src->getOutPort(portSrc));
    outs.remove(this, portDst);

    ModelPortList& ins(getInternalOutPort(portDst));
    ins.remove(src, portSrc);
}

void CoupledModel::delInternalConnection(BaseModel* src, const std::string& portSrc,
                                         BaseModel* dst, const std::string& portDst)
{
    if (not src) {
        throw utils::DevsGraphError(
            _("Cannot delete internal connection with unknown source"));
    }

    if (src == this) {
        throw utils::DevsGraphError(
            _("Cannot delete internal connection if source is coupled model"));
    }

    if (not dst) {
        throw utils::DevsGraphError(
            _("Cannot delete internal connection with unknown destination"));
    }

    if (dst == this) {
        throw utils::DevsGraphError(
            _("Cannot delete internal connection if destination is coupled "
              "model"));
    }

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

void CoupledModel::delAllConnection(BaseModel* m)
{
    if (not m) {
        throw utils::DevsGraphError(
            _("Cannot delete all connection with an unknown model source"));
    }

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
        ins.clear();
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
        ins.clear();
    }
}

void CoupledModel::delAllConnection()
{
    for (ModelList::iterator it = m_modelList.begin(); it != m_modelList.end();
         ++it) {
        delAllConnection(it->second);
    }
}

void CoupledModel::replace(BaseModel* oldmodel, BaseModel* newmodel)
{
    if (not oldmodel) {
        throw utils::DevsGraphError(_("Replace a null model ?"));
    }

    if (not newmodel) {
        throw utils::DevsGraphError(_("Replace a model by null ?"));
    }

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

        if (it->second->getParent() != this) {
            throw utils::DevsGraphError(
                (fmt(_("The model %1% is not the child of %2%")) %
                 it->second->getName() % getName()).str());
        }

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
    if (lst.size() % 4 != 0) {
        throw utils::DevsGraphError(
            _("The basic connections list is malformed."));
    }

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
            throw utils::DevsGraphError(
                (fmt(_("Cannot displace the connection %1% %2% to %3% %4%"))
                 % source % portsource % destination % portdestination).str());
        }
    }
}

void CoupledModel::displace(ModelList& models, CoupledModel* destination)
{
    ModelConnections listInput = saveInputConnections(models);
    ModelConnections listOutput = saveOutputConnections(models);

    for (ModelList::const_iterator it = models.begin(); it != models.end();
         ++it) {
        delAllConnection(it->second);
    }

    if (hasConnectionProblem(models)) {
        throw utils::DevsGraphError(
            _("One or more models are connected to another model"));
    }

    detachModels(models);
    destination->attachModels(models);

    restoreInputConnections(models, destination, listInput);
    restoreOutputConnections(destination, listOutput);
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

BaseModel* CoupledModel::findModel(const std::string& name) const
{
    ModelList::const_iterator it = m_modelList.find(name);
    return (it == m_modelList.end()) ? 0 : it->second;
}

BaseModel* CoupledModel::getModel(const std::string& name) const
{
    ModelList::const_iterator it = m_modelList.find(name);
    return (it == m_modelList.end()) ? 0 : it->second;
}

void CoupledModel::addModel(BaseModel* model)
{
    if (exist(model->getName())) {
        throw utils::DevsGraphError(
            (fmt(_("Cannot add the model '%1%' into the coupled model "
                   "'%2%' (it already exists)"))
             % model->getName() % getName()).str());
    }

    model->setParent(this);
    m_modelList[model->getName()] = model;
}

void CoupledModel::addModel(BaseModel* model, const std::string& name)
{
    if (exist(name)) {
        throw utils::DevsGraphError(
            (fmt(_("Cannot add the model '%1%' into the coupled model "
                   "'%2%' (it already exists)")) % name % getName()).str());
    }

    BaseModel::rename(model, name);
    model->setParent(this);
    addModel(model);
}

AtomicModel* CoupledModel::addAtomicModel(const std::string& name)
{
    if (exist(name)) {
        throw utils::DevsGraphError(
            (fmt(_("Cannot add the model '%1%' into the coupled model "
                   "'%2%' (it already exists)")) % name % getName()).str());
    }

    AtomicModel* x = new AtomicModel(name, this);
    m_modelList[name] = x;
    return x;
}

CoupledModel* CoupledModel::addCoupledModel(const std::string& name)
{
    if (exist(name)) {
        throw utils::DevsGraphError(
            (fmt(_("Cannot add the model '%1%' into the coupled model "
                   "'%2%' (it already exists)")) % name % getName()).str());
    }

    CoupledModel* x = new CoupledModel(name, this);
    m_modelList[name] = x;
    return x;
}

void CoupledModel::delModel(BaseModel* model)
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

void CoupledModel::attachModel(BaseModel* model)
{
    if (exist(model->getName())) {
        throw utils::DevsGraphError(
            _("Cannot attach an already attached model"));
    }

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

void CoupledModel::detachModel(BaseModel* model)
{
    ModelList::iterator it = m_modelList.find(model->getName());
    if (it != m_modelList.end()) {
        it->second->setParent(0);
        m_modelList.erase(it);
    } else {
        throw utils::DevsGraphError(
            (fmt(_("Model %1% is not attached to the coupled model %2%")) %
             model->getName() % getName()).str());
    }
}

void CoupledModel::detachModels(const ModelList& models)
{
    std::for_each(models.begin(), models.end(), DetachModel(this));
}

void CoupledModel::writeXML(std::ostream& out) const
{
    out << "<model name=\"" << getName().c_str() << "\" " << " type=\"coupled\" >\n";
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
                << " <origin model=\"" << jt->first->getName().c_str() << "\" "
                << "port=\"" << jt->second.c_str() << "\" />\n"
                << " <destination model=\"" << getName().c_str() << "\" "
                << "port=\"" << port.c_str() << "\" />\n"
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
                << " <origin model=\"" << getName().c_str() << "\" "
                << "port=\"" << port.c_str() << "\" />\n"
                << " <destination model=\"" << jt->first->getName().c_str()
                << "\" " << "port=\"" << jt->second.c_str() << "\" />\n"
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
                        << " <origin model=\""
                        << (*it).second->getName().c_str() << "\" "
                        << "port=\"" << jt->first.c_str() << "\" />\n"
                        << " <destination model=\""
                        << kt->first->getName().c_str()
                        << "\" port=\"" << kt->second.c_str() << "\" />\n"
                        << "</connection>\n";
                }
            }
        }
    }

}
void CoupledModel::write(std::ostream& out) const
{
    typedef std::stack < const vpz::CoupledModel* > CoupledModelList;
    typedef std::stack < bool > IsWritedCoupledModel;

    CoupledModelList cms;
    IsWritedCoupledModel writed;

    cms.push(this);
    writed.push(false);

    while (not cms.empty()) {
        const vpz::CoupledModel* top(cms.top());

        if (not writed.top()) {
            writed.top() = true;
            out << "<model name=\"" << top->getName().c_str() << "\" "
                << "type=\"coupled\" ";
            top->writeGraphics(out);
            out << " >\n";
            top->writePort(out);
            out << "<submodels>\n";

            const vpz::ModelList& childs(top->getModelList());
            for (vpz::ModelList::const_iterator it = childs.begin();
                 it != childs.end(); ++it) {
                if (it->second->isCoupled()) {
                    cms.push(static_cast < vpz::CoupledModel* >(it->second));
                    writed.push(false);
                } else if (it->second->isAtomic()) {
                    (static_cast < vpz::AtomicModel* >(it->second))->write(out);
                }
            }
        } else {
            out << "</submodels>\n";
            top->writeConnection(out);
            cms.pop();
            writed.pop();
            out << "</model>\n";
        }
    }
}

void CoupledModel::updateDynamics(const std::string& oldname,
                                  const std::string& newname)
{
    std::for_each(m_modelList.begin(), m_modelList.end(),
                  UpdateDynamics(oldname, newname));
}

void CoupledModel::purgeDynamics(const std::set < std::string >& dynamicslist)
{
    std::for_each(m_modelList.begin(), m_modelList.end(),
                  PurgeDynamics(dynamicslist));
}

void CoupledModel::updateObservable(const std::string& oldname,
                                    const std::string& newname)
{
    std::for_each(m_modelList.begin(), m_modelList.end(),
                  UpdateObservable(oldname, newname));
}

void CoupledModel::purgeObservable(const std::set < std::string >& observablelist)
{
    std::for_each(m_modelList.begin(), m_modelList.end(),
                  PurgeObservable(observablelist));
}

void CoupledModel::updateConditions(const std::string& oldname,
                                    const std::string& newname)
{
    std::for_each(m_modelList.begin(), m_modelList.end(),
                  UpdateConditions(oldname, newname));
}

void CoupledModel::purgeConditions(const std::set < std::string >& conditionslist)
{
    std::for_each(m_modelList.begin(), m_modelList.end(),
                  PurgeConditions(conditionslist));
}
void CoupledModel::writeConnection(std::ostream& out) const
{
    out << "<connections>\n";
    writeConnections(out);
    out << "</connections>\n";
}

BaseModel* CoupledModel::find(int x, int y) const
{
    ModelList::const_iterator it = std::find_if(
        m_modelList.begin(), m_modelList.end(), IsInModelList(x, y));
    return (it == m_modelList.end()) ? 0 : it->second;
}

BaseModel* CoupledModel::find(int x, int y, int width, int height) const
{
    ModelList::const_iterator it = m_modelList.begin();
    while (it != m_modelList.end()) {
        if (it->second->x() <= x and x <= it->second->x() + width
            and it->second->y() <= y and y <= it->second->y() + height)
            return it->second;
        ++it;
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
        newname.assign((fmt("%1%-%2%") % name % i).str());
        ++i;
    } while (exist(name));

    return newname;
}

ModelPortList& CoupledModel::getInternalInPort(const std::string& name)
{
    ConnectionList::iterator it = m_internalInputList.find(name);
    if (it == m_internalInputList.end()) {
        throw utils::DevsGraphError(
            (fmt(_("Coupled model %1% have no input port %2%"))
             % getName() % name).str());
    }

    return it->second;
}

const ModelPortList& CoupledModel::getInternalInPort(
    const std::string& name) const
{
    ConnectionList::const_iterator it = m_internalInputList.find(name);
    if (it == m_internalInputList.end()) {
        throw utils::DevsGraphError(
            (fmt(_("Coupled model %1% have no input port %2%"))
             % getName() % name).str());
    }

    return it->second;
}

ModelPortList& CoupledModel::getInternalOutPort(const std::string& name)
{
    ConnectionList::iterator it = m_internalOutputList.find(name);
    if (it == m_internalOutputList.end()) {
        throw utils::DevsGraphError(
            (fmt(_("Coupled model %1% have no output port %2%"))
             % getName() % name).str());
    }

    return it->second;
}

const ModelPortList& CoupledModel::getInternalOutPort(
    const std::string& name) const
{
    ConnectionList::const_iterator it = m_internalOutputList.find(name);
    if (it == m_internalOutputList.end()) {
        throw utils::DevsGraphError(
            (fmt(_("Coupled model %1% have no output port %2%")) % getName() %
             name).str());
    }

    return it->second;
}

void CoupledModel::copyInternalConnection(const ConnectionList& src,
                                          ConnectionList& dst,
                                          const BaseModel& parentSrc,
                                          BaseModel& parentDst)
{
    assert(src.size() == dst.size());

    ConnectionList::const_iterator it = src.begin();
    ConnectionList::iterator jt = dst.begin();

    while (it != src.end()) {
        copyInternalPort(it->second, jt->second, parentSrc, parentDst);
        ++it;
        ++jt;
    }
}

void CoupledModel::copyInternalPort(const ModelPortList& src,
                                    ModelPortList& dst,
                                    const BaseModel& parentSrc,
                                    BaseModel& parentDst)
{
    typedef ModelPortList::const_iterator const_iterator;

    for (const_iterator it = src.begin(); it != src.end(); ++it) {
        if (it->first == &parentSrc) {
            dst.add(&parentDst, it->second);
        } else {
            BaseModel* dstmodel = findModel(it->first->getName());
            assert(dstmodel);
            dst.add(dstmodel, it->second);
        }
    }
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
    typedef ModelPortList::const_iterator const_iterator;

    for (const_iterator it = src.begin(); it != src.end(); ++it) {
        BaseModel* dstmodel = findModel(it->first->getName());
        assert(dstmodel);
        dst.add(dstmodel, it->second);
    }
}

CoupledModel::ModelConnections CoupledModel::saveInputConnections(
    ModelList& models)
{
    ModelConnections listModel;
    ModelList::iterator it = models.begin();

    while (it != models.end()) {
        ConnectionList connectIn;
        ConnectionList::iterator iter =
            it->second->getInputPortList().begin();

        while (iter != it->second->getInputPortList().end()) {
            ModelPortList lst(it->second->getInPort(iter->first));

            connectIn[iter->first] = lst;
            ++iter;
        }
        listModel[it->first] = connectIn;
        ++it;
    }
    return listModel;
}

CoupledModel::ModelConnections CoupledModel::saveOutputConnections(
    ModelList& models)
{
    ModelConnections listModel;
    ModelList::iterator it = models.begin();

    while (it != models.end()) {
        ConnectionList connectOut;
        ConnectionList::iterator iter =
            it->second->getOutputPortList().begin();

        while (iter != it->second->getOutputPortList().end()) {
            ModelPortList lst(it->second->getOutPort(iter->first));

            connectOut[iter->first] = lst;
            ++iter;
        }
        listModel[it->first] = connectOut;
        ++it;
    }
    return listModel;
}

void CoupledModel::restoreInputConnections(ModelList& models,
                                           CoupledModel* destination,
                                           ModelConnections connections)
{
    typedef std::map < std::pair < std::string, std::string >,
    std::string > inputPort_t;
    typedef std::map < std::string, unsigned int > index_t;
    inputPort_t inputPorts;
    index_t indexes;

    for (CoupledModel::ModelConnections::const_iterator iterConnection =
         connections.begin(); iterConnection != connections.end();
         ++iterConnection) {
        ConnectionList connectIn = iterConnection->second;

        for (ConnectionList::const_iterator iterPort = connectIn.begin();
             iterPort != connectIn.end(); ++iterPort) {
            for (ModelPortList::const_iterator iterModel =
                 connectIn[iterPort->first].begin();
                 iterModel != connectIn[iterPort->first].end(); ++iterModel) {
                if (connections.find(iterModel->first->getName()) !=
                    connections.end()) {
                    addInternalConnection(iterModel->first, iterModel->second,
                                          models[iterConnection->first],
                                          iterPort->first);
                } else {
                    std::string portName;

                    if (not destination->existInputPort(iterModel->second)) {
                        portName = iterModel->second;
                        destination->addInputPort(portName);
                        inputPorts[std::make_pair(
                            iterModel->first->getName(),
                            iterModel->second)] = portName;
                        indexes[iterModel->second] = 0;
                    } else {
                        std::pair < std::string, std::string > inputPort(
                            std::make_pair(iterModel->first->getName(),
                                           iterModel->second));
                        inputPort_t::const_iterator it =
                            inputPorts.find(inputPort);

                        if (it == inputPorts.end()) {
                            indexes[iterModel->second]++;
                            portName = (fmt("%1%_%2%") % iterModel->second %
                                        (indexes[iterModel->second])).str();
                            destination->addInputPort(portName);
                            inputPorts[
                                std::make_pair(iterModel->first->getName(),
                                               iterModel->second)] = portName;
                        } else {
                            portName = it->second;
                        }
                    }
                    destination->addInputConnection(
                        portName, iterConnection->first, iterPort->first);
                    if (iterModel->first == this) {
                        if (not existInputConnection(
                                iterModel->second, destination->getName(),
                                portName)) {
                            addInputConnection(iterModel->second,
                                               destination,
                                               portName);
                        }
                    } else {
                        if (not existInternalConnection(
                                iterModel->first->getName(),
                                iterModel->second, destination->getName(),
                                portName)) {
                            addInternalConnection(iterModel->first,
                                                  iterModel->second,
                                                  destination,
                                                  portName);
                        }
                    }
                }
            }
        }
    }
}

void CoupledModel::restoreOutputConnections(CoupledModel* destination,
                                            ModelConnections connections)
{
    typedef std::map < std::pair < std::string, std::string >,
            std::string > outputPort_t;
    typedef std::map < std::string, unsigned int > index_t;
    outputPort_t outputPorts;
    index_t indexes;

    for (CoupledModel::ModelConnections::const_iterator iterConnection =
         connections.begin(); iterConnection != connections.end();
         ++iterConnection) {
        ConnectionList connectOut = iterConnection->second;

        for (ConnectionList::const_iterator iterPort = connectOut.begin();
             iterPort != connectOut.end(); ++iterPort) {
            for (ModelPortList::const_iterator iterModel =
                 connectOut[iterPort->first].begin();
                 iterModel != connectOut[iterPort->first].end(); ++iterModel) {
                if (connections.find(iterModel->first->getName()) ==
                    connections.end()) {
                    std::string portName;

                    if (not destination->existOutputPort(iterModel->second)) {
                        portName = iterModel->second;
                        destination->addOutputPort(portName);
                        outputPorts[std::make_pair(
                            iterModel->first->getName(),
                            iterModel->second)] = portName;
                        indexes[iterModel->second] = 0;
                    } else {
                        std::pair < std::string, std::string > outputPort(
                            std::make_pair(iterModel->first->getName(),
                                           iterModel->second));
                        outputPort_t::const_iterator it =
                            outputPorts.find(outputPort);

                        if (it == outputPorts.end()) {
                            indexes[iterModel->second]++;
                            portName = (fmt("%1%_%2%") % iterModel->second %
                                        (indexes[iterModel->second])).str();
                            destination->addOutputPort(portName);
                            outputPorts[
                                std::make_pair(iterModel->first->getName(),
                                               iterModel->second)] = portName;
                        } else {
                            portName = it->second;
                        }
                    }
                    destination->addOutputConnection(iterConnection->first,
                                                     iterPort->first, portName);
                    if (iterModel->first == this) {
                        if (not existOutputConnection(destination->getName(),
                                                      portName,
                                                      iterModel->second)) {
                            addOutputConnection(destination, portName,
                                                iterModel->second);
                        }
                    } else {
                        if (not existInternalConnection(
                                destination->getName(),portName,
                                iterModel->first->getName(),
                                iterModel->second)) {
                            addInternalConnection(destination, portName,
                                                  iterModel->first,
                                                  iterModel->second);
                        }
                    }
                }
            }
        }
    }
}

void CoupledModel::initConnections()
{
    m_srcConnections.clear();
    m_dstConnections.clear();

    for (ModelList::const_iterator it = m_modelList.begin();
         it != m_modelList.end(); ++it) {
        const ConnectionList& outs(it->second->getOutputPortList());

        for (ConnectionList::const_iterator jt = outs.begin();
             jt != outs.end(); ++jt) {
            const ModelPortList&  ports(jt->second);

            for (ModelPortList::const_iterator kt = ports.begin();
                 kt != ports.end(); ++kt) {
                m_srcConnections.push_back(it->second);
                m_dstConnections.push_back(kt->first);
            }
        }
    }

    initInternalInputConnections();
    initInternalOutputConnections();
}

void CoupledModel::initInternalInputConnections()
{
    ConnectionList& ins(getInternalInputPortList());
    for (ConnectionList::const_iterator it = ins.begin();
         it != ins.end(); ++it) {
        const ModelPortList& ports(it->second);
        ModelPortList::const_iterator jt ;

        for (jt = ports.begin(); jt != ports.end(); ++jt) {
            m_srcConnections.push_back(this);
            m_dstConnections.push_back(jt->first);
        }
    }
}

void CoupledModel::initInternalOutputConnections()
{
    ConnectionList& outs(getInternalOutputPortList());
    for (ConnectionList::const_iterator it = outs.begin();
         it != outs.end(); ++it) {
        const ModelPortList& ports(it->second);
        ModelPortList::const_iterator jt ;

        for (jt = ports.begin(); jt != ports.end(); ++jt) {
            m_srcConnections.push_back(jt->first);
            m_dstConnections.push_back(this);
        }
    }
}

float CoupledModel::distanceModels(BaseModel* src, BaseModel* dst)
{
    return std::sqrt(((float)(dst->x()) - (float)(src->x()))
                     * ((float)(dst->x()) - (float)(src->x()))
                     + ((float)(dst->y()) - (float)(src->y()))
                     * ((float)(dst->y()) - (float)(src->y())));
}

void CoupledModel::repulsionForce()
{
    for (ModelList::iterator it = m_modelList.begin();
         it != m_modelList.end(); ++it) {

        for (ModelList::iterator jt = m_modelList.begin();
             jt != m_modelList.end(); ++jt) {

            if (it->second != jt->second) {
                float distance = distanceModels(it->second, jt->second);

                if (distance < 1000) {
                    float f = -25 * 25;
                    float forceX = ((float)(jt->second->x())
                                    - (float)(it->second->x()) * f)
                        / (distance * distance);

                    float forceY = ((float)(jt->second->y())
                                    - (float)(it->second->y()) * f)
                        / (distance * distance);

                    it->second->setForce(it->second->dx() + forceX,
                                         it->second->dy() + forceY);
                    jt->second->setForce(jt->second->dx() - forceX,
                                         jt->second->dy() - forceY);
                }
            }
        }
    }
}

void CoupledModel::attractionForce()
{
    for (std::vector < BaseModel* >::size_type i = 0; i <
         m_srcConnections.size(); ++i) {
        float distance =
            distanceModels(m_srcConnections[i], m_dstConnections[i]);

        float f = 25.0f;

        float forceX = ((float)(m_dstConnections[i]->x())
                        - (float)(m_srcConnections[i]->x()))
            / (distance * f);
        float forceY = ((float)(m_dstConnections[i]->y())
                        - (float)(m_srcConnections[i]->y()))
            / (distance * f);

        m_srcConnections[i]->setForce(m_srcConnections[i]->dx() + forceX,
                                      m_srcConnections[i]->dy() + forceY);

        m_dstConnections[i]->setForce(m_dstConnections[i]->dx() - forceX,
                                      m_dstConnections[i]->dy() - forceY);
    }
}

bool CoupledModel::newPosition()
{
    bool correct = true;

    for (ModelList::iterator it = m_modelList.begin();
         it != m_modelList.end(); ++it) {

        float norme = (it->second->dx() * it->second->dx())
            + (it->second->dy() *  it->second->dy());

        if (std::sqrt(norme) > 100 and std::sqrt(norme) > 100) {
            it->second->setForce(it->second->dx() * width() /
                                 std::sqrt(norme),
                                 it->second->dy() * height() /
                                 std::sqrt(norme));
        }

        int x = it->second->x();
        int y = it->second->y();
        it->second->setPosition((int)(it->second->x() + it->second->dx()),
                                (int)(it->second->y() + it->second->dy()));

        if (it->second->x() < 0) {
            correct = false;
            it->second->setX(x);
        }
        if (it->second->y() < 0) {
            correct = false;
            it->second->setY(y);
        }

        if (it->second->x() > m_width)
            it->second->setX(m_width);

        if (it->second->y() > m_height)
            it->second->setY(m_height);

        if (it->second->x() != x or it->second->y() != y) {
            correct = false;
        }
    }
    return correct;
}

void CoupledModel::order()
{
    initConnections();

    bool correct=false;
    int iteration = 0;

    while (correct == false and iteration <= 5000) {
        for (ModelList::iterator it = m_modelList.begin();
             it != m_modelList.end(); ++it) {
            it->second->setForce(0.0, 0.0);
        }

        repulsionForce();
        attractionForce();
        correct = newPosition();

        ++iteration;
    }
}

}} // namespace vle vpz
