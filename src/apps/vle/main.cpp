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
#include <vle/utils/Context.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/RemoteManager.hpp>
#include <vle/utils/Filesystem.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/vle.hpp>
#include <vle/version.hpp>
#include <boost/format.hpp>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <fstream>
#include <iterator>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <getopt.h>

#ifdef VLE_HAVE_NLS
# ifndef ENABLE_NLS
#  define ENABLE_NLS
# endif
#  include <libintl.h>
#  include <locale.h>
#  define _(x) gettext(x)
#  define gettext_noop(x) x
#  define N_(x) gettext_noop(x)
#else
#  define _(x) x
#  define N_(x) x
#endif

struct vle_log_standard : vle::utils::Context::LogFunctor
{
    FILE *stream;
    bool color;

    vle_log_standard(FILE *f)
#if defined(__unix__)
        : stream(f)
        , color(isatty(fileno(f)) == 1)
#endif
    {
    }

    virtual void write(const vle::utils::Context& ctx,
                       int priority, const char *file,
                       int line, const char *fn,
                       const char *format,
                       va_list args) noexcept override
    {
        (void)ctx;

        if (color) {
            if (priority == 7)
                fprintf(stream, "\e[90m[dbg] %s:%d %s: \e[39m", file, line, fn);
            else if (priority == 6)
                fprintf(stream, "%s: \e[39m", fn);
            else
                fprintf(stream, "\e[91m[Error]\e[31m %s:\e[39m ", fn);
            vfprintf(stream, format, args);
        } else {
            if (priority == 7)
                fprintf(stream, "[dbg] %s:%d %s: ", file, line, fn);
            else if (priority == 6)
                fprintf(stream, "%s: ", fn);
            else
                fprintf(stream, "[Error] %s: ", fn);
            vfprintf(stream, format, args);
        }
    }
};

struct vle_log_file : vle::utils::Context::LogFunctor
{
    FILE *fp;

    vle_log_file()
        : fp(nullptr)
    {}

    ~vle_log_file()
    {
        if (fp)
            fclose(fp);
    }

    virtual void write(const vle::utils::Context& ctx,
                       int priority, const char *file,
                       int line, const char *fn,
                       const char *format,
                       va_list args) noexcept override
    {
        if (not fp)
            fp = fopen(ctx.getLogFile().string().c_str(), "w");

        if (fp) {
            if (priority == 7)
                fprintf(fp, "[dbg] %s:%d %s: ", file, line, fn);
            else if (priority == 6)
                fprintf(fp, "%s: ", fn);
            else
                fprintf(fp, "[Error] %s: ", fn);

            vfprintf(fp, format, args);
        }
    }
};

static void show_infos() noexcept
{
    try {
        auto ctx = vle::utils::make_context();

        {
            auto paths = ctx->getBinaryPackagesDir();
            printf(_("Binary package repositories: %zu\n"), paths.size());
            for (std::size_t i = 0, e = paths.size(); i != e; ++i)
                printf(_("* %zu: %s\n"), i, paths[i].string().c_str());
        }

        {
            auto paths = ctx->getBinaryPackages();
            printf(_("Binary package: %zu\n"), paths.size());
            for (std::size_t i = 0, e = paths.size(); i != e; ++i)
                printf(_("* %zu: %s\n"), i, paths[i].string().c_str());
        }
    } catch(const std::exception& e) {
        fprintf(stderr, "show_information failure: %s\n", e.what());
    }
}

static void show_version() noexcept
{
    printf(_("Virtual Laboratory Environment - %s\n"
             "Copyright (C) 2003 - 2016 The VLE Development Team.\n"
             "VLE comes with ABSOLUTELY NO WARRANTY.\n"
             "You may redistribute copies of VLE\n"
             "under the terms of the GNU General Public License.\n"
             "For more information about these matters, see the file"
             " named COPYING.\n"), VLE_NAME_COMPLETE);
}

