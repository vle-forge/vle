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
#include <vle/devs/Simulator.hpp>

using namespace vle;


BOOST_AUTO_TEST_CASE(compare)
{
    devs::Time a(1.0);
    devs::Time b(2.0);

    BOOST_REQUIRE(b > a);
    BOOST_REQUIRE(b > 1.0);
    BOOST_REQUIRE(2.0 > a);
    BOOST_REQUIRE(b > 1);
    BOOST_REQUIRE(2 > a);

    devs::Time c = devs::infinity;

    BOOST_REQUIRE((a >= 2.0) == false);

    BOOST_REQUIRE(c > a);
    BOOST_REQUIRE(c > b);
    BOOST_REQUIRE(c > 2.0);
    BOOST_REQUIRE(c > 0.0);

    BOOST_REQUIRE_EQUAL(a, devs::Time(1.0));
    BOOST_REQUIRE_EQUAL(a, 1.0);
    BOOST_REQUIRE_EQUAL(a, 1);

    BOOST_REQUIRE_EQUAL(b, devs::Time(2.0));
    BOOST_REQUIRE_EQUAL(b, 2.0);
    BOOST_REQUIRE_EQUAL(b, 2);

    BOOST_REQUIRE(a == devs::Time(1.0));
    BOOST_REQUIRE(a == 1.0);
    BOOST_REQUIRE(a == 1);

    BOOST_REQUIRE(a < b);
    BOOST_REQUIRE(a < 2.0);
    BOOST_REQUIRE(a < 2);

    BOOST_REQUIRE(a <= devs::Time(1.0));
    BOOST_REQUIRE(a <= 1.0);
    BOOST_REQUIRE(a <= 1);

    BOOST_REQUIRE(a <= devs::Time(devs::infinity));
    BOOST_REQUIRE(a >= -1.0);
}

BOOST_AUTO_TEST_CASE(modify)
{
    devs::Time a(1.0);
    devs::Time b(2.0);
    devs::Time c(3.0);

    double x;

    x = c - b - a;
    BOOST_REQUIRE_EQUAL(x, 0.0);
    BOOST_REQUIRE_EQUAL(a + b, c);
    BOOST_REQUIRE(c >= a + b);
    BOOST_REQUIRE_EQUAL(1.0 + b, c);
    BOOST_REQUIRE(c >= a + 2.0);

    a += b;
    BOOST_REQUIRE_EQUAL(a, c);

    a -= b;
    BOOST_REQUIRE_EQUAL(a + b, c);

    devs::Time z = a;
    ++z;
    BOOST_REQUIRE_EQUAL(z, a + 1);
    --z;
    BOOST_REQUIRE_EQUAL(z, a);
}

BOOST_AUTO_TEST_CASE(modify_and_infinity)
{
    devs::Time a(1.0);

    BOOST_REQUIRE_EQUAL(a + devs::infinity, devs::infinity);
    BOOST_REQUIRE_EQUAL(devs::infinity + a, devs::infinity);
    BOOST_REQUIRE_EQUAL(devs::infinity - a, devs::infinity);

    {
        devs::Time a(1.0);
        devs::Time b(devs::infinity);
        a += b;
        BOOST_REQUIRE_EQUAL(a, devs::infinity);
    }

    {
        devs::Time a(devs::infinity);
        devs::Time b(1.0);
        a += b;
        BOOST_REQUIRE_EQUAL(a, devs::infinity);
        BOOST_REQUIRE_EQUAL(b, 1.0);
    }

    {
        devs::Time b(devs::infinity);
        BOOST_REQUIRE(devs::isInfinity(b));
    }

    {
        devs::Time a(devs::infinity);
        devs::Time b(1.0);
        a -= b;
        BOOST_REQUIRE_EQUAL(a, devs::infinity);
        BOOST_REQUIRE_EQUAL(b, 1.0);
    }

    {
        devs::Time a(devs::infinity);
        devs::Time b(1.0);
        a += b;
        BOOST_REQUIRE_EQUAL(a, devs::infinity);
        BOOST_REQUIRE_EQUAL(b, 1.0);
    }
}

BOOST_AUTO_TEST_CASE(prefix_and_postfix_operator)
{
    devs::Time a(1.0);

    devs::Time b = a++;
    BOOST_REQUIRE_EQUAL(a, 2.0);
    BOOST_REQUIRE_EQUAL(b, 1.0);

    ++a;
    BOOST_REQUIRE_EQUAL(a, 3.0);

    ++(++a);
    BOOST_REQUIRE_EQUAL(a, 5.0);

    (++a) = 7;
    BOOST_REQUIRE_EQUAL(a, 7.0);

    devs::Time c = a--;
    BOOST_REQUIRE_EQUAL(a, 6.0);
    BOOST_REQUIRE_EQUAL(c, 7.0);

    --a;
    BOOST_REQUIRE_EQUAL(a, 5.0);

    --(--a);
    BOOST_REQUIRE_EQUAL(a, 3.0);

    (--a) = 0;
    BOOST_REQUIRE_EQUAL(a, 0.0);
}

