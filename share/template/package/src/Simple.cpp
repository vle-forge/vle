/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2016 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2016 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2016 INRA http://www.inra.fr
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

#include <vle/value/Value.hpp>
#include <vle/devs/Dynamics.hpp>

namespace vd = vle::devs;
namespace vv = vle::value;

namespace examples {

class Simple : public vd::Dynamics
{
public:
    Simple(const vd::DynamicsInit& init, const vd::InitEventList& events)
        : vd::Dynamics(init, events)
    {
    }

    virtual ~Simple()
    {
    }

    virtual vd::Time init(vle::devs::Time /*time*/) override
    {
        return vd::infinity;
    }

    virtual void output(vle::devs::Time /*time*/,
                        vd::ExternalEventList& /*output*/) const override
    {
    }

    virtual vd::Time timeAdvance() const override
    {
        return vd::infinity;
    }

    virtual void internalTransition(vle::devs::Time /*time*/) override
    {
    }

    virtual void externalTransition(
            const vd::ExternalEventList& /*event*/,
            vle::devs::Time /*time*/) override
    {
    }

    virtual void confluentTransitions(
            vle::devs::Time time,
            const vd::ExternalEventList& events) override
    {
        internalTransition(time);
        externalTransition(events, time);
    }

    virtual std::unique_ptr<vle::value::Value> observation(
            const vd::ObservationEvent& /*event*/) const override
    {
        return 0;
    }

    virtual void finish()
    {
    }
};

} // namespace vle example

DECLARE_DYNAMICS(examples::Simple)
