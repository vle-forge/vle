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

#include <vle/utils/unit-test.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Set.hpp>
#include <vle/vle.hpp>
#include <vle/vpz/AtomicModel.hpp>
#include <vle/vpz/CoupledModel.hpp>
#include <vle/vpz/Vpz.hpp>

using namespace vle;

void
atomicmodel_timed_vpz()
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
      "    <output name=\"view\" format=\"local\" package=\"vle.output\" "
      "plugin=\"storage\" />\n"
      "   </outputs>\n"
      "   <observables>\n"
      "    <observable name=\"obs\" >\n"
      "     <port name=\"c\" >\n"
      "      <attachedview name=\"view\" />\n"
      "     </port>\n"
      "    </observable>\n"
      "   </observables>\n"
      "   <view name=\"view\" output=\"view\" type=\"timed\" "
      "timestep=\"1.000000000000000\" />\n"
      "  </views>\n"
      " </experiment>\n"
      "</vle_project>\n";

    vpz::Vpz vpz;
    vpz.parseMemory(xml);

    const auto& v = vpz.project().experiment().views().get("view");

    Ensures(v.type() == vle::vpz::View::TIMED);
    Ensures(v.timestep() == 1.0);
}

void
atomicmodel_vpz_event_output()
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
      "    <output name=\"view\" format=\"local\" package=\"vle.output\" "
      "plugin=\"storage\" />\n"
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

    auto c = vle::vpz::View::INTERNAL | vle::vpz::View::CONFLUENT |
             vle::vpz::View::EXTERNAL | vle::vpz::View::OUTPUT;

    Ensures(v.type() == c);
}

void
atomicmodel_vpz_internal_output()
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
      "    <output name=\"view\" format=\"local\" package=\"vle.output\" "
      "plugin=\"storage\" />\n"
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

    auto c = vle::vpz::View::INTERNAL | vle::vpz::View::OUTPUT;

    Ensures(v.type() == c);
}

void
atomicmodel_vpz_confluent()
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
      "    <output name=\"view\" format=\"local\" package=\"vle.output\" "
      "plugin=\"storage\" />\n"
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

    Ensures(v.type() == c);
}

void
vpz_throw()
{
    const char* xml = "<?x";

    {
        try {
            vpz::Vpz vpz;
            vpz.parseMemory(xml);
        } catch (const std::exception& e) {
            std::cerr << __func__ << ": " << e.what() << '\n';
        }
    }

    vpz::Vpz vpz;
    EnsuresThrow(vpz.parseMemory(xml), std::exception);
}

void
vpz_throw_1()
{
    const char* xml =
      "<?xml version=\"1.0\"?>\n"
      "<vle_project version=\"0.5\" author=\"Gauthier Quesnel\""
      " date=\"Mon, 12 Feb 2007 23:40:31 +0100\" >\n";

    {
        try {
            vpz::Vpz vpz;
            vpz.parseMemory(xml);
        } catch (const std::exception& e) {
            std::cerr << __func__ << ": " << e.what() << '\n';
        }
    }

    vpz::Vpz vpz;
    EnsuresThrow(vpz.parseMemory(xml), std::exception);
}

void
vpz_throw_2()
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
      "   </in2>\n"
      "   <out>\n"
      "    <port name=\"out1\" />\n"
      "    <port name=\"out2\" />\n"
      "   </out>\n"
      "  </model>\n"
      " </structures>\n"
      " <experiment name=\"graphtester\" seed=\"123\" >\n"
      "  <views>\n"
      "   <outputs>\n"
      "    <output name=\"view\" format=\"local\" package=\"vle.output\" "
      "plugin=\"storage\" />\n"
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
    {
        try {
            vpz::Vpz vpz;
            vpz.parseMemory(xml);
        } catch (const std::exception& e) {
            std::cerr << __func__ << ": " << e.what() << '\n';
        }
    }

    vpz::Vpz vpz;
    EnsuresThrow(vpz.parseMemory(xml), std::exception);
}

void
atomicmodel_vpz_throw1()
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
      "    <output name=\"view\" format=\"local\" package=\"vle.output\" "
      "plugin=\"storage\" />\n"
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

    {
        try {
            vpz::Vpz vpz;
            vpz.parseMemory(xml);
        } catch (const std::exception& e) {
            std::cerr << __func__ << ": " << e.what() << '\n';
        }
    }

    vpz::Vpz vpz;
    EnsuresThrow(vpz.parseMemory(xml), std::exception);
}

void
atomicmodel_vpz_throw2()
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
      "    <output name=\"view\" format=\"local\" package=\"vle.output\" "
      "plugin=\"storage\" />\n"
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

    {
        try {
            vpz::Vpz vpz;
            vpz.parseMemory(xml);
        } catch (const std::exception& e) {
            std::cerr << __func__ << ": " << e.what() << '\n';
        }
    }

    vpz::Vpz vpz;
    EnsuresThrow(vpz.parseMemory(xml), std::exception);
}

int
main()
{
    vle::Init app;

    atomicmodel_timed_vpz();
    atomicmodel_vpz_event_output();
    atomicmodel_vpz_internal_output();
    atomicmodel_vpz_confluent();
    vpz_throw();
    vpz_throw_1();
    vpz_throw_2();
    atomicmodel_vpz_throw1();
    atomicmodel_vpz_throw2();

    return unit_test::report_errors();
}
