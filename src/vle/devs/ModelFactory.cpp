/**
 * @file vle/devs/ModelFactory.cpp
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
 * Copyright (C) 2003-2010 ULCO http://www.univ-littoral.fr
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

ModuleCache::~ModuleCache()
{
    std::for_each(m_lst.begin(), m_lst.end(), DeleteModule());
}

void ModuleCache::add(const std::string& /*library*/, Glib::Module* module)
{
    std::pair < ModuleList::iterator, bool > r = m_lst.insert(
        std::pair < std::string, Glib::Module* >(module->get_name(), module));

    if (not r.second) {
        throw utils::InternalError(fmt(_(
                "The Module '%1%' already exist in cache")) %
            module->get_name());
    }
}

bool ModuleCache::exist(const std::string& library) const
{
    return m_lst.find(library) != m_lst.end();
}

Glib::Module* ModuleCache::get(const std::string& library) const
{
    ModuleList::const_iterator it = m_lst.find(library);
    return it == m_lst.end() ? 0 : it->second;
}

ModelFactory::ModelFactory(const vpz::Dynamics& dyn,
                           const vpz::Classes& cls,
                           const vpz::Experiment& exp,
                           const vpz::AtomicModelList& atom,
                           RootCoordinator& root) :
    mDynamics(dyn),
    mClasses(cls),
    mExperiment(exp),
    mAtoms(atom),
    mRoot(root)
{
}

ModelFactory::~ModelFactory()
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
                               const vpz::Strings& conditions,
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

    vpz::ValueList initValues;
    if (not conditions.empty()) {
	for (vpz::Strings::const_iterator it = conditions.begin();
	     it != conditions.end(); ++it) {
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
        attachDynamics(coordinator, sim, dyn, buildPlugin(dyn), initValues);
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

                ObservationEvent* evt = view->addObservable(
                    sim, it->first, coordinator.getCurrentTime());
                if (evt) {
                    coordinator.eventtable().putObservationEvent(evt);
                }
            }
        }
    }

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

Glib::Module* ModelFactory::buildPlugin(const vpz::Dynamic& dyn)
{
    std::string path, error;

    if (dyn.package().empty()) {
        if (utils::Package::package().selected()) {
            error = (fmt(_("Error opening simulator '%1%' from current package:"
                           " '%2%'")) % dyn.library() %
                     utils::Path::path().getPackageLibDir()).str();
            path = utils::Path::path().getPackageLibDir();
        } else {
            error = (fmt(_("Error opening simulator '%1%' from global"
                           " directory:")) % dyn.library()).str();
            path = utils::Path::path().getSimulatorDir();
        }
    } else {
        error = (fmt(_("Error opening simulator '%1%' from package '%2%':")) %
                 dyn.library() % dyn.package()).str();
        path = utils::Path::path().getExternalPackageLibDir(dyn.package());
    }

    std::string filename;

    if (dyn.language().empty()) {
        filename = Glib::Module::build_path(path, dyn.library());
    } else {
        filename = Glib::Module::build_path(path, "pydynamics");
    }

    Glib::Module* module = mModule.get(filename);
    if (not module) {
        module = new Glib::Module(filename);
        if (not (*module)) {
            error += "\n";
            error += Glib::Module::get_last_error();
            delete module;
        } else {
#ifdef G_OS_WIN32
            module->make_resident();
#endif
            mModule.add(dyn.language() == "python" ? "pydynamics" :
                        dyn.library(), module);
            return module;
        }
    } else {
        return module;
    }

    throw utils::FileError(error);
}

Glib::Module* ModelFactory::getPlugin(const std::string& /*name*/)
{
    throw utils::InternalError(
        _("ModelFactory::getPlugin do not use this function"));
}

