/**
 * @file vle/vpz/test/test6.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#define BOOST_TEST_MAIN
#define BOOST_AUTO_TEST_MAIN
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE vpz_classes_extensions

#include <boost/test/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <vle/value/Value.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/graph/CoupledModel.hpp>

struct F
{
    F() { vle::value::init(); }
    ~F() { vle::value::finalize(); }
};

BOOST_GLOBAL_FIXTURE(F)

using namespace vle;

BOOST_AUTO_TEST_CASE(single_class)
{
    const char* xml=
        "<?xml version=\"1.0\"?>\n"
        "<vle_project version=\"0.5\" author=\"Gauthier Quesnel\""
        " date=\"Mon, 12 Feb 2007 23:40:31 +0100\" >\n"
        " <structures>\n"
        "  <model name=\"test1\" type=\"atomic\" >\n"
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
        " <classes>\n"
        "  <class name=\"xxx\">\n"
        "  <model name=\"top\" type=\"coupled\">\n"
        "   <submodels>\n"
        "    <model name=\"top1\" type=\"coupled\">\n"
        "     <in>\n"
        "      <port name=\"in\" />\n"
        "     </in>\n"
        "     <out>\n"
        "      <port name=\"out\" />\n"
        "     </out>\n"
        "     <submodels>\n"
        "      <model name=\"x\" type=\"atomic\" dynamics=\"unittest\" >\n"
        "       <in>\n"
        "        <port name=\"in\" />\n"
        "       </in>\n"
        "       <out>\n"
        "        <port name=\"out\" />\n"
        "       </out>\n"
        "      </model>\n"
        "      <model name=\"a\" type=\"atomic\" dynamics=\"unittest\" >\n"
        "       <in>\n"
        "        <port name=\"in\" />\n"
        "       </in>\n"
        "      </model>\n"
        "      <model name=\"b\" type=\"atomic\" dynamics=\"unittest\" >\n"
        "       <in>\n"
        "        <port name=\"in\" />\n"
        "       </in>\n"
        "      </model>\n"
        "      <model name=\"c\" type=\"atomic\" dynamics=\"unittest\" >\n"
        "       <in>\n"
        "        <port name=\"in1\" />\n"
        "        <port name=\"in2\" />\n"
        "       </in>\n"
        "      </model>\n"
        "     </submodels>\n"
        "     <connections>\n"
        "      <connection type=\"input\">\n"
        "       <origin model=\"top1\" port=\"in\" />\n"
        "       <destination model=\"x\" port=\"in\" />\n"
        "      </connection>\n"
        "      <connection type=\"internal\">\n"
        "       <origin model=\"x\" port=\"out\" />\n"
        "       <destination model=\"x\" port=\"in\" />\n"
        "      </connection>\n"
        "      <connection type=\"internal\">\n"
        "       <origin model=\"x\" port=\"out\" />\n"
        "       <destination model=\"a\" port=\"in\" />\n"
        "      </connection>\n"
        "      <connection type=\"internal\">\n"
        "       <origin model=\"x\" port=\"out\" />\n"
        "       <destination model=\"b\" port=\"in\" />\n"
        "      </connection>\n"
        "      <connection type=\"internal\">\n"
        "       <origin model=\"x\" port=\"out\" />\n"
        "       <destination model=\"c\" port=\"in1\" />\n"
        "      </connection>\n"
        "      <connection type=\"internal\">\n"
        "       <origin model=\"x\" port=\"out\" />\n"
        "       <destination model=\"c\" port=\"in2\" />\n"
        "      </connection>\n"
        "      <connection type=\"output\">\n"
        "       <origin model=\"x\" port=\"out\" />\n"
        "       <destination model=\"top1\" port=\"out\" />\n"
        "      </connection>\n"
        "     </connections>\n"
        "    </model>\n"
        "    <model name=\"top2\" type=\"coupled\">\n"
        "     <in>\n"
        "      <port name=\"in\" />\n"
        "     </in>\n"
        "     <out>\n"
        "      <port name=\"out\" />\n"
        "     </out>\n"
        "     <submodels>\n"
        "      <model name=\"f\" type=\"atomic\" dynamics=\"unittest\" >\n"
        "       <in>\n"
        "        <port name=\"in\" />\n"
        "       </in>\n"
        "      </model>\n"
        "      <model name=\"g\" type=\"atomic\" dynamics=\"unittest\" > \n"
        "       <in>\n"
        "        <port name=\"in\" />\n"
        "       </in>\n"
        "      </model>\n"
        "     </submodels>\n"
        "     <connections>\n"
        "      <connection type=\"input\">\n"
        "       <origin model=\"top2\" port=\"in\" />\n"
        "       <destination model=\"f\" port=\"in\" />\n"
        "      </connection>\n"
        "      <connection type=\"input\">\n"
        "       <origin model=\"top2\" port=\"in\" />\n"
        "       <destination model=\"g\" port=\"in\" />\n"
        "      </connection>\n"
        "     </connections>\n"
        "    </model>\n"
        "    <model name=\"d\" type=\"atomic\" conditions=\"cd\" dynamics=\"unittest\" >\n"
        "     <in>\n"
        "      <port name=\"in\" />\n"
        "     </in>\n"
        "    </model>\n"
        "    <model name=\"e\" type=\"atomic\" dynamics=\"unittest\" >\n"
        "     <in>\n"
        "      <port name=\"in1\" />\n"
        "      <port name=\"in2\" />\n"
        "     </in>\n"
        "    </model>\n"
        "   </submodels>\n"
        "   <connections>\n"
        "    <connection type=\"internal\">\n"
        "     <origin model=\"top1\" port=\"out\" />\n"
        "     <destination model=\"d\" port=\"in\" />\n"
        "    </connection>\n"
        "    <connection type=\"internal\">\n"
        "     <origin model=\"top1\" port=\"out\" />\n"
        "     <destination model=\"e\" port=\"in1\" />\n"
        "    </connection>\n"
        "    <connection type=\"internal\">\n"
        "     <origin model=\"top1\" port=\"out\" />\n"
        "     <destination model=\"e\" port=\"in2\" />\n"
        "    </connection>\n"
        "    <connection type=\"internal\">\n"
        "     <origin model=\"top1\" port=\"out\" />\n"
        "     <destination model=\"top2\" port=\"in\" />\n"
        "    </connection>\n"
        "    <connection type=\"internal\">\n"
        "     <origin model=\"top2\" port=\"out\" />\n"
        "     <destination model=\"top1\" port=\"in\" />\n"
        "    </connection>\n"
        "   </connections>\n"
        "  </model>\n"
        "  </class>\n"
        " </classes>\n"
        "</vle_project>\n";

    vpz::Vpz vpz;
    vpz.parseMemory(xml);

    const vpz::Classes& classes(vpz.project().classes());
    BOOST_REQUIRE_EQUAL(classes.exist("xxx"), true);

    const vpz::Class& classe(classes.get("xxx"));
    BOOST_REQUIRE(classe.model() != 0);

    const graph::Model* mdl = classe.model();
    BOOST_REQUIRE(mdl->isCoupled());

    const graph::CoupledModel* cpl(dynamic_cast < const
                                   graph::CoupledModel*>(mdl));
    BOOST_REQUIRE(cpl);


    delete vpz.project().model().model();
}
