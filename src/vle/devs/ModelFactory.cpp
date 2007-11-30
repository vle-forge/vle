/**
 * @file devs/ModelFactory.cpp
 * @author The VLE Development Team.
 * @brief Read simulations plugin from models directories and manage models
 * classes.
 */

/*
 * Copyright (C) 2004, 05, 06 - The vle Development Team
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <vle/devs/ModelFactory.hpp>
#include <vle/devs/Coordinator.hpp>
#include <vle/devs/Simulator.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/devs/DynamicsWrapper.hpp>
#include <vle/devs/Executive.hpp>
#include <vle/graph/Model.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <vle/utils/XML.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Trace.hpp>

namespace vle { namespace devs {

ModuleCache::~ModuleCache()
{
    std::for_each(m_lst.begin(), m_lst.end(), DeleteModule());
}

void ModuleCache::add(const std::string& library, Glib::Module* module)
{
    Assert(utils::InternalError, not exist(library), boost::format(
            "The Module '%1%' already exist in cache") % library);

    m_lst[library] = module;
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
                           const vpz::Experiment& exp) :
    mDynamics(dyn),
    mClasses(cls),
    mExperiment(exp)
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
        Throw(utils::InternalError, boost::format(
            "Model factory cannot add dynamics %1%: %2%") % dynamics.name() %
            e.what());
    }
}

void ModelFactory::addPermanent(const vpz::Condition& condition)
{
    try {
        vpz::Conditions& conds(mExperiment.conditions());
        conds.add(condition);
    } catch(const std::exception& e) {
        Throw(utils::InternalError, boost::format(
                "Model factory cannot add condition %1%: %2%") %
            condition.name() % e.what());
    }
}

void ModelFactory::addPermanent(const vpz::Observable& observable)
{
    try {
        vpz::Views& views(mExperiment.views());
        views.addObservable(observable);
    } catch(const std::exception& e) {
        Throw(utils::InternalError, boost::format(
                "Model factory cannot add observable %1%: %2%") %
            observable.name() % e.what());
    }
}

Simulator* ModelFactory::createModel(Coordinator& coordinator,
                                     graph::AtomicModel* model,
                                     const std::string& dynamics,
                                     const std::string& condition,
                                     const std::string& observable)
{
    const SimulatorMap& result(coordinator.modellist()); 
    Simulator* sim = new Simulator(model);
    Assert(utils::InternalError, result.find(model) == result.end(),
           boost::format("The model %1% already exist in coordinator") %
           model->getName());
    coordinator.addModel(model, sim);

    const vpz::Dynamic& dyn = mDynamics.get(dynamics);
    switch(dyn.type()) {
    case vpz::Dynamic::LOCAL:
        attachDynamics(coordinator, sim, dyn, getPlugin(dyn.name()));
        break;
    case vpz::Dynamic::DISTANT:
        Throw(utils::NotYetImplemented, "Distant dynamics is not supported");
    }

    if (not condition.empty()) {
        const vpz::Condition& cnd(mExperiment.conditions().get(condition));
        sim->processInitEvents(cnd.firstValues());
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
                Assert(utils::InternalError, view, (boost::format(
                        "The view %1% is unknow of coordinator view list") %
                        *jt));

                StateEvent* evt = view->addObservable(
                    sim, it->first, coordinator.getCurrentTime());
                if (evt) {
                    coordinator.eventtable().putStateEvent(evt);
                }
            }
        }
    }

    return sim;
}

Glib::Module* ModelFactory::buildPlugin(const vpz::Dynamic& dyn)
{
    utils::Path::PathList lst(utils::Path::path().getSimulatorDirs());
    utils::Path::PathList::const_iterator it;

    std::string error((boost::format(
                "Error opening simulator plugin '%1%' in:") %
            dyn.library()).str());

    for (it = lst.begin(); it != lst.end(); ++it) {
        std::string file;

        if (dyn.language().empty()) {
            file = Glib::Module::build_path(*it, dyn.library());
        } else if (dyn.language() == "python") {
            file = Glib::Module::build_path(*it, "pyvle");
        }

        Glib::Module* module = new Glib::Module(file);
        if (not (*module)) {
            error += boost::str(boost::format(
                    "\n[%1%]: %2%") % *it % Glib::Module::get_last_error());
            delete module;
        } else {
            mModule.add(dyn.library(), module);
            return module;
        }
    }
    Throw(utils::FileError, error);
}

Glib::Module* ModelFactory::getPlugin(const std::string& name)
{
    if (mDynamics.exist(name)) {
        const vpz::Dynamic& dyn(mDynamics.get(name));
        Glib::Module* r = mModule.get(dyn.library());
        return (r == 0) ? buildPlugin(dyn) : r;
    } else {
        vpz::Classes::const_iterator clit;
        for (clit = mClasses.begin(); clit != mClasses.end(); ++clit) {
            if ((*clit).second.dynamics().exist(name)) {
                const vpz::Dynamic& dyn((*clit).second.dynamics().get(name));
                Glib::Module* r = mModule.get(dyn.library());
                return (r == 0) ? buildPlugin(dyn) : r;
            }
        }
    }

    Throw(utils::ArgError, boost::format(
            "Dynamics plugin '%1%' not found in model factory\n") % name);
}

void ModelFactory::attachDynamics(Coordinator& coordinator,
                                  devs::Simulator* atom,
                                  const vpz::Dynamic& dyn,
                                  Glib::Module* module)
{
    devs::Dynamics* call = 0;
    void* makeNewDynamics = 0;

    if (dyn.model().empty() or !dyn.language().empty()) {
        bool getSymbol = module->get_symbol("makeNewDynamics", makeNewDynamics);
        Assert(utils::ParseError, getSymbol, boost::format(
               "Error in '%1%', function 'makeNewDynamics' not found: '%2%'\n")
	       % module->get_name() % Glib::Module::get_last_error());
      
        call = ((Dynamics*(*)(const graph::Model&))
                (makeNewDynamics))(*atom->getStructure());
        Assert(utils::ParseError, call, boost::format(
	       "Error in '%1%', function 'makeNewDynamics':"
	       "problem allocation a new Dynamics: '%2%'\n") %
	       module->get_name() % Glib::Module::get_last_error());
    } else {
        std::string functionanme("makeNewDynamics");
	functionanme += dyn.model();
	bool getSymbol = module->get_symbol(functionanme , makeNewDynamics);
	Assert(utils::ParseError, getSymbol, boost::format(
	       "Error in '%1%', function '%2%' not found: '%3%'\n") %
               module->get_name() % functionanme %
               Glib::Module::get_last_error());
      
        call = ((Dynamics*(*)(const graph::Model&))
                (makeNewDynamics))(*atom->getStructure());
	Assert(utils::ParseError, call, boost::format(
	       "Error in '%1%', function '%2%':"
	       "problem allocation a new Dynamics: '%3%'\n") %
               module->get_name() % functionanme %
               Glib::Module::get_last_error());
    }

    if (call->is_executive()) {
        (reinterpret_cast < Executive* >(call))->set_coordinator(&coordinator);
    }
    
    if (call->is_wrapper()) {
        (reinterpret_cast < DynamicsWrapper* >(call))->set_library(dyn.library());
        (reinterpret_cast < DynamicsWrapper* >(call))->set_model(dyn.model());
    }

    atom->addDynamics(call);
}

}} // namespace vle devs
