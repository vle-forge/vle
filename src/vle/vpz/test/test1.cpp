/** 
 * @file test1.cpp
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
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE translator_complete_test
#include <boost/test/unit_test.hpp>
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
#include <vle/utils/Tools.hpp>
#include <limits>
#include <fstream>

using namespace vle;

BOOST_AUTO_TEST_CASE(build)
{
    //vpz::VLESaxParser sax;
    //sax.parse_memory("<?xml version=\"1.0\"?>\n<trame></trame>");
    //
    //BOOST_CHECK(sax.is_value() == false);
    //BOOST_CHECK(sax.get_values().empty() == true);
}

BOOST_AUTO_TEST_CASE(value_bool)
{
    const char* t1 = "<?xml version=\"1.0\"?>\n<boolean>true</boolean>";
    const char* t2 = "<?xml version=\"1.0\"?>\n<boolean>false</boolean>";
    const char* t3 = "<?xml version=\"1.0\"?>\n<boolean>1</boolean>";
    const char* t4 = "<?xml version=\"1.0\"?>\n<boolean>0</boolean>";

    value::Value v;
    
    v = vpz::Vpz::parse_value(t1);
    BOOST_CHECK(value::to_boolean(v)->boolValue() == true);

    v = vpz::Vpz::parse_value(t2);
    BOOST_CHECK(value::to_boolean(v)->boolValue() == false);

    v = vpz::Vpz::parse_value(t3);
    BOOST_CHECK(value::to_boolean(v)->boolValue() == true);

    v = vpz::Vpz::parse_value(t4);
    BOOST_CHECK(value::to_boolean(v)->boolValue() == false);
}

BOOST_AUTO_TEST_CASE(value_integer)
{
    const char* t1 = "<?xml version=\"1.0\"?>\n<integer>100</integer>";
    const char* t2 = "<?xml version=\"1.0\"?>\n<integer>-100</integer>";
    char t3[1000];
    char t4[1000];

    sprintf(t3, "<?xml version=\"1.0\"?>\n<integer>%s</integer>",
            utils::to_string(std::numeric_limits< long >::max()).c_str());
    sprintf(t4, "<?xml version=\"1.0\"?>\n<integer>%s</integer>",
            utils::to_string(std::numeric_limits< long >::min()).c_str());

    
    value::Value v;

    v = vpz::Vpz::parse_value(t1);
    BOOST_CHECK(value::to_integer(v)->intValue() == 100);

    v = vpz::Vpz::parse_value(t2);
    BOOST_CHECK(value::to_integer(v)->intValue() == -100);

    v = vpz::Vpz::parse_value(t3);
    BOOST_CHECK(value::to_integer(v)->longValue() == std::numeric_limits < long
                >::max());

    v = vpz::Vpz::parse_value(t4);
    BOOST_CHECK(value::to_integer(v)->longValue() == std::numeric_limits < long
                >::min());
}

BOOST_AUTO_TEST_CASE(value_double)
{
    const char* t1 = "<?xml version=\"1.0\"?>\n<double>100.5</double>";
    const char* t2 = "<?xml version=\"1.0\"?>\n<double>-100.5</double>";

    value::Value v;

    v = vpz::Vpz::parse_value(t1);
    BOOST_CHECK_CLOSE(value::to_double(v)->doubleValue(), 100.5, 1);

    v = vpz::Vpz::parse_value(t2);
    BOOST_CHECK_CLOSE(value::to_double(v)->doubleValue(), -100.5, 1);
}

BOOST_AUTO_TEST_CASE(value_string)
{
    const char* t1 = "<?xml version=\"1.0\"?>\n<string>a b c d e f g h i j</string>";
    const char* t2 = "<?xml version=\"1.0\"?>\n<string>a\nb\tc\n</string>";

    value::Value v;

    v = vpz::Vpz::parse_value(t1);
    BOOST_CHECK(value::to_string(v)->stringValue() == "a b c d e f g h i j");

    v = vpz::Vpz::parse_value(t2);
    BOOST_CHECK(value::to_string(v)->stringValue() == "a\nb\tc\n");
}

BOOST_AUTO_TEST_CASE(value_set)
{
    const char* t1 = "<?xml version=\"1.0\"?>\n"
        "<set>\n"
        "<integer>1</integer>\n"
        "<string>test</string>\n"
        "</set>";

    const char* t2 = "<?xml version=\"1.0\"?>\n"
        "<set>\n"
        "  <integer>1</integer>\n"
        "  <set>\n"
        "    <string>test</string>\n"
        "  </set>\n"
        "</set>";
    
    value::Set v;
    
    v = value::to_set(vpz::Vpz::parse_value(t1));
    BOOST_CHECK(value::to_string(v->getValue(1))->stringValue() == "test");
    BOOST_CHECK(value::to_integer(v->getValue(0))->intValue() == 1);

    v = value::to_set(vpz::Vpz::parse_value(t2));
    BOOST_CHECK(value::to_integer(v->getValue(0))->intValue() == 1);
    value::Set v2 = value::to_set(v->getValue(1));
    BOOST_CHECK(value::to_string(v2->getValue(0))->stringValue() == "test");
}

BOOST_AUTO_TEST_CASE(value_map)
{
    const char* t1 = "<?xml version=\"1.0\"?>\n"
        "<map>\n"
        " <key name=\"a\">\n"
        "  <integer>10</integer>\n"
        " </key>\n"
        "</map>\n";

    const char* t2 = "<?xml version=\"1.0\"?>\n"
        "<map>\n"
        " <key name=\"a\">\n"
        "  <set>\n"
        "   <integer>1</integer>\n"
        "   <string>test</string>\n"
        "  </set>\n"
        " </key>\n"
        "</map>\n";

    value::Map v;
    
    v = value::to_map(vpz::Vpz::parse_value(t1));
    BOOST_CHECK(value::to_integer(v->getValue("a"))->intValue() == 10);

    v = value::to_map(vpz::Vpz::parse_value(t2));
    value::Set s;

    s = value::to_set(v->getValue("a"));
    BOOST_CHECK(value::to_integer(s->getValue(0))->intValue() == 1);
    BOOST_CHECK(value::to_string(s->getValue(1))->stringValue() == "test");
}

BOOST_AUTO_TEST_CASE(value_tuple)
{
    const char* t1 = "<?xml version=\"1.0\"?>\n"
        "<tuple>1\n"
        "2 "
        "3</tuple>\n";

    value::Tuple v;
    
    v = value::to_tuple(vpz::Vpz::parse_value(t1));
    BOOST_REQUIRE_EQUAL(v->size(), (size_t)3);
    BOOST_CHECK_CLOSE(v->operator[](0), 1.0, 0.1);
    BOOST_CHECK_CLOSE(v->operator[](1), 2.0, 0.1);
    BOOST_CHECK_CLOSE(v->operator[](2), 3.0, 0.1);
}

BOOST_AUTO_TEST_CASE(value_table)
{
    const char* t1 = "<?xml version=\"1.0\"?>\n"
        "<table width=\"2\" height=\"3\">\n"
        "1 2 3 4 5 6"
        "</table>\n";

    value::Table v;
    
    v = value::to_table(vpz::Vpz::parse_value(t1));
    BOOST_REQUIRE_EQUAL(v->width(), (value::TableFactory::index)2);
    BOOST_REQUIRE_EQUAL(v->height(), (value::TableFactory::index)3);
    BOOST_CHECK_CLOSE(v->get(0, 0), 1.0, 0.1);
    BOOST_CHECK_CLOSE(v->get(0, 1), 2.0, 0.1);
    BOOST_CHECK_CLOSE(v->get(0, 2), 3.0, 0.1);
    BOOST_CHECK_CLOSE(v->get(1, 0), 4.0, 0.1);
    BOOST_CHECK_CLOSE(v->get(1, 1), 5.0, 0.1);
    BOOST_CHECK_CLOSE(v->get(1, 2), 6.0, 0.1);
}
