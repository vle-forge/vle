/*
 * @file apps/vle/main.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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
#include <vle/manager/Manager.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Preferences.hpp>
#include <vle/utils/RemoteManager.hpp>
#include <vle/utils/i18n.hpp>
#include <boost/version.hpp>
#include <iostream>
#include <fstream>

namespace vle {

void makeAll()
{
    typedef std::set < std::string > Depends;
    typedef std::map < std::string, Depends > AllDepends;

    AllDepends deps = manager::Manager::depends();
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

    uniq.insert(current);

    std::string error(Path::buildTemp("build-cerr"));
    std::ofstream f(error.c_str());

    for (Depends::iterator it = uniq.begin(); it != uniq.end(); ++it) {
        Package::package().select(*it);
        std::cerr << fmt("Package [%1%]") % *it;
        Package::package().configure();
        Package::package().wait(std::cerr, f);
        if (Package::package().isSuccess()) {
            Package::package().build();
            Package::package().wait(std::cerr, f);
            if (Package::package().isSuccess()) {
                Package::package().install();
                Package::package().wait(std::cerr, f);
            }
        }
    }

    if (not Package::package().isSuccess()) {
        std::cerr << fmt("See %1% for log\n") % error;
    }

    utils::Package::package().select(current);
}

void showDepends()
{
    typedef std::set < std::string > Depends;
    typedef std::map < std::string, Depends > AllDepends;

    AllDepends deps = manager::Manager::depends();

    for (AllDepends::const_iterator it = deps.begin(); it != deps.end(); ++it) {
        if (it->second.empty()) {
            std::cerr << utils::Path::basename(it->first) << ": -\n";
        } else {
            std::cerr << utils::Path::basename(it->first) << ": ";

            Depends::const_iterator jt = it->second.begin();
            while (jt != it->second.end()) {
                Depends::const_iterator kt = jt++;
                std::cerr << *kt;
                if (jt != it->second.end()) {
                    std::cerr << ", ";
                } else {
                    std::cerr << '\n';
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
              std::ostream_iterator < std::string >(std::cerr, "\n"));

    utils::PathList libs(utils::Path::path().getInstalledLibraries());
    std::copy(libs.begin(), libs.end(),
              std::ostream_iterator < std::string >(std::cerr, "\n"));
}

void listPackages()
{
    using utils::Path;

    utils::PathList vpz = Path::path().getInstalledPackages();
    std::sort(vpz.begin(), vpz.end());

    std::copy(vpz.begin(), vpz.end(),
              std::ostream_iterator < std::string >(std::cerr, "\n"));
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

bool cliPackage(int argc, char* argv[], manager::CmdArgs& lst)
{
    using utils::Package;
    using utils::Path;
    using utils::PathList;

    int i = 1;
    bool stop = false;

    utils::Package::package().refresh();

    if (not Package::package().name().empty()) {
        while (stop == false and i < argc) {
            if (strcmp(argv[i], "create") == 0) {
                Package::package().create();
            } else if (strcmp(argv[i], "configure") == 0) {
                Package::package().configure();
                Package::package().wait(std::cerr, std::cerr);
                stop = not Package::package().isSuccess();
            } else if (strcmp(argv[i], "build") == 0) {
                Package::package().build();
                Package::package().wait(std::cerr, std::cerr);
                if (Package::package().isSuccess()) {
                    Package::package().install();
                    Package::package().wait(std::cerr, std::cerr);
                }
                stop = not Package::package().isSuccess();
            } else if (strcmp(argv[i], "test") == 0) {
                Package::package().test();
                Package::package().wait(std::cerr, std::cerr);
                stop = not Package::package().isSuccess();
            } else if (strcmp(argv[i], "install") == 0) {
                Package::package().install();
                Package::package().wait(std::cerr, std::cerr);
                stop = not Package::package().isSuccess();
            } else if (strcmp(argv[i], "clean") == 0) {
                Package::package().clean();
                Package::package().wait(std::cerr, std::cerr);
                stop = not Package::package().isSuccess();
            } else if (strcmp(argv[i], "rclean") == 0) {
                Package::removePackageBinary(Package::package().name());
            } else if (strcmp(argv[i], "package") == 0) {
                Package::package().pack();
                Package::package().wait(std::cerr, std::cerr);
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

    if (not stop) {
        for (; i < argc; ++i) {
            appendToCommandLineList(argv[i], lst);
        }
    }

    return not stop;
}

bool cliDirect(int argc, char* argv[], manager::CmdArgs& lst)
{
    for (int i = 1; i < argc; ++i) {
        appendToCommandLineList(argv[i], lst);
    }

    return true;
}

bool cliRemote(int argc, char* argv[])
{
    bool error = true;

    if (argc > 1) {
        utils::RemoteManager rm;
        error = false;

        if (strcmp(argv[1], "update") == 0) {
            rm.start(utils::REMOTE_MANAGER_UPDATE, std::string(), &std::cout);
            rm.join();
        } else if (argc > 1 and strcmp(argv[1], "install") == 0) {
            for (int i = 2; i < argc; ++i) {
                rm.start(utils::REMOTE_MANAGER_INSTALL, argv[i], &std::cout);
                rm.join();
            }
        } else if (argc > 1 and strcmp(argv[1], "source") == 0) {
            for (int i = 2; i < argc; ++i) {
                rm.start(utils::REMOTE_MANAGER_SOURCE, argv[i], &std::cout);
                rm.join();
            }
        } else if (argc > 1 and strcmp(argv[1], "show") == 0) {
            for (int i = 2; i < argc; ++i) {
                rm.start(utils::REMOTE_MANAGER_SHOW, argv[i], &std::cout);
                rm.join();
            }
        } else if (argc > 1 and strcmp(argv[1], "search") == 0) {
            for (int i = 2; i < argc; ++i) {
                rm.start(utils::REMOTE_MANAGER_SEARCH, argv[i], &std::cout);
                rm.join();
            }
        } else {
            error = true;
        }
    }

    if (error) {
        throw utils::ArgError(
            _("Bad argument:\n"
              "\tvle --remote update\n"
              "\tvle --remote install glue-1.0\n"
              "\tvle --remote source glue-1.0\n"
              "\tvle --remote show glue-1.0\n"
              "\tvle --remote search 'glue*'\n"));
    }

    return not error;
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
        utils::Trace::setLevel(utils::Trace::cast(command.verbose()));
        utils::Package::package().select(command.currentPackage());
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
        std::cerr << fmt(_(
                "Virtual Laboratory Environment - %1%\n"
                "Copyright (C) 2003 - 2011 The VLE Development Team.\n")) %
            VLE_NAME_COMPLETE << "\n" << std::endl;
        return EXIT_SUCCESS;
    }

    if (command.version()) {
        std::cerr << fmt(_(
                "Virtual Laboratory Environment - %1%\n"
                "Copyright (C) 2003 - 2011 The VLE Development Team.\n"
                "VLE comes with ABSOLUTELY NO WARRANTY.\n"
                "You may redistribute copies of VLE\n"
                "under the terms of the GNU General Public License.\n"
                "For more information about these matters, see the file named "
                "COPYING.\n")) % VLE_NAME_COMPLETE << std::endl;
        return EXIT_SUCCESS;
    }

    if (command.list()) {
        listPackages();
        return EXIT_SUCCESS;
    }

    if (argc == 1) {
        std::cerr << fmt(_(
                "Virtual Laboratory Environment - %1%\n"
                "Copyright (C) 2003 - 2011 The VLE Development Team.\n"
                "VLE is a multi-modeling environment to build,\nsimulate "
                "and analyse models of dynamic complex systems.\n"
                "For more information, see manuals with 'man vle' or\n"
                "the VLE website http://sourceforge.net/projects/vle/\n")) %
            VLE_NAME_COMPLETE << std::endl;
        return EXIT_SUCCESS;
    }

    manager::CmdArgs lst;

    bool success = true;
    try {
        if (not utils::Package::package().name().empty()) {
            success = cliPackage(argc, argv, lst);
        } else if (command.remote()) {
            success = cliRemote(argc, argv);
        } else {
            success = cliDirect(argc, argv, lst);
        }
    } catch(const Glib::Error& e) {
        std::cerr << fmt(_("Error: %1%\n")) % e.what();
        manager::finalize();
        return EXIT_FAILURE;
    } catch (const std::exception& e) {
        std::cerr << fmt(_("Error: %1%\n")) % e.what();
        manager::finalize();
        return EXIT_FAILURE;
    }

    if (success and not lst.empty()) {
        manager::Manager manager(command.quiet());
        if (command.manager()) {
            success = manager.runManager(command.allInLocal(),
                                        command.savevpz(),
                                        command.processor(),
                                        lst);
        //} else if (command.simulator()) {
            //success = manager.runSimulator(command.processor(),
                                          //command.port());
        } else if (command.justRun()) {
            success = manager.justRun(command.processor(), lst);
        }

        if (manager.quiet()) {
            manager.close();
        }

        if (not success) {
            if (not manager.filename().empty()) {
                std::cerr << fmt(_(
                        "\n/!\\ vle manager error reported in %1%\n")) %
                    manager.filename();
            } else {
                std::cerr << fmt(_(
                        "\n/!\\ vle manager error reported\n"));
            }
        }

        if (utils::Trace::haveWarning()) {
            std::cerr << fmt(
                "\n/!\\ Some warnings during run: See file %1%\n") %
                utils::Trace::getLogFile();
        }
    }

    manager::finalize();
    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
