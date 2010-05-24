/**
 * @file vle/devs/Executive.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
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


#include <vle/devs/Executive.hpp>

namespace vle { namespace devs {

const graph::AtomicModel*
Executive::createModel(const std::string& name,
                           const std::vector < std::string >& inputs,
                           const std::vector < std::string >& outputs,
                           const std::string& dynamics,
                           const vpz::Strings& conditions,
                           const std::string& observable)
{
    graph::AtomicModel* model = new graph::AtomicModel(name, cpled());
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

const graph::Model*
Executive::createModelFromClass(const std::string& classname,
                                const std::string& modelname)
{
    return m_coordinator.createModelFromClass(classname, cpled(), modelname);
}

void Executive::delModel(const std::string& modelname)
{
    std::vector < std::pair < Simulator*, std::string > > toupdate;

    graph::Model* mdl = cpled()->findModel(modelname);
    if (not mdl) {
        throw utils::DevsGraphError(fmt(
                _("Executive error: unknown model `%1%'")) %
            modelname);
    }

    m_coordinator.getSimulatorsSource(mdl, toupdate);

    m_coordinator.delModel(cpled(), modelname);

    m_coordinator.updateSimulatorsTarget(toupdate);
}

void Executive::addConnection(const std::string& srcModelName,
                              const std::string& srcPortName,
                              const std::string& dstModelName,
                              const std::string& dstPortName)
{
    const std::string& modelName(coupledmodelName());
    graph::Model* srcModel = (modelName == srcModelName)?
        cpled() : cpled()->findModel(srcModelName);
    graph::Model* dstModel = (modelName == dstModelName)?
        cpled() : cpled()->findModel(dstModelName);

    if (srcModel and dstModel) {
        std::vector < std::pair < Simulator*, std::string > > toupdate;

        if (modelName == srcModelName) {
            cpled()->addInputConnection(srcPortName, dstModel, dstPortName);
            m_coordinator.getSimulatorsSource(srcModel, srcPortName, toupdate);
        } else if (modelName == dstModelName) {
            cpled()->addOutputConnection(srcModel, srcPortName, dstPortName);
            graph::ModelPortList lst;
            cpled()->getAtomicModelsTarget(dstPortName, lst);

            for (graph::ModelPortList::iterator it = lst.begin(); it !=
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
    graph::Model* srcModel = (modelName == srcModelName) ?
        cpled() : cpled()->findModel(srcModelName);
    graph::Model* dstModel = (modelName == dstModelName) ?
        cpled() : cpled()->findModel(dstModelName);

    if (srcModel and dstModel) {
        std::vector < std::pair < Simulator*, std::string > > toupdate;
        m_coordinator.getSimulatorsSource(dstModel, dstPortName, toupdate);

        if (modelName == srcModelName) {
            cpled()->delInputConnection(srcPortName, dstModel, dstPortName);
        } else if (modelName == dstModelName) {
            cpled()->delOutputConnection(srcModel, srcPortName, dstPortName);
        } else {
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
    graph::Model* mdl = cpled()->findModel(modelName);
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
    graph::Model* mdl = cpled()->findModel(modelName);
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
    graph::Model* mdl = cpled()->findModel(modelName);
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
    graph::Model* mdl = cpled()->findModel(modelName);
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

    std::vector < graph::AtomicModel* > lst;
    graph::Model::getAtomicModelList(f.project().model().model(), lst);

    for (std::vector < graph::AtomicModel* >::iterator it = lst.begin();
         it != lst.end(); ++it) {
        f.project().model().atomicModels().add(
            *it, vpz::AtomicModel("", "unknow", ""));
    }

    f.write(out);

    delete f.project().model().model();
    f.project().model().setModel(0);
}

}} // namespace vle devs
