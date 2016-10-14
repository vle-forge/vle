/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2016 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2016 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2016 INRA http://www.inra.fr
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


#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE translator_complete_test
#include <boost/test/included/unit_test.hpp>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/output_test_stream.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/vpz/AtomicModel.hpp>
#include <vle/vpz/CoupledModel.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Double.hpp>
#include <vle/vle.hpp>

struct F
{
    vle::Init a;

    F() : a() { }
    ~F() { }
};

BOOST_GLOBAL_FIXTURE(F);

using namespace vle;

BOOST_AUTO_TEST_CASE(atomicmodel_timed_vpz)
{
    const char* xml =
        "<?xml version=\"1.0\"?>\n"
        "<vle_project version=\"0.5\" author=\"Gauthier Quesnel\""
        " date=\"Mon, 12 Feb 2007 23:40:31 +0100\" >\n"
        " <structures>\n"
        "  <model name=\"test1\" type=\"atomic\" observables=\"obs\">\n"
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
        " <experiment name=\"graphtester\" seed=\"123\" >\n"
        "  <views>\n"
        "   <outputs>\n"
        "    <output name=\"view\" format=\"local\" package=\"vle.output\" plugin=\"storage\" />\n"
        "   </outputs>\n"
        "   <observables>\n"
        "    <observable name=\"obs\" >\n"
        "     <port name=\"c\" >\n"
        "      <attachedview name=\"view\" />\n"
        "     </port>\n"
        "    </observable>\n"
        "   </observables>\n"
        "   <view name=\"view\" output=\"view\" type=\"timed\" timestep=\"1.000000000000000\" />\n"
        "  </views>\n"
        " </experiment>\n"
        "</vle_project>\n";

    vpz::Vpz vpz;
    vpz.parseMemory(xml);

    const auto& v = vpz.project().experiment().views().get("view");

    BOOST_REQUIRE(v.type() == vle::vpz::View::TIMED);
    BOOST_REQUIRE(v.timestep() == 1.0);
}

BOOST_AUTO_TEST_CASE(atomicmodel_vpz_event_output)
{
    const char* xml =
        "<?xml version=\"1.0\"?>\n"
        "<vle_project version=\"0.5\" author=\"Gauthier Quesnel\""
        " date=\"Mon, 12 Feb 2007 23:40:31 +0100\" >\n"
        " <structures>\n"
        "  <model name=\"test1\" type=\"atomic\" observables=\"obs\">\n"
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
        " <experiment name=\"graphtester\" seed=\"123\" >\n"
        "  <views>\n"
        "   <outputs>\n"
        "    <output name=\"view\" format=\"local\" package=\"vle.output\" plugin=\"storage\" />\n"
        "   </outputs>\n"
        "   <observables>\n"
        "    <observable name=\"obs\" >\n"
        "     <port name=\"c\" >\n"
        "      <attachedview name=\"view\" />\n"
        "     </port>\n"
        "    </observable>\n"
        "   </observables>\n"
        "   <view name=\"view\" output=\"view\" type=\"event,output\"  />\n"
        "  </views>\n"
        " </experiment>\n"
        "</vle_project>\n";

    vpz::Vpz vpz;
    vpz.parseMemory(xml);

    const auto& v = vpz.project().experiment().views().get("view");

    auto c = vle::vpz::View::INTERNAL | vle::vpz::View::CONFLUENT
        | vle::vpz::View::EXTERNAL | vle::vpz::View::OUTPUT;

    BOOST_REQUIRE(v.type() == c);
}

BOOST_AUTO_TEST_CASE(atomicmodel_vpz_internal_output)
{
    const char* xml =
        "<?xml version=\"1.0\"?>\n"
        "<vle_project version=\"0.5\" author=\"Gauthier Quesnel\""
        " date=\"Mon, 12 Feb 2007 23:40:31 +0100\" >\n"
        " <structures>\n"
        "  <model name=\"test1\" type=\"atomic\" observables=\"obs\">\n"
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
        " <experiment name=\"graphtester\" seed=\"123\" >\n"
        "  <views>\n"
        "   <outputs>\n"
        "    <output name=\"view\" format=\"local\" package=\"vle.output\" plugin=\"storage\" />\n"
        "   </outputs>\n"
        "   <observables>\n"
        "    <observable name=\"obs\" >\n"
        "     <port name=\"c\" >\n"
        "      <attachedview name=\"view\" />\n"
        "     </port>\n"
        "    </observable>\n"
        "   </observables>\n"
        "   <view name=\"view\" output=\"view\" type=\"internal,output\"  />\n"
        "  </views>\n"
        " </experiment>\n"
        "</vle_project>\n";

    vpz::Vpz vpz;
    vpz.parseMemory(xml);

    const auto& v = vpz.project().experiment().views().get("view");

    auto c = vle::vpz::View::INTERNAL |  vle::vpz::View::OUTPUT;

    BOOST_REQUIRE(v.type() == c);
}

