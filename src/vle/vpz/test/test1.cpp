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
    return test;
}
