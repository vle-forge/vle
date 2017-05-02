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
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vle/devs/Simulator.hpp>
#include <vle/gvle/vlevpz.hpp>
#include <vle/utils/unit-test.hpp>
#include <vle/value/Boolean.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/value/Null.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Table.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/User.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/XML.hpp>

const char* vpz_content =
  "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
  "<vle_project version=\"2.0\" author=\"Ronan Trépos\" date=\"2015-Jun-15 "
  "16:13:07\">\n"
  " <structures>\n"
  "<model width=\"482\" x=\"0\" y=\"0\" height=\"376\" type=\"coupled\" "
  "name=\"LogExMM\">\n"
  "<submodels>\n"
  "<model width=\"100\" x=\"42\" y=\"33\" dynamics=\"metaManagerDyn\" \n"
  "       conditions=\"cond,simulation_engine\" height=\"30\" \n"
  "       observables=\"obs\" type=\"atomic\" name=\"LogExMM\">\n"
  "</model>\n"
  "</submodels>\n"
  "<connections>\n"
  "</connections>\n"
  "</model>\n"
  "</structures>\n"
  " <dynamics>\n"
  "<dynamic package=\"vle.recursive\" name=\"metaManagerDyn\" "
  "library=\"MetaManagerDyn\"/>\n"
  "</dynamics>\n"
  "<experiment name=\"ExBohachevskyMM\" combination=\"linear\">\n"
  "<conditions>\n"
  "<condition name=\"cond\">\n"
  " <port name=\"id_input_x\">\n"
  "  <string>cond/init_value_x</string>\n"
  " </port>\n"
  " <port name=\"myport\">\n"
  "  <tuple></tuple>\n"
  "  <tuple>1.5 2 4.6</tuple>\n"
  " </port>\n"
  " <port name=\"id_output_y\">\n"
  "  <map>\n"
  "   <key name=\"integration\">\n"
  "    <string>mse</string>\n"
  "   </key>\n"
  "   <key name=\"mse_observations\">\n"
  "    <tuple></tuple>\n"
  "   </key>\n"
  "   <key name=\"path\">\n"
  "    <string>LogEx:LogEx.y</string>\n"
  "   </key>\n"
  "  </map>\n"
  " </port>\n"
  " <port name=\"id_output_y_noise\">\n"
  "  <map>\n"
  "   <key name=\"integration\">\n"
  "    <string>max</string>\n"
  "   </key>\n"
  "   <key name=\"path\">\n"
  "    <string>view/ExBohachevsky:ExBohachevsky.y_noise</string>\n"
  "   </key>\n"
  "  </map>\n"
  " </port>\n"
  "</condition>\n"
  "</conditions>\n"
  "<views>\n"
  "<outputs>\n"
  "<output package=\"vle.output\" plugin=\"file\" format=\"local\" "
  "location=\"\" name=\"view\"/>\n"
  "</outputs>\n"
  "<observables>\n"
  "<observable name=\"obs\">\n"
  "<port name=\"output_0_0\">\n"
  " <attachedview name=\"view\"/>\n"
  "</port>\n"
  "<port name=\"output_0_1\">\n"
  " <attachedview name=\"view\"/>\n"
  "</port>\n"
  "<port name=\"output_1_0\">\n"
  " <attachedview name=\"view\"/>\n"
  "</port>\n"
  "<port name=\"output_1_1\">\n"
  " <attachedview name=\"view\"/>\n"
  "</port>\n"
  "<port name=\"outputs\">\n"
  " <attachedview name=\"view\"/>\n"
  "</port>\n"
  "</observable>\n"
  "</observables>\n"
  "<view output=\"view\" type=\"timed\" timestep=\"1.000000000000000\" "
  "name=\"view\"/>\n"
  "</views>\n"
  "</experiment>\n"
  "</vle_project>";

void
test_build_value()
{
    QXmlInputSource source;
    source.setData(QString(vpz_content));
    vle::gvle::vleVpz vpz(source);
    vle::gvle::vleDomVpz vleDom(&vpz.getDomDoc());

    // test tuple
    std::unique_ptr<vle::value::Value> t =
      vpz.buildValueFromDoc("cond", "myport", 0);
    EnsuresEqual(t->toTuple().size(), 0);
    t = vpz.buildValueFromDoc("cond", "myport", 1);
    EnsuresEqual(t->toTuple().size(), 3);
    EnsuresApproximatelyEqual(t->toTuple().at(0), 1.5, 1e-5);
    EnsuresApproximatelyEqual(t->toTuple().at(1), 2, 1e-5);
    EnsuresApproximatelyEqual(t->toTuple().at(2), 4.6, 1e-5);
}

int
main()
{
    test_build_value();

    return unit_test::report_errors();
}
