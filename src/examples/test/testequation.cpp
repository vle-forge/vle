/**
 * @file vle/src/examples/test/testequation.cpp
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
#define BOOST_TEST_MODULE test_difference_equation_extension

#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <vle/manager.hpp>
#include <vle/vpz.hpp>
#include <vle/value.hpp>
#include <vle/utils.hpp>

using namespace vle;

BOOST_AUTO_TEST_CASE(test_equation1)
{
    vpz::Vpz file(utils::Path::buildPrefixSharePath(
            utils::Path::path().getPrefixDir(), "examples", "equation1.vpz"));

    vpz::Output& o(file.project().experiment().views().outputs().get("o"));
    o.setLocalStream("", "storage");

    manager::RunQuiet r;
    r.start(file);

    BOOST_REQUIRE_EQUAL(r.haveError(), false);
    oov::OutputMatrixViewList& out(r.outputs());
    BOOST_REQUIRE_EQUAL(out.size(),
                        (oov::OutputMatrixViewList::size_type)1);

    oov::OutputMatrix& view(out["view"]);
    value::MatrixView result(view.values());

    BOOST_REQUIRE_EQUAL(result.shape()[0],
                        (value::MatrixView::size_type)6);
    BOOST_REQUIRE_EQUAL(result.shape()[1],
                        (value::MatrixView::size_type)11);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][10]), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][10]), 52., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][10]), 146., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][10]), 461., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[5][10]), 669., 10e-5);
}

BOOST_AUTO_TEST_CASE(test_equation2)
{
    vpz::Vpz file(utils::Path::buildPrefixSharePath(
            utils::Path::path().getPrefixDir(), "examples", "equation2.vpz"));

    vpz::Output& o(file.project().experiment().views().outputs().get("o"));
    o.setLocalStream("", "storage");

    manager::RunQuiet r;
    r.start(file);

    BOOST_REQUIRE_EQUAL(r.haveError(), false);
    oov::OutputMatrixViewList& out(r.outputs());
    BOOST_REQUIRE_EQUAL(out.size(),
                        (oov::OutputMatrixViewList::size_type)1);

    oov::OutputMatrix& view(out["view"]);
    value::MatrixView result(view.values());

    BOOST_REQUIRE_EQUAL(result.shape()[0],
                        (value::MatrixView::size_type)5);
    BOOST_REQUIRE_EQUAL(result.shape()[1],
                        (value::MatrixView::size_type)11);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][10]), 2045., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][10]), 4092., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][10]), 4094., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][10]), 4093., 10e-5);
}

BOOST_AUTO_TEST_CASE(test_equation3)
{
    vpz::Vpz file(utils::Path::buildPrefixSharePath(
            utils::Path::path().getPrefixDir(), "examples", "equation3.vpz"));

    vpz::Output& o(file.project().experiment().views().outputs().get("o"));
    o.setLocalStream("", "storage");

    manager::RunQuiet r;
    r.start(file);

    BOOST_REQUIRE_EQUAL(r.haveError(), false);
    oov::OutputMatrixViewList& out(r.outputs());
    BOOST_REQUIRE_EQUAL(out.size(),
                        (oov::OutputMatrixViewList::size_type)1);

    oov::OutputMatrix& view(out["view"]);
    value::MatrixView result(view.values());

    BOOST_REQUIRE_EQUAL(result.shape()[0],
                        (value::MatrixView::size_type)6);
    BOOST_REQUIRE_EQUAL(result.shape()[1],
                        (value::MatrixView::size_type)11);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][10]), 461., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][10]), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][10]), 52., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][10]), 146., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[5][10]), 669., 10e-5);
}

BOOST_AUTO_TEST_CASE(test_equation4)
{
    vpz::Vpz file(utils::Path::buildPrefixSharePath(
            utils::Path::path().getPrefixDir(), "examples", "equation5.vpz"));

    vpz::Output& o(file.project().experiment().views().outputs().get("o"));
    o.setLocalStream("", "storage");

    manager::RunQuiet r;
    r.start(file);

    BOOST_REQUIRE_EQUAL(r.haveError(), false);
    oov::OutputMatrixViewList& out(r.outputs());
    BOOST_REQUIRE_EQUAL(out.size(),
                        (oov::OutputMatrixViewList::size_type)1);

    oov::OutputMatrix& view(out["view"]);
    value::MatrixView result(view.values());

    BOOST_REQUIRE_EQUAL(result.shape()[0],
                        (value::MatrixView::size_type)6);
    BOOST_REQUIRE_EQUAL(result.shape()[1],
                        (value::MatrixView::size_type)11);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][10]), 626., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][10]), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][10]), 55., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][10]), 176., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[5][10]), 867., 10e-5);
}

