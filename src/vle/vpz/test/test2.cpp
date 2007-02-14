#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <vle/vpz/SaxVPZ.hpp>
#include <limits>
#include <fstream>

using namespace vle;

void test_vpz()
{
    const char* xml =
        "<?xml version=\"1.0\"?>\n"
        "<vpz version=\"0.5\" author=\"Gauthier Quesnel\" date=\"Mon, 12 Feb 2007 23:40:31 +0100\" >\n"
        " <structures>\n"
        "  <model name=\"test1\" type=\"atomic\">\n"
        "   <init>\n"
        "    <port name=\"init1\" />\n"
        "    <port name=\"init2\" />\n"
        "   </init>\n"
        "   <state>\n"
        "    <port name=\"state1\" />\n"
        "    <port name=\"state2\" />\n"
        "   </state>\n"
        "   <in>\n"
        "    <port name=\"in1\" />\n"
        "    <port name=\"in2\" />\n"
        "   </in>\n"
        "   <out>\n"
        "    <port name=\"out1\" />\n"
        "    <port name=\"out2\" />\n"
        "   </out>\n"
        "  </model>\n"
        " </structures>\n"
        " <dynamics>\n"
        "  <dynamic name=\"null\" />\n"
        " </dynamics>\n"
        " <experiment duration=\"100\" />\n"
        "</vpz>\n";

    vpz::VLESaxParser sax;
    sax.parse_memory(xml);
}


boost::unit_test_framework::test_suite*
init_unit_test_suite(int, char* [])
{
    boost::unit_test_framework::test_suite* test;

    test = BOOST_TEST_SUITE("vpz test");
    test->add(BOOST_TEST_CASE(&test_vpz));
    return test;
}
