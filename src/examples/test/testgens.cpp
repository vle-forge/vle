/**
 * @file examples/test/testgens.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
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
#define BOOST_TEST_MODULE test_gens_model

#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <vle/manager.hpp>
#include <vle/vpz.hpp>
#include <vle/value.hpp>
#include <vle/utils.hpp>
#include <iostream>

struct F
{
    F() { vle::manager::init(); }
    ~F() { vle::manager::finalize(); }
};

BOOST_GLOBAL_FIXTURE(F)

using namespace vle;

BOOST_AUTO_TEST_CASE(test_gens)
{
    /* load the $PREFIX/usr/share/examples/vle-x.y.z/gens.vpz vpz file. */
    vpz::Vpz file(utils::Path::path().getExampleFile("gens.vpz"));

    /* change the output text to storage output */
    vpz::Output& o(file.project().experiment().views().outputs().get("o"));
    vpz::Output& o2(file.project().experiment().views().outputs().get("o2"));
    o.setLocalStream("", "storage");
    o2.setLocalStream("", "storage");

    /* run the simulation */
    manager::RunQuiet r;
    r.start(file);

    /* begin check */
    BOOST_REQUIRE_EQUAL(r.haveError(), false);
    oov::OutputMatrixViewList& out(r.outputs());
    BOOST_REQUIRE_EQUAL(out.size(),
                        (oov::OutputMatrixViewList::size_type)2);

    /* get result of simulation */
    oov::OutputMatrix& view1(out["view1"]);

    /* check matrix */
    value::MatrixView result(view1.values());

    BOOST_REQUIRE_EQUAL(result.shape()[0],
                        (value::MatrixView::size_type)3);
    BOOST_REQUIRE_EQUAL(result.shape()[1],
                        (value::MatrixView::size_type)101);

    {
        value::VectorView vectorTime(view1.getTime());
        BOOST_REQUIRE_EQUAL(vectorTime.shape()[0], 101);
        BOOST_REQUIRE_EQUAL(vectorTime[vectorTime.shape()[0] -
                            1]->toDouble().value(), 100);
        BOOST_REQUIRE_EQUAL(view1.getLastTime(), 100.0);
    }


    BOOST_REQUIRE_EQUAL(value::toDouble(result[0][0]), 0);
    BOOST_REQUIRE_EQUAL(value::toDouble(result[1][0]), 0);
    BOOST_REQUIRE_EQUAL(value::toInteger(result[2][0]), 1);

    BOOST_REQUIRE_EQUAL(value::toDouble(result[0][10]), 10);
    BOOST_REQUIRE_EQUAL(value::toDouble(result[1][10]), 55);
    BOOST_REQUIRE_EQUAL(value::toInteger(result[2][10]), 11);

    BOOST_REQUIRE_EQUAL(value::toDouble(result[0][14]), 14);
    BOOST_REQUIRE_EQUAL(result[1][14], (value::Value*)0);
    BOOST_REQUIRE_EQUAL(value::toInteger(result[2][14]), 15);

    BOOST_REQUIRE_EQUAL(value::toDouble(result[0][31]), 31);
    BOOST_REQUIRE_EQUAL(result[1][31], (value::Value*)0);
    BOOST_REQUIRE_EQUAL(value::toInteger(result[2][31]), 32);

    BOOST_REQUIRE_EQUAL(value::toDouble(result[0][100]), 100);
    BOOST_REQUIRE_EQUAL(value::toDouble(result[1][100]), 2550);
    BOOST_REQUIRE_EQUAL(value::toInteger(result[2][100]), 1);

    vle::utils::finalize();
}

BOOST_AUTO_TEST_CASE(test_gens_with_class)
{
    /* load the $PREFIX/usr/share/examples/vle-x.y.z/gens.vpz vpz file. */
    vpz::Vpz* file = new vpz::Vpz(
        utils::Path::path().getExampleFile("genswithclass.vpz"));

    /* change the output text to storage output */
    vpz::Output& o(file->project().experiment().views().outputs().get("o"));
    vpz::Output& o2(file->project().experiment().views().outputs().get("o2"));
    o.setLocalStream("", "storage");
    o2.setLocalStream("", "storage");

    /* run the simulation */
    manager::RunVerbose r(std::cerr);
    r.start(file);
    file = 0;

    /* begin check */
    BOOST_REQUIRE_EQUAL(r.haveError(), false);
    oov::OutputMatrixViewList& out(r.outputs());
    BOOST_REQUIRE_EQUAL(out.size(),
                        (oov::OutputMatrixViewList::size_type)2);

    /* get result of simulation */
    oov::OutputMatrix& view1(out["view1"]);

    /* check matrix */
    value::MatrixView result(view1.values());

    BOOST_REQUIRE_EQUAL(result.shape()[0],
                        (value::MatrixView::size_type)3);
    BOOST_REQUIRE_EQUAL(result.shape()[1],
                        (value::MatrixView::size_type)101);

    BOOST_REQUIRE_EQUAL(value::toDouble(result[0][0]), 0);
    BOOST_REQUIRE_EQUAL(value::toDouble(result[1][0]), 0);
    BOOST_REQUIRE_EQUAL(value::toInteger(result[2][0]), 2);

    BOOST_REQUIRE_EQUAL(value::toDouble(result[0][5]), 5);
    BOOST_REQUIRE_EQUAL(value::toDouble(result[1][5]), 63);
    BOOST_REQUIRE_EQUAL(value::toInteger(result[2][5]), 7);

    BOOST_REQUIRE_EQUAL(value::toDouble(result[0][7]), 7);
    BOOST_REQUIRE_EQUAL(result[1][7], (value::Value*)0);
    BOOST_REQUIRE_EQUAL(value::toInteger(result[2][7]), 9);

    BOOST_REQUIRE_EQUAL(value::toDouble(result[0][15]), 15);
    BOOST_REQUIRE_EQUAL(value::toDouble(result[1][15]), 528);
    BOOST_REQUIRE_EQUAL(value::toInteger(result[2][15]), 17);

    BOOST_REQUIRE_EQUAL(value::toDouble(result[0][100]), 100);
    BOOST_REQUIRE_EQUAL(value::toDouble(result[1][100]), 11400);
    BOOST_REQUIRE_EQUAL(value::toInteger(result[2][100]), 2);

    vle::utils::finalize();
}
