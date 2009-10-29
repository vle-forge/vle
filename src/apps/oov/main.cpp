/**
 * @file apps/oov/main.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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


#include <vle/manager/VLE.hpp>
#include <apps/oov/OptionGroup.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/oov/OOV.hpp>
#include <vle/oov/NetStreamReader.hpp>
#include <iostream>

using namespace vle;

int main(int argc, char* argv[])
{
    Glib::OptionContext context;

    apps::CommandOptionGroup command;
    context.set_main_group(command);
    vle::manager::init();

    try {
        context.parse(argc, argv);
        command.check();
        utils::Trace::trace().setLogFile(
            utils::Trace::getLogFilename("oov.log"));
        utils::Trace::trace().setLevel(
            static_cast < utils::Trace::Level >(command.verbose()));
    } catch(const Glib::Error& e) {
        std::cerr << _("Error parsing command line: ") << e.what() << std::endl;
        utils::finalize();
        return EXIT_FAILURE;
    } catch(const std::exception& e) {
        std::cerr << _("Command line error: ") << e.what() << std::endl;
        utils::finalize();
        return EXIT_FAILURE;
    }

    if (command.isDaemon()) {
        utils::buildDaemon();
    }

    bool result = true;
    if (command.infos()) {
        std::cerr << _("Oov - the Output of VLE\n");
        utils::printInformations(std::cerr);
    } else if (command.version()) {
        std::cerr << _("Oov - the Output of VLE\n");
        utils::printVersion(std::cerr);
    } else {
        try {
            oov::NetStreamReader net(command.port());
            net.setBufferSize(4096);
            net.process();
        } catch(const Glib::Exception& e) {
            result = false;
            std::cerr << _("\n/!\\ oov Glib error reported: ") <<
                vle::utils::demangle(typeid(e)) << _("\n") << e.what();
        } catch(const std::exception& e) {
            result = false;
            std::cerr << _("\n/!\\ oov exception reported: ") <<
                vle::utils::demangle(typeid(e)) << _("\n") << e.what();
        }
    }

    vle::manager::finalize();
    return result ? EXIT_SUCCESS : EXIT_FAILURE;
}

