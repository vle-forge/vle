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

#include <boost/lexical_cast.hpp>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <vle/devs/Simulator.hpp>
#include <vle/utils/unit-test.hpp>

using namespace vle;

void
compare()
{
    devs::Time a(1.0);
    devs::Time b(2.0);

    Ensures(b > a);
    Ensures(b > 1.0);
    Ensures(2.0 > a);
    Ensures(b > 1);
    Ensures(2 > a);

    devs::Time c = devs::infinity;

    Ensures((a >= 2.0) == false);

    Ensures(c > a);
    Ensures(c > b);
    Ensures(c > 2.0);
    Ensures(c > 0.0);

    EnsuresEqual(a, devs::Time(1.0));
    EnsuresEqual(a, 1.0);
    EnsuresEqual(a, 1);

    EnsuresEqual(b, devs::Time(2.0));
    EnsuresEqual(b, 2.0);
    EnsuresEqual(b, 2);

    Ensures(a == devs::Time(1.0));
    Ensures(a == 1.0);
    Ensures(a == 1);

    Ensures(a < b);
    Ensures(a < 2.0);
    Ensures(a < 2);

    Ensures(a <= devs::Time(1.0));
    Ensures(a <= 1.0);
    Ensures(a <= 1);

    Ensures(a <= devs::Time(devs::infinity));
    Ensures(a >= -1.0);
}

void
modify()
{
    devs::Time a(1.0);
    devs::Time b(2.0);
    devs::Time c(3.0);

    double x;

    x = c - b - a;
    EnsuresEqual(x, 0.0);
    EnsuresEqual(a + b, c);
    Ensures(c >= a + b);
    EnsuresEqual(1.0 + b, c);
    Ensures(c >= a + 2.0);

    a += b;
    EnsuresEqual(a, c);

    a -= b;
    EnsuresEqual(a + b, c);

    devs::Time z = a;
    ++z;
    EnsuresEqual(z, a + 1);
    --z;
    EnsuresEqual(z, a);
}

void
modify_and_infinity()
{
    devs::Time a(1.0);

    EnsuresEqual(a + devs::infinity, devs::infinity);
    EnsuresEqual(devs::infinity + a, devs::infinity);
    EnsuresEqual(devs::infinity - a, devs::infinity);

    {
        devs::Time a(1.0);
        devs::Time b(devs::infinity);
        a += b;
        EnsuresEqual(a, devs::infinity);
    }

    {
        devs::Time a(devs::infinity);
        devs::Time b(1.0);
        a += b;
        EnsuresEqual(a, devs::infinity);
        EnsuresEqual(b, 1.0);
    }

    {
        devs::Time b(devs::infinity);
        Ensures(devs::isInfinity(b));
    }

    {
        devs::Time a(devs::infinity);
        devs::Time b(1.0);
        a -= b;
        EnsuresEqual(a, devs::infinity);
        EnsuresEqual(b, 1.0);
    }

    {
        devs::Time a(devs::infinity);
        devs::Time b(1.0);
        a += b;
        EnsuresEqual(a, devs::infinity);
        EnsuresEqual(b, 1.0);
    }
}

void
prefix_and_postfix_operator()
{
    devs::Time a(1.0);

    devs::Time b = a++;
    EnsuresEqual(a, 2.0);
    EnsuresEqual(b, 1.0);

    ++a;
    EnsuresEqual(a, 3.0);

    ++(++a);
    EnsuresEqual(a, 5.0);

    (++a) = 7;
    EnsuresEqual(a, 7.0);

    devs::Time c = a--;
    EnsuresEqual(a, 6.0);
    EnsuresEqual(c, 7.0);

    --a;
    EnsuresEqual(a, 5.0);

    --(--a);
    EnsuresEqual(a, 3.0);

    (--a) = 0;
    EnsuresEqual(a, 0.0);
}

int
main()
{
    vle::Init app;

    compare();
    modify();
    modify_and_infinity();
    prefix_and_postfix_operator();

    return unit_test::report_errors();
}
