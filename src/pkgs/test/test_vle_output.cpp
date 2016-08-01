/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2016 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2016 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2016 INRA http://www.inra.fr
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
#define BOOST_TEST_MODULE devstime_mdl
#include <boost/test/included/unit_test.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/devs/Executive.hpp>
#include <vle/devs/Coordinator.hpp>
#include <vle/devs/RootCoordinator.hpp>
#include <vle/vpz/CoupledModel.hpp>
#include <vle/vpz/Dynamics.hpp>
#include <vle/vpz/Experiment.hpp>
#include <vle/vpz/Classes.hpp>
#include <vle/oov/Plugin.hpp>
#include <vle/utils/Filesystem.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <stack>
#include <stdexcept>
#include <limits>
#include <fstream>

using namespace vle;

#define xstringify(a) stringify(a)
#define stringify(a) #a

#define DECLARE_DYNAMICS_SYMBOL(symbol_, model_)        \
    extern "C" {                                        \
        VLE_MODULE vle::devs::Dynamics*                 \
        symbol_(const vle::devs::DynamicsInit& init,    \
                const vle::devs::InitEventList& events) \
        {                                               \
            return new model_(init, events);            \
        }                                               \
    }

#define DECLARE_EXECUTIVE_SYMBOL(symbol_, model_)                       \
    extern "C" {                                                        \
        VLE_MODULE vle::devs::Dynamics*                                 \
        symbol_(const vle::devs::ExecutiveInit& init,                   \
                const vle::devs::InitEventList& events)                 \
        {                                                               \
            std::string symbol_test(xstringify(symbol_));               \
            BOOST_REQUIRE(symbol_test.length() > 4);                    \
            BOOST_REQUIRE(symbol_test.compare(0, 4, "exe_") == 0);      \
            return new model_(init, events);                            \
        }                                                               \
    }

#define DECLARE_OOV_SYMBOL(symbol_, model_)     \
    extern "C" {                                \
        VLE_MODULE vle::oov::Plugin*            \
        symbol_(const std::string& location)    \
        {                                       \
            return new model_(location);        \
        }                                       \
    }


class DynamicObs : public devs::Dynamics
{
public:
    DynamicObs(const devs::DynamicsInit& atom,
              const devs::InitEventList& events) :
        devs::Dynamics(atom, events)
    {}

    virtual devs::Time init(devs::Time /* time */) override
    {
        return devs::infinity;
    }

    virtual devs::Time timeAdvance() const override
    {
        return devs::infinity;
    }

    virtual void internalTransition(devs::Time /* time */) override
    {
    }

    virtual void externalTransition(
        const devs::ExternalEventList& /*events*/,
        devs::Time /* time */) override
    {
    }

    virtual void confluentTransitions(
            devs::Time /*time*/,
            const devs::ExternalEventList& /*extEventlist*/) override
    {
    }

    virtual void output(devs::Time /*time*/,
                        devs::ExternalEventList& /*output*/) const override
    {
    }

    virtual std::unique_ptr<value::Value>
    observation(const devs::ObservationEvent&) const override
    {
        return 0;
    }
};

DECLARE_DYNAMICS_SYMBOL(dynamics_obs, DynamicObs)

BOOST_AUTO_TEST_CASE(test_dynamic_obs)
{
    auto ctx = vle::utils::make_context();
    vle::utils::Path p(PKGS_TEST_DIR);
    vle::utils::Path::current_path(p);

    vpz::Vpz file(PKGS_TEST_DIR "/dynamic_obs.vpz");
    devs::RootCoordinator root(ctx);

    try {
        root.load(file);
        file.clear();
        root.init();

        while (root.run());
        std::unique_ptr<value::Map> out = root.outputs();
        BOOST_REQUIRE(out);
        BOOST_REQUIRE_EQUAL(out->size(),2);
        root.finish();
    } catch (const std::exception& e) {
        BOOST_REQUIRE(false);
    }
}
