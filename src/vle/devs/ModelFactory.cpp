/*
 * @file vle/devs/ModelFactory.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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


#include <vle/devs/ModelFactory.hpp>
#include <vle/devs/Coordinator.hpp>
#include <vle/devs/RootCoordinator.hpp>
#include <vle/devs/Simulator.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/devs/DynamicsWrapper.hpp>
#include <vle/devs/Executive.hpp>
#include <vle/graph/Model.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Algo.hpp>

namespace vle { namespace devs {

ModelFactory::ModelFactory(const utils::ModuleManager& modulemgr,
                           const vpz::Dynamics& dyn,
                           const vpz::Classes& cls,
                           const vpz::Experiment& exp,
                           const vpz::AtomicModelList& atom,
                           RootCoordinator& root)
    : mModuleMgr(modulemgr), mDynamics(dyn), mClasses(cls), mExperiment(exp),
    mAtoms(atom), mRoot(root)
{
}

void ModelFactory::cleanCache()
{
    mDynamics.cleanNoPermanent();
    mExperiment.cleanNoPermanent();
}

void ModelFactory::addPermanent(const vpz::Dynamic& dynamics)
{
    try {
        mDynamics.add(dynamics);
    } catch(const std::exception& e) {
        throw utils::InternalError(fmt(_(
            "Model factory cannot add dynamics %1%: %2%")) % dynamics.name() %
            e.what());
    }
}

void ModelFactory::addPermanent(const vpz::Condition& condition)
{
    try {
        vpz::Conditions& conds(mExperiment.conditions());
        conds.add(condition);
    } catch(const std::exception& e) {
        throw utils::InternalError(fmt(_(
                "Model factory cannot add condition %1%: %2%")) %
            condition.name() % e.what());
    }
}

void ModelFactory::addPermanent(const vpz::Observable& observable)
{
    try {
        vpz::Views& views(mExperiment.views());
        views.addObservable(observable);
    } catch(const std::exception& e) {
        throw utils::InternalError(fmt(_(
                "Model factory cannot add observable %1%: %2%")) %
            observable.name() % e.what());
    }
}

void ModelFactory::createModel(Coordinator& coordinator,
                               graph::AtomicModel* model,
                               const std::string& dynamics,
                               const std::vector < std::string >& conditions,
                               const std::string& observable)
{
    const SimulatorMap& result(coordinator.modellist());
    if (result.find(model) != result.end()) {
        throw utils::InternalError(fmt(_(
                "The model '%1%' already exist in coordinator")) %
            model->getName());
    }

    Simulator* sim = new Simulator(model);
    coordinator.addModel(model, sim);

    value::Map initValues;
    if (not conditions.empty()) {
        for (std::vector < std::string >::const_iterator it =
             conditions.begin(); it != conditions.end(); ++it) {
	    const vpz::Condition& cnd(mExperiment.conditions().get(*it));
	    value::MapValue vl;
	    cnd.fillWithFirstValues(vl);

	    for (value::MapValue::const_iterator itv = vl.begin();
		 itv != vl.end(); ++itv) {

                if (initValues.exist(itv->first)) {
                    throw utils::InternalError(fmt(_(
                            "Multiples condition with the same init port " \
                            "name '%1%'")) % itv->first);
                }
                initValues.add(itv->first, itv->second);
            }

            vl.clear();
	}
    }

    const vpz::Dynamic& dyn = mDynamics.get(dynamics);
    switch(dyn.type()) {
    case vpz::Dynamic::LOCAL:
        try {
            sim->addDynamics(attachDynamics(coordinator, sim, dyn, initValues));
        } catch(const std::exception& /*e*/) {
            initValues.value().clear();
            throw;
        }
        break;
    case vpz::Dynamic::DISTANT:
        throw utils::NotYetImplemented(_("Distant dynamics is not supported"));
    }

    if (not observable.empty()) {
        vpz::Observable& ob(mExperiment.views().observables().get(observable));
        const vpz::ObservablePortList& lst(ob.observableportlist());

        for (vpz::ObservablePortList::const_iterator it = lst.begin();
             it != lst.end(); ++it) {
            const vpz::ViewNameList& vnlst(it->second.viewnamelist());
            for (vpz::ViewNameList::const_iterator jt = vnlst.begin();
                 jt != vnlst.end(); ++jt) {

                View* view = coordinator.getView(*jt);

                if (not view) {
                    throw utils::InternalError(fmt(_(
                                "The view '%1%' is unknow of coordinator "
                                "view list")) % *jt);
                }

                view->addObservable(sim, it->first,
                                    coordinator.getCurrentTime());
            }
        }
    }

    initValues.value().clear();

    InternalEvent* evt = sim->init(coordinator.getCurrentTime());
    if (evt) {
        coordinator.eventtable().putInternalEvent(evt);
    }
}

void ModelFactory::createModels(Coordinator& coordinator,
                                const vpz::Model& model)
{
    graph::AtomicModelVector atomicmodellist;
    graph::Model* mdl = model.model();

    if (mdl->isAtomic()) {
        atomicmodellist.push_back((graph::AtomicModel*)mdl);
    } else {
        graph::Model::getAtomicModelList(mdl, atomicmodellist);
    }

    const vpz::AtomicModelList& atoms(model.atomicModels());
    for (graph::AtomicModelVector::iterator it = atomicmodellist.begin();
         it != atomicmodellist.end(); ++it) {
        const vpz::AtomicModel& atom(atoms.get(*it));
        createModel(coordinator, *it, atom.dynamics(), atom.conditions(),
                    atom.observables());
    }

}

