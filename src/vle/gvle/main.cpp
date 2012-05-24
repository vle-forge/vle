/*
 * @file apps/gvle/main.cpp
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
#include <vle/gvle/GVLE.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/vle.hpp>
#include <gtkmm/main.h>
#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <cstdlib>
#include <cerrno>

namespace vle { namespace gvle {

static void print_error(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    std::fprintf(stderr, "GVLE error: ");
    std::vfprintf(stderr, fmt, ap);
    std::fprintf(stderr, "\n");
    va_end(ap);
}

static bool compare(const char *arg, const char *shortname,
                    const char *longname)
{
    return not std::strcmp(arg, shortname) or not std::strcmp(arg, longname);
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

static void print_help()
{
    std::fprintf(stderr,
                 _("GVLE - the Gui of VLE\n"
                   "Virtual Laboratory Environment - %s\n"
                   "Copyright (C) 2003 - 2011 The VLE Development Team.\n"),
                 VLE_NAME_COMPLETE);
}

static void print_infos()
{
    std::fprintf(stderr,
                 _("GVLE - the Gui of VLE\n"
                   "Virtual Laboratory Environment - %s\n"
                   "Copyright (C) 2003 - 2011 The VLE Development Team.\n"
                   "VLE comes with ABSOLUTELY NO WARRANTY.\n"
                   "You may redistribute copies of VLE\n"
                   "under the terms of the GNU General Public License.\n"
                   "For more information about these matters, see the file "
                   "named COPYING.\n"), VLE_NAME_COMPLETE);
}

}} // namespace vle gvle

int main(int argc, char **argv)
{
    vle::uint32_t verbose = 0;
    bool stop = false;
    bool end = false;
    bool result = true;

    vle::Init app;

    Gtk::Main application(&argc, &argv);

    for (argv++; not stop and not end and *argv; argv++) {
        if (vle::gvle::compare(*argv, "-v", "--verbose")) {
            stop = not vle::gvle::convert_verbose(*(++argv), &verbose);
            result = stop;
        } else if (vle::gvle::compare(*argv, "-h", "--help")) {
            vle::gvle::print_help();
            stop = true;
        } else if (vle::gvle::compare(*argv, "-i", "--infos")) {
            vle::gvle::print_infos();
            stop = true;
        } else {
            break;
        }
    }

    if (not stop) {
        vle::gvle::GVLE* g = 0;
	Glib::RefPtr< Gtk::Builder > refBuilder = Gtk::Builder::create();
	try {
            vle::utils::Trace::setLogFile(
                vle::utils::Trace::getLogFilename("gvle.log"));
            vle::utils::Trace::setLevel(
                vle::utils::Trace::cast(verbose));

	    refBuilder->add_from_file(vle::utils::Path::path().
                                      getGladeFile("gvle.glade").c_str());

	    refBuilder->get_widget_derived("WindowPackageBrowser", g);
	    if (*argv) {
		g->setFileName(*argv);
            }
	    application.run(*g);
	    delete g;
	} catch(const Glib::MarkupError& e) {
	    result = false;
	    std::cerr << _("\n/!\\  gvle Glib::MarkupError reported: ") <<
	      vle::utils::demangle(typeid(e)) << "\n" << e.what();
        } catch(const Gtk::BuilderError& e) {
	    result = false;
	    std::cerr << _("\n/!\\  gvle  Gtk::BuilderError reported: ") <<
	      vle::utils::demangle(typeid(e)) << "\n" << e.what();
        } catch(const Glib::Exception& e) {
            result = false;
            vle::gvle::print_error(_("\n/!\\ gvle Glib error reported (%s): %s"),
                                   vle::utils::demangle(typeid(e)).c_str(),
                                   e.what().c_str());
            delete g;
        } catch(const std::exception& e) {
            result = false;
            vle::gvle::print_error(_("\n/!\\ gvle exception reported (%s): %s"),
                                   vle::utils::demangle(typeid(e)).c_str(),
                                   e.what());
            delete g;
        }
    }

    return result ? EXIT_SUCCESS : EXIT_FAILURE;
}
