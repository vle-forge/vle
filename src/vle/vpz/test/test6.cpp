/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2017 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2017 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2017 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <vle/utils/unit-test.hpp>
#include <vle/value/Value.hpp>
#include <vle/vle.hpp>
#include <vle/vpz/CoupledModel.hpp>
#include <vle/vpz/Vpz.hpp>

using namespace vle;

void
single_class()
{
    const char* xml =
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
      "    <model name=\"d\" type=\"atomic\" conditions=\"cd\" "
      "dynamics=\"unittest\" >\n"
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
    EnsuresEqual(classes.exist("xxx"), true);

    const vpz::Class& classe(classes.get("xxx"));
    Ensures(classe.node() != nullptr);

    const vpz::BaseModel* mdl = classe.node();
    Ensures(mdl->isCoupled());

    const vpz::CoupledModel* cpl(dynamic_cast<const vpz::CoupledModel*>(mdl));
    Ensures(cpl);
}

void
single_class_utf8()
{
    const char* xml =
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
      "  <class name=\"xx € ç x\">\n"
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
      "    <model name=\"d\" type=\"atomic\" conditions=\"cd\" "
      "dynamics=\"unittest\" >\n"
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
    EnsuresEqual(classes.exist("xx € ç x"), true);

    const vpz::Class& classe(classes.get("xx € ç x"));
    Ensures(classe.node() != nullptr);

    const vpz::BaseModel* mdl = classe.node();
    Ensures(mdl->isCoupled());

    const vpz::CoupledModel* cpl(dynamic_cast<const vpz::CoupledModel*>(mdl));
    Ensures(cpl);
}

int
main()
{
    vle::Init app;

    single_class();
    single_class_utf8();

    return unit_test::report_errors();
}
