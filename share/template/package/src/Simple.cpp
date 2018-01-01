/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2018 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2018 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2018 INRA http://www.inra.fr
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

/*
 * @@tagdynamic@@
 * @@tagdepends:@@endtagdepends
 */

#include <vle/devs/Dynamics.hpp>
#include <vle/value/Value.hpp>

namespace example {

class Simple : public vle::devs::Dynamics
{
public:
    Simple(const vle::devs::DynamicsInit& init,
           const vle::devs::InitEventList& events)
      : vle::devs::Dynamics(init, events)
    {
    }

    ~Simple() override = default;

    //
    // Process the initialization of the model by initializing the initial
    // state and return the duration of the initial state.
    // Default, @c init function returns vle::devs::infinity
    //
    vle::devs::Time init(vle::devs::Time time) override
    {
        return vle::devs::Dynamics::init(time);
    }

    //
    // Process the output function: add external event into the @c output.
    //
    void output(vle::devs::Time time,
                vle::devs::ExternalEventList& output) const override
    {
        // Example:
        // output.emplace_back("out");
        // output.back().addDouble(0.001);

        vle::devs::Dynamics::output(time, output);
    }

    //
    // Compute the duration of the current state. As the @c init function, @c
    // timeAdvance function default returns vle::devs::infinity.
    //
    vle::devs::Time timeAdvance() const override
    {
        return vle::devs::Dynamics::timeAdvance();
    }

    //
    // Compute the new state of the model with the internal transition
    // function. Default function does nothing.
    //
    void internalTransition(vle::devs::Time time) override
    {
        vle::devs::Dynamics::internalTransition(time);
    }

    //
    // Compute the new state of the model when at least one external event
    // occurs. Default function does nothing.
    //
    void externalTransition(const vle::devs::ExternalEventList& events,
                            vle::devs::Time time) override
    {
        // Example:
        // for (const auto& elem : events) {
        //     if (elem.attributes()->isDouble()) {
        //         //         double value = elem.getDouble().value();
        //     }
        // }

        externalTransition(events, time);
    }

    //
    // Compute the new state of the model when both at least one external event
    // occurs and an internal event. Default function is to call internal
    // transition and external transition as follow:
    //
    void confluentTransitions(
      vle::devs::Time time,
      const vle::devs::ExternalEventList& events) override
    {
        // Example of code:
        // internalTransition(time);
        // externalTransition(events, time);

        confluentTransitions(time, events);
    }

    //
    // Compute the current state of the model at a specified time and for a
    // specified observation port.
    //
    std::unique_ptr<vle::value::Value> observation(
      const vle::devs::ObservationEvent& event) const override
    {
        // Example:
        // return vle::Double::create(123.321);

        return vle::devs::Dynamics::observation(event);
    }

    //
    // When the simulation of the atomic model is finished, the finish method
    // is invoked. Default function does nothing.
    //
    void finish()
    {
        vle::devs::Dynamics::finish();
    }
};

} // namespace example

DECLARE_DYNAMICS(example::Simple)
