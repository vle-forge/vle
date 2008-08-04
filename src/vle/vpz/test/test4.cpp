/**
 * @file src/vle/vpz/test/test4.cpp
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

#include <vle/utils.hpp>
#include <vle/graph.hpp>
#include <vle/vpz.hpp>

using namespace vle;

BOOST_AUTO_TEST_CASE(test_connection)
{
    vpz::Vpz vpz;
    vpz.parseFile(utils::Path::path().buildPrefixSharePath(
            utils::Path::path().getPrefixDir(), "examples", "unittest.vpz"));

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

    delete vpz.project().model().model();
}

BOOST_AUTO_TEST_CASE(test_read_write_read)
{
    vpz::Vpz vpz;
    vpz.parseFile(utils::Path::buildPrefixSharePath(
            utils::Path::path().getPrefixDir(), "examples", "unittest.vpz"));
    delete vpz.project().model().model();
    vpz.clear();
    vpz.parseFile(utils::Path::path().buildPrefixSharePath(
            utils::Path::path().getPrefixDir(), "examples", "unittest.vpz"));
    delete vpz.project().model().model();
    vpz.clear();
    vpz.parseFile(utils::Path::path().buildPrefixSharePath(
            utils::Path::path().getPrefixDir(), "examples", "unittest.vpz"));
    delete vpz.project().model().model();
    vpz.clear();

    vpz.parseFile(utils::Path::path().buildPrefixSharePath(
            utils::Path::path().getPrefixDir(), "examples", "unittest.vpz"));

    std::string str(vpz.writeToString());
    delete vpz.project().model().model();
    vpz.clear();

    vpz.parseMemory(str);
}

BOOST_AUTO_TEST_CASE(test_read_write_read2)
{
    vpz::Vpz vpz;
    vpz.parseFile(utils::Path::buildPrefixSharePath(
            utils::Path::path().getPrefixDir(), "examples", "unittest.vpz"));
    vpz::Vpz vpz2(vpz);
    delete vpz.project().model().model();
    delete vpz2.project().model().model();
}
