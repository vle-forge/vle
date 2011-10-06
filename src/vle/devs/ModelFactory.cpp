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
#include <vle/utils/Debug.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Algo.hpp>

namespace vle { namespace devs {

ModelFactory::ModelFactory(const vpz::Dynamics& dyn,
                           const vpz::Classes& cls,
                           const vpz::Experiment& exp,
                           const vpz::AtomicModelList& atom,
                           RootCoordinator& root)
    : mDynamics(dyn), mClasses(cls), mExperiment(exp), mAtoms(atom), mRoot(root)
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
            sim->addDynamics(attachDynamics(coordinator, sim, dyn, *getPlugin(dyn),
                                            initValues));
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

utils::ModuleCache::iterator ModelFactory::getPlugin(const vpz::Dynamic& dyn)
{
    std::string path;

    if (dyn.package().empty()) {
        if (utils::Package::package().selected()) {
            path = utils::Path::path().getPackageLibDir();
        } else {
            path = utils::Path::path().getSimulatorDir();
        }
    } else {
        path = utils::Path::path().getExternalPackageLibDir(dyn.package());
    }

    try {
        return utils::ModuleCache::instance().load(path, dyn.library());
    } catch(const std::exception& e) {
        std::string error;

        if (dyn.package().empty()) {
            if (utils::Package::package().selected()) {
                error = (fmt(_(
                            "Error opening simulator '%1%' from current "
                            "package: '%2%'")) % dyn.library() %
                    utils::Path::path().getPackageLibDir()).str();
            } else {
                error = (fmt(_(
                            "Error opening simulator '%1%' from global "
                            "directory:")) % dyn.library()).str();
            }
        } else {
            error = (fmt(_(
                        "Error opening simulator '%1%' from package '%2%':")) %
                dyn.library() % dyn.package()).str();
        }

        error += e.what();

        throw utils::ArgError(error);
    }
}

devs::Dynamics* ModelFactory::attachDynamics(Coordinator& coordinator,
                                             devs::Simulator* atom,
                                             const vpz::Dynamic& dyn,
                                             const utils::Module& module,
                                             const InitEventList& events)
{
    typedef Dynamics*(*fctdyn)(const DynamicsInit&, const InitEventList&);
    typedef Dynamics*(*fctexe)(const ExecutiveInit&, const InitEventList&);
    typedef Dynamics*(*fctdw)(const DynamicsWrapperInit&, const InitEventList&);

    std::string makeNewDynamics("makeNewDynamics");
    std::string makeNewExecutive("makeNewExecutive");
    std::string makeNewDynamicsWrapper("makeNewDynamicsWrapper");
    void* symbol = NULL;

    if (dyn.model().empty() or not dyn.language().empty()) {
        if ((symbol = module.get(makeNewDynamics))) {
            fctdyn fct = utils::pointer_to_function < fctdyn >(symbol);
            try {
                return fct(DynamicsInit(
                        *atom->getStructure(), mRoot.rand(),
                        utils::Package::package().getId(dyn.package())),
                    events);
            } catch(const std::exception& e) {
                throw utils::ModellingError(
                    fmt(_("Atomic model `%1%:%2%' (from library `%3%' dynamics"
                          " `%4%' package `%5%') throws error in constructor:"
                          " `%6%'")) % atom->getStructure()->getParentName() %
                    atom->getStructure()->getName() % module.path() %
                    dyn.name() % dyn.package() % e.what());
            }
        }

        if ((symbol = module.get(makeNewExecutive))) {
            fctexe fct = utils::pointer_to_function < fctexe >(symbol);
            try {
                return fct(ExecutiveInit(
                        *atom->getStructure(), mRoot.rand(),
                        utils::Package::package().getId(dyn.package()),
                        coordinator), events);
            } catch(const std::exception& e) {
                throw utils::ModellingError(
                    fmt(_("Executive model `%1%:%2%' (from library `%3%'"
                          " dynamics `%4%' package `%5%') throws error in"
                          " constructor: `%6%'")) %
                    atom->getStructure()->getParentName() %
                    atom->getStructure()->getName() % module.path() %
                    dyn.name() % dyn.package() % e.what());
            }
        }

        if ((symbol = module.get(makeNewDynamicsWrapper))) {
            fctdw fct = utils::pointer_to_function < fctdw >(symbol);
            try {
                return fct(DynamicsWrapperInit(
                        *atom->getStructure(), mRoot.rand(),
                        utils::Package::package().getId(dyn.package()),
                        dyn.library(), dyn.model()), events);
            } catch(const std::exception& e) {
                throw utils::ModellingError(
                    fmt(_("Atomic model wrapper `%1%:%2%' (from library `%3%'"
                          " dynamics `%4%' package `%5%') throws error in"
                          " constructor: `%6%'")) %
                    atom->getStructure()->getParentName() %
                    atom->getStructure()->getName() % module.path() %
                    dyn.name() % dyn.package() % e.what());
            }
        }

        throw utils::ModellingError(fmt(
                _("Dynamic library loading problem: cannot get any dynamics"
                  " executive or wrapper '%1%' in module '%2%'. Have you"
                  " used DECLARE_DYNAMICS, DECLARE_EXECUTIVE, "
                  " DECLARE_DYNAMICSWRAPPER ?")) % dyn.name() % module.path());
    } else {
        makeNewDynamics += dyn.model();
        if ((symbol = module.get(makeNewDynamics))) {
            fctdyn fct = utils::pointer_to_function < fctdyn >(symbol);
            try {
                return fct(DynamicsInit(
                        *atom->getStructure(), mRoot.rand(),
                        utils::Package::package().getId(dyn.package())),
                    events);
            } catch(const std::exception& e) {
                throw utils::ModellingError(
                    fmt(_("Atomic model `%1%:%2%' (from library `%3%' dynamics"
                          " `%4%' model `%5%' package `%6%') throws error in"
                          " constructor: `%7%'")) %
                    atom->getStructure()->getParentName() %
                    atom->getStructure()->getName() % module.path() %
                    dyn.name() % dyn.model() % dyn.package() % e.what());
            }
        }

        makeNewExecutive += dyn.model();
        if ((symbol = module.get(makeNewExecutive))) {
            fctexe fct = utils::pointer_to_function < fctexe >(symbol);
            try {
                return fct(ExecutiveInit(
                        *atom->getStructure(), mRoot.rand(),
                        utils::Package::package().getId(dyn.package()),
                        coordinator), events);
            } catch(const std::exception& e) {
                throw utils::ModellingError(
                    fmt(_("Executive model `%1%:%2%' (from library `%3%'"
                          " dynamics `%4%' model `%5%' package `%6%') throws"
                          " error in constructor: `%7%'")) %
                    atom->getStructure()->getParentName() %
                    atom->getStructure()->getName() % module.path() %
                    dyn.name() % dyn.model() % dyn.package() % e.what());
            }
        }

        makeNewDynamicsWrapper += dyn.model();
        if ((symbol = module.get(makeNewDynamicsWrapper))) {
            fctdw fct = utils::pointer_to_function < fctdw >(symbol);
            try {
                return fct(DynamicsWrapperInit(
                        *atom->getStructure(), mRoot.rand(),
                        utils::Package::package().getId(dyn.package()),
                        dyn.library(), dyn.model()), events);
            } catch(const std::exception& e) {
                throw utils::ModellingError(
                    fmt(_("Atomic model wrapper `%1%:%2%' (from library `%3%'"
                          " dynamics `%4%' model `%5%' package `%6%') throws"
                          " error in constructor: `%7%'")) %
                    atom->getStructure()->getParentName() %
                    atom->getStructure()->getName() % module.path() %
                    dyn.name() % dyn.model() % dyn.package() % e.what());
            }
        }

        throw utils::ModellingError(fmt(
                _("Dynamic library loading problem: cannot get any dynamics"
                  " executive or wrapper '%1%' in module '%2%'. Have you"
                  " used DECLARE_NAMED_DYNAMICS, DECLARE_NAMED_EXECUTIVE, "
                  " DECLARE_NAMED_DYNAMICSWRAPPER ?")) % dyn.name() %
            module.path());
    }
}

}} // namespace vle devs
