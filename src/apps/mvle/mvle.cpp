/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2017 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2017 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2017 INRA http://www.inra.fr
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

#include <iostream>
#include <vle/manager/ExperimentGenerator.hpp>
#include <vle/manager/Manager.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/value/Matrix.hpp>
#include <vle/vle.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/mpi/collectives.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/mpi/environment.hpp>
#include <boost/program_options.hpp>

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <getopt.h>

#ifdef VLE_HAVE_NLS
#ifndef ENABLE_NLS
#define ENABLE_NLS
#endif
#include <libintl.h>
#include <locale.h>
#define _(x) gettext(x)
#define gettext_noop(x) x
#define N_(x) gettext_noop(x)
#else
#define _(x) x
#define N_(x) x
#endif

void
mvle_show_help()
{
    printf(_("Use:\n"
             "  mvle [-h,--help] [-v,--version] [-s|--show] [-P,--package"
             " package_name] vpz_files...\n"
             "\n"
             "Help options:\n"
             "  -h, --help        Show help option\n"
             "\n"
             "Application options:\n"
             "  -s --show         Show the plan\n"
             "  -P --package      Start VLE in package mode\n"
             "  -v --version      Show the version\n"));
}

void
mvle_show_version()
{
    printf(_("mvle - MPI manager for VLE\n"
             "Virtual Laboratory Environment - VLE %s\n"
             "Copyright (C) 2003 - 2016 The VLE Development Team.\n"
             "VLE is a multi-modeling environment to build,\nsimulate "
             "and analyse models of dynamic complex systems.\n"
             "For more information, see manuals with 'man mvle' or\n"
             "the VLE website http://sourceforge.net/projects/vle/\n"),
           vle::string_version().c_str());
}

void
mvle_mpi_error(int error)
{
    int errorclass;
    int len;
    char* buffer = new char[MPI_MAX_ERROR_STRING + 1];
    buffer[MPI_MAX_ERROR_STRING] = '\0';

    MPI_Error_class(error, &errorclass);
    MPI_Error_string(errorclass, buffer, &len);
    buffer[len] = '\0';

    fprintf(stderr, "%s", buffer);
    delete[] buffer;
}

bool
mvle_mpi_init(int* argc, char*** argv, uint32_t* rank, uint32_t* world)
{
    int t_rank;
    int t_world;
    int r;
    bool result = false;

    if ((r = MPI_Init(argc, argv)) == MPI_SUCCESS) {
        if ((r = MPI_Comm_rank(MPI_COMM_WORLD, &t_rank)) == MPI_SUCCESS) {
            /* check the cast of the MPI's rank */
            if (t_rank < 0 or
                static_cast<unsigned int>(t_rank) >
                  std::numeric_limits<uint32_t>::max()) {
                fprintf(stderr, _("bad mpi rank id: %d"), t_rank);
            } else {
                *rank = static_cast<uint32_t>(t_rank);
                if ((r = MPI_Comm_size(MPI_COMM_WORLD, &t_world)) ==
                    MPI_SUCCESS) {
                    /* check the cast of the MPI's world size */
                    if (t_world < 0 and
                        static_cast<unsigned int>(t_world) >
                          std::numeric_limits<uint32_t>::max()) {
                        fprintf(stderr, _("bad mpi world size: %d"), t_world);
                    } else {
                        *world = static_cast<uint32_t>(t_world);
                        result = true;
                    }
                }
            }
        }
    }

    if (r != MPI_SUCCESS) {
        mvle_mpi_error(r);
        result = false;
    }

    return result;
}

bool
mvle_parse_arg(int argc,
               char** argv,
               int* vpz,
               bool* show,
               vle::utils::Package& pack)
{
    int i = 1;

    while (i < argc) {
        if ((std::strcmp(argv[i], "-P") == 0 or
             std::strcmp(argv[i], "--package") == 0) and
            i + 1 <= argc) {
            pack.select(argv[++i]);
        } else if (std::strcmp(argv[i], "-h") == 0 or
                   std::strcmp(argv[i], "--help") == 0) {
            mvle_show_help();
            return false;
        } else if (std::strcmp(argv[i], "-v") == 0 or
                   std::strcmp(argv[i], "--version") == 0) {
            mvle_show_version();
            return false;
        } else if (std::strcmp(argv[i], "-s") == 0 or
                   std::strcmp(argv[i], "--show") == 0) {
            *show = true;
        } else {
            *vpz = i;
        }
        ++i;
    }

    return *vpz < argc and not pack.name().empty();
}

void
mvle_show(const std::string& vpz)
{
    vle::manager::ExperimentGenerator expgen(vpz, 0, 1);

    if (expgen.size()) {
        vle::vpz::Conditions conds;
        vle::vpz::Conditions::iterator it;

        expgen.get(expgen.min(), &conds);

        printf(";");
        for (it = conds.begin(); it != conds.end(); ++it) {
            const vle::vpz::Condition& cond = it->second;
            vle::vpz::Condition::const_iterator jt;

            for (jt = cond.begin(); jt != cond.end(); ++jt) {
                printf("%s.%s;", it->first.c_str(), jt->first.c_str());
            }
        }

        printf("\n");

        for (uint32_t i = expgen.min(); i < expgen.max(); ++i) {
            expgen.get(i, &conds);

            printf("%d;", i);
            for (it = conds.begin(); it != conds.end(); ++it) {
                const vle::vpz::Condition& cond = it->second;
                vle::vpz::Condition::const_iterator jt;

                for (jt = cond.begin(); jt != cond.end(); ++jt) {
                    printf("%s;", jt->second[0]->writeToFile().c_str());
                }
            }
            printf("\n");
        }
    }
}

int
main(int argc, char** argv)
{
    uint32_t rank = 0;
    uint32_t world = 0;
    bool show = false;
    bool result;

    auto ctx = vle::utils::make_context();
    if ((result = mvle_mpi_init(&argc, &argv, &rank, &world))) {
        int vpz = 0;
        vle::utils::Package pack(ctx);
        if ((result = mvle_parse_arg(argc, argv, &vpz, &show, pack))) {
            if (show) {
                while (vpz < argc) {
                    mvle_show(
                      pack.getExpFile(argv[vpz], vle::utils::PKG_BINARY));
                    vpz++;
                }
            } else {
                try {
                    vle::manager::Manager man(
                      ctx,
                      vle::manager::LOG_NONE,
                      vle::manager::SIMULATION_NONE |
                        vle::manager::SIMULATION_SPAWN_PROCESS |
                        vle::manager::SIMULATION_NO_RETURN,
                      &std::cout);

                    printf("MPI node %d/%d start\n", rank, world);

                    while (vpz < argc) {
                        auto v = std::make_unique<vle::vpz::Vpz>(
                          pack.getExpFile(argv[vpz], vle::utils::PKG_BINARY));

                        vle::manager::Error error;
                        auto res =
                          man.run(std::move(v), 1, rank, world, &error);

                        if (error.code) {
                            fprintf(stderr,
                                    "Experimental frames `%s' "
                                    "throws error %s",
                                    argv[vpz],
                                    error.message.c_str());
                        }

                        vpz++;
                    }

                    printf("MPI node %d/%d end\n", rank, world);
                } catch (const std::exception& e) {
                    fprintf(stderr, "manager problem: %s", e.what());
                }
            }
        }
    }

    MPI_Finalize();

    return result ? EXIT_SUCCESS : EXIT_FAILURE;
}
