/** 
 * @file test1.cpp
 * @brief Test unit of the sax value parser.
 * @author The vle Development Team
 */

/*
 * Copyright (C) 2007 - The vle Development Team
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
#include <vle/graph/graph.hpp>

using namespace vle;


BOOST_AUTO_TEST_CASE(test_del_all_connection)
{
    graph::CoupledModel* top = new graph::CoupledModel("top", 0);

    graph::AtomicModel* a(top->addAtomicModel("a"));
    a->addInputPort("in");
    a->addOutputPort("out");

    graph::AtomicModel* b(top->addAtomicModel("b"));
    b->addInputPort("in");
    b->addOutputPort("out");

    top->addInternalConnection("a", "out", "b", "in");
    top->addInternalConnection("b", "out", "a", "in");

    BOOST_REQUIRE(top->existInternalConnection("a", "out", "b", "in"));
    BOOST_REQUIRE(top->existInternalConnection("b", "out", "a", "in"));
    top->delAllConnection();
    BOOST_REQUIRE(not top->existInternalConnection("a", "out", "b", "in"));
    BOOST_REQUIRE(not top->existInternalConnection("b", "out", "a", "in"));
}