static void show_help() noexcept
{
    printf(
        _("%s\nvle-%s [options...]\n\n"
          "help,h      Produce help message\n"
          "version,v   Print version string\n"
          "infos       Informations of VLE\n"
          "restart     Remove configuration file of VLE\n"
          "list        Show the list of installed package\n"
          "log-file    log of simulation are reported to the standard file\n"
          "            ($VLE_HOME/vle-x.y.log"
          "log-stdout  log of the simulation(s) are reported to the "
          "standard output\n"
          "\n"
          "processor,o Select number of processor in manager mode [>= 1]\n"
          "verbose,V   Verbose mode 0 - 7. [default 3]\n"
          "                0 system is unusable\n"
          "                1 action must be taken immediately\n"
          "                2 action must be taken immediately\n"
          "                3 error conditions\n"
          "                4 warning conditions\n"
          "                5 normal, but significant, condition\n"
          "                6 informational message\n"
          "                7 debug-level message\n"
          "manager,M  Use the manager mode to run experimental frames\n"
          "package,P  Select package mode,\n  package name [options]...\n"
          "                vle -P foo create: build new foo package\n"
          "                vle -P foo configure: configure the foo package\n"
          "                vle -P foo build: build the foo package\n"
          "                vle -P foo test: start a unit test campaign\n"
          "                vle -P foo install: install libs\n"
          "                vle -P foo clean: clean up the build directory\n"
          "                vle -P foo rclean: delete binary directories\n"
          "                vle -P foo package: build packages\n"
          "                vle -P foo all: build all depends of foo package\n"
          "                vle -P foo depends: list depends of foo package\n"
          "                vle -P foo list: list vpz and library package\n"
          "remote,R   Select remote mode,\n  remote [command] [packages]...\n"
          "                vle -R update: update the database\n"
          "                vle -R search expression: search in database \n"
          "                vle -R local_search expression: search in local\n"
          "                vle -R install package: install package\n"
          "                vle -R source package: download source package\n"
          "                vle -R show package: show package in database\n"
          "                vle -R localshow package: show package in local\n"
          "config,C   Select configuration mode,\n  config variable value\n"
          "                Update the vle.conf configuration file. Assign\n"
          "                `value' to the `variable'\n"
          "                vle -C vle.author me\n"
          "                vle -C gvle.editor.font Monospace 10\n"),
        VLE_NAME_COMPLETE, VLE_ABI_VERSION);
}

enum cli_mode {
    CLI_MODE_NOTHING = 0,
    CLI_MODE_END = 1 << 1,
    CLI_MODE_CONFIG = 1 << 2,
    CLI_MODE_REMOTE = 1 << 3,
    CLI_MODE_MANAGER = 1 << 4,
    CLI_MODE_PACKAGE = 1 << 5
};

typedef std::vector<std::string> CmdArgs;

static void remove_configuration_file()
{
    auto ctx = vle::utils::make_context();
    printf(_("Remove configuration files\n"));

    try {
        using vle::utils::Path;

        {
            Path filepath(ctx->getConfigurationFile());
            filepath.remove();
        }

        {
            Path filepath(ctx->getLogFile());
            filepath.remove();
        }

        ctx->reset_settings();
        ctx->write_settings();
    } catch (const std::exception &e) {
        fprintf(stderr, _("Failed to remove configuration file: %s\n"),
            e.what());
    }
    }

static void show_package_content(vle::utils::Package& pkg)
{
    std::vector<std::string> pkgcontent;
    try {
        pkg.fillBinaryContent(pkgcontent);
    } catch (const std::exception &e) {
        fprintf(stderr, _("Show package content error: %s\n"),
                e.what());
        return;
    }

    for (const auto& elem : pkgcontent)
        puts(elem.c_str());
}

static std::string search_vpz(const std::string &param,
                              vle::utils::Package& pkg)
{
    assert(not pkg.name().empty());

    {
        vle::utils::Path p(param);
        if (p.is_file())
            return param;
    }

    std::string np = pkg.getExpFile(param, vle::utils::PKG_BINARY);

    vle::utils::Path p(np);
    if (p.is_file(np))
        return np;

    fprintf(stderr, _("Filename '%s' does not exist"), param.c_str());

    return std::string();
}

static vle::manager::LogOptions convert_log_mode(vle::utils::ContextPtr ctx)
{
    switch (ctx->get_log_priority()) {
    case 7:
        return vle::manager::LOG_SUMMARY & vle::manager::LOG_RUN;
    case 6:
        return vle::manager::LOG_SUMMARY;
    default:
        return vle::manager::LOG_NONE;
    }
}

