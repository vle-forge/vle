/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <vle/manager/Manager.hpp>
#include <vle/manager/Simulation.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Preferences.hpp>
#include <vle/utils/RemoteManager.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/vle.hpp>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>

#ifndef NDEBUG
# include <vle/devs/ExternalEvent.hpp>
# include <vle/devs/InternalEvent.hpp>
# include <vle/value/Value.hpp>
#endif

namespace po = boost::program_options;

typedef std::vector < std::string > CmdArgs;

struct VLE
{
    vle::Init app;

    VLE(int verbose, int trace)
    {
        vle::utils::Trace::setLevel(vle::utils::Trace::cast(verbose));

        if (trace < 0)
            vle::utils::Trace::setStandardError();
        else if (trace > 0)
            vle::utils::Trace::setStandardOutput();
        else
            vle::utils::Trace::setLogFile(
                    vle::utils::Trace::getDefaultLogFilename());
    }

    ~VLE()
    {
        if (vle::utils::Trace::haveWarning() &&
                vle::utils::Trace::getType() == vle::utils::TRACE_STREAM_FILE)
            std::cerr << vle::fmt(
                    "\n/!\\ Some warnings during run: See file %1%\n") %
                vle::utils::Trace::getLogFile() << std::endl;
    }
};

static int show_infos()
{
    std::cout << vle::fmt(
        _("Virtual Laboratory Environment - %1%\n"
          "Copyright (C) 2003 - 2014 The VLE Development Team.\n"
          "VLE comes with ABSOLUTELY NO WARRANTY.\n"
          "You may redistribute copies of VLE\n"
          "under the terms of the GNU General Public License.\n"
          "For more information about these matters, see the file named "
          "COPYING.\n")) % VLE_NAME_COMPLETE << std::endl;

    return EXIT_SUCCESS;
}

static int show_help(const po::options_description &desc)
{
    std::cout << desc << std::endl;

    return EXIT_SUCCESS;
}

static int show_version()
{
    std::cout << vle::fmt(_("%1%\n")) % VLE_NAME_COMPLETE << std::endl;

    return EXIT_SUCCESS;
}

static int show_package_list()
{
    std::vector<std::string> pkglist;
    vle::utils::Path::path().fillBinaryPackagesList(pkglist);
    std::copy(pkglist.begin(), pkglist.end(),
              std::ostream_iterator < std::string >(std::cout, "\n"));

    return EXIT_SUCCESS;
}

static int remove_configuration_file()
{
    std::cout << vle::fmt(_("Remove configuration files\n"));
    int ret = EXIT_SUCCESS;

    try {
        std::string filepath;

        filepath = vle::utils::Path::path().getHomeFile("vle.conf");
        if (vle::utils::Path::existFile(filepath))
            std::ofstream ofs(filepath.c_str());

        filepath = vle::utils::Path::path().getHomeFile("vle.log");
        if (vle::utils::Path::existFile(filepath))
            std::ofstream ofs(filepath.c_str());

        filepath = vle::utils::Path::path().getHomeFile("gvle.log");
        if (vle::utils::Path::existFile(filepath))
            std::ofstream ofs(filepath.c_str());

        vle::utils::Preferences prefs("vle.conf");
    } catch (const std::exception &e) {
        std::cerr << vle::fmt(_("Failed to remove configuration file: %1%\n"))
            % e.what();

        ret = EXIT_FAILURE;
    }

    return ret;
}

static void show_package_content(vle::utils::Package& pkg)
{


    std::vector<std::string> pkgcontent;
    try {
        pkg.fillBinaryContent(pkgcontent);
    } catch (const std::exception &e) {
        std::cerr << vle::fmt(_("Show package content error: %1% \n"))
                              % e.what();
        return;
    }

    std::copy(pkgcontent.begin(), pkgcontent.end(),
              std::ostream_iterator < std::string >(std::cout, "\n"));
}

static std::string search_vpz(const std::string &param,
        vle::utils::Package& pkg)
{
    assert(not pkg.name().empty());
    if (vle::utils::Path::existFile(param))
        return param;

    std::string np = pkg.getExpFile(param, vle::utils::PKG_BINARY);

    if (vle::utils::Path::existFile(np))
        return np;

    std::cerr << vle::fmt(_("Filename '%1%' does not exist")) % param;

    return std::string();
}

