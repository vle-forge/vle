/**
 * @file apps/eov/main.cpp
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


#include <vle/utils/Tools.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Socket.hpp>
#include <vle/utils/Path.hpp>
#include <vle/eov/Plugin.hpp>
#include <vle/eov/NetStreamReader.hpp>
#include <vle/eov/MainWindow.hpp>
#include <apps/eov/OptionGroup.hpp>
#include <gtkmm/main.h>
#include <gtkmm/messagedialog.h>
#include <libglademm.h>

using namespace vle;

void eov_on_error(const std::string& type, const std::string& what)
{
    Gtk::MessageDialog box(
        (boost::format("eov reported: %1% (%2%)") % what % type).str(), false,
        Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);

    box.run();
}

int main(int argc, char* argv[])
{
    utils::init();

    Glib::OptionContext context; 
    eov::CommandOptionGroup command;
    context.set_main_group(command);

    try {
        context.parse(argc, argv);
        command.check();
        utils::Trace::trace().setLogFile(
            utils::Trace::getLogFilename("eov.log"));
        utils::Trace::trace().setLevel(
            static_cast < utils::Trace::Level >(command.verbose()));
    } catch(const Glib::Error& e) {
        std::cerr << "Error parsing command line: " << e.what() << std::endl;
        utils::finalize();
        return EXIT_FAILURE;
    } catch(const std::exception& e) {
        std::cerr << "Command line error: " << e.what() << std::endl;
        utils::finalize();
        return EXIT_FAILURE;
    }

    bool result = true;
    if (command.infos()) {
        std::cerr << "EOV - the Eyes of VLE\n";
        utils::printInformations(std::cerr);
    } else if (command.version()) {
        std::cerr << "EOV - the Eyes of VLE\n";
        utils::printVersion(std::cerr);
    } else {
        try {
            Gtk::Main app(argc, argv);
            Glib::RefPtr < Gnome::Glade::Xml > xml(
                Gnome::Glade::Xml::create(
                    utils::Path::path().getGladeFile("eov.glade")));
            eov::MainWindow main(xml, command.port());
            app.run(main.window());
        } catch(const Glib::Exception& e) {
            result = false;
            eov_on_error(vle::utils::demangle(typeid(e)), e.what());
        } catch(const std::exception& e) {
            result = false;
            eov_on_error(vle::utils::demangle(typeid(e)), e.what());
        }
    }

    utils::finalize();
    return result ? EXIT_SUCCESS : EXIT_FAILURE;
}