BOOST_AUTO_TEST_CASE(atomicmodel_vpz_confluent)
{
    const char* xml =
        "<?xml version=\"1.0\"?>\n"
        "<vle_project version=\"0.5\" author=\"Gauthier Quesnel\""
        " date=\"Mon, 12 Feb 2007 23:40:31 +0100\" >\n"
        " <structures>\n"
        "  <model name=\"test1\" type=\"atomic\" observables=\"obs\">\n"
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
        " <experiment name=\"graphtester\" seed=\"123\" >\n"
        "  <views>\n"
        "   <outputs>\n"
        "    <output name=\"view\" format=\"local\" package=\"vle.output\" plugin=\"storage\" />\n"
        "   </outputs>\n"
        "   <observables>\n"
        "    <observable name=\"obs\" >\n"
        "     <port name=\"c\" >\n"
        "      <attachedview name=\"view\" />\n"
        "     </port>\n"
        "    </observable>\n"
        "   </observables>\n"
        "   <view name=\"view\" output=\"view\" type=\"confluent\"  />\n"
        "  </views>\n"
        " </experiment>\n"
        "</vle_project>\n";

    vpz::Vpz vpz;
    vpz.parseMemory(xml);

    const auto& v = vpz.project().experiment().views().get("view");

    auto c = vle::vpz::View::CONFLUENT;

    BOOST_REQUIRE(v.type() == c);
}

BOOST_AUTO_TEST_CASE(atomicmodel_vpz_throw1)
{
    const char* xml =
        "<?xml version=\"1.0\"?>\n"
        "<vle_project version=\"0.5\" author=\"Gauthier Quesnel\""
        " date=\"Mon, 12 Feb 2007 23:40:31 +0100\" >\n"
        " <structures>\n"
        "  <model name=\"test1\" type=\"atomic\" observables=\"obs\">\n"
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
        " <experiment name=\"graphtester\" seed=\"123\" >\n"
        "  <views>\n"
        "   <outputs>\n"
        "    <output name=\"view\" format=\"local\" package=\"vle.output\" plugin=\"storage\" />\n"
        "   </outputs>\n"
        "   <observables>\n"
        "    <observable name=\"obs\" >\n"
        "     <port name=\"c\" >\n"
        "      <attachedview name=\"view\" />\n"
        "     </port>\n"
        "    </observable>\n"
        "   </observables>\n"
        "   <view name=\"view\" output=\"view\" type=\"confluent,\"  />\n"
        "  </views>\n"
        " </experiment>\n"
        "</vle_project>\n";

    vpz::Vpz vpz;
    BOOST_REQUIRE_THROW(vpz.parseMemory(xml), std::exception);
}

BOOST_AUTO_TEST_CASE(atomicmodel_vpz_throw2)
{
    const char* xml =
        "<?xml version=\"1.0\"?>\n"
        "<vle_project version=\"0.5\" author=\"Gauthier Quesnel\""
        " date=\"Mon, 12 Feb 2007 23:40:31 +0100\" >\n"
        " <structures>\n"
        "  <model name=\"test1\" type=\"atomic\" observables=\"obs\">\n"
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
        " <experiment name=\"graphtester\" seed=\"123\" >\n"
        "  <views>\n"
        "   <outputs>\n"
        "    <output name=\"view\" format=\"local\" package=\"vle.output\" plugin=\"storage\" />\n"
        "   </outputs>\n"
        "   <observables>\n"
        "    <observable name=\"obs\" >\n"
        "     <port name=\"c\" >\n"
        "      <attachedview name=\"view\" />\n"
        "     </port>\n"
        "    </observable>\n"
        "   </observables>\n"
        "   <view name=\"view\" output=\"view\" type=\"timed\"  />\n"
        "  </views>\n"
        " </experiment>\n"
        "</vle_project>\n";

    vpz::Vpz vpz;
    BOOST_REQUIRE_THROW(vpz.parseMemory(xml), std::exception);
}
