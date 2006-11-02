/** 
 * @file main.cpp
 * @brief Main function.
 * @author The vle Development Team
 * @date lun, 29 mai 2006 12:00:38 +0200
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

#include <apps/vle/OptionGroup.hpp>
#include <vle/manager/VLE.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Trace.hpp>



int main(int argc, char* argv[])
{
    Glib::OptionContext context;

    vle::apps::CommandOptionGroup command;
    vle::apps::ManagerOptionGroup manag;
    vle::apps::SimulatorOptionGroup simu;
    vle::apps::GlobalOptionGroup global;

    context.set_main_group(command);
    context.add_group(manag);
    context.add_group(simu);
    context.add_group(global);

    try {
        context.parse(argc, argv);
        command.check();
        manag.check();
        simu.check();
        global.check();
        vle::utils::Trace::trace().set_level(global.verbose());
    } catch(const Glib::Error& e) {
        std::cerr << "Error parsing command line: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch(const std::exception& e) {
        std::cerr << "Command line error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    if (global.infos()) {
        vle::manager::VLE::printInformations(std::cerr);
        return EXIT_SUCCESS;
    } else if (global.version()) {
        vle::manager::VLE::printVersion(std::cerr);
        return EXIT_SUCCESS;
    }

    vle::manager::VLE vle(command.port());
    bool result;

    if (command.manager()) {
        result = vle.runManager(manag.daemon(), manag.allInLocal(),
                                vle::manager::VLE::CmdArgs(argv + 1, argv +
                                                           argc));
    } else if (command.simulator()) {
        result = vle.runSimulator(simu.process());
    } else if (command.justRun()) {
        result = vle.justRun(vle::manager::VLE::CmdArgs(argv + 1, argv + argc));
    }
    return result ? EXIT_SUCCESS : EXIT_FAILURE;
}

