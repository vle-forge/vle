/*
 * @file examples/fsa/FDDevs.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
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


#include <vle/extension/fsa/FDDevs.hpp>

using namespace boost::assign;

namespace vle { namespace examples { namespace fsa {

namespace vf = vle::extension::fsa;
namespace vd = vle::devs;

enum State { a = 1, b, c };

class devs1 : public vf::FDDevs
{
public:
    devs1(const vd::DynamicsInit& init,
          const vd::InitEventList& events) :
        vf::FDDevs(init, events)
    {
        states(this) << a << b << c;

        duration(this, a) << 6;
        duration(this, b) << 5;
        duration(this, c) << 2;

        internal(this, a) >> b;
        internal(this, b) >> a;
        internal(this, c) >> a;

        //	    output(this, a) >> "out";
        outputFunc(this, &devs1::out) >> a;

        external(this, a, "in") >> c;
        external(this, b, "in") >> c;

        initialState(a);
    }

    virtual ~devs1() { }

    void out(const vd::Time& /*time*/,
             vd::ExternalEventList& output) const
    { output.addEvent(buildEvent("out")); }
};

class devs2 : public vf::FDDevs
{
public:
    devs2(const vd::DynamicsInit& init,
          const vd::InitEventList& events) :
        vf::FDDevs(init, events)
    {
        states(this) << a << b << c;

        duration(this, a) << 6;
        duration(this, b) << 5;
        duration(this, c) << 2;

        internal(this, a) >> b;
        internal(this, b) >> a;
        internal(this, c) >> a;

        //	    output(this, a) >> "out";
        outputFunc(this, &devs2::out) >> a;

        external(this, a, "in") >> c;

        initialState(a);
    }

    virtual ~devs2() { }

    void out(const vd::Time& /*time*/,
             vd::ExternalEventList& output) const
    { output.addEvent(buildEvent("out")); }
};

DECLARE_NAMED_DYNAMICS(devs1, devs1)
DECLARE_NAMED_DYNAMICS(devs2, devs2)

}}} // namespace vle examples fsa