static vle::manager::LogOptions convert_log_mode()
{
    switch (vle::utils::Trace::getLevel()) {
    case vle::utils::TRACE_LEVEL_DEVS:
        return vle::manager::LOG_SUMMARY & vle::manager::LOG_RUN;
    case vle::utils::TRACE_LEVEL_EXTENSION:
    case vle::utils::TRACE_LEVEL_MODEL:
        return vle::manager::LOG_SUMMARY;
    case vle::utils::TRACE_LEVEL_ALWAYS:
    default:
        return vle::manager::LOG_NONE;
    }
}

static int run_manager(CmdArgs::const_iterator it, CmdArgs::const_iterator end,
        int processor, vle::utils::Package& pkg)
{
    vle::manager::Manager man(convert_log_mode(),
                              vle::manager::SIMULATION_NONE |
                              vle::manager::SIMULATION_NO_RETURN,
                              &std::cout);
    vle::utils::ModuleManager modules;
    int success = EXIT_SUCCESS;

    for (; it != end; ++it) {
        vle::manager::Error error;
        vle::value::Matrix *res = man.run(new vle::vpz::Vpz(
                                               search_vpz(*it, pkg)),
                modules,
                processor,
                0,
                1,
                &error);

        if (error.code) {
            std::cerr << vle::fmt(_("Experimental frames `%s' throws error %s"))
                % (*it) % error.message.c_str();

            success = EXIT_FAILURE;
        }

        delete res;
    }

    return success;
}

static int run_simulation(CmdArgs::const_iterator it,
        CmdArgs::const_iterator end, vle::utils::Package& pkg)
{
    vle::manager::Simulation sim(convert_log_mode(),
                                 vle::manager::SIMULATION_NONE |
                                 vle::manager::SIMULATION_NO_RETURN,
                                 &std::cout);
    vle::utils::ModuleManager modules;
    int success = EXIT_SUCCESS;

    for (; it != end; ++it) {
        vle::manager::Error error;
        vle::value::Map *res = sim.run(new vle::vpz::Vpz(search_vpz(*it, pkg)),
                                       modules,
                                       &error);

        if (error.code) {
            std::cerr << vle::fmt(_("Simulator `%s' throws error %s")) %
                (*it) % error.message.c_str();

            success = EXIT_FAILURE;
        }

        delete res;
    }

    return success;
}

static bool init_package(vle::utils::Package& pkg, const CmdArgs &args)
{
    if (not pkg.existsBinary()) {
        if (not pkg.existsSource()) {
            if (std::find(args.begin(), args.end(), "create") == args.end()) {
                std::cerr << vle::fmt(
                    _("Package `%1%' does not exist. Use the create command"
                      " before other command.\n")) % pkg.name();

                return false;
            }
        }
    }
    return true;
}

static int manage_package_mode(const std::string &packagename, bool manager,
                               int processor, const CmdArgs &args)
{
    CmdArgs::const_iterator it = args.begin();
    CmdArgs::const_iterator end = args.end();
    bool stop = false;

    vle::utils::Package pkg(packagename);

    if (not init_package(pkg, args))
        return EXIT_FAILURE;

    for (; not stop and it != end; ++it) {
        if (*it == "create") {
            try {
                pkg.create();
            } catch (const std::exception &e) {
                std::cerr << vle::fmt("Cannot create package: %1%")
                                  % e.what()
                          << std::endl;
                stop = true;
            }

        } else if (*it == "configure") {
            pkg.configure();
            pkg.wait(std::cerr, std::cerr);
            stop = not pkg.isSuccess();
        } else if (*it == "build") {
            pkg.build();
            pkg.wait(std::cerr, std::cerr);
            if (pkg.isSuccess()) {
                pkg.install();
                pkg.wait(std::cerr, std::cerr);
            }
            stop = not pkg.isSuccess();
        } else if (*it == "test") {
            pkg.test();
            pkg.wait(std::cerr, std::cerr);
            stop = not pkg.isSuccess();
        } else if (*it == "install") {
            pkg.install();
            pkg.wait(std::cerr, std::cerr);
            stop = not pkg.isSuccess();
        } else if (*it == "clean") {
            pkg.clean();
        } else if (*it == "rclean") {
            pkg.rclean();
        } else if (*it == "package") {
            pkg.pack();
            pkg.wait(std::cerr, std::cerr);
            stop = not pkg.isSuccess();
        } else if (*it == "all") {
            std::cerr << "all is not yet implemented\n";
            stop = true;
        } else if (*it == "depends") {
            std::cerr << "Depends is not yet implemented\n";
            stop = true;
        } else if (*it == "list") {
            show_package_content(pkg);
        } else {
            break;
        }
    }

    int ret = EXIT_SUCCESS;

    if (stop)
        ret = EXIT_FAILURE;
    else if (it != end) {
        if (manager)
            ret = run_manager(it, end, processor, pkg);
        else
            ret = run_simulation(it, end, pkg);
    }

    return ret;
}