void ModelFactory::attachDynamics(Coordinator& coordinator,
                                  devs::Simulator* atom,
                                  const vpz::Dynamic& dyn,
                                  Glib::Module* module,
                                  const InitEventList& events)
{
    std::string dynamicsSymbolName("makeNewDynamics");
    std::string executiveSymbolName("makeNewExecutive");
    std::string dynamicsWrapperSymbolName("makeNewDynamicsWrapper");
    devs::Dynamics* r = 0;

    if (dyn.model().empty() or not dyn.language().empty()) {
        try {
            r = getDynamicsObject(coordinator, *atom->getStructure(),
                                  dyn, module, events,
                                  dynamicsSymbolName,
                                  executiveSymbolName,
                                  dynamicsWrapperSymbolName);

        } catch (const std::exception& e) {
            throw utils::ModellingError(fmt(
                    _("Dynamic library loading problem: cannot get dynamics"
                      " '%1%' in module '%2%': %3%")) % dyn.name() %
                module->get_name() % e.what());
        }
    } else {
        try {
            r = getDynamicsObject(coordinator, *atom->getStructure(),
                                  dyn, module, events,
                                  dynamicsSymbolName + dyn.model(),
                                  executiveSymbolName + dyn.model(),
                                  dynamicsWrapperSymbolName + dyn.model());
        } catch (const std::exception& e) {
            throw utils::ModellingError(fmt(
                    _("Dynamic library loading problem: cannot get dynamics"
                      " '%1%', model '%2%' in module '%3%': %4%")) % dyn.name()
                % dyn.model() % module->get_name() % e.what());
        }
    }

    assert(r);
    atom->addDynamics(r);
}

Dynamics* ModelFactory::getDynamicsObject(Coordinator& coordinator,
                                          const graph::AtomicModel& atom,
                                          const vpz::Dynamic& dyn,
                                          Glib::Module* module,
                                          const InitEventList& events,
                                          const std::string& dynamicsSymbol,
                                          const std::string& executiveSymbol,
                                          const std::string& dynwrapperSymbol)
{
    typedef Dynamics* (*fctdyn) ( const DynamicsInit&, const InitEventList&);
    typedef Dynamics* (*fctexe) ( const ExecutiveInit&, const InitEventList&);
    typedef Dynamics* (*fctdw) ( const DynamicsWrapperInit&,
                                 const InitEventList&);

    Dynamics* dynamics;
    void* symbol;

    if (module->get_symbol(dynamicsSymbol, symbol)) {
        fctdyn fct(utils::pointer_to_function < fctdyn >(symbol));
        try {
            dynamics = fct(DynamicsInit(
                    atom, mRoot.rand(),
                    utils::Package::package().getId(dyn.package())), events);
        } catch(const std::exception& e) {
            throw utils::ModellingError(fmt(
                    _("atomic model '%1%:%2%' throw error: %3%")) %
                atom.getParentName() % atom.getName() % e.what());
        }
        return dynamics;
    }

    if (module->get_symbol(executiveSymbol, symbol)) {
        fctexe fct(utils::pointer_to_function < fctexe >(symbol));
        try {
            dynamics = fct(ExecutiveInit(
                    atom, mRoot.rand(),
                    utils::Package::package().getId(dyn.package()),
                    coordinator), events);
        } catch(const std::exception& e) {
            throw utils::ModellingError(fmt(
                    _("executive model '%1%:%2%' throw error: %3%")) %
                atom.getParentName() % atom.getName() % e.what());
        }
        return dynamics;
    }

    if (module->get_symbol(dynwrapperSymbol, symbol)) {
        fctdw fct(utils::pointer_to_function < fctdw >(symbol));
        try {
            dynamics = fct(
                DynamicsWrapperInit(
                    atom, mRoot.rand(),
                    utils::Package::package().getId(dyn.package()),
                    dyn.library(), dyn.model()), events);
        } catch(const std::exception& e) {
            throw utils::ModellingError(fmt(
                    _("dynamics wrapper '%1%:%2%' throw error: %3%")) %
                atom.getParentName() % atom.getName() % e.what());
        }
        return dynamics;
    }
    throw utils::ArgError(_("Not a VLE dynamics, executive or dynamicswrapper"
                            " plugin"));
}

}} // namespace vle devs
