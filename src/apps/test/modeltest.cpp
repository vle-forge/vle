/** 
 * @file main.cpp
 * @brief 
 * @author The vle Development Team
 * @date jeu, 18 mai 2006 17:55:16 +0200
 */

/*
 * Copyright (C) 2006 - The vle Development Team
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

#include <vle/graph/CoupledModel.hpp>
#include <vle/graph/AtomicModel.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Path.hpp>
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
    graph::CoupledModel* cpl = new graph::CoupledModel(0);
    cpl->setName("top model");

    graph::AtomicModel* mdl = new graph::AtomicModel(cpl);
    mdl->setName("testing model");
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
    bool success;

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
