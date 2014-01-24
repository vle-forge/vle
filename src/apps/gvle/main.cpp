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

#include <vle/gvle/GVLE.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/vle.hpp>
#include <cstdlib>
#include <iostream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

typedef std::vector < std::string > CmdArgs;

struct GVLE
{
    vle::Init app;

    GVLE(int verbose)
    {
        vle::utils::Trace::setLogFile(
                vle::utils::Trace::getLogFilename("gvle.log"));
        vle::utils::Trace::setLevel(vle::utils::Trace::cast(verbose));
    }

    ~GVLE()
    {}
};

static int show_infos()
{
    std::cout << vle::fmt(
            _("GVLE - The GUI of VLE\n"
                "Virtual Laboratory Environment - %1%\n"
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

enum ProgramOptionsCode
{
    PROGRAM_OPTIONS_FAILURE = -1,
    PROGRAM_OPTIONS_END = 0,
    PROGRAM_OPTIONS_RUN = 1
};

struct ProgramOptions
{
    ProgramOptions(int *verbose, std::string *packagename, CmdArgs *args)
        : generic(_("Allowed options")), hidden(_("Hidden options")),
        verbose(verbose), packagename(packagename), args(args)
    {
        generic.add_options()
            ("help,h", _("Produce help message"))
            ("version,v", _("Print version string"))
            ("infos", _("Informations of VLE"))
            ("verbose,V", po::value < int >(verbose)->default_value(0),
             ("Verbose mode 0 - 3. [default 0]\n"
              "0 no trace and no long exception\n"
              "1 small simulation trace and long exception\n"
              "2 long simulation trace\n"
              "3 all simulation trace"))
            ("package,P", po::value < std::string >(packagename),
             ("Select a package"));

        hidden.add_options()
            ("input", po::value < CmdArgs >(), _("input"));

        desc.add(generic).add(hidden);
    }

    ~ProgramOptions()
    {}

    int run(int argc, char *argv[])
    {
        po::positional_options_description p;
        p.add("input", -1);

        try {
            po::store(po::command_line_parser(argc,
                        argv).options(desc).positional(p).run(), vm);
            po::notify(vm);

            if (vm.count("input"))
                *args = vm["input"].as < CmdArgs >();

            if (vm.count("help"))
                return show_help(generic);

            if (vm.count("version"))
                return show_version();

            if (vm.count("infos"))
                return show_infos();
        } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;

            return PROGRAM_OPTIONS_FAILURE;
        }

        return PROGRAM_OPTIONS_RUN;
    }

    po::options_description desc, generic, hidden;
    po::variables_map vm;
    int *verbose;
    std::string *packagename;
    CmdArgs *args;
};

int main(int argc, char **argv)
{
    int ret;
    int verbose = 0;
    std::string packagename;
    CmdArgs args;

    {
       ProgramOptions prgs(&verbose, &packagename, &args);
       ret = prgs.run(argc, argv);

       if (ret == PROGRAM_OPTIONS_END)
           return EXIT_SUCCESS;
       else if (ret == PROGRAM_OPTIONS_FAILURE)
           return EXIT_FAILURE;
    }

    vle::gvle::GVLE::start(argc, argv, packagename,
           args.empty() ? std::string() : args.front());

    return EXIT_SUCCESS;
}
