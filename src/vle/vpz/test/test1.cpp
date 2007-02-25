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

#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
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

void test_build()
{
    //vpz::VLESaxParser sax;
    //sax.parse_memory("<?xml version=\"1.0\"?>\n<trame></trame>");
    //
    //BOOST_CHECK(sax.is_value() == false);
    //BOOST_CHECK(sax.get_values().empty() == true);
}

void test_value_bool()
{
    const char* t1 = "<?xml version=\"1.0\"?>\n<boolean>true</boolean>";
    const char* t2 = "<?xml version=\"1.0\"?>\n<boolean>false</boolean>";
    const char* t3 = "<?xml version=\"1.0\"?>\n<boolean>1</boolean>";
    const char* t4 = "<?xml version=\"1.0\"?>\n<boolean>0</boolean>";
    vpz::VLESaxParser sax;
    value::Value v;

    sax.parse_memory(t1);
    v = sax.get_value(0);
    BOOST_CHECK(value::to_boolean(v)->boolValue() == true);

    sax.parse_memory(t2);
    v = sax.get_value(0);
    BOOST_CHECK(value::to_boolean(v)->boolValue() == false);

    sax.parse_memory(t3);
    v = sax.get_value(0);
    BOOST_CHECK(value::to_boolean(v)->boolValue() == true);

    sax.parse_memory(t4);
    v = sax.get_value(0);
    BOOST_CHECK(value::to_boolean(v)->boolValue() == false);
}

void test_value_integer()
{
    const char* t1 = "<?xml version=\"1.0\"?>\n<integer>100</integer>";
    const char* t2 = "<?xml version=\"1.0\"?>\n<integer>-100</integer>";
    char t3[1000];
    char t4[1000];

    sprintf(t3, "<?xml version=\"1.0\"?>\n<integer>%s</integer>",
            utils::to_string(std::numeric_limits< long >::max()).c_str());
    sprintf(t4, "<?xml version=\"1.0\"?>\n<integer>%s</integer>",
            utils::to_string(std::numeric_limits< long >::min()).c_str());

    vpz::VLESaxParser sax;
    value::Value v;

    sax.parse_memory(t1);
    v = sax.get_value(0);
    BOOST_CHECK(value::to_integer(v)->intValue() == 100);

    sax.parse_memory(t2);
    v = sax.get_value(0);
    BOOST_CHECK(value::to_integer(v)->intValue() == -100);

    sax.parse_memory(t3);
    v = sax.get_value(0);
    BOOST_CHECK(value::to_integer(v)->longValue() == std::numeric_limits < long >::max());

    sax.parse_memory(t4);
    v = sax.get_value(0);
    BOOST_CHECK(value::to_integer(v)->longValue() == std::numeric_limits < long >::min());
}

void test_value_double()
{
    const char* t1 = "<?xml version=\"1.0\"?>\n<double>100.5</double>";
    const char* t2 = "<?xml version=\"1.0\"?>\n<double>-100.5</double>";

    vpz::VLESaxParser sax;
    value::Value v;

    sax.parse_memory(t1);
    v = sax.get_value(0);
    BOOST_CHECK_CLOSE(value::to_double(v)->doubleValue(), 100.5, 1);

    sax.parse_memory(t2);
    v = sax.get_value(0);
    BOOST_CHECK_CLOSE(value::to_double(v)->doubleValue(), -100.5, 1);
}

void test_value_string()
{
    const char* t1 = "<?xml version=\"1.0\"?>\n<string>a b c d e f g h i j</string>";
    const char* t2 = "<?xml version=\"1.0\"?>\n<string>a\nb\tc\n</string>";

    vpz::VLESaxParser sax;
    value::Value v;

    sax.parse_memory(t1);
    v = sax.get_value(0);
    BOOST_CHECK(value::to_string(v)->stringValue() == "a b c d e f g h i j");

    sax.parse_memory(t2);
    v = sax.get_value(0);
    BOOST_CHECK(value::to_string(v)->stringValue() == "a\nb\tc\n");
}

