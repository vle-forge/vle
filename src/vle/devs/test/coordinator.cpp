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


#define BOOST_TEST_MAIN
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE devstime_test
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <limits>
#include <fstream>
#include <vle/devs/Coordinator.hpp>
#include <vle/devs/RootCoordinator.hpp>
#include <vle/vpz/CoupledModel.hpp>
#include <vle/vpz/Dynamics.hpp>
#include <vle/vpz/Experiment.hpp>
#include <vle/vpz/Classes.hpp>
#include <vle/utils/ModuleManager.hpp>

using namespace vle;

class Model : public vle::devs::Dynamics
{
    int state;

public:
    Model(const vle::devs::DynamicsInit& init,
          const vle::devs::InitEventList& events)
        : vle::devs::Dynamics(init, events)
    {}

    virtual ~Model()
    {}

    virtual vle::devs::Time init(const vle::devs::Time& /* time */)
    {
        state = 0;
        return vle::devs::infinity;
    }

    virtual void output(const vle::devs::Time& /* time */,
                        vle::devs::ExternalEventList& output) const
    {
        output.emplace_back("out");

        output.back().attributes() =
            std::make_shared<value::String>("My message");
    }

    virtual vle::devs::Time timeAdvance() const
    { return vle::devs::infinity; }

    virtual void internalTransition(
        const vle::devs::Time& /* time */)
    { }

    virtual void externalTransition(
        const vle::devs::ExternalEventList& events,
        const vle::devs::Time& /* time */)
    {
        for (const auto& elem : events)
            if (elem.onPort("x"))
                state = 1;
    }

    virtual void confluentTransitions(
        const vle::devs::Time& time,
        const vle::devs::ExternalEventList& extEventlist)
    {
        internalTransition(time);
        externalTransition(extEventlist, time);
    }

    virtual std::unique_ptr<vle::value::Value>
    observation(const vle::devs::ObservationEvent& /* event */) const
    {
        return std::unique_ptr<vle::value::Value>();
    }

    virtual void finish()
    {}
};

BOOST_AUTO_TEST_CASE(test_del_coupled_model)
{
    utils::ModuleManager modules;
    vpz::Dynamics dyns;
    vpz::Classes classes;
    vpz::Experiment expe;
    devs::RootCoordinator root(modules);
    devs::Coordinator coord(modules,dyns,classes,expe,root);
    vpz::CoupledModel* depth0 = new vpz::CoupledModel("depth0", 0);
    vpz::CoupledModel* depth1(depth0->addCoupledModel("depth1"));
    vpz::AtomicModel* depth2 = depth1->addAtomicModel("depth2");
    devs::Simulator* simdepth2 = new devs::Simulator(depth2);
    coord.addModel(depth2,simdepth2);

    BOOST_CHECK_NO_THROW(coord.delModel(depth0,"depth1"));

    delete depth0;
    delete simdepth2;
}
