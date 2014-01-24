/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
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

    virtual vd::Time init(const vd::Time& /*time*/)
    {
        return vd::infinity;
    }

    virtual void output(const vd::Time& /*time*/,
                        vd::ExternalEventList& /*output*/) const
    {
    }

    virtual vd::Time timeAdvance() const
    {
        return vd::infinity;
    }

    virtual void internalTransition(const vd::Time& /*time*/)
    {
    }

    virtual void externalTransition(const vd::ExternalEventList& /*event*/,
                                    const vd::Time& /*time*/)
    {
    }

    virtual void confluentTransitions(const vd::Time& time,
                                      const vd::ExternalEventList& events)
    {
        internalTransition(time);
        externalTransition(events, time);
    }

    virtual vv::Value* observation(const vd::ObservationEvent& /*event*/) const
    {
        return 0;
    }

    virtual void finish()
    {
    }
};

} // namespace vle example

DECLARE_DYNAMICS(examples::Simple)
