/**
 * @file apps/vle/main.cpp
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


#include <apps/vle/OptionGroup.hpp>
#include <vle/manager/VLE.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/PathOptionGroup.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/Debug.hpp>

int main(int argc, char* argv[])
{
    Glib::OptionContext context;

    vle::apps::CommandOptionGroup command;
    vle::apps::ManagerOptionGroup manag;
    vle::apps::GlobalOptionGroup global;
    vle::utils::PathOptionGroup path;

    context.set_main_group(command);
    context.add_group(manag);
    context.add_group(global);
    context.add_group(path);

    try {
        context.parse(argc, argv);
        command.check();
        manag.check();
        global.check();
        vle::utils::Trace::trace().setLevel(
            static_cast < vle::utils::Trace::Level >(global.verbose()));
        path.assignToPath();
    } catch(const Glib::Error& e) {
        std::cerr << "Error parsing command line: " << e.what() << std::endl;
        vle::utils::finalize();
        return EXIT_FAILURE;
    } catch(const std::exception& e) {
        std::cerr << "Command line error: " << e.what() << std::endl;
        vle::utils::finalize();
        return EXIT_FAILURE;
    }

    bool result = true;
    if (global.infos()) {
        vle::utils::printInformations(std::cerr);
    } else if (global.version()) {
        vle::utils::printVersion(std::cerr);
    } else {
        vle::manager::VLE vle(command.port());

        if (command.manager()) {
            result = vle.runManager(manag.allInLocal(), manag.savevpz(),
                                    command.processor(),
                                    vle::manager::CmdArgs(argv + 1, argv +
                                                          argc));
        } else if (command.simulator()) {
            result = vle.runSimulator(command.processor());
        } else if (command.justRun()) {
            result = vle.justRun(command.processor(),
                                 vle::manager::CmdArgs(argv + 1, argv + argc));
        }
    }

    vle::utils::finalize();
    return result ? EXIT_SUCCESS : EXIT_FAILURE;
}

