/**
 * @file vle/src/examples/test/testfsa.cpp
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
#define BOOST_TEST_MODULE test_fsa_extension

#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <vle/manager.hpp>
#include <vle/value.hpp>
#include <vle/vpz.hpp>
#include <vle/utils.hpp>

struct F
{
    F() { vle::manager::init(); }
    ~F() { vle::manager::finalize(); }
};

BOOST_GLOBAL_FIXTURE(F)

using namespace vle;

BOOST_AUTO_TEST_CASE(test_moore1)
{
    vpz::Vpz file(utils::Path::path().getExampleFile("moore.vpz"));

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
                       (value::MatrixView::size_type)4);
    BOOST_REQUIRE_EQUAL(result.shape()[1],
                        (value::MatrixView::size_type)101);

    BOOST_REQUIRE_EQUAL(value::toString(result[1][0]), "a");
    BOOST_REQUIRE_EQUAL(value::toString(result[2][0]), "1");
    BOOST_REQUIRE_EQUAL(value::toString(result[3][0]), "1");

    BOOST_REQUIRE_EQUAL(value::toString(result[1][14]), "c");
    BOOST_REQUIRE_EQUAL(value::toString(result[2][14]), "3");
    BOOST_REQUIRE_EQUAL(value::toString(result[3][14]), "3");

    BOOST_REQUIRE_EQUAL(value::toString(result[1][22]), "c");
    BOOST_REQUIRE_EQUAL(value::toString(result[2][22]), "2");
    BOOST_REQUIRE_EQUAL(value::toString(result[3][22]), "2");

    BOOST_REQUIRE_EQUAL(value::toString(result[1][100]), "c");
    BOOST_REQUIRE_EQUAL(value::toString(result[2][100]), "2");
    BOOST_REQUIRE_EQUAL(value::toString(result[3][100]), "2");
}

BOOST_AUTO_TEST_CASE(test_moore2)
{
    vpz::Vpz file(utils::Path::path().getExampleFile("moore2.vpz"));

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
                       (value::MatrixView::size_type)4);
    BOOST_REQUIRE_EQUAL(result.shape()[1],
                        (value::MatrixView::size_type)101);

    BOOST_REQUIRE_EQUAL(value::toString(result[1][0]), "a");
    BOOST_REQUIRE_EQUAL(value::toInteger(result[2][0]), 0);
    BOOST_REQUIRE_EQUAL(value::toString(result[3][0]), "1");

    BOOST_REQUIRE_EQUAL(value::toString(result[1][100]), "a");
    BOOST_REQUIRE_EQUAL(value::toInteger(result[2][100]), 9);
    BOOST_REQUIRE_EQUAL(value::toString(result[3][100]), "2");
}

BOOST_AUTO_TEST_CASE(test_mealy1)
{
    vpz::Vpz file(utils::Path::path().getExampleFile("mealy.vpz"));

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
                       (value::MatrixView::size_type)4);
    BOOST_REQUIRE_EQUAL(result.shape()[1],
                        (value::MatrixView::size_type)101);

    BOOST_REQUIRE_EQUAL(value::toString(result[1][0]), "a");
    BOOST_REQUIRE_EQUAL(value::toString(result[2][0]), "1");
    BOOST_REQUIRE_EQUAL(value::toString(result[3][0]), "1");

    BOOST_REQUIRE_EQUAL(value::toString(result[1][14]), "a");
    BOOST_REQUIRE_EQUAL(value::toString(result[2][14]), "1");
    BOOST_REQUIRE_EQUAL(value::toString(result[3][14]), "2");

    BOOST_REQUIRE_EQUAL(value::toString(result[1][22]), "a");
    BOOST_REQUIRE_EQUAL(value::toString(result[2][22]), "1");
    BOOST_REQUIRE_EQUAL(value::toString(result[3][22]), "1");

    BOOST_REQUIRE_EQUAL(value::toString(result[1][61]), "c");
    BOOST_REQUIRE_EQUAL(value::toString(result[2][61]), "2");
    BOOST_REQUIRE_EQUAL(value::toString(result[3][61]), "3");

    BOOST_REQUIRE_EQUAL(value::toString(result[1][100]), "a");
    BOOST_REQUIRE_EQUAL(value::toString(result[2][100]), "1");
    BOOST_REQUIRE_EQUAL(value::toString(result[3][100]), "1");
}

BOOST_AUTO_TEST_CASE(test_mealy2)
{
    vpz::Vpz file(utils::Path::path().getExampleFile("mealy2.vpz"));

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
                       (value::MatrixView::size_type)4);
    BOOST_REQUIRE_EQUAL(result.shape()[1],
                        (value::MatrixView::size_type)101);

    BOOST_REQUIRE_EQUAL(value::toString(result[1][0]), "a");
    BOOST_REQUIRE_EQUAL(value::toInteger(result[2][0]), 0);
    BOOST_REQUIRE_EQUAL(value::toString(result[3][0]), "1");

    BOOST_REQUIRE_EQUAL(value::toString(result[1][100]), "a");
    BOOST_REQUIRE_EQUAL(value::toInteger(result[2][100]), 9);
    BOOST_REQUIRE_EQUAL(value::toString(result[3][100]), "2");
}

BOOST_AUTO_TEST_CASE(test_statechart1)
{
    vpz::Vpz file(utils::Path::path().getExampleFile("statechart.vpz"));

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
                       (value::MatrixView::size_type)4);
    BOOST_REQUIRE_EQUAL(result.shape()[1],
                        (value::MatrixView::size_type)101);

    BOOST_REQUIRE_EQUAL(value::toString(result[1][0]), "a");
    BOOST_REQUIRE_EQUAL(value::toString(result[2][0]), "1");
    BOOST_REQUIRE_EQUAL(value::toString(result[3][0]), "1");

    BOOST_REQUIRE_EQUAL(value::toString(result[1][100]), "a");
    BOOST_REQUIRE_EQUAL(value::toString(result[2][100]), "1");
    BOOST_REQUIRE_EQUAL(value::toString(result[3][100]), "1");
}

BOOST_AUTO_TEST_CASE(test_statechart2)
{
    vpz::Vpz file(utils::Path::path().getExampleFile("statechart2.vpz"));

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
                        (value::MatrixView::size_type)1001);

    BOOST_REQUIRE_EQUAL(value::toInteger(result[1][0]), 0);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][0]), 2.45601, 10e-2);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][0]), 3.0058, 10e-2);
    BOOST_REQUIRE_EQUAL(value::toString(result[4][0]), "2");

    BOOST_REQUIRE_EQUAL(value::toInteger(result[1][25]), 0);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][25]), 6.91764, 10e-2);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][25]), 3.0058, 10e-2);
    BOOST_REQUIRE_EQUAL(value::toString(result[4][25]), "3");

    BOOST_REQUIRE_EQUAL(value::toInteger(result[1][1000]), 0);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][1000]), 102.532, 10e-2);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][1000]), 100.913, 10e-2);
    BOOST_REQUIRE_EQUAL(value::toString(result[4][1000]), "4");
}

BOOST_AUTO_TEST_CASE(test_statechart3)
{
    vpz::Vpz file(utils::Path::path().getExampleFile("statechart3.vpz"));

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
                        (value::MatrixView::size_type)1001);

    BOOST_REQUIRE_EQUAL(value::toInteger(result[1][0]), 0);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][0]), 2.45601, 10e-2);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][0]), 3.0058, 10e-2);
    BOOST_REQUIRE_EQUAL(value::toString(result[4][0]), "2");

    BOOST_REQUIRE_EQUAL(value::toInteger(result[1][25]), 1);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][25]), 6.91764, 10e-2);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][25]), 3.0058, 10e-2);
    BOOST_REQUIRE_EQUAL(value::toString(result[4][25]), "3");

    BOOST_REQUIRE_EQUAL(value::toInteger(result[1][1000]), 29);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][1000]), 102.532, 10e-2);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][1000]), 100.913, 10e-2);
    BOOST_REQUIRE_EQUAL(value::toString(result[4][1000]), "4");
}

BOOST_AUTO_TEST_CASE(test_statechart4)
{
    vpz::Vpz file(utils::Path::path().getExampleFile("statechart4.vpz"));

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
                       (value::MatrixView::size_type)7);
    BOOST_REQUIRE_EQUAL(result.shape()[1],
                        (value::MatrixView::size_type)1001);

    BOOST_REQUIRE_EQUAL(value::toInteger(result[1][0]), 0);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][0]), 2.45601, 10e-2);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][0]), 3.0058, 10e-2);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][0]), 4.46163, 10e-2);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[5][0]), 0., 10e-2);
    BOOST_REQUIRE_EQUAL(value::toString(result[6][0]), "2");

    BOOST_REQUIRE_EQUAL(value::toInteger(result[1][25]), 1);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][25]), 5.53795, 10e-2);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][25]), 3.0058, 10e-2);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][25]), 4.46163, 10e-2);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[5][25]), 0., 10e-2);
    BOOST_REQUIRE_EQUAL(value::toString(result[6][25]), "3");

    BOOST_REQUIRE_EQUAL(value::toInteger(result[1][523]), 16);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][523]), 55.9634, 10e-2);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][523]), 52.9811, 10e-2);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][523]), 53.5509, 10e-2);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[5][523]), 29., 10e-2);
    BOOST_REQUIRE_EQUAL(value::toString(result[6][523]), "4");

    BOOST_REQUIRE_EQUAL(value::toInteger(result[1][1000]), 29);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][1000]), 100.174, 10e-2);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][1000]), 100.985, 10e-2);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][1000]), 101.733, 10e-2);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[5][1000]), 57., 10e-2);
    BOOST_REQUIRE_EQUAL(value::toString(result[6][1000]), "2");
}

BOOST_AUTO_TEST_CASE(test_statechart5)
{
    vpz::Vpz file(utils::Path::path().getExampleFile("statechart5.vpz"));

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
                        (value::MatrixView::size_type)1001);

    BOOST_REQUIRE_EQUAL(value::toInteger(result[1][0]), 0);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][0]), 2.45601, 10e-2);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][0]), 3.0058, 10e-2);
    BOOST_REQUIRE_EQUAL(value::toString(result[4][0]), "2");

    BOOST_REQUIRE_EQUAL(value::toInteger(result[1][25]), 0);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][25]), 6.91764, 10e-2);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][25]), 3.0058, 10e-2);
    BOOST_REQUIRE_EQUAL(value::toString(result[4][25]), "3");

    BOOST_REQUIRE_EQUAL(value::toInteger(result[1][609]), 0);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][609]), 63.8111, 10e-2);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][609]), 61.0465, 10e-2);
    BOOST_REQUIRE_EQUAL(value::toString(result[4][609]), "5");

    BOOST_REQUIRE_EQUAL(value::toInteger(result[1][1000]), 0);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][1000]), 102.532, 10e-2);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][1000]), 100.913, 10e-2);
    BOOST_REQUIRE_EQUAL(value::toString(result[4][1000]), "2");
}

BOOST_AUTO_TEST_CASE(test_statechart6)
{
    vpz::Vpz file(utils::Path::path().getExampleFile("statechart6.vpz"));

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
                        (value::MatrixView::size_type)1001);

    BOOST_REQUIRE_EQUAL(value::toInteger(result[1][0]), 0);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][0]), 2.45601, 10e-2);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][0]), 3.0058, 10e-2);
    BOOST_REQUIRE_EQUAL(value::toString(result[4][0]), "2");

    BOOST_REQUIRE_EQUAL(value::toInteger(result[1][25]), 0);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][25]), 6.91764, 10e-2);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][25]), 3.0058, 10e-2);
    BOOST_REQUIRE_EQUAL(value::toString(result[4][25]), "3");

    BOOST_REQUIRE_EQUAL(value::toInteger(result[1][690]), 0);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][690]), 69.8031, 10e-2);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][690]), 70.1079, 10e-2);
    BOOST_REQUIRE_EQUAL(value::toString(result[4][690]), "5");

    BOOST_REQUIRE_EQUAL(value::toInteger(result[1][1000]), 0);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][1000]), 102.532, 10e-2);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][1000]), 100.913, 10e-2);
    BOOST_REQUIRE_EQUAL(value::toString(result[4][1000]), "4");
}

BOOST_AUTO_TEST_CASE(test_stage)
{
    vpz::Vpz file(utils::Path::path().getExampleFile("stage.vpz"));

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
                       (value::MatrixView::size_type)3);
    BOOST_REQUIRE_EQUAL(result.shape()[1],
                        (value::MatrixView::size_type)331);

    BOOST_REQUIRE_EQUAL(value::toString(result[2][0]), "1");
    BOOST_REQUIRE_EQUAL(value::toString(result[2][186]), "2");
    BOOST_REQUIRE_EQUAL(value::toString(result[2][193]), "3");
    BOOST_REQUIRE_EQUAL(value::toString(result[2][202]), "4");
    BOOST_REQUIRE_EQUAL(value::toString(result[2][282]), "5");
    BOOST_REQUIRE_EQUAL(value::toString(result[2][295]), "6");
    BOOST_REQUIRE_EQUAL(value::toString(result[2][304]), "7");
    BOOST_REQUIRE_EQUAL(value::toString(result[2][323]), "8");
    BOOST_REQUIRE_EQUAL(value::toString(result[2][330]), "9");
}
