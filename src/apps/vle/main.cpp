/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2012 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2012 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2012 INRA http://www.inra.fr
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
#include <iostream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

typedef std::vector < std::string > CmdArgs;

struct VLE
{
    vle::Init app;

    VLE(int verbose)
    {
        vle::utils::Trace::setLevel(vle::utils::Trace::cast(verbose));
    }

    ~VLE()
    {
        if (vle::utils::Trace::haveWarning())
            std::cerr << vle::fmt(
                "\n/!\\ Some warnings during run: See file %1%\n") %
                vle::utils::Trace::getLogFile() << std::endl;
    }
};

static int show_infos()
{
    std::cout << vle::fmt(
        _("Virtual Laboratory Environment - %1%\n"
          "Copyright (C) 2003 - 2012 The VLE Development Team.\n"
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
    std::cout << vle::fmt(_("Packages from: `%1%'\n")) %
        vle::utils::Path::path().getPackagesDir();

    vle::utils::PathList vpz = vle::utils::Path::path().getInstalledPackages();
    std::sort(vpz.begin(), vpz.end());

    std::copy(vpz.begin(), vpz.end(),
              std::ostream_iterator < std::string >(std::cout, "\n"));

    return EXIT_SUCCESS;
}

static void show_package_content()
{
    std::cout << vle::fmt(_("Package content from: `%1%'\n")) %
        vle::utils::Path::path().getPackageDir();

    vle::utils::PathList tmp;

    tmp = vle::utils::Path::path().getInstalledExperiments();
    std::sort(tmp.begin(), tmp.end());
    std::copy(tmp.begin(), tmp.end(),
              std::ostream_iterator < std::string >(std::cout, "\n"));

    tmp = vle::utils::Path::path().getInstalledLibraries();
    std::sort(tmp.begin(), tmp.end());
    std::copy(tmp.begin(), tmp.end(),
              std::ostream_iterator < std::string >(std::cout, "\n"));
}

static std::string search_vpz(const std::string &param)
{
    assert(not vle::utils::Package::package().name().empty());

    if (vle::utils::Path::existFile(param))
        return param;

    std::string np = vle::utils::Path::path().getPackageExpFile(param);

    if (vle::utils::Path::existFile(np))
        return np;

    std::cerr << vle::fmt(_("Filename '%1%' does not existi")) % param;

    return std::string();
}

static int run_manager(CmdArgs::const_iterator it, CmdArgs::const_iterator end,
        int processor)
{
    vle::manager::Manager man(vle::manager::LOG_SUMMARY,
                              vle::manager::SIMULATION_NONE |
                              vle::manager::SIMULATION_NO_RETURN,
                              &std::cout);
    vle::utils::ModuleManager modules;
    int success = EXIT_SUCCESS;

    for (; it != end; ++it) {
        vle::manager::Error error;
        vle::value::Matrix *res = man.run(new vle::vpz::Vpz(search_vpz(*it)),
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
        CmdArgs::const_iterator end)
{
    vle::manager::Simulation sim(vle::manager::LOG_SUMMARY,
                                 vle::manager::SIMULATION_NONE |
                                 vle::manager::SIMULATION_NO_RETURN,
                                 &std::cout);
    vle::utils::ModuleManager modules;
    int success = EXIT_SUCCESS;

    for (; it != end; ++it) {
        vle::manager::Error error;
        vle::value::Map *res = sim.run(new vle::vpz::Vpz(search_vpz(*it)),
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

static int manage_package_mode(const std::string &packagename, bool manager,
                               int processor, const CmdArgs &args)
{
    CmdArgs::const_iterator it = args.begin();
    CmdArgs::const_iterator end = args.end();
    bool stop = false;

    vle::utils::Package::package().refresh();
    vle::utils::Package::package().select(packagename);

    for (; not stop and it != end; ++it) {
        if (*it == "create") {
            vle::utils::Package::package().create();
        } else if (*it == "configure") {
            vle::utils::Package::package().configure();
            vle::utils::Package::package().wait(std::cerr, std::cerr);
            stop = not vle::utils::Package::package().isSuccess();
        } else if (*it == "build") {
            vle::utils::Package::package().build();
            vle::utils::Package::package().wait(std::cerr, std::cerr);
            if (vle::utils::Package::package().isSuccess()) {
                vle::utils::Package::package().install();
                vle::utils::Package::package().wait(std::cerr, std::cerr);
            }
            stop = not vle::utils::Package::package().isSuccess();
        } else if (*it == "test") {
            vle::utils::Package::package().test();
            vle::utils::Package::package().wait(std::cerr, std::cerr);
            stop = not vle::utils::Package::package().isSuccess();
        } else if (*it == "install") {
            vle::utils::Package::package().install();
            vle::utils::Package::package().wait(std::cerr, std::cerr);
            stop = not vle::utils::Package::package().isSuccess();
        } else if (*it == "clean") {
            vle::utils::Package::package().clean();
            vle::utils::Package::package().wait(std::cerr, std::cerr);
            stop = not vle::utils::Package::package().isSuccess();
        } else if (*it == "rclean") {
            vle::utils::Package::removePackageBinary(
                vle::utils::Package::package().name());
        } else if (*it == "package") {
            vle::utils::Package::package().pack();
            vle::utils::Package::package().wait(std::cerr, std::cerr);
            stop = not vle::utils::Package::package().isSuccess();
        } else if (*it == "all") {
            std::cerr << "all is not yet implemented\n";
            stop = true;
        } else if (*it == "depends") {
            std::cerr << "Depends is not yet implemented\n";
            stop = true;
        } else if (*it == "list") {
            show_package_content();
        } else {
            break;
        }
    }

    if (not stop and it != end) {
        if (manager)
            return run_manager(it, end, processor);
        else
            return run_simulation(it, end);
    }

    return EXIT_SUCCESS;
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

    try {
        vle::utils::RemoteManager rm;
        rm.start(act, args.front(), &std::cout);
        rm.join();
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
    ProgramOptions(int *verbose, int *processor, bool *manager_mode,
            std::string *packagename, std::string *remotecmd, std::string
            *configvar, CmdArgs *args)
        : generic(_("Allowed options")), hidden(_("Hidden options")),
        verbose(verbose), processor(processor), manager_mode(manager_mode),
        packagename(packagename), remotecmd(remotecmd), configvar(configvar),
        args(args)
    {
        generic.add_options()
            ("help,h", _("Produce help message"))
            ("version,v", _("Print version string"))
            ("infos", _("Informations of VLE"))
            ("list", _("Show the list of installed package"))
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
                "\tvle -R install packages: download package\n"
                "\tvle -R install source: download source package\n"
                "\tvle -R show package: show package"))
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

            if (vm.count("help"))
                return show_help(generic);

            if (vm.count("version"))
                return show_version();

            if (vm.count("infos"))
                return show_infos();

            if (vm.count("list"))
                return show_package_list();

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
    int *verbose, *processor;
    bool *manager_mode;
    std::string *packagename, *remotecmd, *configvar;
    CmdArgs *args;
};

int main(int argc, char *argv[])
{
    int ret;
    int verbose = 0;
    int processor = 1;
    bool manager_mode = false;
    std::string packagename, remotecmd, configvar;
    CmdArgs args;

    {
        ProgramOptions prgs(&verbose, &processor, &manager_mode,
                &packagename, &remotecmd, &configvar, &args);

        ret = prgs.run(argc, argv);

        if (ret == PROGRAM_OPTIONS_FAILURE)
            return EXIT_FAILURE;
        else if (ret == PROGRAM_OPTIONS_END or args.empty())
            return EXIT_SUCCESS;
    }

    VLE app(verbose); /* We are in package, remote or configuration mode, we
                         need to initialize VLE's API. */

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

