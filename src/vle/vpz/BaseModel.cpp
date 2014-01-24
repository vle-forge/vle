/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
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
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <algorithm>
#include <stack>

namespace vle { namespace vpz {

BaseModel::BaseModel(const std::string& name, CoupledModel* parent) :
    m_parent(parent),
    m_x(-1),
    m_y(-1),
    m_width(-1),
    m_height(-1),
    m_name(name)
{
    if (parent) {
        parent->addModel(this);
    }
}

BaseModel::BaseModel(const BaseModel& mdl) :
    m_parent(0),
    m_inPortList(mdl.m_inPortList),
    m_outPortList(mdl.m_outPortList),
    m_x(mdl.m_x),
    m_y(mdl.m_y),
    m_width(mdl.m_width),
    m_height(mdl.m_height),
    m_name(mdl.m_name)
{
    std::for_each(mdl.m_inPortList.begin(), mdl.m_inPortList.end(),
                  CopyWithoutConnection(m_inPortList));

    std::for_each(mdl.m_outPortList.begin(), mdl.m_outPortList.end(),
                  CopyWithoutConnection(m_outPortList));
}

void BaseModel::swap(BaseModel& mdl)
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

void BaseModel::getAtomicModelsSource(const std::string& portname,
                                  ModelPortList& result)
{
    std::stack < std::pair < ModelPortList*, BaseModel* > > stack;

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
                    stack.push(std::make_pair(
                            &cpled->getInPort(port), mdl));
                } else {
                    stack.push(std::make_pair(
                            &cpled->getInternalOutPort(port), mdl));
                }
            }
        }
    }
}

void BaseModel::getAtomicModelsTarget(const std::string& portname,
                                  ModelPortList& result)
{
    std::stack < std::pair < ModelPortList*, BaseModel* > > stack;

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
                    stack.push(std::make_pair(
                            &cpled->getOutPort(port), mdl));
                } else {
                    stack.push(std::make_pair(
                            &cpled->getInternalInPort(port), mdl));
                }
            }
        }
    }
}

void BaseModel::rename(BaseModel* mdl, const std::string& newname)
{
    if (not mdl) {
        throw utils::DevsGraphError(fmt(
                _("Cannot rename empty model with '%1%'")) % newname);
    }

    CoupledModel* parent = mdl->getParent();
    if (parent) {
        ModelList::iterator it = parent->getModelList().find(mdl->getName());

        if (it == parent->getModelList().end()) {
            throw utils::DevsGraphError(
                _("Cannot rename a model without parent"));
        }

	ModelList::iterator itfind = parent->getModelList().find(newname);
	if (itfind != parent->getModelList().end()) {
	    throw utils::DevsGraphError(fmt(
		      _("Coupled model %1% already has submodel %2%"))
		  % parent->getName() % newname);
	} else {
	    mdl->m_name.assign(newname);
	    parent->getModelList().erase(it);
	    parent->addModel(mdl);
	}
    } else {
        mdl->m_name.assign(newname);
    }
}

std::string BaseModel::getParentName() const
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
        if (not lst.empty()) {
            result += ',';
        }
    }
    return result;
}