static int run_manager(CmdArgs::const_iterator it, CmdArgs::const_iterator end,
                       int processor, vle::utils::Package& pkg)
{
    auto ctx = vle::utils::make_context();
    vle::manager::Manager man(ctx, convert_log_mode(ctx),
                              vle::manager::SIMULATION_NONE |
                              vle::manager::SIMULATION_NO_RETURN,
                              &std::cout);
    int success = EXIT_SUCCESS;

    for (; it != end; ++it) {
        vle::manager::Error error;
        std::unique_ptr<vle::value::Matrix> res =
            man.run(
                std::unique_ptr<vle::vpz::Vpz>(
                    new vle::vpz::Vpz(search_vpz(*it, pkg))),
                processor,
                0,
                1,
                &error);

        if (error.code) {
            fprintf(stderr, _("Experimental frames `%s' throws error %s"),
                    it->c_str(), error.message.c_str());
            success = EXIT_FAILURE;
        }
    }

    return success;
}

static int run_simulation(CmdArgs::const_iterator it,
                          CmdArgs::const_iterator end, vle::utils::Package& pkg)
{
    auto ctx = vle::utils::make_context();
    vle::manager::Simulation sim(ctx, convert_log_mode(ctx),
                                 vle::manager::SIMULATION_NONE |
                                 vle::manager::SIMULATION_NO_RETURN,
                                 &std::cout);
    int success = EXIT_SUCCESS;

    for (; it != end; ++it) {
        vle::manager::Error error;
        std::unique_ptr<vle::value::Map> res =
            sim.run(std::make_unique<vle::vpz::Vpz>(search_vpz(*it, pkg)),
                    &error);

        if (error.code) {
            fprintf(stderr, _("Simulator `%s' throws error %s\n"),
                    it->c_str(), error.message.c_str());
            success = EXIT_FAILURE;
        }
    }

    return success;
}

static bool init_package(vle::utils::Package& pkg, const CmdArgs &args)
{
    if (not pkg.existsBinary() and not pkg.existsSource()) {
        if (std::find(std::begin(args), std::end(args), "create")
            == std::end(args)) {
            fprintf(stderr, _("Package `%s' does not exist. Use the "
                              "create command before other command.\n"),
                    pkg.name().c_str());

            return false;
        }
    }

    return true;
}

