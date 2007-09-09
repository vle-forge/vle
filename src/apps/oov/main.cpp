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

#include <apps/oov/OptionGroup.hpp>
#include <vle/manager/VLE.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/oov/OOV.hpp>
#include <vle/oov/NetStreamReader.hpp>

using namespace vle;

int main(int argc, char* argv[])
{
    Glib::OptionContext context;

    apps::CommandOptionGroup command;
    context.set_main_group(command);

    try {
        context.parse(argc, argv);
        command.check();
        utils::Trace::trace().setLogFile(
            utils::Trace::getLogFilename("oov.log"));
        utils::Trace::trace().setLevel(
            static_cast < utils::Trace::Level >(command.verbose()));
    } catch(const Glib::Error& e) {
        std::cerr << "Error parsing command line: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch(const std::exception& e) {
        std::cerr << "Command line error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    if (command.infos()) {
        manager::VLE::printInformations(std::cerr);
        return EXIT_SUCCESS;
    } else if (command.version()) {
        manager::VLE::printVersion(std::cerr);
        return EXIT_SUCCESS;
    }

    if (command.isDaemon()) {
        utils::buildDaemon();
    }

    try {
        oov::NetStreamReader net(command.port());
        net.setBufferSize(30);
        net.process();
    } catch(const std::exception& e) {
        std::cerr << "oov failure: " << utils::demangle(e.what()) << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

