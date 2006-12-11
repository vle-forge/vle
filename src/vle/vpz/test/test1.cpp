#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>


void vpz_creation()
{
}

void vpz_methd()
{
}

boost::unit_test_framework::test_suite*
init_unit_test_suite(int, char* [])
{
    boost::unit_test_framework::test_suite* test;

    test = BOOST_TEST_SUITE("vpz test");
    test->add(BOOST_TEST_CASE(&vpz_creation));
    test->add(BOOST_TEST_CASE(&vpz_methd));

    return test;
}