static int manage_package_mode(vle::utils::ContextPtr ctx,
                               bool manager_mode, int processor, CmdArgs args)
{
    if (args.empty()) {
        fprintf(stderr, _("missing package\n"));
        return EXIT_FAILURE;
    }

    std::string packagename = std::move(args.front());
    args.erase(args.begin());
    auto it = args.begin();
    auto end = args.end();
    bool stop = false;

    vle::utils::Package pkg(ctx, packagename);

    if (not init_package(pkg, args))
        return EXIT_FAILURE;

    for (; not stop and it != end; ++it) {
        if (*it == "create") {
            try {
                pkg.create();
            } catch (const std::exception &e) {
                fprintf(stderr, _("Cannot create package: %s\n"), e.what());
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
            fprintf(stderr, _("all is not yet implemented\n"));
            stop = true;
        } else if (*it == "depends") {
            fprintf(stderr, _("Depends is not yet implemented\n"));
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
        if (manager_mode)
            ret = run_manager(it, end, processor, pkg);
        else
            ret = run_simulation(it, end, pkg);
    }

    return ret;
}

static int manage_remote_mode(vle::utils::ContextPtr ctx, CmdArgs args)
{
    if (args.empty()) {
        fprintf(stderr, _("missing argument\n"));
        return EXIT_FAILURE;
    }

    std::string remotecmd = args.front();
    args.erase(args.begin());

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
        fprintf(stderr, _("Remote error: remote command `%s' unrecognised\n"),
                remotecmd.c_str());
        ret = EXIT_FAILURE;
        return ret;
    }

    try {
        vle::utils::RemoteManager rm(ctx);
        switch (act) {
        case vle::utils::REMOTE_MANAGER_UPDATE:
            rm.start(act, "", &std::cout);
            break;
        default:
            if (args.size() != 1) {
                fprintf(stderr, _("Remote error: command '%s' expects "
                                  "1 argument (got %zu\n"),
                        remotecmd.c_str(), args.size());
                ret = EXIT_FAILURE;
                return ret;
            }
            rm.start(act, args.front(), &std::cout);
            break;
        }

        rm.join();

        if (rm.hasError()) {
            fprintf(stderr, _("Remote error: %s\n"), rm.messageError().c_str());
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
                printf(_("No package has to be updated\n"));
            } else {
                printf(_("Packages to update (re-install them):"));
                for (; itb != ite; itb++)
                    printf(_("%s\tfrom %s\t (new version: %d.%d.%d)\n"),
                           itb->name.c_str(), itb->url.c_str(),
                           itb->major, itb->minor, itb->patch);
            }
            break;
        case vle::utils::REMOTE_MANAGER_SOURCE:
            if (itb == ite) {
                printf(_("No package has been downloaded\n"));
            } else {
                printf(_("Package downloaded:"));
                for (; itb != ite; itb++) {
                    printf(_("%s\t from %s\n"), itb->name.c_str(),
                           itb->url.c_str());
                }
            }
            break;
        case vle::utils::REMOTE_MANAGER_INSTALL:
            if (itb == ite) {
                printf(_("No package has been installed"));
            } else {
                printf(_("Package installed: "));
                for (; itb != ite; itb++) {
                    printf(_("%s\tfrom %s "), itb->name.c_str(),
                           itb->url.c_str());
                }
            }
            break;
        case vle::utils::REMOTE_MANAGER_LOCAL_SEARCH:
            if (itb == ite) {
                printf(_("No local package has been found"));
            } else {
                printf(_("Found local packages:"));
                for (; itb != ite; itb++)
                    puts(itb->name.c_str());
            }
            break;
        case vle::utils::REMOTE_MANAGER_SEARCH:
            if (itb == ite) {
                printf(_("No remote package has been found"));
            } else {
                printf(_("Found remote packages:"));
                for (; itb != ite; itb++) {
                    printf(_("%s\nfrom %s"), itb->name.c_str(),
                           itb->url.c_str());
                }
            }
            break;
        case vle::utils::REMOTE_MANAGER_SHOW:
            if (itb == ite) {
                printf(_("No remote package has been found"));
            } else {
                for (; itb != ite; itb++) {
                    printf(_("%s: %s\n"), itb->name.c_str(),
                           itb->description.c_str());
                }
            }
            break;
        case vle::utils::REMOTE_MANAGER_LOCAL_SHOW:
            if (itb == ite) {
                printf(_("No local package has been found"));
            } else {
                for (; itb != ite; itb++) {
                    printf(_("%s: %s\n"), itb->name.c_str(),
                           itb->description.c_str());
                }
            }
            break;
        }
    } catch (const std::exception &e) {
        fprintf(stderr, _("Remote error: %s\n"), e.what());
        ret = EXIT_FAILURE;
    }

    return ret;
}

struct Comma {
    std::string operator()(const std::string &a, const std::string &b) const
    {
        if (a.empty())
            return b;

        return a + ',' + b;
    }
};

static int manage_config_mode(vle::utils::ContextPtr ctx, CmdArgs args)
{
    if (args.empty()) {
        fprintf(stderr, _("missing variable name\n"));
        return EXIT_FAILURE;
    }

    std::string configvar = args.front();
    args.erase(args.begin());

    int ret = EXIT_SUCCESS;

    try {
        std::string concat = std::accumulate(args.begin(), args.end(),
                                             std::string(), Comma());

        if (not ctx->set_setting(configvar, concat))
            throw vle::utils::ArgError(
                (boost::format(_("Unknown variable `%1%'")) %
                 configvar).str());

    } catch (const std::exception &e) {
        fprintf(stderr, _("Config error: %s\n"), e.what());
        ret = EXIT_FAILURE;
    }

    return ret;
}

