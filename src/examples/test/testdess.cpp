/*
 * @file examples/test/testdess.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
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
#define BOOST_TEST_MODULE test_dess_extension

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

BOOST_AUTO_TEST_CASE(test_dess1)
{
    vpz::Vpz file(utils::Path::path().getExampleFile("sir2.vpz"));

    vpz::Output& o(file.project().experiment().views().outputs().get("o3"));
    o.setLocalStream("", "storage", std::string());

    manager::RunQuiet r;
    r.start(file);

    BOOST_REQUIRE_EQUAL(r.haveError(), false);
    oov::OutputMatrixViewList& out(r.outputs());
    BOOST_REQUIRE_EQUAL(out.size(),
                        (oov::OutputMatrixViewList::size_type)1);

    oov::OutputMatrix& view(out["dessview"]);
    value::MatrixView result(view.values());

    BOOST_REQUIRE_EQUAL(result.shape()[0],
                        (value::MatrixView::size_type)6);
    BOOST_REQUIRE_EQUAL(result.shape()[1],
                        (value::MatrixView::size_type)2000);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][319]), 3.185, 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[5][319]), 1.0, 10e-5);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][483]), 4.82, 10e-5);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[5][483]), 2.0, 10e-5);
}

BOOST_AUTO_TEST_CASE(test_dess2)
{
    vpz::Vpz file(utils::Path::path().getExampleFile("sir3.vpz"));

    vpz::Output& o(file.project().experiment().views().outputs().get("o5"));
    o.setLocalStream("", "storage", std::string());

    manager::RunQuiet r;
    r.start(file);

    BOOST_REQUIRE_EQUAL(r.haveError(), false);
    oov::OutputMatrixViewList& out(r.outputs());
    BOOST_REQUIRE_EQUAL(out.size(),
                        (oov::OutputMatrixViewList::size_type)1);

    oov::OutputMatrix& view(out["dessview"]);
    value::MatrixView result(view.values());

    BOOST_REQUIRE_EQUAL(result.shape()[0],
                        (value::MatrixView::size_type)6);
    BOOST_REQUIRE_EQUAL(result.shape()[1],
                        (value::MatrixView::size_type)2000);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][321]), 3.2030279, 10e-2);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[5][321]), 1.0, 10e-2);

    BOOST_REQUIRE_CLOSE(value::toDouble(result[4][483]), 4.8268177, 10e-2);
    BOOST_REQUIRE_CLOSE(value::toDouble(result[5][483]), 2.0, 10e-2);
}
