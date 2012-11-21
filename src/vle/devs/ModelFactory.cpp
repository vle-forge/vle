/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2012 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2012 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2012 INRA http://www.inra.fr
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


#include <vle/devs/ModelFactory.hpp>
#include <vle/devs/Coordinator.hpp>
#include <vle/devs/RootCoordinator.hpp>
#include <vle/devs/Simulator.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/devs/DynamicsWrapper.hpp>
#include <vle/devs/Executive.hpp>
#include <vle/vpz/BaseModel.hpp>
#include <vle/vpz/AtomicModel.hpp>
#include <vle/vpz/CoupledModel.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Algo.hpp>

namespace vle { namespace devs {

ModelFactory::ModelFactory(const utils::ModuleManager& modulemgr,
                           const vpz::Dynamics& dyn,
                           const vpz::Classes& cls,
                           const vpz::Experiment& exp,
                           RootCoordinator& root)
    : mModuleMgr(modulemgr), mDynamics(dyn), mClasses(cls), mExperiment(exp),
      mRoot(root)
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
                               vpz::AtomicModel* model,
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
                    initValues.value().clear();
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
    try {
        sim->addDynamics(attachDynamics(coordinator, sim, dyn, initValues));
    } catch(const std::exception& /*e*/) {
        initValues.value().clear();
        throw;
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
                    initValues.value().clear();
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
    vpz::AtomicModelVector atomicmodellist;
    vpz::BaseModel* mdl = model.model();

    if (mdl->isAtomic()) {
        atomicmodellist.push_back((vpz::AtomicModel*)mdl);
    } else {
        vpz::BaseModel::getAtomicModelList(mdl, atomicmodellist);
    }

    for (vpz::AtomicModelVector::iterator it = atomicmodellist.begin();
         it != atomicmodellist.end(); ++it) {
        createModel(coordinator,
                    *it,
                    (*it)->dynamics(),
                    (*it)->conditions(),
                    (*it)->observables());
    }

}

vpz::BaseModel* ModelFactory::createModelFromClass(Coordinator& coordinator,
                                                 vpz::CoupledModel* parent,
                                                 const std::string& classname,
                                                 const std::string& modelname)
{
    vpz::Class& classe(mClasses.get(classname));
    vpz::BaseModel* mdl(classe.model()->clone());
    vpz::AtomicModelVector atomicmodellist;
    vpz::BaseModel::getAtomicModelList(mdl, atomicmodellist);
    parent->addModel(mdl, modelname);

    for (vpz::AtomicModelVector::iterator it = atomicmodellist.begin();
         it != atomicmodellist.end(); ++it) {
        createModel(coordinator,
                    *it,
                    (*it)->dynamics(),
                    (*it)->conditions(),
                    (*it)->observables());
    }

    return mdl;
}

static devs::Dynamics* buildNewDynamicsWrapper(
    devs::Simulator* atom,
    const vpz::Dynamic& dyn,
    const InitEventList& events,
    void* symbol)
{
    typedef Dynamics*(*fctdw)(const DynamicsWrapperInit&, const InitEventList&);

    fctdw fct = utils::functionCast < fctdw >(symbol);

    try {
        return fct(DynamicsWrapperInit(
                *atom->getStructure(),
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
    const vpz::Dynamic& dyn,
    const InitEventList& events,
    void *symbol)
{
    typedef Dynamics*(*fctdyn)(const DynamicsInit&, const InitEventList&);

    fctdyn fct = utils::functionCast < fctdyn >(symbol);

    try {
        return fct(DynamicsInit(
                *atom->getStructure(),
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
    const vpz::Dynamic& dyn,
    const InitEventList& events,
    void *symbol)
{
    typedef Dynamics*(*fctexe)(const ExecutiveInit&, const InitEventList&);

    fctexe fct = utils::functionCast < fctexe >(symbol);

    try {
        return fct(ExecutiveInit(
                *atom->getStructure(),
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
    void *symbol = NULL;
    utils::ModuleType type = utils::MODULE_DYNAMICS;

    try {
        symbol = mModuleMgr.get(dyn.package(), dyn.library(),
                                utils::MODULE_DYNAMICS, &type);
    } catch (const std::exception& e) {
        throw utils::ModellingError(fmt(
                _("Dynamic library loading problem: cannot get any"
                  " dynamics, executive or wrapper '%1%' in library"
                  " '%2%' package '%3%'\n:%4%")) % dyn.name() %
            dyn.library() % dyn.package() % e.what());
    }

    switch (type) {
    case utils::MODULE_DYNAMICS:
        return buildNewDynamics(atom, dyn, events, symbol);
    case utils::MODULE_DYNAMICS_EXECUTIVE:
        return buildNewExecutive(coordinator, atom, dyn, events, symbol);
    case utils::MODULE_DYNAMICS_WRAPPER:
        return buildNewDynamicsWrapper(atom, dyn, events, symbol);
    default:
        throw utils::ModellingError();
    }
}

}} // namespace vle devs
