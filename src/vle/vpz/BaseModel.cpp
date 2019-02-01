/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * https://www.vle-project.org
 *
 * Copyright (c) 2003-2018 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2018 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2018 INRA http://www.inra.fr
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

#include <algorithm>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <stack>
#include <utility>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/vpz/AtomicModel.hpp>
#include <vle/vpz/CoupledModel.hpp>

namespace vle {
namespace vpz {

BaseModel::BaseModel(std::string name, CoupledModel* parent)
  : m_parent(parent)
  , m_x(-1)
  , m_y(-1)
  , m_width(-1)
  , m_height(-1)
  , m_name(std::move(name))
{
    if (parent) {
        parent->addModel(this);
    }
}

BaseModel::BaseModel(const BaseModel& mdl)
  : m_parent(nullptr)
  , m_inPortList(mdl.m_inPortList)
  , m_outPortList(mdl.m_outPortList)
  , m_x(mdl.m_x)
  , m_y(mdl.m_y)
  , m_width(mdl.m_width)
  , m_height(mdl.m_height)
  , m_name(mdl.m_name)
{
    std::for_each(mdl.m_inPortList.begin(),
                  mdl.m_inPortList.end(),
                  CopyWithoutConnection(m_inPortList));

    std::for_each(mdl.m_outPortList.begin(),
                  mdl.m_outPortList.end(),
                  CopyWithoutConnection(m_outPortList));
}

void
BaseModel::swap(BaseModel& mdl)
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

void
BaseModel::getAtomicModelsSource(const std::string& portname,
                                 ModelPortList& result)
{
    std::stack<std::pair<ModelPortList*, BaseModel*>> stack;

    stack.push(std::make_pair(&getInPort(portname), this));

    while (not stack.empty()) {
        ModelPortList* top = stack.top().first;
        BaseModel* source = stack.top().second;
        stack.pop();

        ModelPortList::iterator it, jt;
        for (it = top->begin(); it != top->end(); ++it) {
            const std::string& port(it->second);
            BaseModel* mdl(it->first);

            if (mdl->isAtomic()) {
                result.add(mdl, port);
            } else {
                CoupledModel* cpled = mdl->toCoupled();
                if (cpled == source->getParent()) {
                    stack.push(std::make_pair(&cpled->getInPort(port), mdl));
                } else {
                    stack.push(
                      std::make_pair(&cpled->getInternalOutPort(port), mdl));
                }
            }
        }
    }
}

void
BaseModel::getAtomicModelsTarget(const std::string& portname,
                                 ModelPortList& result)
{
    std::stack<std::pair<ModelPortList*, BaseModel*>> stack;

    stack.push(std::make_pair(&getOutPort(portname), this));

    while (not stack.empty()) {
        ModelPortList* top = stack.top().first;
        BaseModel* source = stack.top().second;
        stack.pop();

        ModelPortList::iterator it, jt;
        for (it = top->begin(); it != top->end(); ++it) {
            const std::string& port(it->second);
            BaseModel* mdl(it->first);

            if (mdl->isAtomic()) {
                result.add(mdl, port);
            } else {
                CoupledModel* cpled = mdl->toCoupled();
                if (cpled == source->getParent()) {
                    stack.push(std::make_pair(&cpled->getOutPort(port), mdl));
                } else {
                    stack.push(
                      std::make_pair(&cpled->getInternalInPort(port), mdl));
                }
            }
        }
    }
}

void
BaseModel::rename(BaseModel* mdl, const std::string& newname)
{
    if (not mdl) {
        throw utils::DevsGraphError(_("Cannot rename empty model with '%s'"),
                                    newname.c_str());
    }

    CoupledModel* parent = mdl->getParent();
    if (parent) {
        auto it = parent->getModelList().find(mdl->getName());

        if (it == parent->getModelList().end()) {
            throw utils::DevsGraphError(
              _("Cannot rename a model without parent"));
        }

        auto itfind = parent->getModelList().find(newname);
        if (itfind != parent->getModelList().end()) {
            throw utils::DevsGraphError(
              _("Coupled model %s already has submodel %s"),
              parent->getName().c_str(),
              newname.c_str());
        } else {
            mdl->m_name.assign(newname);
            parent->getModelList().erase(it);
            parent->addModel(mdl);
        }
    } else {
        mdl->m_name.assign(newname);
    }
}

std::string
BaseModel::getParentName() const
{
    std::list<std::string> lst;
    CoupledModel* parent = m_parent;

    while (parent) {
        lst.push_front(parent->getName());
        parent = parent->getParent();
    }

    std::string result;
    while (not lst.empty()) {
        result += lst.front();
        lst.pop_front();
        if (not lst.empty()) {
            result += ',';
        }
    }
    return result;
}

std::string
BaseModel::getCompleteName() const
{
    std::list<const BaseModel*> stack;
    std::list<const BaseModel*>::iterator it, jt;
    std::string result;

    stack.push_back(this);

    while (stack.front()->getParent()) {
        stack.push_front(stack.front()->getParent());
    }

    it = stack.begin();
    while (it != stack.end()) {
        result += (*it)->getName();
        jt = it++;
        if (it != stack.end()) {
            result += ',';
        }
    }

    return result;
}

void
BaseModel::getParents(CoupledModelVector& parents) const
{
    CoupledModel* parent = m_parent;

    while (parent) {
        parents.push_back(parent);
        parent = parent->getParent();
    }
}

BaseModel*
BaseModel::getModel(const CoupledModelVector& lst, const std::string& name)
{
    if (lst.empty()) {
        return findModel(name);
    } else {
        if (not isCoupled()) {
            throw utils::DevsGraphError(_("Bad use of getModel from a list"));
        }
        auto it = lst.rbegin();
        auto* top = static_cast<CoupledModel*>(this);
        CoupledModel* other = *it;

        if (other->getName() != top->getName()) {
            throw utils::DevsGraphError(
              _("Get model have not the same name '%s' and '%s'"),
              top->getName().c_str(),
              other->getName().c_str());
        }

        it++;

        while (it != lst.rend()) {
            other = *it;
            BaseModel* tmp = top->findModel(other->getName());

            if (not tmp) {
                throw utils::DevsGraphError(_("Model '%s' not found"),
                                            other->getName().c_str());
            }

            if (not tmp->isCoupled()) {
                throw utils::DevsGraphError(
                  _("Model '%s' is not a coupled model"),
                  other->getName().c_str());
            }

            if (tmp->getName() != other->getName()) {
                throw utils::DevsGraphError(
                  _("Model '%s' have not the same parent"),
                  other->getName().c_str());
            }

            top = static_cast<CoupledModel*>(tmp);
            ++it;
        }

        return top->findModel(name);
    }
}

BaseModel*
BaseModel::findModelFromPath(const std::string& path) const
{
    std::vector<std::string> splitVect;
    boost::split(splitVect, path, boost::is_any_of(","));
    std::vector<std::string>::const_iterator it = splitVect.begin();
    std::vector<std::string>::const_iterator ite = splitVect.end();
    BaseModel* currModel = nullptr;
    const CoupledModel* currCoupledModel = nullptr;
    for (; it != ite; it++) {
        if (currModel == nullptr) {
            if (this->isAtomic()) {
                return nullptr;
            }
            currCoupledModel = static_cast<const CoupledModel*>(this);
        } else {
            if (currModel->isAtomic()) {
                return nullptr;
            }
            currCoupledModel = static_cast<const CoupledModel*>(currModel);
        }
        auto itml = currCoupledModel->getModelList().find(*it);
        if (itml == currCoupledModel->getModelList().end()) {
            return nullptr;
        }
        currModel = itml->second;
    }
    return currModel;
}

ModelPortList&
BaseModel::addInputPort(const std::string& name)
{
    auto it(m_inPortList.find(name));
    if (it == m_inPortList.end()) {
        if (isCoupled()) {
            auto* cpl = static_cast<CoupledModel*>(this);
            cpl->getInternalInputPortList().insert(
              ConnectionList::value_type(name, ModelPortList()));
        }
        return (*m_inPortList
                   .insert(ConnectionList::value_type(name, ModelPortList()))
                   .first)
          .second;
    } else {
        return it->second;
    }
}

ModelPortList&
BaseModel::addOutputPort(const std::string& name)
{
    auto it(m_outPortList.find(name));
    if (it == m_outPortList.end()) {
        if (isCoupled()) {
            auto* cpl = static_cast<CoupledModel*>(this);
            cpl->getInternalOutputPortList().insert(
              ConnectionList::value_type(name, ModelPortList()));
        }
        return (*m_outPortList
                   .insert(ConnectionList::value_type(name, ModelPortList()))
                   .first)
          .second;
    } else {
        return it->second;
    }
}

void
BaseModel::delInputPort(const std::string& name)
{
    ModelPortList& lst(getInPort(name));
    for (auto& elem : lst) {
        if (elem.first != m_parent) {
            ModelPortList& toclean(elem.first->getOutPort(elem.second));
            toclean.erase(this);
        } else {
            ModelPortList& toclean(m_parent->getInternalInPort(elem.second));
            toclean.erase(this);
        }
    }
    if (isCoupled()) {
        CoupledModel* tmp(toCoupled(this));
        ModelPortList& intern(tmp->getInternalInPort(name));
        auto jt = intern.begin();
        while (jt != intern.end()) {
            jt->first->getInPort(jt->second).erase(this);
            ++jt;
        }
        intern.clear();
    }

    lst.clear();
    m_inPortList.erase(name);
}

void
BaseModel::delOutputPort(const std::string& name)
{
    ModelPortList& lst(getOutPort(name));
    for (auto& elem : lst) {
        if (elem.first != m_parent) {
            ModelPortList& toclean(elem.first->getInPort(elem.second));
            toclean.erase(this);
        } else {
            ModelPortList& toclean(m_parent->getInternalOutPort(elem.second));
            toclean.erase(this);
        }
    }
    if (isCoupled()) {
        CoupledModel* tmp(toCoupled(this));
        ModelPortList& intern(tmp->getInternalOutPort(name));
        auto jt = intern.begin();
        while (jt != intern.end()) {
            jt->first->getOutPort(jt->second).erase(this);
            ++jt;
        }
        intern.clear();
    }

    lst.clear();
    m_outPortList.erase(name);
}

void
BaseModel::addInPort(ModelPortList& model,
                     ModelPortList& intern,
                     const std::string& name)
{
    for (auto& elem : model) {
        if (elem.first != m_parent) {
            elem.first->getOutPort(elem.second).add(this, name);
        } else {
            m_parent->getInternalInPort(elem.second).add(this, name);
        }
    }
    if (isCoupled()) {
        auto jt = intern.begin();
        while (jt != intern.end()) {
            jt->first->getInPort(jt->second).add(this, name);
            ++jt;
        }
    }
}

void
BaseModel::addOutPort(ModelPortList& model,
                      ModelPortList& intern,
                      const std::string& name)
{
    for (auto& elem : model) {
        if (elem.first != m_parent) {
            elem.first->getInPort(elem.second).add(this, name);
        } else {
            m_parent->getInternalOutPort(elem.second).add(this, name);
        }
    }
    if (isCoupled()) {
        auto jt = intern.begin();
        while (jt != intern.end()) {
            jt->first->getOutPort(jt->second).add(this, name);
            ++jt;
        }
    }
}

ModelPortList&
BaseModel::renameInputPort(const std::string& old_name,
                           const std::string& new_name)
{
    auto iter = getInputPortList().find(old_name);
    ModelPortList connect(iter->second);
    ModelPortList internalConnect;
    if (isCoupled()) {
        CoupledModel* tmp(toCoupled(this));
        internalConnect = tmp->getInternalInPort(old_name);
    }

    delInputPort(old_name);

    auto it(m_inPortList.find(new_name));
    if (it == m_inPortList.end()) {
        if (isCoupled()) {
            auto* cpl = static_cast<CoupledModel*>(this);
            cpl->getInternalInputPortList().insert(
              ConnectionList::value_type(new_name, internalConnect));
        }
        addInPort(connect, internalConnect, new_name);
        return (*m_inPortList
                   .insert(ConnectionList::value_type(new_name, connect))
                   .first)
          .second;
    } else {
        return it->second;
    }
}

ModelPortList&
BaseModel::renameOutputPort(const std::string& old_name,
                            const std::string& new_name)
{
    auto iter = getOutputPortList().find(old_name);
    ModelPortList connect(iter->second);
    ModelPortList internalConnect;
    if (isCoupled()) {
        CoupledModel* tmp(toCoupled(this));
        internalConnect = tmp->getInternalOutPort(old_name);
    }

    delOutputPort(old_name);

    auto it(m_outPortList.find(new_name));
    if (it == m_outPortList.end()) {
        if (isCoupled()) {
            auto* cpl = static_cast<CoupledModel*>(this);
            cpl->getInternalOutputPortList().insert(
              ConnectionList::value_type(new_name, internalConnect));
        }
        addOutPort(connect, internalConnect, new_name);
        return (*m_outPortList
                   .insert(ConnectionList::value_type(new_name, connect))
                   .first)
          .second;
    } else {
        return it->second;
    }
}

void
BaseModel::addInputPort(const std::list<std::string>& lst)
{
    std::for_each(lst.begin(), lst.end(), AddInputPort(*this));
}

void
BaseModel::addOutputPort(const std::list<std::string>& lst)
{
    std::for_each(lst.begin(), lst.end(), AddOutputPort(*this));
}

const ModelPortList&
BaseModel::getInPort(const std::string& name) const
{
    auto it = m_inPortList.find(name);
    if (it == m_inPortList.end()) {
        throw utils::DevsGraphError(
          _("Coupled model %s have no input port %s"),
          getName().c_str(),
          name.c_str());
    }

    return it->second;
}

const ModelPortList&
BaseModel::getOutPort(const std::string& name) const
{
    auto it = m_outPortList.find(name);
    if (it == m_outPortList.end()) {
        throw utils::DevsGraphError(
          _("Coupled model %s have no output port %s"),
          getName().c_str(),
          name.c_str());
    }

    return it->second;
}

ModelPortList&
BaseModel::getInPort(const std::string& name)
{
    auto it = m_inPortList.find(name);
    if (it == m_inPortList.end()) {
        throw utils::DevsGraphError(_("Model %s have no input port %s"),
                                    getName().c_str(),
                                    name.c_str());
    }

    return it->second;
}

ModelPortList&
BaseModel::getOutPort(const std::string& name)
{
    auto it = m_outPortList.find(name);
    if (it == m_outPortList.end()) {
        throw utils::DevsGraphError(_("Model %s have no output port %s"),
                                    getName().c_str(),
                                    name.c_str());
    }

    return it->second;
}

bool
BaseModel::existInputPort(const std::string& name)
{
    return m_inPortList.find(name) != m_inPortList.end();
}

bool
BaseModel::existOutputPort(const std::string& name)
{
    return m_outPortList.find(name) != m_outPortList.end();
}

int
BaseModel::getInputPortIndex(const std::string& name) const
{
    auto it = m_inPortList.find(name);

    if (it == m_inPortList.end()) {
        throw utils::DevsGraphError(_("Input port %s not exist in model %s"),
                                    name.c_str(),
                                    getName().c_str());
    }

    return utils::numeric_cast<int>(std::distance(m_inPortList.begin(), it));
}

int
BaseModel::getOutputPortIndex(const std::string& name) const
{
    auto it = m_outPortList.find(name);

    if (it == m_outPortList.end()) {
        throw utils::DevsGraphError(_("Output port %s not exist in model %s"),
                                    name.c_str(),
                                    getName().c_str());
    }

    return utils::numeric_cast<int>(std::distance(m_outPortList.begin(), it));
}

void
BaseModel::writePortListXML(std::ostream& out) const
{
    if (not m_inPortList.empty()) {
        out << "<in>\n";

        auto it = m_inPortList.begin();
        for (; it != m_inPortList.end(); ++it) {
            out << "<port name=\"" << (*it).first.c_str() << "\" />\n";
        }
        out << "</in>\n";
    }

    if (not m_outPortList.empty()) {
        out << "<out>\n";
        auto it = m_outPortList.begin();
        for (; it != m_outPortList.end(); ++it) {
            out << "<port name=\"" << (*it).first.c_str() << "\" />\n";
        }
        out << "</out>\n";
    }
}

void
BaseModel::writeGraphics(std::ostream& out) const
{
    if (x() >= 0) {
        out << "x=\"" << x() << "\" ";
    }
    if (y() >= 0) {
        out << "y=\"" << y() << "\" ";
    }
    if (width() >= 0) {
        out << "width=\"" << width() << "\" ";
    }
    if (height() >= 0) {
        out << "height=\"" << height() << "\" ";
    }
}

void
BaseModel::writePort(std::ostream& out) const
{
    const vpz::ConnectionList& ins(getInputPortList());
    if (not ins.empty()) {
        out << "<in>\n";
        for (const auto& in : ins) {
            out << " <port name=\"" << in.first.c_str() << "\" />\n";
        }
        out << "</in>\n";
    }

    const vpz::ConnectionList& outs(getOutputPortList());
    if (not outs.empty()) {
        out << "<out>\n";
        for (const auto& outs_it : outs) {
            out << " <port name=\"" << outs_it.first.c_str() << "\" />\n";
        }
        out << "</out>\n";
    }
}

AtomicModel*
BaseModel::toAtomic(BaseModel* model)
{
    if (model and model->isAtomic()) {
        return static_cast<AtomicModel*>(model);
    }
    return nullptr;
}

CoupledModel*
BaseModel::toCoupled(BaseModel* model)
{
    if (model and model->isCoupled()) {
        return static_cast<CoupledModel*>(model);
    }
    return nullptr;
}

void
BaseModel::getAtomicModelList(BaseModel* model, AtomicModelVector& list)
{
    if (model->isAtomic()) {
        list.push_back(static_cast<AtomicModel*>(model));
    } else {
        std::list<CoupledModel*> coupledModelList;
        coupledModelList.push_back((CoupledModel*)model);
        while (!coupledModelList.empty()) {
            CoupledModel* m = coupledModelList.front();
            ModelList& v(m->getModelList());

            for (auto& elem : v) {
                BaseModel* n = elem.second;
                if (n->isAtomic()) {
                    list.push_back(static_cast<AtomicModel*>(n));
                } else {
                    coupledModelList.push_back(static_cast<CoupledModel*>(n));
                }
            }
            coupledModelList.pop_front();
        }
    }
}

bool
BaseModel::isInList(const ModelList& lst, BaseModel* m)
{
    return lst.find(m->getName()) != lst.end();
}

BaseModel::BaseModel()
{
    throw utils::NotYetImplemented("BaseModel::BaseModel not developed");
}

BaseModel&
BaseModel::operator=(const BaseModel& /* mdl */)
{
    throw utils::NotYetImplemented("BaseModel::operator= not developed");
}
}
} // namespace vle vpz