static int manage_remote_mode(const std::string &remotecmd, const CmdArgs &args)
{
    vle::utils::RemoteManagerActions act = vle::utils::REMOTE_MANAGER_UPDATE;
    int ret = EXIT_SUCCESS;

    if (remotecmd == "update")
        act = vle::utils::REMOTE_MANAGER_UPDATE;
    else if (remotecmd == "source")
        act = vle::utils::REMOTE_MANAGER_SOURCE;
    else if (remotecmd == "install")
        act = vle::utils::REMOTE_MANAGER_INSTALL;
    else if (remotecmd == "local_search")
        act = vle::utils::REMOTE_MANAGER_LOCAL_SEARCH;
    else if (remotecmd == "search")
        act = vle::utils::REMOTE_MANAGER_SEARCH;
    else if (remotecmd == "show")
        act = vle::utils::REMOTE_MANAGER_SHOW;
    else if (remotecmd == "localshow")
        act = vle::utils::REMOTE_MANAGER_LOCAL_SHOW;
    else {
        std::cerr << vle::fmt(_("Remote error: remote command "
                "'%1%' unrecognised \n")) % remotecmd;
        ret = EXIT_FAILURE;
        return ret;
    }

    try {
        vle::utils::RemoteManager rm;
        switch (act) {
        case vle::utils::REMOTE_MANAGER_UPDATE:
            rm.start(act, "", &std::cout);
            break;
        default:
            if (args.size() != 1) {
                std::cerr << vle::fmt(_("Remote error: command '%1%' expects "
                        "1 argument (got %2%)\n")) %
                        remotecmd % args.size();
                ret = EXIT_FAILURE;
                return ret;
            }
            rm.start(act, args.front(), &std::cout);
            break;
        }

        rm.join();

        if (rm.hasError()) {
            std::cerr << vle::fmt(_("Remote error: %1%\n")) % rm.messageError();
            ret = EXIT_FAILURE;
            return ret;
        }

        vle::utils::Packages res;
        rm.getResult(&res);
        vle::utils::Packages::const_iterator itb = res.begin();
        vle::utils::Packages::const_iterator ite = res.end();
        switch (act) {
        case vle::utils::REMOTE_MANAGER_UPDATE:
            if (itb == ite) {
                std::cout << "No package has to be updated"
                        << std::endl;
            } else {
                std::cout << "Packages to update (re-install them):"
                        << std::endl;
                for (; itb != ite; itb++) {
                    std::cout << itb->name << "\t from " << itb->url
                            << "\t (new version:  " << itb->major << "."
                            << itb->minor << "." << itb->patch << ")"
                            << std::endl;
                }
            }
            break;
        case vle::utils::REMOTE_MANAGER_SOURCE:
            if (itb == ite) {
                std::cout << "No package has been downloaded" << std::endl;
            } else {
                std::cout << "Package downloaded:" << std::endl;
                for (; itb != ite; itb++) {
                    std::cout << itb->name << "\t from " << itb->url
                            << std::endl;
                }
            }
            break;
        case vle::utils::REMOTE_MANAGER_INSTALL:
            if (itb == ite) {
                std::cout << "No package has been installed" << std::endl;
            } else {
                std::cout << "Package installed:" << std::endl;
                for (; itb != ite; itb++) {
                    std::cout << itb->name << "\t from " << itb->url
                        << std::endl;
                }
            }
            break;
        case vle::utils::REMOTE_MANAGER_LOCAL_SEARCH:
            if (itb == ite) {
                std::cout << "No local package has been found" << std::endl;
            } else {
                std::cout << "Found local packages:" << std::endl;
                for (; itb != ite; itb++) {
                    std::cout << itb->name << std::endl;
                }
            }
            break;
        case vle::utils::REMOTE_MANAGER_SEARCH:
            if (itb == ite) {
                std::cout << "No remote package has been found" << std::endl;
            } else {
                std::cout << "Found remote packages:" << std::endl;
                for (; itb != ite; itb++) {
                    std::cout << itb->name << "\t from " << itb->url
                        << std::endl;
                }
            }
            break;
        case vle::utils::REMOTE_MANAGER_SHOW:
            if (itb == ite) {
                std::cout << "No remote package has been found" << std::endl;
            } else {
                for (; itb != ite; itb++) {
                    std::cout << *itb << std::endl;
                }
            }
            break;
        case vle::utils::REMOTE_MANAGER_LOCAL_SHOW:
            if (itb == ite) {
                std::cout << "No local package has been found" << std::endl;
            } else {
                for (; itb != ite; itb++) {
                    std::cout << *itb << std::endl;
                }
            }
            break;
        }
    } catch (const std::exception &e) {
        std::cerr << vle::fmt(_("Remote error: %1%\n")) % e.what();
        ret = EXIT_FAILURE;
    }

    return ret;
}