int main(int argc, char **argv)
{
    unsigned int mode = CLI_MODE_NOTHING;
    int verbose_level = 0;
    int processor_number = 1;
    int log_stdout = 1;
    int restart_conf = 0;
    int opt_index;
    int ret = EXIT_SUCCESS;

    const char* const short_opts = "hvV::p:MPRC";
    const struct option long_opts[] = {
        {"help", 0, nullptr, 'h'},
        {"version", 0, nullptr, 'v'},
        {"infos", 0, nullptr, 'i'},
        {"restart", 0, &restart_conf, 1},
        {"list", 0, nullptr, 'l'},
        {"log-file", 0, &log_stdout, 0},
        {"log-stdout", 0, &log_stdout, 1},
        {"log-stderr", 0, &log_stdout, 2},
        {"verbose", 2, nullptr, 'V'},
        {"processor", 1, nullptr, 'p'},
        {"manager", 0, nullptr, 'M'},
        {"package", 0, nullptr, 'P'},
        {"manager", 0, nullptr, 'R'},
        {"config", 0, nullptr, 'C'},
    };

    for (;;) {
        const auto opt = getopt_long(argc, argv, short_opts,
                                     long_opts, &opt_index);
        if (opt == -1)
            break;

        switch (opt) {
        case 'C':
            mode |= CLI_MODE_CONFIG;
            break;
        case 'R':
            mode |= CLI_MODE_REMOTE;
            break;
        case 'P':
            mode |= CLI_MODE_PACKAGE;
            break;
        case 'M':
            mode |= CLI_MODE_MANAGER;
            break;
        case 'p':
            try {
                processor_number = std::stoi(::optarg);
                if (processor_number <= 0)
                    throw std::exception();
            } catch (const std::exception& /* e */) {
                fprintf(stderr, _("Bad processor_number: %s. "
                                  "Assume processor_number=1\n"), ::optarg);
                processor_number = 1;
            }
            break;
        case 'V':
            try {
                verbose_level = std::stoi(::optarg);
                if (verbose_level < 0)
                    throw std::exception();
            } catch (const std::exception& /* e */) {
                fprintf(stderr, _("Bad verbose_level: %s. "
                                  "Assume verbose_level=3\n"), ::optarg);
                verbose_level = 3;
            }
            break;
        case 'v':
            mode |= CLI_MODE_END;
            show_version();
            break;

        case 'i':
            mode |= CLI_MODE_END;
            show_infos();
            break;

        case 'h':
            mode |= CLI_MODE_END;
            show_help();
            break;

        case '?':
        default:
            mode |= CLI_MODE_END;
            ret = EXIT_FAILURE;
            fprintf(stderr, _("Unknown command line option\n"));
            break;
        }
    }

    //
    // If --restart we remove configuration files but we continue the
    // prcess.
    //
    if (restart_conf)
        remove_configuration_file();

    //
    // If help(), infos(), versions() or and error in command line
    // expression, we close application.
    //
    if (mode & CLI_MODE_END or mode & CLI_MODE_NOTHING)
        return ret;

    //
    // Otherwise, starts the simulation engines
    //
    auto ctx = vle::utils::make_context();
    vle::Init m_app;
    verbose_level = verbose_level < 7 ? std::max(verbose_level, 0) :
        std::min(verbose_level, 7);

    if (log_stdout == 0)
        ctx->set_log_function(std::make_unique<vle_log_standard>(stdout));
    else if (log_stdout == 1)
        ctx->set_log_function(std::make_unique<vle_log_standard>(stderr));
    else
        ctx->set_log_function(std::make_unique<vle_log_file>());

    CmdArgs commands(argv + ::optind, argv + argc);

    switch (mode) {
    case CLI_MODE_PACKAGE:
        ret = manage_package_mode(ctx, true, processor_number,
                                  std::move(commands));
        break;
    case CLI_MODE_MANAGER:
        ret = manage_package_mode(ctx, false, processor_number,
                                  std::move(commands));
        break;
    case CLI_MODE_REMOTE:
        ret = manage_remote_mode(ctx, std::move(commands));
        break;
    case CLI_MODE_CONFIG:
        ret = manage_config_mode(ctx, std::move(commands));
        break;
    default:
        fprintf(stderr, _("Select only one mode in manager, "
                          "remote or config\n"));
        break;
    };

    return ret;
}
