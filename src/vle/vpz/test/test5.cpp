/** 
 * @file test5.cpp
 * @brief Test unit of the sax value parser.
 * @author The vle Development Team
 */

/*
 * Copyright (C) 2007 - The vle Development Team
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#define BOOST_TEST_MAIN
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE value_complete_test
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <vle/vpz/Vpz.hpp>
#include <vle/vpz/SaxVPZ.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/Table.hpp>
#include <vle/value/XML.hpp>
#include <vle/utils/Tools.hpp>
#include <limits>
#include <fstream>

using namespace vle;

BOOST_AUTO_TEST_CASE(trame_parameter)
{
    const char* xml = 
        "<?xml version=\"1.0\"?>\n"
        "<trame type=\"parameter\" date=\"0.33\">\n"
        "<![CDATA["
        "empty"
        "]]>\n"
        " <set>\n"
        "</trame>";

    oov::Trame& tr = vpz::Vpz::parse_trame(xml);
    BOOST_CHECK(tr->is_parameter());

    oov::ParameterTrame& ptr = to_parametertrame(tr);
    BOOST_REQUIRE_EQUAL(ptr.time(), .33);
    BOOST_REQUIRE_EQUAL(ptr.data, "empty");
}

BOOST_AUTO_TEST_CASE(trame_addobservable)
{
    const char* xml =
        "<?xml version=\"1.0\"?>\n"
        "<trame type=\"add\" date=\".33\" name=\"mdl\" parent=\"\" port=\"x\""
        " view=\"view1\" />";

    oov::Trame& tr = vpz::Vpz::parse_trame(xml);
    BOOST_CHECK(tr->is_addobservable());

    oov::ParameterTrame& ptr = to_addobservabletrame(tr);
    BOOST_REQUIRE_EQUAL(ptr.time(), .33);
    BOOST_REQUIRE_EQUAL(ptr.name(), "mdl");
    BOOST_REQUIRE_EQUAL(ptr.parent(), "");
    BOOST_REQUIRE_EQUAL(ptr.port(), "x");
    BOOST_REQUIRE_EQUAL(ptr.view(), "view1");
}

BOOST_AUTO_TEST_CASE(trame_delobservable)
{
    const char* xml =
        "<?xml version=\"1.0\"?>\n"
        "<trame type=\"del\" date=\".33\" name=\"mdl\" parent=\"\" port=\"x\""
        " view=\"view1\" />";

    oov::Trame& tr = vpz::Vpz::parse_trame(xml);
    BOOST_CHECK(tr->is_newobservable());

    oov::ParameterTrame& ptr = to_newobservabletrame(tr);
    BOOST_REQUIRE_EQUAL(ptr.time(), .33);
    BOOST_REQUIRE_EQUAL(ptr.name(), "mdl");
    BOOST_REQUIRE_EQUAL(ptr.parent(), "");
    BOOST_REQUIRE_EQUAL(ptr.port(), "x");
    BOOST_REQUIRE_EQUAL(ptr.view(), "view1");
}
