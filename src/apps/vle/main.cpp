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
#include <vle/utils/Trace.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/i18n.hpp>
#include <boost/filesystem.hpp>
#include <boost/version.hpp>
#include <iostream>

namespace vle {

void appendToCommandLineList(const char* param, manager::CmdArgs& out)
{
    namespace fs = boost::filesystem;

    fs::path p(param);
#if BOOST_VERSION > 103600
    if (fs::is_regular_file(p)) {
#else
    if (fs::is_regular(p)) {
#endif
        out.push_back(p.file_string().c_str());
        return;
    } else if (not utils::Package::package().name().empty()) {
        fs::path np(utils::Path::path().getPackageExpFile(param));
#if BOOST_VERSION > 103600
        if (fs::is_regular_file(np)) {
#else
        if (fs::is_regular(np)) {
#endif
            out.push_back(np.file_string().c_str());
            return;
        }
    }
    throw utils::ArgError(fmt(_("Filename '%1%' does not exist")) % param);
}

void buildCommandLineList(int argc, char* argv[], manager::CmdArgs& lst)
{
    using utils::Package;
    using utils::Path;
    using utils::PathList;

    int i = 1;
    bool stop = false;

    if (not Package::package().name().empty()) {
        while (stop == false and i < argc) {
            if (strcmp(argv[i], "create") == 0) {
                Package::package().create();
            } else if (strcmp(argv[i], "configure") == 0) {
                Package::package().configure();
                Package::package().wait(std::cout, std::cerr);
                stop = not Package::package().isSuccess();
            } else if (strcmp(argv[i], "build") == 0) {
                Package::package().build();
                Package::package().wait(std::cout, std::cerr);
                if (Package::package().isSuccess()) {
                    Package::package().install();
                    Package::package().wait(std::cout, std::cerr);
                }
                stop = not Package::package().isSuccess();
            } else if (strcmp(argv[i], "install") == 0) {
                Package::package().install();
                Package::package().wait(std::cout, std::cerr);
                stop = not Package::package().isSuccess();
            } else if (strcmp(argv[i], "clean") == 0) {
                Package::package().clean();
                Package::package().wait(std::cout, std::cerr);
                stop = not Package::package().isSuccess();
            } else if (strcmp(argv[i], "package") == 0) {
                Package::package().pack();
                Package::package().wait(std::cout, std::cerr);
                stop = not Package::package().isSuccess();
            } else if (strcmp(argv[i], "list") == 0) {
                PathList vpz(Path::path().getInstalledExperiments());
                std::copy(vpz.begin(), vpz.end(), std::ostream_iterator
                          < std::string >(std::cout, "\n"));
                PathList libs(utils::Path::path().getInstalledLibraries());
                std::copy(libs.begin(), libs.end(), std::ostream_iterator
                          < std::string >(std::cout, "\n"));
            } else {
                break;
            }
            ++i;
        }
    }
    if (stop == false) {
        for (; i < argc; ++i) {
            appendToCommandLineList(argv[i], lst);
        }
    }
}

} // namespace vle

int main(int argc, char* argv[])
{
    using namespace vle;

    manager::init();

    Glib::OptionContext context;
    apps::OptionGroup command;
    context.set_main_group(command);

    try {
        context.parse(argc, argv);
        command.check();
        utils::Trace::trace().setLevel(static_cast < utils::Trace::Level >(
                command.verbose()));
        utils::Package::package().select(command.package());
    } catch(const Glib::Error& e) {
        std::cerr << fmt(_("Command line error: %1%\n")) % e.what();
        manager::finalize();
        return EXIT_FAILURE;
    } catch(const std::exception& e) {
        std::cerr << fmt(_("Command line error: %1%\n")) % e.what();
        manager::finalize();
        return EXIT_FAILURE;
    }

    if (command.infos()) {
        utils::printInformations(std::cerr);
        return EXIT_SUCCESS;
    }

    if (command.version()) {
        utils::printVersion(std::cerr);
        return EXIT_SUCCESS;
    }

    if (argc == 1) {
        utils::printHelp(std::cerr);
        return EXIT_SUCCESS;
    }

    manager::CmdArgs lst;

    try {
        buildCommandLineList(argc, argv, lst);
    } catch(const Glib::Error& e) {
        std::cerr << fmt(_("Error: %1%\n")) % e.what();
        manager::finalize();
        return EXIT_FAILURE;
    } catch (const std::exception& e) {
        std::cerr << fmt(_("Error: %1%\n")) % e.what();
        manager::finalize();
        return EXIT_FAILURE;
    }

    bool result = true;
    if (not lst.empty()) {
        manager::VLE vle(command.port());

        if (command.manager()) {
            result = vle.runManager(command.allInLocal(), command.savevpz(),
                                    command.processor(), lst);
        } else if (command.simulator()) {
            result = vle.runSimulator(command.processor());
        } else if (command.justRun()) {
            result = vle.justRun( command.processor(), lst);
        }
    }

    manager::finalize();
    return result ? EXIT_SUCCESS : EXIT_FAILURE;
}
