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


#include <vle/devs/Executive.hpp>
#include <vle/vpz/Vpz.hpp>

namespace vle { namespace devs {

void Executive::addObservableToView(const std::string& model,
                                    const std::string& portname,
                                    const std::string& view)
{
    vpz::BaseModel* mdl = cpled()->findModel(model);

    if (mdl == 0) {
        throw utils::DevsGraphError(fmt(
                _("Executive error: unknown model `%1%'")) % model);
    }

    if (mdl->isCoupled()) {
        throw utils::DevsGraphError(fmt(
                _("Executive error: can not add observable to coupled "
                  "model %1%")) % model);
    }

    vpz::AtomicModel* atom = mdl->toAtomic();

    m_coordinator.addObservableToView(atom, portname, view);
}

const vpz::AtomicModel*
Executive::createModel(const std::string& name,
                           const std::vector < std::string >& inputs,
                           const std::vector < std::string >& outputs,
                           const std::string& dynamics,
                           const std::vector < std::string >& conditions,
                           const std::string& observable)
{
    vpz::AtomicModel* model = new vpz::AtomicModel(name, cpled());
    std::vector < std::string >::const_iterator it;

    for (it = inputs.begin(); it != inputs.end(); ++it) {
        model->addInputPort(*it);
    }

    for (it = outputs.begin(); it != outputs.end(); ++it) {
        model->addOutputPort(*it);
    }

    m_coordinator.createModel(model, dynamics, conditions, observable);
    return model;
}

const vpz::BaseModel*
Executive::createModelFromClass(const std::string& classname,
                                const std::string& modelname)
{
    return m_coordinator.createModelFromClass(classname, cpled(), modelname);
}

void Executive::delModel(const std::string& modelname)
{
    std::vector < std::pair < Simulator*, std::string > > toupdate;

    vpz::BaseModel* mdl = cpled()->findModel(modelname);
    if (not mdl) {
        throw utils::DevsGraphError(fmt(
                _("Executive error: unknown model `%1%'")) %
            modelname);
    }

    m_coordinator.getSimulatorsSource(mdl, toupdate);

    m_coordinator.delModel(cpled(), modelname);

    m_coordinator.updateSimulatorsTarget(toupdate);
}

void Executive::renameModel(const std::string& oldname,
                            const std::string& newname)
{
    vpz::BaseModel* mdl = cpled()->findModel(oldname);
    if (mdl == 0) {
        throw utils::DevsGraphError(
            fmt(_("Executive error: rename `%1%' into `%2%' failed, `%1%' "
                  "does not exist")) % oldname % newname);
    }

    if (cpled()->exist(newname)) {
        throw utils::DevsGraphError(
            fmt(_("Executive error: rename `%1%' into `%2%' failed, `%2%' "
                  "already exists")) % oldname % newname);
    }

    try {
        vpz::BaseModel::rename(mdl, newname);
    } catch (const std::exception& e) {
        throw utils::DevsGraphError(
            fmt(_("Executive error: rename `%1%' into `%2%' failed: `%3%' ")) %
            oldname % newname % e.what());
    }
}

void Executive::addConnection(const std::string& srcModelName,
                              const std::string& srcPortName,
                              const std::string& dstModelName,
                              const std::string& dstPortName)
{
    const std::string& modelName(coupledmodelName());
    vpz::BaseModel* srcModel = (modelName == srcModelName)?
        cpled() : cpled()->findModel(srcModelName);
    vpz::BaseModel* dstModel = (modelName == dstModelName)?
        cpled() : cpled()->findModel(dstModelName);

    if (srcModel and dstModel) {
        std::vector < std::pair < Simulator*, std::string > > toupdate;

        if (modelName == srcModelName) {
            cpled()->addInputConnection(srcPortName, dstModel, dstPortName);
            m_coordinator.getSimulatorsSource(srcModel, srcPortName, toupdate);
        } else if (modelName == dstModelName) {
            cpled()->addOutputConnection(srcModel, srcPortName, dstPortName);
            vpz::ModelPortList lst;
            cpled()->getAtomicModelsTarget(dstPortName, lst);

            for (vpz::ModelPortList::iterator it = lst.begin(); it !=
                 lst.end(); ++it) {
                m_coordinator.getSimulatorsSource(it->first, it->second,
                                                  toupdate);
            }
        } else {
            cpled()->addInternalConnection(srcModel, srcPortName, dstModel,
                                           dstPortName);
            m_coordinator.getSimulatorsSource(dstModel, dstPortName, toupdate);
        }

        m_coordinator.updateSimulatorsTarget(toupdate);
    } else {
        throw utils::DevsGraphError(fmt(
                _("Executive error: cannot add connection (`%1%', `%2%') to "
                  "(`%3%', `%4%'")) % srcModelName % srcPortName % dstModelName
            % dstPortName);
    }
}

void Executive::removeConnection(const std::string& srcModelName,
                                 const std::string& srcPortName,
                                 const std::string& dstModelName,
                                 const std::string& dstPortName)
{
    const std::string& modelName(coupledmodelName());
    vpz::BaseModel* srcModel = cpled()->findModel(srcModelName);
    vpz::BaseModel* dstModel = cpled()->findModel(dstModelName);

    if (not srcModel and srcModelName == modelName) {
        srcModel = cpled();
    }

    if (not dstModel and dstModelName == modelName) {
        dstModel = cpled();
    }

    if (srcModel and dstModel) {
        std::vector < std::pair < Simulator*, std::string > > toupdate;
        if (cpled() == srcModel) {
            m_coordinator.getSimulatorsSource(dstModel, dstPortName, toupdate);
            cpled()->delInputConnection(srcPortName, dstModel, dstPortName);
        } else if (cpled() == dstModel) {
            cpled()->delOutputConnection(srcModel, srcPortName, dstPortName);
        } else {
            m_coordinator.getSimulatorsSource(dstModel, dstPortName, toupdate);
            cpled()->delInternalConnection(srcModel, srcPortName, dstModel,
                                           dstPortName);
        }

        m_coordinator.updateSimulatorsTarget(toupdate);
    } else {
        throw utils::DevsGraphError(fmt(
                _("Executive error: cannot remove connection (%1%, %2%) to "
                  "(`%3%', `%4%')")) % srcModelName % srcPortName %
            dstModelName % dstPortName);
    }
}

void Executive::addInputPort(const std::string& modelName,
                             const std::string& portName)
{
    vpz::BaseModel* mdl = cpled()->findModel(modelName);
    if (not mdl) {
        throw utils::DevsGraphError(fmt(
                _("Executive error: unknown model `%1%'")) %
            modelName);
    }

    mdl->addInputPort(portName);
}

void Executive::addOutputPort(const std::string& modelName,
                              const std::string& portName)
{
    vpz::BaseModel* mdl = cpled()->findModel(modelName);
    if (not mdl) {
        throw utils::DevsGraphError(fmt(
                _("Executive error: unknown model `%1%'")) %
            modelName);
    }

    mdl->addOutputPort(portName);
}

void Executive::removeInputPort(const std::string& modelName,
                                const std::string& portName)
{
    vpz::BaseModel* mdl = cpled()->findModel(modelName);
    if (not mdl) {
        throw utils::DevsGraphError(fmt(
                _("Executive error: unknown model `%1%'")) %
            modelName);
    }

    std::vector < std::pair < Simulator*, std::string > > toupdate;
    m_coordinator.getSimulatorsSource(mdl, portName, toupdate);
    mdl->delInputPort(portName);
    m_coordinator.updateSimulatorsTarget(toupdate);
}

void Executive::removeOutputPort(const std::string& modelName,
                                 const std::string& portName)
{
    vpz::BaseModel* mdl = cpled()->findModel(modelName);
    if (not mdl) {
        throw utils::DevsGraphError(fmt(
                _("Executive error: unknown model `%1%'")) %
            modelName);
    }

    if (mdl->isAtomic()) {
        mdl->delOutputPort(portName);
        m_coordinator.removeSimulatorTargetPort(mdl->toAtomic(), portName);
    } else {
        std::vector < std::pair < Simulator*, std::string > > toupdate;
        m_coordinator.getSimulatorsSource(mdl, portName, toupdate);
        mdl->delOutputPort(portName);
        m_coordinator.updateSimulatorsTarget(toupdate);
    }
}

void Executive::dump(std::ostream& out, const std::string& name) const
{
    vpz::Vpz f;

    f.project().setAuthor(getModelName());
    f.project().model().setModel(coupledmodel().clone());
    f.project().dynamics().add(m_coordinator.dynamics());
    f.project().experiment().addConditions(m_coordinator.conditions());
    f.project().experiment().setName(name);
    f.project().experiment().setBegin(0.0);
    f.project().experiment().setDuration(1.0);

    if (not f.project().dynamics().exist("unknow")) {
        f.project().dynamics().add(vpz::Dynamic("unknow"));
    }

    // std::vector < vpz::AtomicModel* > lst;
    // vpz::BaseModel::getAtomicModelList(f.project().model().model(), lst);

    // for (std::vector < vpz::AtomicModel* >::iterator it = lst.begin();
    //      it != lst.end(); ++it) {
    //     f.project().model().atomicModels().add(
    //         *it, vpz::AtomicModelTemp("", "unknow", ""));
    // }

    f.write(out);

    delete f.project().model().model();
    f.project().model().setModel(0);
}

}} // namespace vle devs
