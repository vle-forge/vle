/** 
 * @file test4.cpp
 * @brief Test unit of the vpz parser.
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
#define BOOST_TEST_MODULE translator_complete_test
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/output_test_stream.hpp>

#include <vle/utils/utils.hpp>
#include <vle/graph/graph.hpp>
#include <vle/vpz/vpz.hpp>

using namespace vle;

BOOST_AUTO_TEST_CASE(test_connection)
{
    vpz::Vpz vpz;
    vpz.parse_file(utils::Path::path().build_prefix_share_path("examples", "coupled.vpz"));

    const vpz::Model& model(vpz.project().model());
    BOOST_REQUIRE(model.model());
    BOOST_REQUIRE(model.model()->isCoupled());

    graph::CoupledModel* cpled((graph::CoupledModel*)model.model());
    BOOST_REQUIRE_EQUAL(cpled->getName(), "top");
    BOOST_REQUIRE(cpled->exist("top1"));
    BOOST_REQUIRE(cpled->exist("top2"));
    BOOST_REQUIRE(cpled->exist("d"));
    BOOST_REQUIRE(cpled->exist("e"));

    graph::CoupledModel* top1((graph::CoupledModel*)cpled->findModel("top1"));
    graph::AtomicModel* x((graph::AtomicModel*)top1->findModel("x"));

    graph::TargetModelList out;
    x->getTargetPortList("out", out);

    BOOST_REQUIRE_EQUAL(out.size(), (graph::TargetModelList::size_type)10);
}

BOOST_AUTO_TEST_CASE(test_read_write_read)
{
    vpz::Vpz vpz;
    vpz.parse_file(utils::Path::path().build_prefix_share_path("examples", "coupled.vpz"));
    vpz.clear();
    vpz.parse_file(utils::Path::path().build_prefix_share_path("examples", "coupled.vpz"));
}
