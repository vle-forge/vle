/**
 * @file src/vle/value/test/test1.cpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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





#define BOOST_TEST_MAIN
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE atomicdynamics_test
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <limits>
#include <fstream>
#include <vle/value.hpp>

using namespace vle;


BOOST_AUTO_TEST_CASE(check_clone)
{
    value::Map mp = value::MapFactory::create();
    mp->addValue("x1", value::StringFactory::create("toto"));
    mp->addValue("x2", value::StringFactory::create("toto"));
    mp->addValue("x3", value::StringFactory::create("toto"));

    value::Map mpclone = value::toMapValue(mp->clone());
    BOOST_REQUIRE(mp->getValue("x1").get() != mpclone->getValue("x1").get());
    BOOST_REQUIRE(mp->getValue("x2").get() != mpclone->getValue("x2").get());
    BOOST_REQUIRE(mp->getValue("x3").get() != mpclone->getValue("x3").get());

    value::Set st = value::SetFactory::create();
    st->addValue(value::StringFactory::create("toto"));
    st->addValue(value::StringFactory::create("toto"));
    st->addValue(value::StringFactory::create("toto"));

    value::Set stclone = value::toSetValue(st->clone());

    BOOST_REQUIRE_EQUAL(st->size(), stclone->size());

    BOOST_REQUIRE(st->getValue(0).get() != stclone->getValue(0).get());
    BOOST_REQUIRE(st->getValue(1).get() != stclone->getValue(1).get());
    BOOST_REQUIRE(st->getValue(2).get() != stclone->getValue(2).get());
}
