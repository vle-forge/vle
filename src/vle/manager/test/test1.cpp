/**
 * @file vle/manager/test/test1.cpp
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
#define BOOST_TEST_MODULE manager_complete_test

#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <iostream>
#include <vle/manager.hpp>
#include <vle/vpz.hpp>
#include <vle/value.hpp>
#include <vle/utils.hpp>

using namespace vle;

BOOST_AUTO_TEST_CASE(build_experimental_frames)
{
    vpz::Vpz file(utils::Path::buildPrefixSharePath(
            utils::Path::path().getPrefixDir(),
            "examples", "unittest.vpz"));

    file.project().experiment().replicas().setSeed(123);
    file.project().experiment().replicas().setNumber(4);

    vpz::Conditions& cnds(file.project().experiment().conditions());
    cnds.get("ca").addValueToPort("x", value::Double::create(1.0));
    cnds.get("ca").addValueToPort("x", value::Double::create(2.0));

    using namespace manager;

    ManagerRunMono r(std::cout, false);
    r.start(file);

    const OutputSimulationMatrix& out(r.outputSimulationMatrix());
    BOOST_REQUIRE_EQUAL(out.shape()[0],
                        (OutputSimulationMatrix::size_type)4);
    BOOST_REQUIRE_EQUAL(out.shape()[1],
                        (OutputSimulationMatrix::size_type)3);

    for (OutputSimulationMatrix::size_type x(0); x < out.shape()[0]; ++x) {
        for (OutputSimulationMatrix::size_type y = 0; y < out.shape()[1]; ++y) {
            typedef value::MatrixView::size_type tmp_type ;

            BOOST_REQUIRE_EQUAL(out[x][y].size(),
                                (oov::OutputMatrixViewList::size_type)2);

            tmp_type i, j;

            oov::OutputMatrixViewList::const_iterator it(out[x][y].begin());
            i = it->second.values().shape()[0];
            j = it->second.values().shape()[1];

            BOOST_REQUIRE_EQUAL(i, (tmp_type)(5));
            BOOST_REQUIRE_EQUAL(j, (tmp_type)(101));

            ++it;
            i = it->second.values().shape()[0];
            j = it->second.values().shape()[1];
            BOOST_REQUIRE_EQUAL(i, (tmp_type)(4));
            BOOST_REQUIRE_EQUAL(j, (tmp_type)(101));
        }
    }

    delete file.project().model().model();
}

BOOST_AUTO_TEST_CASE(build_linear_combination_size)
{
    vpz::Vpz file(utils::Path::buildPrefixSharePath(
            utils::Path::path().getPrefixDir(),
            "examples", "unittest.vpz"));

    file.project().experiment().replicas().setSeed(123);
    file.project().experiment().replicas().setNumber(4);
    file.project().experiment().setCombination("linear");

    vpz::Conditions& cnds(file.project().experiment().conditions());
    cnds.get("ca").addValueToPort("x", value::Double::create(1.0));
    cnds.get("ca").addValueToPort("x", value::Double::create(2.0));
    cnds.get("ca").addValueToPort("x", value::Double::create(3.0));
    cnds.get("ca").addValueToPort("x", value::Double::create(4.0));

    cnds.get("cb").addValueToPort("x", value::Double::create(5.0));
    cnds.get("cb").addValueToPort("x", value::Double::create(6.0));
    cnds.get("cb").addValueToPort("x", value::Double::create(7.0));
    cnds.get("cb").addValueToPort("x", value::Double::create(8.0));

    cnds.get("cd").addValueToPort("x", value::Double::create(9.0));
    cnds.get("cd").addValueToPort("x", value::Double::create(10.0));

    using namespace manager;

    ManagerRunMono r(std::cout, false);
    BOOST_REQUIRE_THROW(r.start(file), std::runtime_error);
}


BOOST_AUTO_TEST_CASE(build_linear_output_matrix_size)
{
    vpz::Vpz file(utils::Path::buildPrefixSharePath(
            utils::Path::path().getPrefixDir(),
            "examples", "unittest.vpz"));

    file.project().experiment().replicas().setSeed(123);
    file.project().experiment().replicas().setNumber(2);
    file.project().experiment().setCombination("linear");

    vpz::Conditions& cnds(file.project().experiment().conditions());
    cnds.get("ca").addValueToPort("x", value::Double::create(1.0));
    cnds.get("ca").addValueToPort("x", value::Double::create(2.0));
    cnds.get("ca").addValueToPort("x", value::Double::create(3.0));
    cnds.get("ca").addValueToPort("x", value::Double::create(4.0));

    cnds.get("cb").addValueToPort("x", value::Double::create(6.0));
    cnds.get("cb").addValueToPort("x", value::Double::create(7.0));
    cnds.get("cb").addValueToPort("x", value::Double::create(8.0));
    cnds.get("cb").addValueToPort("x", value::Double::create(9.0));

    using namespace manager;

    ManagerRunMono r(std::cout, false);
    r.start(file);

    const OutputSimulationMatrix& out(r.outputSimulationMatrix());
    BOOST_REQUIRE_EQUAL(out.shape()[0],
                        (OutputSimulationMatrix::size_type)2);
    BOOST_REQUIRE_EQUAL(out.shape()[1],
                        (OutputSimulationMatrix::size_type)5);

    for (OutputSimulationMatrix::size_type x(0); x < out.shape()[0]; ++x) {
        for (OutputSimulationMatrix::size_type y = 0; y < out.shape()[1]; ++y) {
            typedef value::MatrixView::size_type tmp_type ;

            BOOST_REQUIRE_EQUAL(out[x][y].size(),
                                (oov::OutputMatrixViewList::size_type)2);

            tmp_type i, j;

            oov::OutputMatrixViewList::const_iterator it(out[x][y].begin());
            i = it->second.values().shape()[0];
            j = it->second.values().shape()[1];

            BOOST_REQUIRE_EQUAL(i, (tmp_type)(5));
            BOOST_REQUIRE_EQUAL(j, (tmp_type)(101));

            ++it;
            i = it->second.values().shape()[0];
            j = it->second.values().shape()[1];
            BOOST_REQUIRE_EQUAL(i, (tmp_type)(4));
            BOOST_REQUIRE_EQUAL(j, (tmp_type)(101));

            BOOST_REQUIRE_EQUAL(100, value::toDouble(
                    (out[x][y].begin())->second.values()[0][100]));
            BOOST_REQUIRE_EQUAL(101, value::toInteger(
                    (out[x][y].begin())->second.values()[1][100]));
            BOOST_REQUIRE_EQUAL(101, value::toInteger(
                    (out[x][y].begin())->second.values()[2][100]));
            BOOST_REQUIRE_EQUAL(201, value::toInteger(
                    (out[x][y].begin())->second.values()[3][100]));
        }
    }
}