struct Comma:
    public std::binary_function < std::string, std::string, std::string >
{
    std::string operator()(const std::string &a, const std::string &b) const
    {
        if (a.empty())
            return b;

        return a + ',' + b;
    }
};

static int manage_config_mode(const std::string &configvar, const CmdArgs &args)
{
    int ret = EXIT_SUCCESS;

    try {
        vle::utils::Preferences prefs("vle.conf");

        std::string concat = std::accumulate(args.begin(), args.end(),
                std::string(), Comma());

        if (not prefs.set(configvar, concat))
            throw vle::utils::ArgError(vle::fmt(_("Unknown variable `%1%'")) %
                    configvar);

    } catch (const std::exception &e) {
        std::cerr << vle::fmt(_("Config error: %1%\n")) % e.what();
        ret = EXIT_FAILURE;
    }

    return ret;
}

enum ProgramOptionsCode
{
    PROGRAM_OPTIONS_FAILURE = -1,
    PROGRAM_OPTIONS_END = 0,
    PROGRAM_OPTIONS_PACKAGE = 1,
    PROGRAM_OPTIONS_REMOTE = 2,
    PROGRAM_OPTIONS_CONFIG = 3,
};

struct ProgramOptions
{
    ProgramOptions(int *verbose, int *trace, int *processor,
            bool *manager_mode, std::string *packagename,
            std::string *remotecmd, std::string *configvar, CmdArgs *args)
        : generic(_("Allowed options")), hidden(_("Hidden options")),
        verbose(verbose), trace(trace), processor(processor),
        manager_mode(manager_mode), packagename(packagename),
        remotecmd(remotecmd), configvar(configvar), args(args)
    {
        generic.add_options()
            ("help,h", _("Produce help message"))
            ("version,v", _("Print version string"))
            ("infos", _("Informations of VLE"))
            ("restart", _("Remove configuration file of VLE"))
            ("list", _("Show the list of installed package"))
            ("log-file", _("Trace of simulation(s) are reported to the standard"
                         " file ($VLE_HOME/vle.log"))
            ("log-stdout", _("Trace of the simulation(s) are reported to the"
                         " standard output"))
            ("manager,m", _("Use the manager mode to run experimental frames"))
            ("processor,o", po::value < int >(processor)->default_value(1),
             _("Select number of processor in manager mode [>= 0]"))
            ("verbose,V", po::value < int >(verbose)->default_value(0),
             ("Verbose mode 0 - 3. [default 0]\n"
              "0 no trace and no long exception\n"
              "1 small simulation trace and long exception\n"
              "2 long simulation trace\n"
              "3 all simulation trace"))
            ("package,P", po::value < std::string >(packagename),
             _("Select package mode,\n  package name [options]...\n"
                 "vle -P foo create: build new foo package\n"
                 "vle -P foo configure: configure the foo package\n"
                 "vle -P foo build: build the foo package\n"
                 "vle -P foo test: start a unit test campaign\n"
                 "vle -P foo install: install libs\n"
                 "vle -P foo clean: clean up the build directory\n"
                 "vle -P foo rclean: delete binary directories\n"
                 "vle -P foo package: build packages\n"
                 "vle -P foo all: build all depends of foo package\n"
                 "vle -P foo depends: list depends of foo package\n"
                 "vle -P foo list: list vpz and library package"))
            ("remote,R", po::value < std::string >(remotecmd),
             _("Select remote mode,\n  remote [command] [packages]...\n"
                "\tvle -R update: update the database\n"
                "\tvle -R search expression: search in database \n"
                "\tvle -R local_search expression: search in local\n"
                "\tvle -R install package: install package\n"
                "\tvle -R source package: download source package\n"
                "\tvle -R show package: show package in database\n"
                "\tvle -R localshow package: show package in local"))
            ("config,C", po::value < std::string >(configvar),
             _("Select configuration mode,\n  config variable value\n"
                "Update the vle.conf configuration file. Assign `value' to the"
                " `variable'\n"
                "vle -C vle.author me\n"
                "vle -C gvle.editor.font Monospace 10"))
            ;

        hidden.add_options()
            ("input", po::value < CmdArgs >(), _("input"))
            ;

        desc.add(generic).add(hidden);
    }

