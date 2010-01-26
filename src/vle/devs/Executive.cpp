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
    graph::AtomicModel* model = new graph::AtomicModel(name, coupledmodel());
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
    return m_coordinator.createModelFromClass(classname, coupledmodel(),
                                              modelname);
}

void Executive::delModel(const std::string& modelname)
{
    std::vector < std::pair < Simulator*, std::string > > toupdate;

    graph::Model* mdl = coupledmodel()->findModel(modelname);
    if (not mdl) {
        throw utils::DevsGraphError(fmt(
                _("Executive error: unknown model `%1%'")) %
            modelname);
    }

    m_coordinator.getSimulatorsSource(mdl, toupdate);

    m_coordinator.delModel(coupledmodel(), modelname);

    m_coordinator.updateSimulatorsTarget(toupdate);
}

void Executive::addConnection(const std::string& srcModelName,
                              const std::string& srcPortName,
                              const std::string& dstModelName,
                              const std::string& dstPortName)
{
    const std::string& modelName(coupledmodelName());
    graph::Model* srcModel = (modelName == srcModelName)?
        coupledmodel() : coupledmodel()->findModel(srcModelName);
    graph::Model* dstModel = (modelName == dstModelName)?
        coupledmodel() : coupledmodel()->findModel(dstModelName);

    if (srcModel and dstModel) {
        if (modelName == srcModelName) {
            coupledmodel()->addInputConnection(srcPortName,
                                               dstModel, dstPortName);
        } else if (modelName == dstModelName) {
            coupledmodel()->addOutputConnection(srcModel, srcPortName,
                                                dstPortName);
        } else {
            coupledmodel()->addInternalConnection(srcModel, srcPortName,
                                                  dstModel, dstPortName);
        }

        std::vector < std::pair < Simulator*, std::string > > toupdate;
        m_coordinator.getSimulatorsSource(dstModel, dstPortName, toupdate);

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
        coupledmodel() : coupledmodel()->findModel(srcModelName);
    graph::Model* dstModel = (modelName == dstModelName) ?
        coupledmodel() : coupledmodel()->findModel(dstModelName);

    if (srcModel and dstModel) {
        std::vector < std::pair < Simulator*, std::string > > toupdate;
        m_coordinator.getSimulatorsSource(dstModel, dstPortName, toupdate);

        if (modelName == srcModelName) {
            coupledmodel()->delInputConnection(srcPortName, dstModel,
                                               dstPortName);
        } else if (modelName == dstModelName) {
            coupledmodel()->delOutputConnection(srcModel, srcPortName,
                                                dstPortName);
        } else {
            coupledmodel()->delInternalConnection(srcModel, srcPortName,
                                                  dstModel, dstPortName);
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
    graph::Model* mdl = coupledmodel()->findModel(modelName);
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
    graph::Model* mdl = coupledmodel()->findModel(modelName);
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
    graph::Model* mdl = coupledmodel()->findModel(modelName);
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
    graph::Model* mdl = coupledmodel()->findModel(modelName);
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

}} // namespace vle devs
