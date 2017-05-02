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

#include <stdexcept>
#include <vle/utils/Exception.hpp>
#include <vle/utils/unit-test.hpp>
#include <vle/value/Value.hpp>
#include <vle/vle.hpp>
#include <vle/vpz/Vpz.hpp>

using namespace vle;
using namespace vpz;

void
vpz_obs_port()
{
    Observable obs("Obs");
    EnsuresEqual(obs.exist("port"), false);
    EnsuresThrow(obs.get("port"), utils::ArgError);
    EnsuresNotThrow(obs.del("port"), std::exception);
    EnsuresNotThrow(obs.add("port"), std::exception);
    EnsuresThrow(obs.add("port"), utils::ArgError);
    EnsuresEqual(obs.exist("port"), true);
    EnsuresNotThrow(obs.get("port"), std::exception);
    EnsuresNotThrow(obs.del("port"), std::exception);
    EnsuresEqual(obs.exist("port"), false);
}

void
vpz_add_output()
{
    Views views;

    views.addStreamOutput("out1", "", "storage", "");
    views.addStreamOutput("out2", "", "storage", "");
    views.addStreamOutput("out3", "", "storage", "");

    EnsuresNotThrow(views.addEventView("view1", View::FINISH, "out1"),
                    std::exception);
    EnsuresNotThrow(views.addEventView("view2", View::OUTPUT, "out4"),
                    std::exception);

    EnsuresNotThrow(views.addTimedView("view3", 1.0, "out3"), std::exception);
    EnsuresThrow(views.addTimedView("view3", 1.0, "out1"), utils::ArgError);
    EnsuresThrow(views.addTimedView("view4", 0.5, "out4"), utils::ArgError);
    EnsuresThrow(views.addTimedView("view4", 0.0, "out2"), utils::ArgError);
}

int
main()
{
    vle::Init app;

    vpz_obs_port();
    vpz_add_output();

    return unit_test::report_errors();
}
