/*
 * @file vle/manager/test/test2.cpp
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
#define BOOST_TEST_MODULE manager_combination_storage_test

#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <iostream>
#include <vle/manager/Manager.hpp>
#include <vle/manager/VLE.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/utils/Path.hpp>

struct F {
    F()
    {
        vle::manager::init();
    }

    ~F()
    {
        vle::manager::finalize();
    }

};

BOOST_GLOBAL_FIXTURE(F)

using namespace vle;

BOOST_AUTO_TEST_CASE(combination_storage_test){
    using namespace manager;

    vpz::Vpz file(utils::Path::path().getExampleFile("unittest.vpz"));

    file.project().experiment().setCombination("linear");
    file.project().experiment().replicas().setSeed(123);
    file.project().experiment().replicas().setNumber(1);

    vpz::Conditions& cnds(file.project().experiment().conditions());
    cnds.get("ca").clearValueOfPort("x");
    cnds.get("cb").clearValueOfPort("x");
    cnds.get("cc").clearValueOfPort("x");
    cnds.get("cd").clearValueOfPort("x");

    cnds.get("ca").addValueToPort("x", value::Double::create(1.0));
    cnds.get("ca").addValueToPort("x", value::Double::create(2.0));
    cnds.get("cb").addValueToPort("x", value::Double::create(3.0));
    cnds.get("cc").addValueToPort("x", value::Double::create(4.0));
    cnds.get("cc").addValueToPort("x", value::Double::create(5.0));
    cnds.get("cd").addValueToPort("x", value::Double::create(6.0));

    bool writefile = false;
    bool storecomb = true;
    bool commonseed = true;

    ManagerRunMono r(std::cout, writefile, storecomb, commonseed);
    r.start(file);

    const OutputSimulationMatrix& out(r.outputSimulationMatrix());
    BOOST_REQUIRE_EQUAL(out.shape()[0],
                        (OutputSimulationMatrix::size_type)1);
    BOOST_REQUIRE_EQUAL(out.shape()[1],
                        (OutputSimulationMatrix::size_type)2);

    BOOST_REQUIRE_EQUAL(r.getCombinations().size(), 2);

    BOOST_REQUIRE_EQUAL(value::toDouble(r.getInputFromCombination(0, "ca", "x")), 1.0);
    BOOST_REQUIRE_EQUAL(value::toDouble(r.getInputFromCombination(0, "cb", "x")), 3.0);
    BOOST_REQUIRE_EQUAL(value::toDouble(r.getInputFromCombination(0, "cc", "x")), 4.0);
    BOOST_REQUIRE_EQUAL(value::toDouble(r.getInputFromCombination(0, "cd", "x")), 6.0);
    BOOST_REQUIRE_EQUAL(value::toDouble(r.getInputFromCombination(1, "ca", "x")), 2.0);
    BOOST_REQUIRE_EQUAL(value::toDouble(r.getInputFromCombination(1, "cb", "x")), 3.0);
    BOOST_REQUIRE_EQUAL(value::toDouble(r.getInputFromCombination(1, "cc", "x")), 5.0);
    BOOST_REQUIRE_EQUAL(value::toDouble(r.getInputFromCombination(1, "cd", "x")), 6.0);



    file.project().experiment().setCombination("total");
    ManagerRunMono r2(std::cout, writefile, storecomb, commonseed);
    r2.start(file);

    const OutputSimulationMatrix& out2(r2.outputSimulationMatrix());
    BOOST_REQUIRE_EQUAL(out2.shape()[0],
                        (OutputSimulationMatrix::size_type)1);
    BOOST_REQUIRE_EQUAL(out2.shape()[1],
                        (OutputSimulationMatrix::size_type)4);

    BOOST_REQUIRE_EQUAL(r2.getCombinations().size(), 4);

    BOOST_REQUIRE_EQUAL(value::toDouble(r2.getInputFromCombination(0, "ca", "x")), 1.0);
    BOOST_REQUIRE_EQUAL(value::toDouble(r2.getInputFromCombination(0, "cb", "x")), 3.0);
    BOOST_REQUIRE_EQUAL(value::toDouble(r2.getInputFromCombination(0, "cc", "x")), 4.0);
    BOOST_REQUIRE_EQUAL(value::toDouble(r2.getInputFromCombination(0, "cd", "x")), 6.0);
    BOOST_REQUIRE_EQUAL(value::toDouble(r2.getInputFromCombination(1, "ca", "x")), 1.0);
    BOOST_REQUIRE_EQUAL(value::toDouble(r2.getInputFromCombination(1, "cb", "x")), 3.0);
    BOOST_REQUIRE_EQUAL(value::toDouble(r2.getInputFromCombination(1, "cc", "x")), 5.0);
    BOOST_REQUIRE_EQUAL(value::toDouble(r2.getInputFromCombination(1, "cd", "x")), 6.0);
    BOOST_REQUIRE_EQUAL(value::toDouble(r2.getInputFromCombination(2, "ca", "x")), 2.0);
    BOOST_REQUIRE_EQUAL(value::toDouble(r2.getInputFromCombination(2, "cb", "x")), 3.0);
    BOOST_REQUIRE_EQUAL(value::toDouble(r2.getInputFromCombination(2, "cc", "x")), 4.0);
    BOOST_REQUIRE_EQUAL(value::toDouble(r2.getInputFromCombination(2, "cd", "x")), 6.0);
    BOOST_REQUIRE_EQUAL(value::toDouble(r2.getInputFromCombination(3, "ca", "x")), 2.0);
    BOOST_REQUIRE_EQUAL(value::toDouble(r2.getInputFromCombination(3, "cb", "x")), 3.0);
    BOOST_REQUIRE_EQUAL(value::toDouble(r2.getInputFromCombination(3, "cc", "x")), 5.0);
    BOOST_REQUIRE_EQUAL(value::toDouble(r2.getInputFromCombination(3, "cd", "x")), 6.0);

    delete file.project().model().model();
}

