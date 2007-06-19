/** 
 * @file test3.cpp
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

#define BOOST_TEST_MAIN
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE translator_complete_test
#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/output_test_stream.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/vpz/Translator.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/graph/CoupledModel.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Double.hpp>
#include <vle/vpz/Translator.hpp>

using namespace vle;

BOOST_AUTO_TEST_CASE(test_translator)
{
    const char* xml=
        "<?xml version=\"1.0\"?>\n"
        "<vle_project version=\"0.5\" author=\"Gauthier Quesnel\""
        " date=\"Mon, 12 Feb 2007 23:40:31 +0100\" >\n"
        " <structures>\n"
        "  <model name=\"test1\" type=\"novle\" translator=\"xxx\" >\n"
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
        " <translators>\n"
        "  <translator name=\"xxx\" library=\"tr1\">\n"
        "   <![CDATA["
        "<?xml version=\"1.0\"?>"
        "<test>"
        " <models number=\"2\" />"
        "</test>"
        "]]>"
        "  </translator>\n"
        " </translators>\n"
        "</vle_project>\n";
    
    vpz::Vpz vpz;
    vpz.parse_memory(xml);
    vpz.expandTranslator();

    const vpz::Project& prj(vpz.project());

    const vpz::NoVLEs& novles(prj.novles());
    BOOST_REQUIRE(novles.empty());
    
    const vpz::Dynamics& dyns(prj.dynamics());
    BOOST_REQUIRE_EQUAL(dyns.size(), (vpz::Dynamics::size_type)1);
    
    const vpz::Experiment& exp(prj.experiment());
    const vpz::Views& views(exp.views());
    BOOST_REQUIRE_EQUAL(views.outputs().size(), (vpz::Outputs::size_type)1);
    BOOST_REQUIRE_EQUAL(views.size(), (vpz::Views::size_type)1);

    const vpz::Conditions& conditions(exp.conditions());
    BOOST_REQUIRE_EQUAL(conditions.size(), (vpz::Conditions::size_type)1);

    const vpz::Model& model(vpz.project().model());
    BOOST_REQUIRE(model.model());
    BOOST_REQUIRE(model.model()->isCoupled());

    graph::CoupledModel* cpled((graph::CoupledModel*)model.model());
    BOOST_REQUIRE_EQUAL(cpled->getName(), "toto");
    BOOST_REQUIRE(cpled->exist("tutu"));
    BOOST_REQUIRE(cpled->exist("tata"));

    graph::Connection* cnx(
        cpled->getInternalConnection(std::string("tutu"), "out",
                                     std::string("tata"), "in"));
    BOOST_REQUIRE(cnx);
}
