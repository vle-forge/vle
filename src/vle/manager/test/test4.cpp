/*
 * @file vle/manager/test/test4.cpp
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
#define BOOST_TEST_MODULE manager_thread

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

struct F
{
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

BOOST_AUTO_TEST_CASE(manager_thread_result_access)
{
    using namespace manager;

    vpz::Vpz file(utils::Path::path().getExampleFile("unittest.vpz"));

    file.project().experiment().setCombination("linear");
    file.project().experiment().replicas().setSeed(123);
    file.project().experiment().replicas().setNumber(2);
    file.project().experiment().setDuration(1.0);

    vpz::Conditions& cnds(file.project().experiment().conditions());
    cnds.get("ca").clearValueOfPort("x");
    cnds.get("cb").clearValueOfPort("x");
    cnds.get("cc").clearValueOfPort("x");
    cnds.get("cd").clearValueOfPort("x");

    cnds.get("ca").addValueToPort("x", value::Double::create(1.0));
    cnds.get("ca").addValueToPort("x", value::Double::create(2.0));
    cnds.get("cb").addValueToPort("x", value::Double::create(3.0));
    cnds.get("cb").addValueToPort("x", value::Double::create(4.0));
    cnds.get("cc").addValueToPort("x", value::Double::create(5.0));
    cnds.get("cc").addValueToPort("x", value::Double::create(6.0));
    cnds.get("cd").addValueToPort("x", value::Double::create(7.0));
    cnds.get("cd").addValueToPort("x", value::Double::create(8.0));

    //set output
    vle::vpz::Views& vle_views = file.project().experiment().views();
    vle::vpz::Output& view1 = vle_views.outputs().get(vle_views.get("view1").output());
    if (view1.plugin() != "storage") {
        view1.setLocalStream("", "storage");
    }
    vle::vpz::Output& view2 = vle_views.outputs().get(vle_views.get("view2").output());
    if (view2.plugin() != "storage") {
        view2.setLocalStream("", "storage");
    }

    bool writefile = false;
    bool storecomb = true;
    bool commonseed = true;
    bool nbthreads = 2;

    ManagerRunThread r(std::cout, writefile, nbthreads, storecomb, commonseed);
    r.start(file);

    //check total number of simulations : replicas*combinations
    const OutputSimulationMatrix& out(r.outputSimulationMatrix());
    BOOST_REQUIRE_EQUAL(out.shape()[0],
            (OutputSimulationMatrix::size_type)2);
    BOOST_REQUIRE_EQUAL(out.shape()[1],
            (OutputSimulationMatrix::size_type)2);

    //check valid access results
    for (OutputSimulationMatrix::size_type x(0); x < out.shape()[0]; ++x) {
        for (OutputSimulationMatrix::size_type y = 0; y < out.shape()[1]; ++y) {
            typedef value::MatrixView::size_type tmp_type;

            //check number of views
            BOOST_REQUIRE_EQUAL(out[x][y].size(),
                    (oov::OutputMatrixViewList::size_type)2);

            //check view1
            tmp_type i, j;
            oov::OutputMatrixViewList::const_iterator it(out[x][y].begin());
            i = it->second.values().shape()[0];
            j = it->second.values().shape()[1];

            BOOST_REQUIRE_EQUAL(i, (tmp_type)(5));
            BOOST_REQUIRE_EQUAL(j, (tmp_type)(2));

            //check view 2
            ++it;
            i = it->second.values().shape()[0];
            j = it->second.values().shape()[1];

            BOOST_REQUIRE_EQUAL(i, (tmp_type)(4));
            BOOST_REQUIRE_EQUAL(j, (tmp_type)(2));

        }
    }
    delete file.project().model().model();
}

BOOST_AUTO_TEST_CASE(manager_thread_fast_producer)
{
    using namespace manager;

    vpz::Vpz file(utils::Path::path().getExampleFile("unittest.vpz"));

    file.project().experiment().setCombination("total");
    file.project().experiment().replicas().setSeed(123);
    file.project().experiment().replicas().setNumber(2);
    file.project().experiment().setDuration(100.0);

    vpz::Conditions& cnds(file.project().experiment().conditions());
    cnds.get("ca").clearValueOfPort("x");
    cnds.get("cb").clearValueOfPort("x");
    cnds.get("cc").clearValueOfPort("x");
    cnds.get("cd").clearValueOfPort("x");

    cnds.get("ca").addValueToPort("x", value::Double::create(1.0));
    cnds.get("ca").addValueToPort("x", value::Double::create(2.0));
    cnds.get("cb").addValueToPort("x", value::Double::create(3.0));
    cnds.get("cb").addValueToPort("x", value::Double::create(4.0));
    cnds.get("cc").addValueToPort("x", value::Double::create(5.0));
    cnds.get("cc").addValueToPort("x", value::Double::create(6.0));
    cnds.get("cd").addValueToPort("x", value::Double::create(7.0));
    cnds.get("cd").addValueToPort("x", value::Double::create(8.0));

    //set output
    vle::vpz::Views& vle_views = file.project().experiment().views();
    vle::vpz::Output& view1 = vle_views.outputs().get(vle_views.get("view1").output());
    if (view1.plugin() != "storage") {
        view1.setLocalStream("", "storage");
    }
    vle::vpz::Output& view2 = vle_views.outputs().get(vle_views.get("view2").output());
    if (view2.plugin() != "storage") {
        view2.setLocalStream("", "storage");
    }

    bool writefile = false;
    bool storecomb = true;
    bool commonseed = true;
    bool nbthreads = 1;

    ManagerRunThread r(std::cout, writefile, nbthreads, storecomb, commonseed);
    r.start(file);

    //check total number of simulations : replicas*combinations
    const OutputSimulationMatrix& out(r.outputSimulationMatrix());
    BOOST_REQUIRE_EQUAL(out.shape()[0],
            (OutputSimulationMatrix::size_type)2);
    BOOST_REQUIRE_EQUAL(out.shape()[1],
            (OutputSimulationMatrix::size_type)16);


    delete file.project().model().model();
}

BOOST_AUTO_TEST_CASE(manager_thread_fast_consumer)
{
    using namespace manager;

    vpz::Vpz file(utils::Path::path().getExampleFile("unittest.vpz"));

    file.project().experiment().setCombination("linear");
    file.project().experiment().replicas().setSeed(123);
    file.project().experiment().replicas().setNumber(1);
    file.project().experiment().setDuration(1.0);

    vpz::Conditions& cnds(file.project().experiment().conditions());
    cnds.get("ca").clearValueOfPort("x");
    cnds.get("cb").clearValueOfPort("x");
    cnds.get("cc").clearValueOfPort("x");
    cnds.get("cd").clearValueOfPort("x");

    cnds.get("ca").addValueToPort("x", value::Double::create(1.0));
    cnds.get("ca").addValueToPort("x", value::Double::create(2.0));
    cnds.get("cb").addValueToPort("x", value::Double::create(3.0));
    cnds.get("cb").addValueToPort("x", value::Double::create(4.0));
    cnds.get("cc").addValueToPort("x", value::Double::create(5.0));
    cnds.get("cc").addValueToPort("x", value::Double::create(6.0));
    cnds.get("cd").addValueToPort("x", value::Double::create(7.0));
    cnds.get("cd").addValueToPort("x", value::Double::create(8.0));

    //set output
    vle::vpz::Views& vle_views = file.project().experiment().views();
    vle::vpz::Output& view1 = vle_views.outputs().get(vle_views.get("view1").output());
    if (view1.plugin() != "storage") {
        view1.setLocalStream("", "storage");
    }
    vle::vpz::Output& view2 = vle_views.outputs().get(vle_views.get("view2").output());
    if (view2.plugin() != "storage") {
        view2.setLocalStream("", "storage");
    }

    bool writefile = false;
    bool storecomb = true;
    bool commonseed = true;
    bool nbthreads = 10;

    ManagerRunThread r(std::cout, writefile, nbthreads, storecomb, commonseed);
    r.start(file);

    //check total number of simulations : replicas*combinations
    const OutputSimulationMatrix& out(r.outputSimulationMatrix());
    BOOST_REQUIRE_EQUAL(out.shape()[0],
            (OutputSimulationMatrix::size_type)1);
    BOOST_REQUIRE_EQUAL(out.shape()[1],
            (OutputSimulationMatrix::size_type)2);

    delete file.project().model().model();
}

