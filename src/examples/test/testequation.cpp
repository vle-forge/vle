/*
 * @file examples/test/testequation.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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

struct F
{
    F() { vle::manager::init(); }
    ~F() { vle::manager::finalize(); }
};

BOOST_GLOBAL_FIXTURE(F)

using namespace vle;

BOOST_AUTO_TEST_CASE(test_equation1)
{
    vpz::Vpz file(utils::Path::path().getExampleFile("equation1.vpz"));

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
    vpz::Vpz file(utils::Path::path().getExampleFile("equation2.vpz"));

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
    vpz::Vpz file(utils::Path::path().getExampleFile("equation3.vpz"));

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
    vpz::Vpz file(utils::Path::path().getExampleFile("equation4.vpz"));

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

BOOST_AUTO_TEST_CASE(test_equation5)
{
    vpz::Vpz file(utils::Path::path().getExampleFile("equation5.vpz"));

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
                        (value::MatrixView::size_type)21);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][5]), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][5]), 12., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][5]), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][5]), 30., 10e-5);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][10]), 0., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][10]), 77., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][10]), 195., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][10]), 565., 10e-5);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][15]), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][15]), 92., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][15]), 605., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][15]), 2770., 10e-5);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][20]), 0., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][20]), 157., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][20]), 1190., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][20]), 7480., 10e-5);
}

BOOST_AUTO_TEST_CASE(test_equation6)
{
    vpz::Vpz file(utils::Path::path().getExampleFile("equation6.vpz"));

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
                        (value::MatrixView::size_type)21);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][5]), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][5]), 57., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][5]), 59., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][5]), 58., 10e-5);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][10]), 0., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][10]), 975., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][10]), 977., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][10]), 976., 10e-5);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][15]), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][15]), 15673., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][15]), 15675., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][15]), 15674., 10e-5);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][20]), 0., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][20]), 250831., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][20]), 250833., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][20]), 250832., 10e-5);
}

BOOST_AUTO_TEST_CASE(test_equation7)
{
    vpz::Vpz file(utils::Path::path().getExampleFile("equation7.vpz"));

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
                        (value::MatrixView::size_type)21);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][5]), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][5]), 22., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][5]), 41., 10e-5);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][10]), 0., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][10]), 77., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][10]), 341., 10e-5);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][15]), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][15]), 102., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][15]), 786., 10e-5);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][20]), 0., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][20]), 157., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][20]), 1486., 10e-5);
}

BOOST_AUTO_TEST_CASE(test_equation8)
{
    vpz::Vpz file(utils::Path::path().getExampleFile("equation8.vpz"));

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
                        (value::MatrixView::size_type)21);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][5]), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][5]), 22., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][5]), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][5]), 31., 10e-5);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][10]), 0., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][10]), 77., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][10]), 0., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][10]), 561., 10e-5);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][15]), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][15]), 102., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][15]), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][15]), 1411., 10e-5);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][20]), 0., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][20]), 157., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][20]), 0., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][20]), 2741., 10e-5);
}

BOOST_AUTO_TEST_CASE(test_equation9)
{
    vpz::Vpz file(utils::Path::path().getExampleFile("equation9.vpz"));

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
                        (value::MatrixView::size_type)21);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][5]), 11., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][5]), 18., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][5]), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][5]), 31., 10e-5);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][10]), 3., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][10]), 49., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][10]), 196., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][10]), 605., 10e-5);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][15]), 13., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][15]), 94., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][15]), 511., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][15]), 2500., 10e-5);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][20]), 11., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][20]), 115., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][20]), 1006., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][20]), 6541., 10e-5);
}

BOOST_AUTO_TEST_CASE(test_equation10)
{
    vpz::Vpz file(utils::Path::path().getExampleFile("equation10.vpz"));

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
                        (value::MatrixView::size_type)21);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][5]), 75., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][5]), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][5]), 15., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][5]), 25., 10e-5);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][10]), 730., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][10]), 0., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][10]), 80., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][10]), 225., 10e-5);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][15]), 3130., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][15]), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][15]), 95., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][15]), 650., 10e-5);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][20]), 8110., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][20]), 0., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][20]), 160., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][20]), 1250., 10e-5);
}

BOOST_AUTO_TEST_CASE(test_equation11)
{
    vpz::Vpz file(utils::Path::path().getExampleFile("equation11.vpz"));

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
                        (value::MatrixView::size_type)21);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][5]), 60., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][5]), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][5]), 15., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][5]), 10., 10e-5);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][10]), 430., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][10]), 0., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][10]), 80., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][10]), 0., 10e-5);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][15]), 1290., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][15]), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][15]), 95., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][15]), 10., 10e-5);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][20]), 2460., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][20]), 0., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][20]), 160., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][20]), 0., 10e-5);
}

BOOST_AUTO_TEST_CASE(test_equation12)
{
    vpz::Vpz file(utils::Path::path().getExampleFile("equation12.vpz"));

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
                        (value::MatrixView::size_type)21);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][5]), 7., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][5]), 12., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][5]), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][5]), 27., 10e-5);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][10]), 14., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][10]), 62., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][10]), 165., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][10]), 504., 10e-5);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][15]), 21., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][15]), 147., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][15]), 640., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][15]), 2681., 10e-5);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][20]), 28., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][20]), 267., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][20]), 1610., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][20]), 8683., 10e-5);
}

BOOST_AUTO_TEST_CASE(test_equation13)
{
    vpz::Vpz file(utils::Path::path().getExampleFile("equation13.vpz"));

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
                        (value::MatrixView::size_type)21);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][5]), 54., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][5]), 5., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][5]), 29., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][5]), 28., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[5][5]), 10., 10e-5);
}

BOOST_AUTO_TEST_CASE(test_equation14)
{
    vpz::Vpz file(utils::Path::path().getExampleFile("equation14.vpz"));

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
                        (value::MatrixView::size_type)21);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][5]), 54., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][5]), 5., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][5]), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][5]), 28., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[5][5]), 10., 10e-5);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][10]), 2470., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][10]), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][10]), 0., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][10]), 1396., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[5][10]), 0., 10e-5);
}

BOOST_AUTO_TEST_CASE(test_equation15)
{
    vpz::Vpz file(utils::Path::path().getExampleFile("equation15.vpz"));

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

    BOOST_REQUIRE_CLOSE(value::toDouble(result[1][10]), 450., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[2][10]), 10., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[3][10]), 52., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][10]), 145., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[5][10]), 657., 10e-5);
}

BOOST_AUTO_TEST_CASE(test_asynchronous)
{
    vpz::Vpz file(utils::Path::path().getExampleFile("asynchronous.vpz"));

    vpz::Output& o(file.project().experiment().views().outputs().get("view"));
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
        (value::MatrixView::size_type)6);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][0]), 0., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][1]), 5., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][2]), 6., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][3]), 6., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][4]), 7., 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][5]), 6., 10e-5);
}
