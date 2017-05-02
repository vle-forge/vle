/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2017 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2017 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2017 INRA http://www.inra.fr
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

#include <boost/format.hpp>
#include <fstream>
#include <iostream>
#include <limits>
#include <stack>
#include <stdexcept>
#include <vle/devs/Coordinator.hpp>
#include <vle/devs/Dynamics.hpp>
#include <vle/devs/Executive.hpp>
#include <vle/devs/RootCoordinator.hpp>
#include <vle/oov/Plugin.hpp>
#include <vle/utils/Filesystem.hpp>
#include <vle/utils/unit-test.hpp>
#include <vle/vpz/Classes.hpp>
#include <vle/vpz/CoupledModel.hpp>
#include <vle/vpz/Dynamics.hpp>
#include <vle/vpz/Experiment.hpp>

using namespace vle;

#define xstringify(a) stringify(a)
#define stringify(a) #a

#define DECLARE_DYNAMICS_SYMBOL(symbol_, model_)                              \
    extern "C" {                                                              \
    VLE_MODULE vle::devs::Dynamics* symbol_(                                  \
      const vle::devs::DynamicsInit& init,                                    \
      const vle::devs::InitEventList& events)                                 \
    {                                                                         \
        return new model_(init, events);                                      \
    }                                                                         \
    }

#define DECLARE_EXECUTIVE_SYMBOL(symbol_, model_)                             \
    extern "C" {                                                              \
    VLE_MODULE vle::devs::Dynamics* symbol_(                                  \
      const vle::devs::ExecutiveInit& init,                                   \
      const vle::devs::InitEventList& events)                                 \
    {                                                                         \
        std::string symbol_test(xstringify(symbol_));                         \
        Ensures(symbol_test.length() > 4);                                    \
        Ensures(symbol_test.compare(0, 4, "exe_") == 0);                      \
        return new model_(init, events);                                      \
    }                                                                         \
    }

#define DECLARE_OOV_SYMBOL(symbol_, model_)                                   \
    extern "C" {                                                              \
    VLE_MODULE vle::oov::Plugin* symbol_(const std::string& location)         \
    {                                                                         \
        return new model_(location);                                          \
    }                                                                         \
    }

class DynamicObs : public devs::Dynamics
{
public:
    DynamicObs(const devs::DynamicsInit& atom,
               const devs::InitEventList& events)
      : devs::Dynamics(atom, events)
      , d(0.0)
    {
    }

    virtual devs::Time init(devs::Time /* time */) override
    {
        return 0;
    }

    virtual devs::Time timeAdvance() const override
    {
        return devs::infinity;
    }

    virtual void internalTransition(devs::Time /* time */) override
    {
        d = 3.9;
    }

    virtual void externalTransition(const devs::ExternalEventList& /*events*/,
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

    virtual std::unique_ptr<value::Value> observation(
      const devs::ObservationEvent&) const override
    {
        return vle::value::Double::create(d);
    }
    double d;
};

DECLARE_DYNAMICS_SYMBOL(dynamics_obs, DynamicObs)

void
test_dynamic_obs()
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

        while (root.run())
            ;
        std::unique_ptr<value::Map> out = root.finish();
        Ensures(out);
        EnsuresEqual(out->size(), 4);

        // 6 lines: header + 5 timed obs (timestep = 1, duration = 4)
        EnsuresEqual(out->getMatrix("viewTimed").rows(), 6);
        // std::cout << "dbg viewTimed\n" << out->getMatrix("viewTimed") <<
        // "\n";

        // 2 lines: header + only one input
        EnsuresEqual(out->getMatrix("viewOutput").rows(), 2);
        EnsuresApproximatelyEqual(
          out->getMatrix("viewOutput").getDouble(1, 1), 0.0, 10e-4);
        // std::cout << "dbg viewOutput\n" << out->getMatrix("viewOutput") <<
        // "\n";

        // 2 lines: header + only one intenral transition
        EnsuresEqual(out->getMatrix("viewInternal").rows(), 2);
        EnsuresApproximatelyEqual(
          out->getMatrix("viewInternal").getDouble(1, 1), 3.9, 10e-4);
        // std::cout << "dbg viewInternal\n" << out->getMatrix("viewInternal")
        // << "\n";

        // 2 lines: header +finish
        EnsuresEqual(out->getMatrix("viewFinish").rows(), 2);
        // std::cout << "dbg viewFinish\n" << out->getMatrix("viewFinish") <<
        // "\n";

    } catch (const std::exception& e) {
        Ensures(false);
    }
}

int
main()
{
    auto ctx = vle::utils::make_context();

    // We check if user use make install or not otherwise, configure(),
    // build() and install() will fail.

    if (ctx->getBinaryPackagesDir()[0].exists() and
        ctx->getBinaryPackagesDir()[1].exists()) {
        test_dynamic_obs();
    }

    return unit_test::report_errors();
}
