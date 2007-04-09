/**
 * @file utils/Tools.cpp
 * @author The VLE Development Team.
 * @brief Most of tools to use into VLE projects, plugins etc.
 */

/*
 * Copyright (c) 2004, 2005 The vle Development Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include <vle/utils/Tools.hpp>
#include <vle/utils/Trace.hpp>

#include <glibconfig.h>
#include <glibmm/markup.h>
#include <glibmm/stringutils.h>
#include <glib/gstdio.h>
#include <config.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "config.h"
#include <cstring>
#include <iomanip>

#include <glib.h>
#include <glib/gstdio.h>

#ifdef G_OS_WIN32
#include <io.h>
#endif

#ifdef HAVE_EXECINFO_H
#include <execinfo.h>
#endif

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

#ifdef HAVE_GCC_ABI_DEMANGLE
#include <cxxabi.h>
#endif

#ifdef G_OS_UNIX
#include <unistd.h>
#endif

std::string vle::utils::write_to_temp(const std::string& prefix,
                                 const std::string& buffer)
{
    std::string filename;
    int fd;

    if (prefix.size())
        fd = Glib::file_open_tmp(filename, prefix);
    else
        fd = Glib::file_open_tmp(filename);

    Assert(InternalError, fd != -1, boost::format(
            "Cannot open file %2% with prefix %1% in tempory directory\n") %
        prefix % filename);

#ifdef G_OS_WIN32
    ssize_t sz = ::_write(fd, buffer.c_str(), buffer.size());
#else
    ssize_t sz = ::write(fd, buffer.c_str(), buffer.size());
#endif

    Assert(InternalError, sz != -1 and sz != 0,
           boost::format("Cannot write buffer in file %1% in tempory "
                         "directory\n") % filename);

#ifdef G_OS_WIN32
    ::_close(fd);
#else
    ::close(fd);
#endif

    return filename;
}

std::string vle::utils::get_current_date()
{
    char ch[81];
    struct tm* pdh = 0;
    time_t intps;

    intps = time(NULL);
    pdh = localtime(&intps);
    strftime(ch, 81, "%a, %d %b %Y %H:%M:%S %z", pdh);

    return ch;
}

bool vle::utils::is_vle_string(const Glib::ustring& str)
{
    if (str.is_ascii() == false)
        return false;

    if (str.empty() == true)
        return false;

    const size_t sz = str.size();
    for (size_t i = 0; i < sz; ++i) {
        if (Glib::Ascii::isalpha(str[i]) == false)
            return false;
    }
    return true;
}

Glib::ustring vle::utils::print_trace_report()
{
    Glib::ustring msg;
#ifdef HAVE_EXECINFO_H
    if (utils::Trace::trace().get_level() >= 1) {
        const int size_buffer = 255;
        void* array[size_buffer];
        char** result = 0;
        msg.assign("----------\n");

        int size = backtrace(array, size_buffer);
        result = backtrace_symbols(array, size);

        for (int i = 1; i < size; ++i) {
            char* p = ::index(result[i], '(');
            char* l = ::rindex(result[i], '+');

            std::ostringstream out;
            out << std::setw(3) << i << ". ";
            if (p and l) {
                std::string mangled(p + 1, l - p - 1);
                out << std::string(result[i], p - result[i])
                    << "\n    " << utils::demangle(mangled)
                    << " " << l << "\n";
            } else {
                out << "unkown function\n    " << result[i] << "\n";
            }
            msg += out.str();
            msg += "\n";
        }
    }
    return msg;
#else
    return msg;
#endif
}

void vle::utils::print_trace_signals(int signal_number)
{
    std::cerr << "\n/!\\ " << Glib::get_prgname()
        << "  " << Glib::strsignal(signal_number) << " reported.\n";
    if (utils::Trace::trace().get_level() >= 1) {
        std::cerr << utils::print_trace_report();
    }
    std::cerr << std::endl;
    abort();
}

void vle::utils::install_signal()
{
#ifdef HAVE_SIGNAL_H
#ifdef G_OS_UNIX
    //if (utils::Trace::trace().get_level() != 3) {
    signal(SIGILL, print_trace_signals);
    signal(SIGBUS, print_trace_signals);
    signal(SIGSEGV, print_trace_signals);
    //}
#endif
#endif
}

std::string vle::utils::demangle(const std::type_info& in) 
{
    std::string result;
#ifdef HAVE_GCC_ABI_DEMANGLE
    result.assign(utils::demangle(in.name()));
#else
    result.assign(in.name());
#endif
    return result;
}

std::string vle::utils::demangle(const std::string& in)
{
    std::string result;
#ifdef HAVE_GCC_ABI_DEMANGLE
    char* output = NULL;
    int status;
    output = abi::__cxa_demangle(in.c_str(), NULL, NULL, &status);
    if (status == 0 and output)
        result.assign(output);

    if (output)
        free(output);
#else
    result.assign(in);
#endif
    return result;
}

void vle::utils::initUserDirectory()
{
    buildDirectory(Glib::build_filename(getUserDirectory(), "plugins"));
    buildDirectory(Glib::build_filename(getUserDirectory(), "models"));
    buildDirectory(Glib::build_filename(getUserDirectory(), "observers"));
    buildDirectory(Glib::build_filename(getUserDirectory(), "bin"));
    buildDirectory(Glib::build_filename(getUserDirectory(), "pixmaps"));
    buildDirectory(Glib::build_filename(getUserDirectory(), "glade"));
    buildDirectory(Glib::build_filename(getUserDirectory(), "eovplugin"));
}

bool vle::utils::buildDirectory(const std::string& dirname)
{
    if (!Glib::file_test(dirname, Glib::FILE_TEST_IS_DIR |
                         Glib::FILE_TEST_EXISTS)) {
#ifdef G_OS_WIN32
        if (_mkdir(dirname.c_str()) == -1) {
#else
        if (g_mkdir(dirname.c_str(), 0755) == -1) {
#endif
            TRACE1(boost::format("Building directory %1% failed\n") % dirname);
            return false;
        } else {
            TRACE1(boost::format("Make directory %1% success\n") % dirname);
            return true;
        }
    }
    return true;
}

std::string vle::utils::getUserDirectory()
{
    std::string home(Glib::build_filename(Glib::get_home_dir(), ".vle"));

    if (!Glib::file_test(home, Glib::FILE_TEST_IS_DIR |
                         Glib::FILE_TEST_EXISTS)) {
#ifdef G_OS_WIN32
        if (_mkdir(home.c_str()) == -1) {
#else
        if (g_mkdir(home.c_str(), 0755) == -1) {
#endif
            TRACE1("Error mkdir $(home)/.vle directory user\n");
            return std::string();
        }
    }
    return home;
}

void vle::utils::buildDaemon()
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
