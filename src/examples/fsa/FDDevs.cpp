/**
 * @file examples/fsa/FDDevs.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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


#include <vle/extension/FDDevs.hpp>

using namespace boost::assign;

namespace vle { namespace examples { namespace fsa {

namespace ve = vle::extension;
namespace vd = vle::devs;

enum State { a = 1, b, c };

class devs1 : public ve::FDDevs
{
public:
    devs1(const vd::DynamicsInit& init,
          const vd::InitEventList& events) :
        ve::FDDevs(init, events)
    {
        ve::states(this) << a << b << c;

        ve::duration(this, a) << 6;
        ve::duration(this, b) << 5;
        ve::duration(this, c) << 2;

        ve::internal(this, a) >> b;
        ve::internal(this, b) >> a;
        ve::internal(this, c) >> a;

        //	    ve::output(this, a) >> "out";
        ve::outputFunc(this, &devs1::out) >> a;

        ve::external(this, a, "in") >> c;
        ve::external(this, b, "in") >> c;

        initialState(a);
    }

    virtual ~devs1() { }

    void out(const vd::Time& /*time*/,
             vd::ExternalEventList& output) const
    { output.addEvent(buildEvent("out")); }
};

class devs2 : public ve::FDDevs
{
public:
    devs2(const vd::DynamicsInit& init,
          const vd::InitEventList& events) :
        ve::FDDevs(init, events)
    {
        ve::states(this) << a << b << c;

        ve::duration(this, a) << 6;
        ve::duration(this, b) << 5;
        ve::duration(this, c) << 2;

        ve::internal(this, a) >> b;
        ve::internal(this, b) >> a;
        ve::internal(this, c) >> a;

        //	    ve::output(this, a) >> "out";
        ve::outputFunc(this, &devs2::out) >> a;

        ve::external(this, a, "in") >> c;

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