void test_value_set()
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
    
    vpz::VLESaxParser sax;
    value::Set v;
    
    sax.parse_memory(t1);
    v = value::to_set(sax.get_value(0));
    BOOST_CHECK(value::to_string(v->getValue(1))->stringValue() == "test");
    BOOST_CHECK(value::to_integer(v->getValue(0))->intValue() == 1);

    sax.parse_memory(t2);
    v = value::to_set(sax.get_value(0));
    BOOST_CHECK(value::to_integer(v->getValue(0))->intValue() == 1);
    value::Set v2 = value::to_set(v->getValue(1));
    BOOST_CHECK(value::to_string(v2->getValue(0))->stringValue() == "test");
}

void test_value_map()
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

    vpz::VLESaxParser sax;
    value::Map v;
    
    sax.parse_memory(t1);
    v = value::to_map(sax.get_value(0));
    BOOST_CHECK(value::to_integer(v->getValue("a"))->intValue() == 10);

    sax.parse_memory(t2);
    v = value::to_map(sax.get_value(0));
    value::Set s;

    s = value::to_set(v->getValue("a"));
    BOOST_CHECK(value::to_integer(s->getValue(0))->intValue() == 1);
    BOOST_CHECK(value::to_string(s->getValue(1))->stringValue() == "test");
}

void test_value_tuple()
{
    const char* t1 = "<?xml version=\"1.0\"?>\n"
        "<tuple>1\n"
        "2 "
        "3</tuple>\n";

    vpz::VLESaxParser sax;
    value::Tuple v;
    
    sax.parse_memory(t1);
    v = value::to_tuple(sax.get_value(0));
    BOOST_REQUIRE_EQUAL(v->size(), (size_t)3);
    BOOST_CHECK_CLOSE(v->operator[](0), 1.0, 0.1);
    BOOST_CHECK_CLOSE(v->operator[](1), 2.0, 0.1);
    BOOST_CHECK_CLOSE(v->operator[](2), 3.0, 0.1);
}

void test_value_table()
{
    const char* t1 = "<?xml version=\"1.0\"?>\n"
        "<table width=\"2\" height=\"3\">\n"
        "1 2 3 4 5 6"
        "</table>\n";

    vpz::VLESaxParser sax;
    value::Table v;
    
    sax.parse_memory(t1);
    v = value::to_table(sax.get_value(0));
    BOOST_REQUIRE_EQUAL(v->width(), (value::TableFactory::index)2);
    BOOST_REQUIRE_EQUAL(v->height(), (value::TableFactory::index)3);
    BOOST_CHECK_CLOSE(v->get(0, 0), 1.0, 0.1);
    BOOST_CHECK_CLOSE(v->get(0, 1), 2.0, 0.1);
    BOOST_CHECK_CLOSE(v->get(0, 2), 3.0, 0.1);
    BOOST_CHECK_CLOSE(v->get(1, 0), 4.0, 0.1);
    BOOST_CHECK_CLOSE(v->get(1, 1), 5.0, 0.1);
    BOOST_CHECK_CLOSE(v->get(1, 2), 6.0, 0.1);
}

boost::unit_test_framework::test_suite*
init_unit_test_suite(int, char* [])
{
    boost::unit_test_framework::test_suite* test;

    test = BOOST_TEST_SUITE("vpz test");
    test->add(BOOST_TEST_CASE(&test_build));
    test->add(BOOST_TEST_CASE(&test_value_bool));
    test->add(BOOST_TEST_CASE(&test_value_integer));
    test->add(BOOST_TEST_CASE(&test_value_double));
    test->add(BOOST_TEST_CASE(&test_value_string));
    test->add(BOOST_TEST_CASE(&test_value_set));
    test->add(BOOST_TEST_CASE(&test_value_map));
    test->add(BOOST_TEST_CASE(&test_value_tuple));
    test->add(BOOST_TEST_CASE(&test_value_table));
    return test;
}
