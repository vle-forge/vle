/**
 * @file examples/test/testpetrinet.cpp
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
#define BOOST_TEST_MODULE test_petrinet_extension

#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/version.hpp>
#include <stdexcept>
#include <vle/manager.hpp>
#include <vle/vpz.hpp>
#include <vle/value.hpp>
#include <vle/utils.hpp>

struct F
{
    F() { vle::manager::init(); }
    ~F() { vle::manager::finalize(); }
};

BOOST_GLOBAL_FIXTURE(F)

using namespace vle;

BOOST_AUTO_TEST_CASE(test_petrinet_and)
{
    vpz::Vpz file(utils::Path::path().getExampleFile("petrinet-and.vpz"));

    vpz::Output& o(file.project().experiment().views().outputs().get("out"));
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

    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][10]), 0.75, 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][10]), 1.0, 10e-5);
}

BOOST_AUTO_TEST_CASE(test_petrinet_or)
{
    vpz::Vpz file(utils::Path::path().getExampleFile("petrinet-or.vpz"));

    vpz::Output& o(file.project().experiment().views().outputs().get("out"));
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

    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][10]), 0.75, 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][10]), 2.0, 10e-5);
}

BOOST_AUTO_TEST_CASE(test_petrinet_nand1)
{
    vpz::Vpz file(utils::Path::path().getExampleFile("petrinet-nand1.vpz"));

    vpz::Output& o(file.project().experiment().views().outputs().get("out"));
    o.setLocalStream("", "storage");

    manager::RunQuiet r;
    r.start(file);

    BOOST_REQUIRE_EQUAL(r.haveError(), false);
    oov::OutputMatrixViewList out(r.outputs());
    BOOST_REQUIRE_EQUAL(out.size(),
                        (oov::OutputMatrixViewList::size_type)1);

    oov::OutputMatrix& view(out["view"]);
    value::MatrixView result(view.values());

    BOOST_REQUIRE_EQUAL(result.shape()[0],
                        (value::MatrixView::size_type)5);
    BOOST_REQUIRE_EQUAL(result.shape()[1],
                        (value::MatrixView::size_type)11);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][10]), 0., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][10]), 0., 10e-5);
}

BOOST_AUTO_TEST_CASE(test_petrinet_nand2)
{
    vpz::Vpz file(utils::Path::path().getExampleFile("petrinet-nand2.vpz"));

    vpz::Output& o(file.project().experiment().views().outputs().get("out"));
    o.setLocalStream("", "storage");

    manager::RunQuiet r;
    r.start(file);

    BOOST_REQUIRE_EQUAL(r.haveError(), false);
    oov::OutputMatrixViewList out(r.outputs());
    BOOST_REQUIRE_EQUAL(out.size(),
                        (oov::OutputMatrixViewList::size_type)1);

    oov::OutputMatrix& view(out["view"]);
    value::MatrixView result(view.values());

    BOOST_REQUIRE_EQUAL(result.shape()[0],
                        (value::MatrixView::size_type)5);
    BOOST_REQUIRE_EQUAL(result.shape()[1],
                        (value::MatrixView::size_type)11);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][10]), 0.5, 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][10]), 1., 10e-5);
}

BOOST_AUTO_TEST_CASE(test_petrinet_and_timed)
{
    vpz::Vpz file(utils::Path::path().getExampleFile("petrinet-and-timed.vpz"));

    vpz::Output& o(file.project().experiment().views().outputs().get("out"));
    o.setLocalStream("", "storage");

    manager::RunQuiet r;
    r.start(file);

    BOOST_REQUIRE_EQUAL(r.haveError(), false);
    oov::OutputMatrixViewList out(r.outputs());
    BOOST_REQUIRE_EQUAL(out.size(),
                        (oov::OutputMatrixViewList::size_type)1);

    oov::OutputMatrix& view(out["view"]);
    value::MatrixView result(view.values());

    BOOST_REQUIRE_EQUAL(result.shape()[0],
                        (value::MatrixView::size_type)5);
    BOOST_REQUIRE_EQUAL(result.shape()[1],
                        (value::MatrixView::size_type)20);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][19]), 1.7, 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][19]), 3., 10e-5);
}

BOOST_AUTO_TEST_CASE(test_petrinet_or_priority)
{
    vpz::Vpz file(utils::Path::path().getExampleFile("petrinet-or-priority.vpz"));

    vpz::Output& o(file.project().experiment().views().outputs().get("out"));
    o.setLocalStream("", "storage");

    manager::RunQuiet r;
    r.start(file);

    BOOST_REQUIRE_EQUAL(r.haveError(), false);
    oov::OutputMatrixViewList out(r.outputs());
    BOOST_REQUIRE_EQUAL(out.size(),
                        (oov::OutputMatrixViewList::size_type)1);

    oov::OutputMatrix& view(out["view"]);
    value::MatrixView result(view.values());

    BOOST_REQUIRE_EQUAL(result.shape()[0],
                        (value::MatrixView::size_type)5);
    BOOST_REQUIRE_EQUAL(result.shape()[1],
                        (value::MatrixView::size_type)11);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][10]), 1., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][10]), 6., 10e-5);
}

BOOST_AUTO_TEST_CASE(test_petrinet_meteo)
{
    vpz::Vpz file(utils::Path::path().getExampleFile("petrinet-meteo.vpz"));

    vpz::Output& o(file.project().experiment().views().outputs().get("out"));
    o.setLocalStream("", "storage");

    manager::RunQuiet r;
    r.start(file);

    BOOST_REQUIRE_EQUAL(r.haveError(), false);
    oov::OutputMatrixViewList out(r.outputs());
    BOOST_REQUIRE_EQUAL(out.size(),
                        (oov::OutputMatrixViewList::size_type)1);

    oov::OutputMatrix& view(out["view"]);
    value::MatrixView result(view.values());

    BOOST_REQUIRE_EQUAL(result.shape()[0],
                        (value::MatrixView::size_type)6);
    BOOST_REQUIRE_EQUAL(result.shape()[1],
                        (value::MatrixView::size_type)31);

#if BOOST_VERSION < 104000
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][30]), 29., 10e-5);
#else
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][30]), 28., 10e-5);
#endif
    BOOST_REQUIRE_CLOSE(value::toDouble(result[5][30]), 4., 10e-5);
}

BOOST_AUTO_TEST_CASE(test_petrinet_inout)
{
    vpz::Vpz file(utils::Path::path().getExampleFile("petrinet-inout.vpz"));

    vpz::Output& o(file.project().experiment().views().outputs().get("out"));
    o.setLocalStream("", "storage");

    manager::RunQuiet r;
    r.start(file);

    BOOST_REQUIRE_EQUAL(r.haveError(), false);
    oov::OutputMatrixViewList out(r.outputs());
    BOOST_REQUIRE_EQUAL(out.size(),
                        (oov::OutputMatrixViewList::size_type)1);

    oov::OutputMatrix& view(out["view"]);
    value::MatrixView result(view.values());

    BOOST_REQUIRE_EQUAL(result.shape()[0],
                        (value::MatrixView::size_type)5);
    BOOST_REQUIRE_EQUAL(result.shape()[1],
                        (value::MatrixView::size_type)11);

    BOOST_REQUIRE_CLOSE((double)value::toInteger(result[2][10]), 6., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][10]), 1., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][10]), 6., 10e-5);
}

BOOST_AUTO_TEST_CASE(test_petrinet_conflict)
{
    vpz::Vpz file(utils::Path::path().getExampleFile("petrinet-conflict.vpz"));

    vpz::Output& o(file.project().experiment().views().outputs().get("out"));
    o.setLocalStream("", "storage");

    manager::RunQuiet r;
    r.start(file);

    BOOST_REQUIRE_EQUAL(r.haveError(), false);
    oov::OutputMatrixViewList out(r.outputs());
    BOOST_REQUIRE_EQUAL(out.size(),
                        (oov::OutputMatrixViewList::size_type)1);

    oov::OutputMatrix& view(out["view"]);
    value::MatrixView result(view.values());

    BOOST_REQUIRE_EQUAL(result.shape()[0],
                        (value::MatrixView::size_type)5);
    BOOST_REQUIRE_EQUAL(result.shape()[1],
                        (value::MatrixView::size_type)11);

    BOOST_REQUIRE_CLOSE((double)value::toInteger(result[2][10]), 1., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][10]), 1., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][10]), 5., 10e-5);
}
