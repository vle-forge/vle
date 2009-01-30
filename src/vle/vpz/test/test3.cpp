/**
 * @file vle/vpz/test/test3.cpp
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
#define BOOST_TEST_MODULE translator_complete_test
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/output_test_stream.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Double.hpp>

struct F
{
    F() { vle::value::init(); }
    ~F() { vle::value::finalize(); }
};

BOOST_FIXTURE_TEST_SUITE(vpz_translator, F)

using namespace vle;

BOOST_AUTO_TEST_CASE(test_translator)
{
}

BOOST_AUTO_TEST_SUITE_END()