    ~ProgramOptions()
    {
    }

    int run(int argc, char *argv[])
    {
        po::positional_options_description p;
        p.add("input", -1);

        try {
            po::store(po::command_line_parser(argc,
                        argv).options(desc).positional(p).run(), vm);
            po::notify(vm);

            if (vm.count("manager"))
                *manager_mode = true;

            if (vm.count("input"))
                *args = vm["input"].as < CmdArgs >();

            if (vm.count("log-file"))
                *trace = 0;

            if (vm.count("log-stdout"))
                *trace = 1;

            if (vm.count("help"))
                return show_help(generic);

            if (vm.count("version"))
                return show_version();

            if (vm.count("infos"))
                return show_infos();

            if (vm.count("list"))
                return show_package_list();

            if (vm.count("restart"))
                return remove_configuration_file();

            if (vm.count("package"))
                return PROGRAM_OPTIONS_PACKAGE;

            if (vm.count("remote"))
                return PROGRAM_OPTIONS_REMOTE;

            if (vm.count("config"))
                return PROGRAM_OPTIONS_CONFIG;
        } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;

            return PROGRAM_OPTIONS_FAILURE;
        }

        std::cerr << _("Nothing to do. Use package, remote or config mode."
                " See the help.\n");

        return PROGRAM_OPTIONS_END;
    }

    po::options_description desc, generic, hidden;
    po::variables_map vm;
    int *verbose, *trace, *processor;
    bool *manager_mode;
    std::string *packagename, *remotecmd, *configvar;
    CmdArgs *args;
};

int main(int argc, char *argv[])
{
    int ret;
    int verbose = 0;
    int processor = 1;
    int trace = -1; /* < 0 = stderr, 0 = file and > 0 = stdout */
    bool manager_mode = false;
    std::string packagename, remotecmd, configvar;
    CmdArgs args;

    {
        ProgramOptions prgs(&verbose, &trace, &processor, &manager_mode,
                &packagename, &remotecmd, &configvar, &args);

        ret = prgs.run(argc, argv);

        if (ret == PROGRAM_OPTIONS_FAILURE)
            return EXIT_FAILURE;
        else if (ret == PROGRAM_OPTIONS_END
                or (ret != PROGRAM_OPTIONS_REMOTE and args.empty()))
            return EXIT_SUCCESS;
    }

    VLE app(verbose, trace); /* We are in package, remote or configuration
                                mode, we need to initialize VLE's API. */

    switch (ret) {
    case PROGRAM_OPTIONS_PACKAGE:
        return manage_package_mode(packagename, manager_mode, processor,
                args);
    case PROGRAM_OPTIONS_REMOTE:
        return manage_remote_mode(remotecmd, args);
    case PROGRAM_OPTIONS_CONFIG:
        return manage_config_mode(configvar, args);
    default:
        break;
    };

    return EXIT_SUCCESS;
}