graph::Model* ModelFactory::createModelFromClass(Coordinator& coordinator,
                                                 graph::CoupledModel* parent,
                                                 const std::string& classname,
                                                 const std::string& modelname)
{
    vpz::Class& classe(mClasses.get(classname));
    graph::Model* mdl(classe.model()->clone());
    vpz::AtomicModelList outlist;

    std::for_each(classe.atomicModels().begin(), classe.atomicModels().end(),
                  vpz::CopyAtomicModel(outlist, *mdl));

    graph::AtomicModelVector atomicmodellist;
    graph::Model::getAtomicModelList(mdl, atomicmodellist);
    parent->addModel(mdl, modelname);

    for (graph::AtomicModelVector::iterator it = atomicmodellist.begin();
         it != atomicmodellist.end(); ++it) {
        vpz::AtomicModel& atominfo(outlist.get(*it));
        createModel(coordinator,
                    static_cast < graph::AtomicModel* >(*it),
                    atominfo.dynamics(),
                    atominfo.conditions(), atominfo.observables());
    }

    return mdl;
}

static devs::Dynamics* buildNewDynamicsWrapper(
    devs::Simulator* atom,
    utils::Rand& rnd,
    const vpz::Dynamic& dyn,
    const InitEventList& events,
    const utils::ModuleManager& modulemgr)
{
    typedef Dynamics*(*fctdw)(const DynamicsWrapperInit&, const InitEventList&);

    void* symbol;

    symbol = modulemgr.get(dyn.package(), dyn.library(),
                           utils::MODULE_DYNAMICS_WRAPPER);

    fctdw fct = utils::functionCast < fctdw >(symbol);

    try {
        return fct(DynamicsWrapperInit(
                *atom->getStructure(), rnd,
                utils::Package::package().getId(dyn.package()),
                dyn.library()), events);
    } catch(const std::exception& e) {
        throw utils::ModellingError(
            fmt(_("Atomic model wrapper `%1%:%2%' (from dynamics `%3%'"
                  " library `%4%' package `%5%') throws error in"
                  " constructor: `%6%'")) %
            atom->getStructure()->getParentName() %
            atom->getStructure()->getName() %
            dyn.name() % dyn.library() % dyn.package() % e.what());
    }
}

static devs::Dynamics* buildNewDynamics(
    devs::Simulator* atom,
    utils::Rand& rnd,
    const vpz::Dynamic& dyn,
    const InitEventList& events,
    const utils::ModuleManager& modulemgr)
{
    typedef Dynamics*(*fctdyn)(const DynamicsInit&, const InitEventList&);

    void *symbol;

    symbol = modulemgr.get(dyn.package(), dyn.library(),
                           utils::MODULE_DYNAMICS);

    fctdyn fct = utils::functionCast < fctdyn >(symbol);

    try {
        return fct(DynamicsInit(
                *atom->getStructure(), rnd,
                utils::Package::package().getId(dyn.package())),
            events);
    } catch(const std::exception& e) {
        throw utils::ModellingError(
            fmt(_("Atomic model `%1%:%2%' (from dynamics `%3%' library"
                  " `%4%' package `%5%') throws error in constructor:"
                  " `%6%'")) % atom->getStructure()->getParentName() %
            atom->getStructure()->getName() % dyn.name() % dyn.library() %
            dyn.package() % e.what());
    }
}

static devs::Dynamics* buildNewExecutive(
    Coordinator& coordinator,
    devs::Simulator* atom,
    utils::Rand& rnd,
    const vpz::Dynamic& dyn,
    const InitEventList& events,
    const utils::ModuleManager& modulemgr)
{
    typedef Dynamics*(*fctexe)(const ExecutiveInit&, const InitEventList&);

    void *symbol;

    symbol = modulemgr.get(dyn.package(), dyn.library(),
                           utils::MODULE_EXECUTIVE);

    fctexe fct = utils::functionCast < fctexe >(symbol);

    try {
        return fct(ExecutiveInit(
                *atom->getStructure(), rnd,
                utils::Package::package().getId(dyn.package()),
                coordinator), events);
    } catch(const std::exception& e) {
        throw utils::ModellingError(
            fmt(_("Executive model `%1%:%2%' (from dynamics `%3%'"
                  " library `%4%' package `%5%') throws error in"
                  " constructor: `%6%'")) %
            atom->getStructure()->getParentName() %
            atom->getStructure()->getName() % dyn.name() % dyn.library() %
            dyn.package() % e.what());
    }
}


devs::Dynamics* ModelFactory::attachDynamics(Coordinator& coordinator,
                                             devs::Simulator* atom,
                                             const vpz::Dynamic& dyn,
                                             const InitEventList& events)
{
    utils::ModuleType type;

    type = mModuleMgr.determine(dyn.package(), dyn.library(),
                                utils::MODULE_DYNAMICS);

    switch (type) {
        case utils::MODULE_DYNAMICS_WRAPPER:
            return buildNewDynamicsWrapper(atom, mRoot.rand(), dyn, events,
                                           mModuleMgr);
        case utils::MODULE_DYNAMICS:
            return buildNewDynamics(atom, mRoot.rand(), dyn, events,
                                    mModuleMgr);
        case utils::MODULE_EXECUTIVE:
            return buildNewExecutive(coordinator, atom, mRoot.rand(), dyn,
                                     events, mModuleMgr);
        default:
            throw utils::ModellingError(fmt(
                    _("Dynamic library loading problem: cannot get any dynamics"
                      " executive or wrapper '%1%' in library '%2%' package"
                      " '%3%'. Have you used DECLARE_DYNAMICS,"
                      " DECLARE_EXECUTIVE, DECLARE_DYNAMICSWRAPPER ?")) %
                dyn.name() % dyn.library() % dyn.package());
    }
}

}} // namespace vle devs
