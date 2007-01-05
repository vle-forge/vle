#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <vle/vpz/SaxVPZ.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Double.hpp>

using namespace vle;

void test_build()
{
    vpz::VLESaxParser sax;
    sax.parse_memory("<xml version=\"1.0\">\n");

    BOOST_CHECK(sax.is_value());
    BOOST_CHECK(sax.get_value_stack().get() != 0);
}

void test_value_bool()
{
    const char* t1 = "<xml version=\"1.0\">\n<BOOLEAN VALUE=\"true\" />";
    const char* t2 = "<xml version=\"1.0\">\n<BOOLEAN VALUE=\" \" />";
    const char* t3 = "<xml version=\"1.0\">\n<BOOLEAN VALUE=\"  0 \" />";
    vpz::VLESaxParser sax;
    value::Value v;

    sax.parse_memory(t1);
    BOOST_CHECK(v->isBoolean());
    BOOST_CHECK(value::to_boolean(v)->boolValue() == true);

    sax.parse_memory(t2);
    BOOST_CHECK(v->isBoolean());
    BOOST_CHECK(value::to_boolean(v)->boolValue() == false);

    sax.parse_memory(t3);
    BOOST_CHECK(v->isBoolean());
    BOOST_CHECK(value::to_boolean(v)->boolValue() == false);
}

void test_value_integer()
{
    const char* t1 = "<xml version=\"1.0\">\n<INTEGER VALUE=\" 0 \" />";
    const char* t2 = "<xml version=\"1.0\">\n<INTEGER VALUE=\"11\" />";
    const char* t3 = "<xml version=\"1.0\">\n<INTEGER VALUE=\"-11 \" />";
    const char* t4 = "<xml version=\"1.0\">\n<INTEGER VALUE=\" 1 \" />";
    vpz::VLESaxParser sax;
    value::Value v;

    sax.parse_memory(t1);
    BOOST_CHECK(v->isInteger());
    BOOST_CHECK(value::to_integer(v)->intValue() == 0);
    
    sax.parse_memory(t2);
    BOOST_CHECK(v->isInteger());
    BOOST_CHECK(value::to_integer(v)->intValue() == 11);

    sax.parse_memory(t3);
    BOOST_CHECK(v->isInteger());
    BOOST_CHECK(value::to_integer(v)->intValue() == -11);
    
    sax.parse_memory(t4);
    BOOST_CHECK(v->isInteger());
    BOOST_CHECK(value::to_integer(v)->intValue() == 1);
}

boost::unit_test_framework::test_suite*
init_unit_test_suite(int, char* [])
{
    boost::unit_test_framework::test_suite* test;

    test = BOOST_TEST_SUITE("vpz test");
    test->add(BOOST_TEST_CASE(&test_build));
    test->add(BOOST_TEST_CASE(&test_value_bool));
    test->add(BOOST_TEST_CASE(&test_value_integer));

    return test;
}
