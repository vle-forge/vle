/** 
 * @file test2.cpp
 * @brief Test unit of the vpz parser.
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
#include <vle/vpz/Vpz.hpp>
#include <limits>
#include <fstream>

using namespace vle;

void test_vpz()
{
    const char* xml =
        "<?xml version=\"1.0\"?>\n"
        "<vle_project version=\"0.5\" author=\"Gauthier Quesnel\""
        " date=\"Mon, 12 Feb 2007 23:40:31 +0100\" >\n"
        " <structures>\n"
        //"  <model name=\"test1\" type=\"atomic\">\n"
        //"   <init>\n"
        //"    <port name=\"init1\" />\n"
        //"    <port name=\"init2\" />\n"
        //"   </init>\n"
        //"   <state>\n"
        //"    <port name=\"state1\" />\n"
        //"    <port name=\"state2\" />\n"
        //"   </state>\n"
        //"   <in>\n"
        //"    <port name=\"in1\" />\n"
        //"    <port name=\"in2\" />\n"
        //"   </in>\n"
        //"   <out>\n"
        //"    <port name=\"out1\" />\n"
        //"    <port name=\"out2\" />\n"
        //"   </out>\n"
        //"  </model>\n"
        " </structures>\n"
        //" <dynamics>\n"
        //"  <dynamic name=\"null\" />\n"
        //" </dynamics>\n"
        //" <experiment duration=\"100\" />\n"
        "</vle_project>\n";
    //std::cout << xml << std::endl;
    vpz::VLESaxParser sax;
    sax.parse_memory(xml);

    const vpz::Vpz& vpz = sax.vpz();
    BOOST_REQUIRE_EQUAL(vpz.author(), "Gauthier Quesnel");
    BOOST_REQUIRE_CLOSE(vpz.version(), 0.5f, 0.01);
    BOOST_REQUIRE_EQUAL(vpz.date(), "Mon, 12 Feb 2007 23:40:31 +0100");
}


boost::unit_test_framework::test_suite*
init_unit_test_suite(int, char* [])
{
    boost::unit_test_framework::test_suite* test;

    test = BOOST_TEST_SUITE("vpz test");
    test->add(BOOST_TEST_CASE(&test_vpz));
    return test;
}
