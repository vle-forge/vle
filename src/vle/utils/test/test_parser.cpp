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
#define BOOST_TEST_MODULE utils_library_test_parser
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Parser.hpp>
#include <vle/vle.hpp>

using namespace vle;

struct F
{
    vle::Init a;

    F() : a() { }
    ~F() { }
};

BOOST_GLOBAL_FIXTURE(F)

const std::string& str(
    "# this file is a test\n"
    "test { # comment\n"
    "a = 1, 2, 3;"
    "b = \"1\", \"2\", \"3\";"
    "}\n");

BOOST_AUTO_TEST_CASE(ParserStr)
{
    std::istringstream in(str);
    vle::utils::Parser parser(in);

    BOOST_REQUIRE_EQUAL(parser.root().blocks.size(), 1);

    const vle::utils::Block& block = parser.root().getBlock("test");
    BOOST_REQUIRE_EQUAL(block.name, "test");
    BOOST_REQUIRE_EQUAL(block.strings.size(), 3);
    BOOST_REQUIRE_EQUAL(block.reals.size(), 3);

    {
        typedef vle::utils::Block::Reals::const_iterator Iterator;
        std::pair < Iterator, Iterator > r = block.reals.equal_range("a");
        BOOST_REQUIRE_EQUAL(block.reals.count("a"), 3);
        BOOST_REQUIRE_EQUAL(r.first++->second, 1);
        BOOST_REQUIRE_EQUAL(r.first++->second, 2);
        BOOST_REQUIRE_EQUAL(r.first++->second, 3);
    }
    {
        typedef vle::utils::Block::Strings::const_iterator Iterator;
        std::pair < Iterator, Iterator > r = block.strings.equal_range("b");
        BOOST_REQUIRE_EQUAL(block.strings.count("b"), 3);
        BOOST_REQUIRE_EQUAL(r.first++->second, "1");
        BOOST_REQUIRE_EQUAL(r.first++->second, "2");
        BOOST_REQUIRE_EQUAL(r.first++->second, "3");
    }
}
