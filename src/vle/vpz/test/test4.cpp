/** 
 * @file test4.cpp
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

using namespace vle;

BOOST_AUTO_TEST_CASE(test_translator)
{
    vpz::Vpz vpz;
    vpz.parse_file("coupled.vpz");

    const vpz::Model& model(vpz.project().model());
    BOOST_REQUIRE(model.model());
    BOOST_REQUIRE(model.model()->isCoupled());

    graph::CoupledModel* cpled((graph::CoupledModel*)model.model());
    BOOST_REQUIRE_EQUAL(cpled->getName(), "top");
    BOOST_REQUIRE(cpled->exist("top1"));
    BOOST_REQUIRE(cpled->exist("top2"));
    BOOST_REQUIRE(cpled->exist("d"));
    BOOST_REQUIRE(cpled->exist("e"));

    graph::CoupledModel* top1((graph::CoupledModel*)cpled->findModel("top1"));
    graph::AtomicModel* x((graph::AtomicModel*)top1->findModel("x"));

    graph::TargetModelList out;
    x->getTargetPortList("out", out);

    BOOST_REQUIRE_EQUAL(out.size(), (graph::TargetModelList::size_type)10);
}


BOOST_AUTO_TEST_CASE(test_translator_write)
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
        "  <translator name=\"xxx\" library=\"vletesttr1\">\n"
        "   <![CDATA["
        "<?xml version=\"1.0\"?>\n"
        "<test>"
        " <models number=\"2\" />"
        "</test>"
        "]]>"
        "  </translator>\n"
        " </translators>\n"
        " <experiment name=\"exp\" duration=\"1\" seed=\"1\" />"
        "</vle_project>\n";

    vpz::Vpz vpz1;
    vpz::Vpz vpz2;
    std::string copyfilename(utils::build_temp("coupled2.vpz"));

    vpz1.parse_memory(xml);
    vpz1.write(copyfilename);
    vpz2.parse_file(copyfilename);

    const vpz::NoVLEs& novles1(vpz1.project().novles());
    const vpz::NoVLEs& novles2(vpz2.project().novles());

    BOOST_REQUIRE_EQUAL(novles1.size(), novles2.size());
    BOOST_REQUIRE(novles1.exist("xxx"));
    BOOST_REQUIRE(novles2.exist("xxx"));

    const vpz::NoVLE& novle1(novles1.get("xxx"));
    const vpz::NoVLE& novle2(novles2.get("xxx"));

    BOOST_REQUIRE_EQUAL(novle1.name(), novle2.name());
    BOOST_REQUIRE_EQUAL(novle1.data(), novle2.data());
    BOOST_REQUIRE_EQUAL(novle1.library(), novle2.library());
}
