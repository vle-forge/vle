/*
 * @file vle/utils/Tools.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2012 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2012 INRA http://www.inra.fr
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


#include <vle/utils/Tools.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Socket.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/utils/Module.hpp>
#include <vle/version.hpp>

#include <glibmm/markup.h>
#include <glibmm/stringutils.h>
#include <glibmm/thread.h>
#include <glibmm/miscutils.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <libxml/parser.h>
#include <cmath>

#include <glib.h>
#include <glib/gstdio.h>

#ifdef G_OS_WIN32
#include <io.h>
#endif

#ifdef VLE_HAVE_EXECINFO_H
#include <execinfo.h>
#endif

#ifdef VLE_HAVE_SIGNAL_H
#include <signal.h>
#endif

#ifdef VLE_HAVE_GCC_ABI_DEMANGLE
#include <cxxabi.h>
#endif

#ifdef G_OS_UNIX
#include <unistd.h>
#endif

namespace vle { namespace utils {

std::string toScientificString(const double& v, bool locale)
{
    std::ostringstream o;
    if (locale) {
        std::locale selected("");
        o.imbue(selected);
    }

    o << std::setprecision(std::numeric_limits<double>::digits10) << v;

    return o.str();
}

bool isAlnumString(const std::string& str)
{
    const size_t sz = str.size();
    for (size_t i = 0; i < sz; ++i) {
        if (Glib::Ascii::isalpha(str[i]) == false)
            return false;
    }
    return true;
}

std::string demangle(const std::type_info& in)
{
    std::string result;
#ifdef VLE_HAVE_GCC_ABI_DEMANGLE
    result.assign(utils::demangle(in.name()));
#else
    result.assign(in.name());
#endif
    return result;
}

std::string demangle(const std::string& in)
{
    std::string result;
#ifdef VLE_HAVE_GCC_ABI_DEMANGLE
    char* output = NULL;
    int status;
    output = abi::__cxa_demangle(in.c_str(), NULL, NULL, &status);
    if (status == 0 and output) {
        result.assign(output);
    }

    free(output);
#else
    result.assign(in);
#endif
    return result;
}

std::string getUserDirectory()
{
#ifdef G_OS_WIN32
    std::string home(utils::Path::buildDirname(Glib::get_home_dir(), "vle"));
#else
    std::string home(utils::Path::buildDirname(Glib::get_home_dir(), ".vle"));
#endif

    if (not utils::Path::existDirectory(home)) {
#ifdef G_OS_WIN32
        if (_mkdir(home.c_str()) == -1) {
#else
        if (g_mkdir(home.c_str(), 0755) == -1) {
#endif
            throw utils::InternalError(fmt(
                _("Error mkdir '%1%' directory user\n")) % home);
        }
    }
    return home;
}

void buildDaemon()
{
#ifdef G_OS_WIN32
    g_chdir("c://");
#else
    g_chdir("//");

    if (::fork())
        ::exit(0);

    ::setsid();

    if (::fork())
        ::exit(0);
#endif
    for (int i = 0; i < FOPEN_MAX; ++i)
        ::close(i);
}

void init()
{
    xmlDefaultSAXHandlerInit();

    if (not Glib::thread_supported()) {
        Glib::thread_init();
    }

    utils::ModuleCache::init();
    utils::Path::init();
    utils::Trace::init();
    utils::net::Base::init();

#ifdef VLE_HAVE_NLS
    setlocale(LC_ALL, "");
    bindtextdomain(VLE_LOCALE_NAME, utils::Path::path().getLocaleDir().c_str());
    textdomain(VLE_LOCALE_NAME);
#endif
}

void finalize()
{
    utils::ModuleCache::kill();
    utils::Path::kill();
    utils::Trace::kill();
    xmlCleanupParser();
}

void printHelp(std::ostream& out)
{
    out << fmt(_("Virtual Laboratory Environment - %1%\n"
                 "Copyright (C) 2003 - 2012 The VLE Development Team.\n"
                 "VLE is a multi-modeling environment to build,\nsimulate "
                 "and analyse models of dynamic complex systems.\n"
                 "For more information, see manuals with 'man vle' or\n"
                 "the VLE website http://sourceforge.net/projects/vle/\n")) %
        VLE_NAME_COMPLETE << std::endl;
}

void printInformations(std::ostream& out)
{
    out << fmt(_("Virtual Laboratory Environment - %1%\n"
                 "Copyright (C) 2003 - 2012 The VLE Development Team.\n")) %
        VLE_NAME_COMPLETE << "\n" << std::endl;
}

void printVersion(std::ostream& out)
{
    out << fmt(_("Virtual Laboratory Environment - %1%\n"
                 "Copyright (C) 2003 - 2012 The VLE Development Team.\n"
                 "VLE comes with ABSOLUTELY NO WARRANTY.\n"
                 "You may redistribute copies of VLE\n"
                 "under the terms of the GNU General Public License.\n"
                 "For more information about these matters, see the file named "
                 "COPYING.\n")) % VLE_NAME_COMPLETE << std::endl;
}

}} // namespace vle utils
