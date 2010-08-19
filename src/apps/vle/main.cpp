/**
 * @file apps/vle/main.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
 * Copyright (C) 2003-2010 ULCO http://www.univ-littoral.fr
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
#include <boost/version.hpp>
#include <iostream>

namespace vle {

void makeAll()
{
    typedef std::set < std::string > Depends;
    typedef std::map < std::string, Depends > AllDepends;

    AllDepends deps = manager::depends();
    Depends uniq;

    for (AllDepends::const_iterator it = deps.begin(); it != deps.end(); ++it) {
        for (Depends::const_iterator jt = it->second.begin(); jt !=
             it->second.end(); ++jt) {
            uniq.insert(*jt);
        }
    }

    using utils::Path;
    using utils::Package;

    std::string current = Package::package().name();

    std::string error(Path::buildTemp("build-cerr"));
    std::ofstream f(error.c_str());

    for (Depends::iterator it = uniq.begin(); it != uniq.end(); ++it) {
        Package::package().select(*it);
        std::cout << fmt("Package [%1%]") % *it;
        Package::package().configure();
        Package::package().wait(std::cout, f);
        if (Package::package().isSuccess()) {
            Package::package().build();
            Package::package().wait(std::cout, f);
            if (Package::package().isSuccess()) {
                Package::package().install();
                Package::package().wait(std::cout, f);
            }
        }
    }

    if (not Package::package().isSuccess()) {
        std::cout << fmt("See %1% for log\n") % error;
    }

    utils::Package::package().select(current);
}

void showDepends()
{
    typedef std::set < std::string > Depends;
    typedef std::map < std::string, Depends > AllDepends;

    AllDepends deps = manager::depends();

    for (AllDepends::const_iterator it = deps.begin(); it != deps.end(); ++it) {
        if (it->second.empty()) {
            std::cout << utils::Path::basename(it->first) << ": -\n";
        } else {
            std::cout << utils::Path::basename(it->first) << ": ";

            Depends::const_iterator jt = it->second.begin();
            while (jt != it->second.end()) {
                Depends::const_iterator kt = jt++;
                std::cout << *kt;
                if (jt != it->second.end()) {
                    std::cout << ", ";
                } else {
                    std::cout << '\n';
                }
            }
        }
    }
}

void listContentPackage()
{
    using utils::Path;

    utils::PathList packages = Path::path().getInstalledExperiments();
    std::sort(packages.begin(), packages.end());

    std::copy(packages.begin(), packages.end(),
              std::ostream_iterator < std::string >(std::cout, "\n"));

    utils::PathList libs(utils::Path::path().getInstalledLibraries());
    std::copy(libs.begin(), libs.end(),
              std::ostream_iterator < std::string >(std::cout, "\n"));
}

void listPackages()
{
    using utils::Path;

    utils::PathList vpz = Path::path().getInstalledPackages();
    std::sort(vpz.begin(), vpz.end());

    std::copy(vpz.begin(), vpz.end(),
              std::ostream_iterator < std::string >(std::cout, "\n"));
}

void appendToCommandLineList(const char* param, manager::CmdArgs& out)
{
    using utils::Path;
    using utils::Package;

    const std::string p(param);
    if (Path::existFile(p)) {
        if (not Package::package().name().empty()) {
            const std::string np = Path::path().getPackageExpFile(param);
            if (Path::existFile(np)) {
                throw utils::ArgError(fmt(
                        _("Filename '%1%' exists in current directory (%2%) "
                          "and in the package exp directory (%3%).")) % param %
                    utils::Path::getCurrentPath() % np);
            }
        }
        out.push_back(p);
        return;
    } else if (not Package::package().name().empty()) {
        std::string np = Path::path().getPackageExpFile(param);
        if (Path::existFile(np)) {
            out.push_back(np);
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
            } else if (strcmp(argv[i], "test") == 0) {
                Package::package().test();
                Package::package().wait(std::cout, std::cerr);
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
            } else if (strcmp(argv[i], "all") == 0) {
                makeAll();
                stop = not Package::package().isSuccess();
            } else if (strcmp(argv[i], "depends") == 0) {
                showDepends();
                stop = not Package::package().isSuccess();
            } else if (strcmp(argv[i], "list") == 0) {
                listContentPackage();
                stop = not Package::package().isSuccess();
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

    if (command.list()) {
        listPackages();
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
        if (command.manager()) {
            result = manager::runManager(command.allInLocal(),
                                         command.savevpz(),
                                         command.processor(),
                                         lst);
        } else if (command.simulator()) {
            result = manager::runSimulator(command.processor(),
                                           command.port());
        } else if (command.justRun()) {
            result = manager::justRun(command.processor(), lst);
        }
    }

    manager::finalize();
    return result ? EXIT_SUCCESS : EXIT_FAILURE;
}
