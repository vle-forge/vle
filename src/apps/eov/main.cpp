/*
 * @file apps/eov/main.cpp
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

#include <vle/manager/Manager.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/Path.hpp>
#include <vle/eov/MainWindow.hpp>
#include <gtkmm/main.h>
#include <libglademm.h>
#include <cstdlib>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <cerrno>

namespace vle { namespace eov {

static void print_error(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    std::fprintf(stderr, "Eov error: ");
    std::vfprintf(stderr, fmt, ap);
    std::fprintf(stderr, "\n");
    va_end(ap);
}

static bool compare(const char *argv, const char *shortname,
                    const char *longname)
{
    return not std::strcmp(argv, shortname) or not std::strcmp(argv, longname);
}

static bool convert_int(const char *str, vle::uint32_t *value)
{
    char *end;
    long int result;

    result = std::strtol(str, &end, 10);

    if ((errno == ERANGE and (result == LONG_MAX || result == LONG_MIN))
        or (errno != 0 and result == 0)) {
        return false;
    } else {
        *value = result;
        return true;
    }
}

static bool convert_port(const char *str, vle::uint32_t *port)
{
    bool success = false;

    if (not str) {
        print_error(_("-p or --port needs a integer argument"));
    } else {
        if (not convert_int(str, port)) {
            print_error(_("Port: Cannot convert `%s' to 0..65535"), str);
        } else {
            if (*port > 65535) {
                print_error(_("Port: Cannot convert `%d' to a norrmal "
                              "port number [0..65535]"), *port);
            } else {
                success = true;
            }
        }
    }

    return success;
}

static bool convert_verbose(const char *str, vle::uint32_t *level)
{
    bool success = false;

    if (not str) {
        print_error(_("-v or --verbose needs a integer argument"));
    } else {
        if (not convert_int(str, level)) {
            print_error(_("Verbose: cannot convert `%s' to 0..3"), str);
        } else {
            if (*level > 3) {
                print_error(_("Can not convert `%d' to normal level "
                              "number"), *level);
            } else {
                return true;
            }
        }
    }

    return success;
}

static void print_infos()
{
    std::fprintf(stderr,
                 _("EOV - the Eyes of VLE\n"
                   "Virtual Laboratory Environment - %s\n"
                   "Copyright (C) 2003 - 2011 The VLE Development Team.\n"
                   "VLE comes with ABSOLUTELY NO WARRANTY.\n"
                   "You may redistribute copies of VLE\n"
                   "under the terms of the GNU General Public License.\n"
                   "For more information about these matters, see the file"
                   " named COPYING.\n"), VLE_NAME_COMPLETE);
}

static void print_help()
{
    std::fprintf(
        stderr, _("EOV - the Eyes of VLE\n"
                  "Virtual Laboratory Environment - %s\n"
                  "Copyright (C) 2003 - 2011 The VLE Development Team.\n"
                  "\n"
                  "  eov [--help|-h] [--infos|-i] [--verbose|-v level]"
                  "[--port|-p port]\n"
                  "\n"
                  "    --help,-h\n\tShow summary of options.\n"
                  "    --infos,-i\n\tShow some informations about Eov.\n"
                  "    --verbose level,-v level\n\tSet the verbosity of "
                  "Application. Default is 0.\n"
                  "    --port port, -p port\n\tDefine the listening port"
                  "for VLE application. Default is 8000.\n"),
        VLE_NAME_COMPLETE);
}

}} // namespace vle eov

int main(int argc, char* argv[])
{
    vle::uint32_t port = 8000;
    vle::uint32_t verbose = 0;
    bool stop = false;          /**< A boolean to stop the
                                 * application. */
    bool result = true;         /**< The state of the application at
                                 * the end of the application. */

    for (argv++; not stop and *argv; argv++) {
        if (vle::eov::compare(*argv, "-p", "--port")) {
            stop = not vle::eov::convert_port(*(++argv), &port);
            result = stop;
        } else if (vle::eov::compare(*argv, "-v", "--verbose")) {
            stop = not vle::eov::convert_verbose(*(++argv), &verbose);
            result = stop;
        } else if (vle::eov::compare(*argv, "-h", "--help")) {
            vle::eov::print_help();
            stop = true;
        } else if (vle::eov::compare(*argv, "-i", "--infos")) {
            vle::eov::print_infos();
            stop = true;
        } else {
            vle::eov::print_error(_("Unkwnow argument `%s'"), *argv);
            stop = true;
            result = false;
        }
    }

    if (not stop and not *argv) {
        vle::manager::init();

        try {
            vle::utils::Trace::setLogFile(
                vle::utils::Trace::getLogFilename("eov.log"));
            vle::utils::Trace::setLevel(
                vle::utils::Trace::cast(verbose));

            Gtk::Main app(argc, argv);

            Glib::RefPtr < Gnome::Glade::Xml > xml =
                Gnome::Glade::Xml::create(
                    vle::utils::Path::path().getGladeFile("eov.glade"));

            vle::eov::MainWindow main(xml, port);

            app.run(main.window());
        } catch(const Glib::Exception& e) {
            result = false;
            vle::eov::print_error(_("Eov failed (throws %s): %s"),
                                  vle::utils::demangle(typeid(e)).c_str(),
                                  e.what().c_str());
        } catch(const std::exception& e) {
            result = false;
            vle::eov::print_error(_("Eov failed (throws %s): %s"),
                                  vle::utils::demangle(typeid(e)).c_str(),
                                  e.what());
        }

        vle::manager::finalize();
    }

    return result ? EXIT_SUCCESS : EXIT_FAILURE;
}