std::string BaseModel::getCompleteName() const
{
    std::list < const BaseModel* > stack;
    std::list < const BaseModel* >::iterator it, jt;
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

void BaseModel::getParents(CoupledModelVector& parents) const
{
    CoupledModel* parent = m_parent;

    while (parent) {
        parents.push_back(parent);
        parent = parent->getParent();
    }
}

BaseModel* BaseModel::getModel(const CoupledModelVector& lst,
                                 const std::string& name)
{
    if (lst.empty()) {
        return findModel(name);
    } else {
        if (not isCoupled()) {
            throw utils::DevsGraphError(_("Bad use of getModel from a list"));
        }
        CoupledModelVector::const_reverse_iterator it = lst.rbegin();
        CoupledModel* top = static_cast < CoupledModel* >(this);
        CoupledModel* other = *it;

        if (other->getName() != top->getName()) {
            throw utils::DevsGraphError(fmt(
                    _("Get model have not the same name '%1%' and '%2%'"))
                % top->getName() % other->getName());
        }

        it++;

        while (it != lst.rend()) {
            other = *it;
            BaseModel* tmp = top->findModel(other->getName());

            if (not tmp) {
                throw utils::DevsGraphError(fmt(
                        _("Model '%1%' not found")) % other->getName());
            }

            if (not tmp->isCoupled()) {
                throw utils::DevsGraphError(fmt(
                        _("Model '%1%' is not a coupled model")) %
                    other->getName());
            }

            if (tmp->getName() != other->getName()) {
                throw utils::DevsGraphError(fmt(
                        _("Model '%1%' have not the same parent")) %
                    other->getName());
            }

            top = static_cast < CoupledModel* >(tmp);
            ++it;
        }

        return top->findModel(name);
    }
}

BaseModel* BaseModel::findModelFromPath(const std::string& path) const
{
    std::vector < std::string > splitVect;
    boost::split(splitVect, path, boost::is_any_of(","));
    std::vector < std::string >::const_iterator it = splitVect.begin();
    std::vector < std::string >::const_iterator ite = splitVect.end();
    BaseModel* currModel = 0;
    const CoupledModel* currCoupledModel = 0;
    for (; it != ite; it++) {
        if (currModel == 0) {
            if (this->isAtomic()) {
                return 0;
            }
            currCoupledModel = static_cast<const CoupledModel*>(this);
        }else{
            if (currModel->isAtomic()) {
                return 0;
            }
            currCoupledModel = static_cast<const CoupledModel*>(currModel);
        }
        ModelList::const_iterator itml =
            currCoupledModel->getModelList().find(*it);
        if (itml == currCoupledModel->getModelList().end()) {
            return 0;
        }
        currModel = itml->second;
    }
    return currModel;
}

ModelPortList& BaseModel::addInputPort(const std::string& name)
{
    ConnectionList::iterator it(m_inPortList.find(name));
    if (it == m_inPortList.end()) {
        if (isCoupled()) {
            CoupledModel* cpl = static_cast < CoupledModel* >(this);
            cpl->getInternalInputPortList().insert(
                ConnectionList::value_type(name, ModelPortList()));
        }
        return (*m_inPortList.insert(
                ConnectionList::value_type(
                    name, ModelPortList())).first).second;
    } else {
        return it->second;
    }
}

ModelPortList& BaseModel::addOutputPort(const std::string& name)
{
    ConnectionList::iterator it(m_outPortList.find(name));
    if (it == m_outPortList.end()) {
        if (isCoupled()) {
            CoupledModel* cpl = static_cast < CoupledModel* >(this);
            cpl->getInternalOutputPortList().insert(
                ConnectionList::value_type(name, ModelPortList()));
        }
        return (*m_outPortList.insert(
                ConnectionList::value_type(name,
                                           ModelPortList())).first).second;
    } else {
        return it->second;
    }
}

void BaseModel::delInputPort(const std::string& name)
{
    ModelPortList& lst(getInPort(name));
    for (ModelPortList::iterator it = lst.begin(); it != lst.end(); ++it) {
        if (it->first != m_parent) {
            ModelPortList& toclean(it->first->getOutPort(it->second));
            toclean.erase(this);
        } else {
            ModelPortList& toclean(m_parent->getInternalInPort(it->second));
            toclean.erase(this);
        }
    }
    if (isCoupled()) {
	CoupledModel* tmp(toCoupled(this));
	ModelPortList& intern(tmp->getInternalInPort(name));
	ModelPortList::iterator jt = intern.begin();
	while(jt != intern.end()) {
	    jt->first->getInPort(jt->second).erase(this);
	    ++jt;
	}
	intern.clear();
    }

    lst.clear();
    m_inPortList.erase(name);
}

void BaseModel::delOutputPort(const std::string & name)
{
    ModelPortList& lst(getOutPort(name));
    for(ModelPortList::iterator it = lst.begin(); it != lst.end(); ++it) {
        if (it->first != m_parent) {
            ModelPortList& toclean(it->first->getInPort(it->second));
            toclean.erase(this);
        } else {
            ModelPortList& toclean(m_parent->getInternalOutPort(it->second));
            toclean.erase(this);
        }
    }
    if (isCoupled()) {
	CoupledModel* tmp(toCoupled(this));
	ModelPortList& intern(tmp->getInternalOutPort(name));
	ModelPortList::iterator jt = intern.begin();
	while(jt != intern.end()) {
	    jt->first->getOutPort(jt->second).erase(this);
	    ++jt;
	}
	intern.clear();
    }

    lst.clear();
    m_outPortList.erase(name);
}

void BaseModel::addInPort(ModelPortList&  model, ModelPortList& intern,
		      const std::string& name)
{
    for (ModelPortList::iterator it = model.begin(); it != model.end(); ++it) {
	if (it->first != m_parent) {
	    it->first->getOutPort(it->second).add(this, name);
	} else {
	    m_parent->getInternalInPort(it->second).add(this, name);
	}
    }
    if (isCoupled()) {
	ModelPortList::iterator jt = intern.begin();
	while (jt != intern.end()) {
	    jt->first->getInPort(jt->second).add(this, name);
	    ++jt;
	}
    }
}

void BaseModel::addOutPort(ModelPortList&  model, ModelPortList& intern,
			       const std::string& name)
{
    for (ModelPortList::iterator it = model.begin(); it != model.end(); ++it) {
	if (it->first != m_parent) {
	    it->first->getInPort(it->second).add(this, name);
	} else {
	    m_parent->getInternalOutPort(it->second).add(this, name);
	}
    }
    if (isCoupled()) {
	ModelPortList::iterator jt = intern.begin();
	while (jt != intern.end()) {
	    jt->first->getOutPort(jt->second).add(this, name);
	    ++jt;
	}
    }
}

ModelPortList& BaseModel::renameInputPort(const std::string& old_name,
				      const std::string& new_name)
{
    ConnectionList::iterator iter = getInputPortList().find(old_name);
    ModelPortList connect(iter->second);
    ModelPortList internalConnect;
    if (isCoupled()) {
	CoupledModel* tmp(toCoupled(this));
	internalConnect = tmp->getInternalInPort(old_name);

    }

    delInputPort(old_name);

    ConnectionList::iterator it(m_inPortList.find(new_name));
    if (it == m_inPortList.end()) {
	if (isCoupled()) {
	    CoupledModel* cpl = static_cast < CoupledModel* >(this);
	    cpl->getInternalInputPortList().insert(
                ConnectionList::value_type(new_name, internalConnect));
	}
	addInPort(connect, internalConnect,  new_name);
	return (*m_inPortList.insert(
                ConnectionList::value_type(new_name, connect)).first).second;
    } else {
	return it->second;
    }
}

ModelPortList& BaseModel::renameOutputPort(const std::string& old_name,
				       const std::string& new_name)
{
    ConnectionList::iterator iter = getOutputPortList().find(old_name);
    ModelPortList connect(iter->second);
    ModelPortList internalConnect;
    if (isCoupled()) {
	CoupledModel* tmp(toCoupled(this));
	internalConnect = tmp->getInternalOutPort(old_name);
    }


    delOutputPort(old_name);

    ConnectionList::iterator it(m_outPortList.find(new_name));
    if (it == m_outPortList.end()) {
	if (isCoupled()) {
	    CoupledModel* cpl = static_cast < CoupledModel* >(this);
	    cpl->getInternalOutputPortList().insert(
                ConnectionList::value_type(new_name, internalConnect));
	}
	addOutPort(connect, internalConnect, new_name);
	return (*m_outPortList.insert(
                ConnectionList::value_type(new_name, connect)).first).second;
    } else {
	return it->second;
    }
}

void BaseModel::addInputPort(const std::list < std::string > & lst)
{
    std::for_each(lst.begin(), lst.end(), AddInputPort(*this));
}

void BaseModel::addOutputPort(const std::list < std::string > & lst)
{
    std::for_each(lst.begin(), lst.end(), AddOutputPort(*this));
}

const ModelPortList& BaseModel::getInPort(const std::string& name) const
{
    ConnectionList::const_iterator it = m_inPortList.find(name);
    if (it == m_inPortList.end()) {
        throw utils::DevsGraphError(fmt(
                _("Coupled model %1% have no input port %2%")) % getName() % name);
    }

    return it->second;
}

const ModelPortList& BaseModel::getOutPort(const std::string& name) const
{
    ConnectionList::const_iterator it = m_outPortList.find(name);
    if (it == m_outPortList.end()) {
        throw utils::DevsGraphError(fmt(
                _("Coupled model %1% have no output port %2%")) % getName() % name);
    }

    return it->second;
}

ModelPortList& BaseModel::getInPort(const std::string& name)
{
    ConnectionList::iterator it = m_inPortList.find(name);
    if (it == m_inPortList.end()) {
        throw utils::DevsGraphError(fmt(
	  _("Model %1% have no input port %2%")) % getName() % name);
    }

    return it->second;
}

ModelPortList& BaseModel::getOutPort(const std::string& name)
{
    ConnectionList::iterator it = m_outPortList.find(name);
    if (it == m_outPortList.end()) {
        throw utils::DevsGraphError(fmt(
                _("Model %1% have no output port %2%")) % getName() % name);
    }

    return it->second;
}

bool BaseModel::existInputPort(const std::string & name)
{
    return m_inPortList.find(name) != m_inPortList.end();
}

bool BaseModel::existOutputPort(const std::string & name)
{
    return m_outPortList.find(name) != m_outPortList.end();
}

int BaseModel::getInputPortIndex(const std::string& name) const
{
    ConnectionList::const_iterator it = m_inPortList.find(name);

    if (it == m_inPortList.end()) {
        throw utils::DevsGraphError(fmt(
                _("Input port %1% not exist in model %2%")) % name % getName());
    }

    return std::distance(m_inPortList.begin(), it);
}

int BaseModel::getOutputPortIndex(const std::string& name) const
{
    ConnectionList::const_iterator it = m_outPortList.find(name);

    if (it == m_outPortList.end()) {
        throw utils::DevsGraphError(fmt(
                _("Output port %1% not exist in model %2%")) % name %
            getName());
    }

    return std::distance(m_outPortList.begin(), it);
}

void BaseModel::writePortListXML(std::ostream& out) const
{
    if (not m_inPortList.empty()) {
        out << "<in>\n";

        ConnectionList::const_iterator it = m_inPortList.begin();
        for (;it != m_inPortList.end(); ++it) {
            out << "<port name=\"" << (*it).first.c_str() << "\" />\n";
        }
        out << "</in>\n";
    }

    if (not m_outPortList.empty()) {
        out << "<out>\n";
	ConnectionList::const_iterator it = m_outPortList.begin();
        for(; it != m_outPortList.end(); ++it) {
            out << "<port name=\"" << (*it).first.c_str() << "\" />\n";
	}
        out << "</out>\n";
    }
}

void BaseModel::writeGraphics(std::ostream& out) const
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

void BaseModel::writePort(std::ostream& out) const
{
    const vpz::ConnectionList& ins(getInputPortList());
    if (not ins.empty()) {
        out << "<in>\n";
        for (vpz::ConnectionList::const_iterator it = ins.begin();
             it != ins.end(); ++it) {
            out << " <port name=\"" << it->first.c_str() << "\" />\n";
        }
        out << "</in>\n";
    }

    const vpz::ConnectionList& outs(getOutputPortList());
    if (not outs.empty()) {
        out << "<out>\n";
        for (vpz::ConnectionList::const_iterator it = outs.begin();
             it != outs.end(); ++it) {
            out << " <port name=\"" << it->first.c_str() << "\" />\n";
        }
        out << "</out>\n";
    }

}

AtomicModel* BaseModel::toAtomic(BaseModel* model)
{
    if (model and model->isAtomic()) {
        return static_cast<AtomicModel*>(model);
    }
    return 0;
}

CoupledModel* BaseModel::toCoupled(BaseModel* model)
{
    if (model and model->isCoupled()) {
        return static_cast<CoupledModel*>(model);
    }
    return 0;
}

void BaseModel::getAtomicModelList(BaseModel* model,
                               AtomicModelVector& list)
{
    if (model->isAtomic()) {
        list.push_back((AtomicModel*)model);
    } else {
	std::list < CoupledModel* > coupledModelList;
	coupledModelList.push_back((CoupledModel*)model);
        while (!coupledModelList.empty()) {
            CoupledModel* m = coupledModelList.front();
            ModelList& v(m->getModelList());

            for (ModelList::iterator it = v.begin(); it != v.end(); ++it) {
		BaseModel* n = it->second;
                if (n->isAtomic()) {
                    list.push_back(static_cast < AtomicModel*>(n));
                } else {
                    coupledModelList.push_back(static_cast < CoupledModel*>(n));
                }
	    }
	    coupledModelList.pop_front();
	}
    }
}

bool BaseModel::isInList(const ModelList& lst, BaseModel* m)
{
    return lst.find(m->getName()) != lst.end();
}

BaseModel::BaseModel() :
    m_parent(0),
    m_x(-1),
    m_y(-1),
    m_width(-1),
    m_height(-1)
{
    throw utils::NotYetImplemented("BaseModel::BaseModel not developed");
}

BaseModel& BaseModel::operator=(const BaseModel& /* mdl */)
{
    throw utils::NotYetImplemented("BaseModel::operator= not developed");
}

}} // namespace vle vpz
