/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#define BOOST_TEST_MAIN
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE vpz_structures

#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <vle/value/Value.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/vle.hpp>
#include <stdexcept>


struct F
{
    vle::Init a;

    F() : a() { }
    ~F() { }
};

BOOST_GLOBAL_FIXTURE(F)

using namespace vle;
using namespace vpz;

BOOST_AUTO_TEST_CASE(vpz_obs_port)
{
    Observable obs("Obs");
    BOOST_REQUIRE_EQUAL(obs.exist("port"), false);
    BOOST_REQUIRE_THROW(obs.get("port"), utils::ArgError);
    BOOST_REQUIRE_NO_THROW(obs.del("port"));
    BOOST_REQUIRE_NO_THROW(obs.add("port"));
    BOOST_REQUIRE_THROW(obs.add("port"), utils::ArgError);
    BOOST_REQUIRE_EQUAL(obs.exist("port"), true);
    BOOST_REQUIRE_NO_THROW(obs.get("port"));
    BOOST_REQUIRE_NO_THROW(obs.del("port"));
    BOOST_REQUIRE_EQUAL(obs.exist("port"), false);
}

BOOST_AUTO_TEST_CASE(vpz_add_output)
{
    Views views;

    views.addLocalStreamOutput("out1", "", "storage", "");
    views.addLocalStreamOutput("out2", "", "storage", "");
    views.addLocalStreamOutput("out3", "", "storage", "");

    BOOST_REQUIRE_NO_THROW(views.addEventView("view1", "out1"));
    BOOST_REQUIRE_NO_THROW(views.addEventView("view2", "out4"));

    BOOST_REQUIRE_NO_THROW(views.addTimedView("view3", 1.0, "out3"));
    BOOST_REQUIRE_THROW(views.addTimedView("view3", 1.0, "out1"),
                        utils::ArgError);
    BOOST_REQUIRE_THROW(views.addTimedView("view4", 0.5, "out4"),
                        utils::ArgError);
    BOOST_REQUIRE_THROW(views.addTimedView("view4", 0.0, "out2"),
                        utils::ArgError);
}
