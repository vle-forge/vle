#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <vle/vpz/SaxVPZ.hpp>

void test_build()
{
    vle::vpz::SaxVPZ sax;

    vle::value::Value* v = sax.parse_memory_value(0);
    BOOST_CHECK(v == 0);
}

void test_value_bool()
{
    const char* t1 = "<xml version=\"1.0\">\n<BOOLEAN VALUE=\"true\" />";
    const char* t2 = "<xml version=\"1.0\">\n<BOOLEAN VALUE=\" \" />";
    const char* t3 = "<xml version=\"1.0\">\n<BOOLEAN VALUE=\"  0 \" />";
    vle::vpz::SaxVPZ sax;
    vle::value::Value* v;

    v = sax.parse_memory_value(t1);
    BOOST_CHECK(v->isBoolean());
    BOOST_CHECK(((vle::value::Boolean*)(v))->boolValue() == true);
    delete v;
    
    v = sax.parse_memory_value(t2);
    BOOST_CHECK(v->isBoolean());
    BOOST_CHECK(((vle::value::Boolean*)(v))->boolValue() == false);
    delete v;

    v = sax.parse_memory_value(t3);
    BOOST_CHECK(v->isBoolean());
    BOOST_CHECK(((vle::value::Boolean*)(v))->boolValue() == false);
    delete v;
}

void test_value_integer()
{
    const char* t1 = "<xml version=\"1.0\">\n<INTEGER VALUE=\" 0 \" />";
    const char* t2 = "<xml version=\"1.0\">\n<INTEGER VALUE=\"11\" />";
    const char* t3 = "<xml version=\"1.0\">\n<INTEGER VALUE=\"-11 \" />";
    const char* t4 = "<xml version=\"1.0\">\n<INTEGER VALUE=\" z \" />";
    vle::vpz::SaxVPZ sax;
    vle::value::Value* v;

    v = sax.parse_memory_value(t1);
    BOOST_CHECK(v->isInteger());
    BOOST_CHECK(((vle::value::Integer*)(v))->intValue() == 0);
    delete v;
    
    v = sax.parse_memory_value(t2);
    BOOST_CHECK(v->isInteger());
    BOOST_CHECK(((vle::value::Integer*)(v))->intValue() == 11);
    delete v;

    v = sax.parse_memory_value(t3);
    BOOST_CHECK(v->isInteger());
    BOOST_CHECK(((vle::value::Integer*)(v))->intValue() == -11);
    delete v;
    
    v = sax.parse_memory_value(t4);
    BOOST_CHECK(v->isInteger());
    BOOST_CHECK(((vle::value::Integer*)(v))->intValue() == -11);
    delete v;
    BOOST_CHECK(((vle::value::Integer*)(v))->intValue() == !);
    delete v;
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
