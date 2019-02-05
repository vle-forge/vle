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

#include <vle/devs/Dynamics.hpp>
#include <vle/devs/DynamicsWrapper.hpp>
#include <vle/devs/Executive.hpp>
#include <vle/utils/Algo.hpp>
#include <vle/utils/Package.hpp>
#include <vle/vpz/AtomicModel.hpp>
#include <vle/vpz/BaseModel.hpp>
#include <vle/vpz/CoupledModel.hpp>

#include "devs/Coordinator.hpp"
#include "devs/DynamicsDbg.hpp"
#include "devs/DynamicsInit.hpp"
#include "devs/DynamicsObserver.hpp"
#include "devs/ModelFactory.hpp"
#include "devs/RootCoordinator.hpp"
#include "devs/Simulator.hpp"
#include "utils/i18n.hpp"

#include <utility>

namespace vle {
namespace devs {

ModelFactory::ModelFactory(utils::ContextPtr context,
                           std::map<std::string, View>& eventviews,
                           const vpz::Dynamics& dyn,
                           const vpz::Classes& cls,
                           const vpz::Experiment& exp)
  : mContext(std::move(context))
  , mEventViews(eventviews)
  , mDynamics(dyn)
  , mClasses(cls)
  , mExperiment(exp)
{}

void
ModelFactory::createModel(Coordinator& coordinator,
                          const vpz::Conditions& experiment_conditions,
                          vpz::AtomicModel* model,
                          const std::string& dynamics,
                          const std::vector<std::string>& conditions,
                          const std::string& observable)
{
    const vpz::Dynamic& dyn = mDynamics.get(dynamics);
    auto sim = coordinator.addModel(model);

    InitEventList initValues;

    if (not conditions.empty()) {
        for (const auto& elem : conditions) {
            const auto& cnd = experiment_conditions.get(elem);
            auto vl = cnd.fillWithFirstValues();

            for (auto& elem : vl) {
                if (initValues.exist(elem.first))
                    throw utils::InternalError(
                      _("Multiples condition with the same init port "
                        "name '%s'"),
                      elem.first.c_str());

                initValues.add(elem.first, elem.second);
            }
        }
    }

    sim->addDynamics(
      attachDynamics(coordinator, sim, dyn, initValues, observable));

    if (not observable.empty()) {
        vpz::Observable& ob(mExperiment.views().observables().get(observable));
        const vpz::ObservablePortList& lst(ob.observableportlist());

        for (const auto& elem : lst) {
            const vpz::ViewNameList& vnlst(elem.second.viewnamelist());
            for (const auto& viewname : vnlst)
                coordinator.addObservableToView(model, elem.first, viewname);
        }
    }

    coordinator.processInit(sim);
}

void
ModelFactory::createModels(Coordinator& coordinator, const vpz::Model& model)
{
    vpz::AtomicModelVector atomicmodellist;
    vpz::BaseModel* mdl = model.node();

    if (mdl) {
        if (mdl->isAtomic()) {
            atomicmodellist.push_back(static_cast<vpz::AtomicModel*>(mdl));
        } else {
            vpz::BaseModel::getAtomicModelList(mdl, atomicmodellist);
        }

        for (auto& elem : atomicmodellist) {
            createModel(coordinator,
                        mExperiment.conditions(),
                        elem,
                        (elem)->dynamics(),
                        (elem)->conditions(),
                        (elem)->observables());
        }
    }
}

vpz::BaseModel*
ModelFactory::createModelFromClass(Coordinator& coordinator,
                                   vpz::CoupledModel* parent,
                                   const std::string& classname,
                                   const std::string& modelname,
                                   const vpz::Conditions& conditions,
                                   const std::vector<std::string>& inputs,
                                   const std::vector<std::string>& outputs)
{
    vpz::Class& classe(mClasses.get(classname));
    vpz::BaseModel* mdl(classe.node()->clone());

    for (const auto& elem : inputs)
        mdl->addInputPort(elem);

    for (const auto& elem : outputs)
        mdl->addOutputPort(elem);

    vpz::AtomicModelVector atomicmodellist;
    vpz::BaseModel::getAtomicModelList(mdl, atomicmodellist);
    parent->addModel(mdl, modelname);

    for (auto& elem : atomicmodellist) {
        createModel(coordinator,
                    conditions,
                    elem,
                    (elem)->dynamics(),
                    (elem)->conditions(),
                    (elem)->observables());
    }

    return mdl;
}

std::unique_ptr<Dynamics>
buildNewDynamicsWrapper(utils::ContextPtr context,
                        devs::Simulator* atom,
                        const vpz::Dynamic& dyn,
                        const InitEventList& events,
                        void* symbol)
{
    using fctdw =
      vle::devs::Dynamics* (*)(const vle::devs::DynamicsWrapperInit&,
                               const vle::devs::InitEventList&);

    auto fct = utils::functionCast<fctdw>(symbol);

    try {
        utils::PackageTable pkg_table;

        return std::unique_ptr<Dynamics>(
          fct(DynamicsWrapperInit{ dyn.library(),
                                   context,
                                   *atom->getStructure(),
                                   pkg_table.get(dyn.package()) },
              events));
    } catch (const std::exception& e) {
        throw utils::ModellingError(
          _("Atomic model wrapper `%s:%s' (from dynamics `%s'"
            " library `%s' package `%s') throws error in"
            " constructor: `%s'"),
          atom->getStructure()->getParentName().c_str(),
          atom->getStructure()->getName().c_str(),
          dyn.name().c_str(),
          dyn.library().c_str(),
          dyn.package().c_str(),
          e.what());
    }
}

//
// Compute the dynamics observer options from the observable. If observable is
// empty or equal to Tined, the model can be observed with a TimedView
// otherwise, we compute which functions to observe.
//
bool
haveEventView(const vpz::Views& views, const std::string& observable)
{
    if (observable.empty())
        return false;

    const auto& obs(views.observables().get(observable));
    const auto& lst(obs.observableportlist());

    for (const auto& elem : lst) {
        const auto& viewnamelist(elem.second.viewnamelist());
        for (const auto& viewname : viewnamelist) {
            const auto& view = views.get(viewname);
            if (view.is_enable() and not(view.type() & vpz::View::TIMED))
                return true;
        }
    }

    return false;
}

void
assignEventView(std::map<std::string, View>& views,
                const vpz::Views& vpzviews,
                const std::string& observable,
                const std::unique_ptr<DynamicsObserver>& dynamics)
{
    const auto& obs(vpzviews.observables().get(observable));
    const auto& lst(obs.observableportlist());

    for (const auto& elem : lst) {
        const auto& viewnamelist(elem.second.viewnamelist());
        for (const auto& viewname : viewnamelist) {
            const auto& v = vpzviews.get(viewname);

            if (v.type() == vpz::View::TIMED)
                continue;

            auto it = views.find(viewname);
            if (it == views.end())
                continue;

            auto& vv = it->second;
            if (v.type() & vpz::View::OUTPUT)
                dynamics->ppOutput.emplace_back(&vv, elem.first);

            if (v.type() & vpz::View::INTERNAL)
                dynamics->ppInternal.emplace_back(&vv, elem.first);

            if (v.type() & vpz::View::EXTERNAL)
                dynamics->ppExternal.emplace_back(&vv, elem.first);

            if (v.type() & vpz::View::CONFLUENT)
                dynamics->ppConfluent.emplace_back(&vv, elem.first);

            if (v.type() & vpz::View::FINISH)
                dynamics->ppFinish.emplace_back(&vv, elem.first);
        }
    }
}

std::unique_ptr<Dynamics>
buildNewDynamics(utils::ContextPtr context,
                 std::map<std::string, View>& views,
                 const vpz::Views& vpzviews,
                 const std::string& observable,
                 devs::Simulator* atom,
                 const vpz::Dynamic& dyn,
                 const InitEventList& events,
                 void* symbol)
{
    using fctdyn = vle::devs::Dynamics* (*)(const vle::devs::DynamicsInit&,
                                            const vle::devs::InitEventList&);

    auto fct = utils::functionCast<fctdyn>(symbol);

    try {
        utils::PackageTable pkg_table;

        DynamicsInit init{ context,
                           *atom->getStructure(),
                           pkg_table.get(dyn.package()) };
        auto dynamics = std::unique_ptr<Dynamics>(fct(init, events));

        if (haveEventView(vpzviews, observable)) {
            auto observation = std::make_unique<DynamicsObserver>(
              init, events, atom->getObservations());

            if (atom->getStructure()->needDebug()) {
                auto debug = std::make_unique<DynamicsDbg>(init, events);
                debug->set(std::move(dynamics));
                observation->set(std::move(debug));
            } else {
                observation->set(std::move(dynamics));
            }

            assignEventView(views, vpzviews, observable, observation);

            return std::move(observation);
        }

        if (atom->getStructure()->needDebug()) {
            auto debug = std::make_unique<DynamicsDbg>(init, events);
            debug->set(std::move(dynamics));
            return std::move(debug);
        } else {
            return dynamics;
        }
    } catch (const std::exception& e) {
        throw utils::ModellingError(
          _("Atomic model `%s:%s' (from dynamics `%s' library"
            " `%s' package `%s') throws error in constructor:"
            " `%s'"),
          atom->getStructure()->getParentName().c_str(),
          atom->getStructure()->getName().c_str(),
          dyn.name().c_str(),
          dyn.library().c_str(),
          dyn.package().c_str(),
          e.what());
    }
}

std::unique_ptr<Dynamics>
buildNewExecutive(utils::ContextPtr context,
                  std::map<std::string, View>& views,
                  const vpz::Views& vpzviews,
                  const std::string& observable,
                  Coordinator& coordinator,
                  devs::Simulator* atom,
                  const vpz::Dynamic& dyn,
                  const InitEventList& events,
                  void* symbol)
{
    using fctexe = vle::devs::Dynamics* (*)(const vle::devs::ExecutiveInit&,
                                            const vle::devs::InitEventList&);

    auto fct = utils::functionCast<fctexe>(symbol);

    try {
        utils::PackageTable pkg_table;

        ExecutiveInit executiveinit{ coordinator,
                                     context,
                                     *atom->getStructure(),
                                     pkg_table.get(dyn.package()) };

        DynamicsInit init{ context,
                           *atom->getStructure(),
                           pkg_table.get(dyn.package()) };

        auto executive = std::unique_ptr<Dynamics>(fct(executiveinit, events));

        if (haveEventView(vpzviews, observable)) {
            auto observation = std::make_unique<DynamicsObserver>(
              init, events, atom->getObservations());

            if (atom->getStructure()->needDebug()) {
                auto debug = std::make_unique<DynamicsDbg>(init, events);
                debug->set(std::move(executive));
                observation->set(std::move(debug));
            } else {
                observation->set(std::move(executive));
            }

            assignEventView(views, vpzviews, observable, observation);

            return std::move(observation);
        }

        if (atom->getStructure()->needDebug()) {
            auto debug = std::make_unique<DynamicsDbg>(init, events);
            debug->set(std::move(executive));
            return std::move(debug);
        } else {
            return executive;
        }
    } catch (const std::exception& e) {
        throw utils::ModellingError(
          _("Executive model `%s:%s' (from dynamics `%s'"
            " library `%s' package `%s') throws error in"
            " constructor: `%s'"),
          atom->getStructure()->getParentName().c_str(),
          atom->getStructure()->getName().c_str(),
          dyn.name().c_str(),
          dyn.library().c_str(),
          dyn.package().c_str(),
          e.what());
    }
}

std::unique_ptr<Dynamics>
ModelFactory::attachDynamics(Coordinator& coordinator,
                             devs::Simulator* atom,
                             const vpz::Dynamic& dyn,
                             const InitEventList& events,
                             const std::string& observable)
{
    void* symbol = nullptr;
    auto type = utils::Context::ModuleType::MODULE_DYNAMICS;

    try {
        /* If \e package is not empty we assume that library is the shared
         * library. Otherwise, we load the global symbol stores in \e
         * library/executable and we cast it into a \e
         * vle::devs::Dynamics... Only useful for unit test or to build
         * executable with dynamics.
         */
        if (not dyn.package().empty()) {
            symbol =
              mContext->get_symbol(dyn.package(),
                                   dyn.library(),
                                   utils::Context::ModuleType::MODULE_DYNAMICS,
                                   &type);
        } else {
            symbol = mContext->get_symbol(dyn.library());

            if (dyn.library().length() >= 4) {
                if (dyn.library().compare(0, 4, "exe_") == 0)
                    type =
                      utils::Context::ModuleType::MODULE_DYNAMICS_EXECUTIVE;
                else if (dyn.library().compare(0, 4, "wra_") == 0)
                    type = utils::Context::ModuleType::MODULE_DYNAMICS_WRAPPER;
            }
        }
    } catch (const std::exception& e) {
        throw utils::ModellingError(
          _("Dynamic library loading problem: cannot get any"
            " dynamics, executive or wrapper '%s' in library"
            " '%s' package '%s'\n:%s"),
          dyn.name().c_str(),
          dyn.library().c_str(),
          dyn.package().c_str(),
          e.what());
    }

    switch (type) {
    case utils::Context::ModuleType::MODULE_DYNAMICS:
        return buildNewDynamics(mContext,
                                mEventViews,
                                mExperiment.views(),
                                observable,
                                atom,
                                dyn,
                                events,
                                symbol);
    case utils::Context::ModuleType::MODULE_DYNAMICS_EXECUTIVE:
        return buildNewExecutive(mContext,
                                 mEventViews,
                                 mExperiment.views(),
                                 observable,
                                 coordinator,
                                 atom,
                                 dyn,
                                 events,
                                 symbol);
    case utils::Context::ModuleType::MODULE_DYNAMICS_WRAPPER:
        return buildNewDynamicsWrapper(mContext, atom, dyn, events, symbol);
    default:
        throw utils::InternalError("Missing type");
    }
}
}
} // namespace vle devs
