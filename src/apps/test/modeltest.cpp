/**
 * @file src/apps/test/modeltest.cpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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




#include <vle/graph/CoupledModel.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Trace.hpp>
#include <glibmm/fileutils.h>
#include <glibmm/spawn.h>
#include <iostream>

using namespace vle;


void print_help()
{
    std::cerr << "vlemodeltest modelname [xml dynamics files]" << std::endl;
}

void build_vpz(const std::string& modelname, vpz::Vpz& vpz)
{
    graph::CoupledModel* cpl = new graph::CoupledModel("top model", 0);

    graph::AtomicModel* mdl = new graph::AtomicModel("testing model", cpl);
    cpl->addModel(mdl);
    vpz.project().model().setModel(cpl);

    vpz::Dynamic dyn(modelname);
    dyn.setLibrary(modelname);
    vpz.project().experiment().setDuration(1);
    vpz.project().experiment().setName("testing");
}

bool run_model(const std::string& modelname)
{
    try {
        std::cout << "Building VPZ file .......... ";
        vpz::Vpz v;
        build_vpz(modelname, v);
        std::string filename(utils::build_temp("vlemodeltest"));
        v.write(filename);
        std::cout << "ok\n";

        std::cout << "VLE started ................ ";

        std::string cmd("vle -r -V 3 ");
        cmd += filename;

        std::string stderror, stdoutput;
        int status;

        Glib::spawn_command_line_sync(cmd, &stdoutput, &stderror, &status);

        if (status != 0) {
            std::cout << "failed\n";
            std::cerr << stdoutput << "\n" << stderror << std::endl;
            return false;
        } else {
            std::cout << "ok\n";
            return true;
        }
    } catch(const std::exception& e) {
        std::cerr << "failed\n" << e.what() << std::endl;
        return false; 
    }
}

int main(int argc, char* argv[])
{
    vle::utils::Trace::trace().setLevel(vle::utils::Trace::DEBUG);

    bool success = true;

    switch(argc) {
    case 1:
        print_help();
        success = true;
        break;

    case 2:
        success = run_model(argv[1]);
        break;

    default:
        print_help();
    }

    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
