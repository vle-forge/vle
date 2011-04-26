/*
 * @file vle/utils/Package.cpp
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


#include <vle/utils/Package.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Preferences.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Exception.hpp>
#include <boost/filesystem.hpp>
#include <boost/version.hpp>
#include <glibmm/timer.h>
#include <glibmm/stringutils.h>
#include <glibmm/miscutils.h>
#include <glibmm/shell.h>
#include <fstream>
#include <ostream>
#include <iostream>
#include <cstring>
#include <glib/gstdio.h>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/version.hpp>
#include <boost/config.hpp>

#ifdef BOOST_WINDOWS
#   include <windows.h>
#   include <io.h>
#else
#   include <sys/wait.h>
#   include <unistd.h>
#   include <errno.h>
#endif

namespace fs = boost::filesystem;

namespace vle { namespace utils {

Package* Package::m_package = 0;


#ifdef BOOST_WINDOWS
/**
 * @brief A specific \b Win32 function to convert a path into a 8.3 path.
 * Attention, the file must exists instead the parameter is returned.
 */
static std::string convertPathTo83Path(const std::string& path)
{
    std::string newvalue(path);
    DWORD lenght;

    lenght = GetShortPathName(path.c_str(), NULL, 0);
    if (lenght > 0) {
        TCHAR* buffer = new TCHAR[lenght];
        lenght = GetShortPathName(path.c_str(), buffer, lenght);
        if (lenght > 0) {
            newvalue.assign(static_cast < char* >(buffer));
        }
        delete [] buffer;
    }

    return newvalue;
}

static std::string buildEnvironmentVariable(const std::string& variable,
                                            const std::string& value,
                                            bool append)
{
    std::string result(variable);
    result += "=";
    result += value;

    if (append) {
        char* env = std::getenv(variable.c_str());

        if (env != NULL) {
            std::string old(env, std::strlen(env));
#ifdef BOOST_WINDOWS
            result += ";";
#else
            result += ":";
#endif
            result += old;
        }
    }

    return result;
}
#endif

/**
 * @brief Build list of string which represents the 'VARIABLE=VALUE' environment
 * variable.
 *
 * @attention @b Win32: the PKG_CONFIG_FILE is path is update with the vle's
 * pkgconfig directory. The BOOST_ROOT, BOOST_INCLUDEDIR and BOOST_LIBRARYDIR
 * are assigned with the vle's patch to avoid conflict with different boost
 * version.
 *
 * @return A list of string.
 */
static std::list < std::string > prepareEnvironmentVariable()
{
    std::list < std::string > envp;
    gchar** allenv = g_listenv();
    gchar** it = allenv;

    while (*it != NULL) {

#ifdef BOOST_WINDOWS
        if (strcasecmp(*it, "PATH") and
            strcasecmp(*it, "PKG_CONFIG_PATH") and
            strcasecmp(*it, "BOOST_ROOT") and
            strcasecmp(*it, "BOOST_INCLUDEDIR") and
            strcasecmp(*it, "BOOST_LIBRARYDIR")) {

            const gchar* res = g_getenv(*it);
            if (res) {
                std::string buffer(*it);
                buffer += "=";
                buffer += res;
                envp.push_back(buffer);
            }
        }
#else
        const gchar* res = g_getenv(*it);
        if (res) {
            std::string buffer(*it);
            buffer += "=";
            buffer += res;
            envp.push_back(buffer);
        }
#endif

        it++;
    }

    g_strfreev(allenv);

#ifdef BOOST_WINDOWS
    /*
     * @attention The Win32 version needs a 8.3 path for the VLE's
     * PKG_CONFIG_PATH and update four others environment variables.
     */
    envp.push_back(buildEnvironmentVariable(
            "PATH",
            Path::buildFilename(
                convertPathTo83Path(Path::path().getPrefixDir()),
                "bin"),
            true));

    envp.push_back(buildEnvironmentVariable(
            "PKG_CONFIG_PATH",
            Path::buildFilename(
                convertPathTo83Path(Path::path().getPrefixDir()),
                "lib", "pkgconfig"),
            true));

    envp.push_back(buildEnvironmentVariable(
            "BOOST_INCLUDEDIR",
            Path::buildFilename(Path::path().getPrefixDir(), "include",
                                "boost"),
            false));

    envp.push_back(buildEnvironmentVariable(
            "BOOST_LIBRARYDIR",
            Path::buildFilename(Path::path().getPrefixDir(), "lib"),
            false));

    envp.push_back(buildEnvironmentVariable(
            "BOOST_ROOT",
            Path::path().getPrefixDir(),
            false));
#endif

    return envp;
}

void Package::create()
{
    Path& p = utils::Path::path();

    fs::create_directory(p.getPackageDir());
    p.copyTemplate("package", p.getPackageDir());

    appendOutput(_("Package creating - done\n"));
}

void Package::configure()
{
    Path& p = utils::Path::path();

    fs::create_directory(p.getPackageBuildDir());

    std::string cmd = (vle::fmt(mCommandConfigure) % p.getPackageDir()).str();
    std::list < std::string > argv;
    buildCommandLine(cmd, argv);

    std::list < std::string > envp = prepareEnvironmentVariable();

    try {
        process(p.getPackageBuildDir(), argv, envp);
    } catch(const std::exception& e) {
        throw utils::InternalError(fmt(
                _("Pkg configure error: configure failed %1%")) % e.what());
    }
}

void Package::test()
{
    Path& p = utils::Path::path();
    fs::create_directory(p.getPackageBuildDir());

    std::list < std::string > argv;
    buildCommandLine(mCommandTest, argv);

    std::list < std::string > envp = prepareEnvironmentVariable();

    try {
        process(p.getPackageBuildDir(), argv, envp);
    } catch(const Glib::SpawnError& e) {
        throw utils::InternalError(fmt(
                _("Pkg error: test launch failed %1%")) % e.what());
    }
}

void Package::build()
{
    Path& p = utils::Path::path();
    fs::create_directory(p.getPackageBuildDir());

    std::list < std::string > argv;
    buildCommandLine(mCommandBuild, argv);

    std::list < std::string > envp = prepareEnvironmentVariable();

    try {
        process(p.getPackageBuildDir(), argv, envp);
    } catch(const Glib::SpawnError& e) {
        throw utils::InternalError(fmt(
                _("Pkg build error: build failed %1%")) % e.what());
    }
}

void Package::install()
{
    Path& p = utils::Path::path();
    fs::create_directory(p.getPackageBuildDir());

    std::list < std::string > argv;
    buildCommandLine(mCommandInstall, argv);

    fs::path builddir = p.getPackageBuildDir();

    if (not fs::exists(builddir)) {
        throw utils::ArgError(fmt(
                _("Pkg build error: directory '%1%' does not exist")) %
#if BOOST_VERSION > 104500
            builddir.string());
#else
            builddir.file_string());
#endif
    }

    if (not fs::is_directory(builddir)) {
        throw utils::ArgError(fmt(
                _("Pkg build error: '%1%' is not a directory")) %
#if BOOST_VERSION > 104500
            builddir.string());
#else
            builddir.file_string());
#endif
    }

    std::list < std::string > envp = prepareEnvironmentVariable();

    try {
#if BOOST_VERSION > 104500
        process(builddir.string(), argv, envp);
#else
        process(builddir.file_string(), argv, envp);
#endif
    } catch(const Glib::SpawnError& e) {
        throw utils::InternalError(fmt(
                _("Pkg build error: install lib failed %1%")) % e.what());
    }
}

void Package::clean()
{
    Path& p = utils::Path::path();
    fs::create_directory(p.getPackageBuildDir());

    fs::path makefile = Path::path().getPackageBuildDir();
    makefile /= "Makefile";
    if (fs::exists(makefile)) {
        std::list < std::string > argv;
        buildCommandLine(mCommandClean, argv);

        std::list < std::string > envp = prepareEnvironmentVariable();

        try {
            process(p.getPackageBuildDir(), argv, envp);
        } catch(const Glib::SpawnError& e) {
            throw utils::InternalError(fmt(
                    _("Pkg clean error: clean failed %1%")) % e.what());
        }
        fs::remove(makefile);
    }

    fs::path cmakecache = Path::path().getPackageBuildDir();
    cmakecache /= "CMakeCache.txt";
    if (fs::exists(cmakecache)) {
        fs::remove(cmakecache);
    }
}

void Package::pack()
{
    Path& p = utils::Path::path();
    fs::create_directory(p.getPackageBuildDir());

    std::list < std::string > argv;
    buildCommandLine(mCommandPack, argv);

    std::list < std::string > envp = prepareEnvironmentVariable();

    try {
        process(p.getPackageBuildDir(), argv, envp);
    } catch(const Glib::SpawnError& e) {
        throw utils::InternalError(fmt(
                _("Pkg packaging error: package failed %1%")) % e.what());
    }
}

void Package::unzip(const std::string& package, const std::string& filename)
{
    Path& p = utils::Path::path();

    fs::path newdir = p.getPackageDir();
    newdir /= package;
    fs::create_directory(newdir);

    std::list < std::string > argv;
    buildCommandLine(mCommandUnzip, argv);

    std::list < std::string > envp = prepareEnvironmentVariable();

    try {
        process(newdir.string(), argv, envp);
    } catch(const Glib::SpawnError& e) {
        throw utils::InternalError(fmt(
                _("Pkg packaging error: unzip `%1%' failed %2%")) % filename %
            e.what());
    }
}

void Package::waitProcess()
{
    m_success = false;
#ifdef BOOST_WINDOWS
    DWORD status = 0;

    /*
     * WaitForSingleObject:
     * http://msdn.microsoft.com/en-us/library/ms687032(VS.85).aspx
     *
     * GetExitCodeProcess:
     * http://msdn.microsoft.com/en-us/library/ms683189(VS.85).aspx
     */
    WaitForSingleObject(m_pid, INFINITE);
    if (GetExitCodeProcess(m_pid, &status)) {
        if (status > 0) {
            appendError((fmt(_("Error '%1%' in subprocess")) %
                         status).str());
        } else {
            m_success = true;
        }
    }
#else
    int status;
    int result;

    result = waitpid(m_pid, &status, 0);
    if (result != -1) {
        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) > 0) {
                appendError((fmt(_("Error '%1%' in subprocess")) %
                             WEXITSTATUS(status)).str());
            } else {
                m_success = true;
            }
        }
    }
#endif

    m_out->join();  /* we wait the end of data readed from thread */
    m_err->join();
    Glib::spawn_close_pid(m_pid); /* we close process (win32 requirement) */

    m_out = 0;
    m_err = 0;
    m_stop = true;
}

void Package::wait(std::ostream& out, std::ostream& err)
{
    std::string o, e;

    if (not m_stop) {
        while (m_stop == false) {
            getOutputAndClear(o);
            getErrorAndClear(e);
            out << o;
            err << e;
            Glib::usleep(100000);
            o.clear();
            e.clear();
        }

        getOutputAndClear(o);
        getErrorAndClear(e);

        if (not o.empty()) {
            out << o;
        }

        if (not e.empty()) {
            err << e;
        }
    }
}

void Package::changeToOutputDirectory()
{
    if (utils::Package::package().selected()) {
        namespace fs = boost::filesystem;

        fs::path outputdir = utils::Path::path().getPackageOutputDir();

        if (not fs::exists(outputdir)) {
            fs::create_directories(outputdir);
            g_chdir(outputdir.string().c_str());
        } else {
            if (fs::is_directory(outputdir)) {
                g_chdir(outputdir.string().c_str());
            }
        }
    }
}

void Package::removePackage(const std::string& package)
{
    if (not package.empty()) {
        fs::path tmp = Path::path().getPackagesDir();
        tmp /= package;

        if (fs::exists(tmp)) {
            fs::remove_all(tmp);
            fs::remove(tmp);
        }
    }
}

bool Package::existsPackage(const std::string& package)
{
    if (not package.empty()) {
        fs::path tmp = Path::path().getPackagesDir();
        tmp /= package;

        return fs::exists(tmp);
    }

    return false;
}

void Package::addFile(const std::string& path, const std::string& name)
{
    if (Package::package().selected()) {
        fs::path tmp = Path::path().getPackageDir();
        tmp /= path;
        tmp /= name;

        if (not fs::exists(tmp)) {
#if BOOST_VERSION > 104500
            std::ofstream file(tmp.string().c_str());
#else
            std::ofstream file(tmp.file_string().c_str());
#endif
        }
    }
}

bool Package::existsFile(const std::string& path)
{
    if (Package::package().selected()) {
        fs::path tmp = Path::path().getPackageDir();
        tmp /= path;

#if BOOST_VERSION > 103600
        return fs::exists(tmp) and fs::is_regular_file(tmp);
#else
        return fs::exists(tmp) and fs::is_regular(tmp);
#endif
    }

    return false;
}

void Package::addDirectory(const std::string& path, const std::string& name)
{
    if (Package::package().selected()) {
        fs::path tmp(Path::path().getPackageDir());
        tmp /= path;
        tmp /= name;

        if (not fs::exists(tmp)) {
            fs::create_directory(tmp);
        }
    }
}

bool Package::existsDirectory(const std::string& path)
{
    if (Package::package().selected()) {
        fs::path tmp = Path::path().getPackageDir();
        tmp /= path;

        return fs::exists(tmp) and fs::is_directory(tmp);
    }

    return false;
}

void Package::remove(const std::string& path)
{
    fs::path tmp = Path::path().getPackageDir();
    tmp /= path;

    fs::remove_all(tmp);
    fs::remove(tmp);
}

std::string Package::rename(const std::string& oldname,
                            const std::string& newname)
{
    fs::path oldfilepath = Path::path().getPackageDir();
    oldfilepath /= oldname;

#if BOOST_VERSION > 103600
    fs::path newfilepath = oldfilepath.parent_path();
#else
    fs::path newfilepath = oldfilepath.branch_path();
#endif
    newfilepath /= newname;

    if (not fs::exists(oldfilepath) or fs::exists(newfilepath)) {
        throw utils::ArgError(fmt(
                _("In Package `%1%', can not rename `%2%' in `%3%'")) %
#if BOOST_VERSION > 104500
            name() % oldfilepath.string() % newfilepath.string());
#else
            name() % oldfilepath.file_string() % newfilepath.file_string());
#endif
    }

    fs::rename(oldfilepath, newfilepath);

#if BOOST_VERSION > 104500
    return newfilepath.string();
#else
    return newfilepath.file_string();
#endif
}

void Package::copy(const std::string& source, std::string& target)
{
    fs::copy_file(source, target);
}

void Package::select(const std::string& name)
{
    m_table.current(name);

    Path::path().updateDirs();

    if (selected()) {
        changeToOutputDirectory();
    }
}

void Package::refresh()
{
    utils::Preferences prefs;

    prefs.get("vle.packages.configure", &mCommandConfigure);
    prefs.get("vle.packages.test", &mCommandTest);
    prefs.get("vle.packages.build", &mCommandBuild);
    prefs.get("vle.packages.install", &mCommandInstall);
    prefs.get("vle.packages.clean", &mCommandClean);
    prefs.get("vle.packages.package", &mCommandPack);
    prefs.get("vle.packages.unzip", &mCommandUnzip);
}

Package::Package()
    : m_stop(true), m_success(false), m_out(0), m_err(0), m_wait(0), m_pid(0)
{
}
                            /*   manage thread   */

void Package::process(const std::string& workingDir,
                      const std::list < std::string >& argv,
                      const std::list < std::string >& envp)
{
    if (m_out != 0 or m_err != 0) {
        throw utils::InternalError(_("Package error: wait an unknow process"));
    }

    m_stop = false;

    if (utils::Trace::isInLevel(utils::TRACE_LEVEL_DEVS)) {
        std::list < std::string >::const_iterator it = envp.begin();
        std::list < std::string >::const_iterator et = envp.end();

        for (; it != et; ++it) {
            utils::Trace::send((fmt("-[%1%]") % (*it)).str());
        }
    }

    try {
        int out, err;

        Glib::spawn_async_with_pipes(workingDir,
                                     argv,
                                     envp,
				     Glib::SPAWN_SEARCH_PATH |
                                     Glib::SPAWN_DO_NOT_REAP_CHILD,
                                     sigc::slot < void >(),
                                     &m_pid,
                                     0, &out, &err);

        m_out = Glib::Thread::create(
            sigc::bind(sigc::mem_fun(*this, &Package::readStandardOutputStream),
                       out), true);

        m_err = Glib::Thread::create(
            sigc::bind(sigc::mem_fun(*this, &Package::readStandardErrorStream),
                       err), true);

        m_wait = Glib::Thread::create(
            sigc::mem_fun(*this, &Package::waitProcess), false);

    } catch(const std::exception& e) {
        appendError((fmt(_("Std exception: '%1%'")) % e.what()).str());
	throw utils::InternalError(e.what());
    } catch(const Glib::Exception& e) {
        appendError((fmt(_("Glib exception: '%1%'")) % e.what()).str());
	throw utils::InternalError(e.what());
    }
}

void Package::readStandardOutputStream(int stream)
{
    const size_t bufferSize = 64;
    char* buffer = new char[bufferSize];
    int result;

    do {
        std::memset(buffer, 0, bufferSize);
#ifdef BOOST_WINDOWS
        result = ::_read(stream, buffer, bufferSize - 1);
#else
        result = ::read(stream, buffer, bufferSize - 1);
#endif

        if (result == -1) {
            appendOutput(_("Error reading stream"));
            if (m_stop) {
                result = 0;
            }
        } else if (result == 0) {
            appendOutput("\n");
        } else {
            appendOutput(buffer);
        }
    } while (result);

    delete[] buffer;
#ifdef BOOST_WINDOWS
    ::_close(stream);
#else
    ::close(stream);
#endif
}

void Package::readStandardErrorStream(int stream)
{
    const size_t bufferSize = 64;
    char* buffer = new char[bufferSize];
    int result;

    do {
        std::memset(buffer, 0, bufferSize);
#ifdef BOOST_WINDOWS
        result = ::_read(stream, buffer, bufferSize - 1);
#else
        result = ::read(stream, buffer, bufferSize - 1);
#endif

        if (result == -1) {
            appendError(_("Error reading stream"));
            if (m_stop) {
                result = 0;
            }
        } else if (result == 0) {
            appendError("\n");
        } else {
            appendError(buffer);
        }
    } while (result);

    delete[] buffer;
#ifdef BOOST_WINDOWS
    ::_close(stream);
#else
    ::close(stream);
#endif
}

void Package::getOutput(std::string& str) const
{
    Glib::Mutex::Lock lock(m_mutex);

    str.append(m_strout);
}

void Package::getError(std::string& str) const
{
    Glib::Mutex::Lock lock(m_mutex);

    str.append(m_strerr);
}

void Package::getOutputAndClear(std::string& str)
{
    Glib::Mutex::Lock lock(m_mutex);

    str.append(m_strout);
    m_strout.clear();
}

void Package::getErrorAndClear(std::string& str)
{
    Glib::Mutex::Lock lock(m_mutex);

    str.append(m_strerr);
    m_strerr.clear();
}

void Package::appendOutput(const char* str)
{
    Glib::Mutex::Lock lock(m_mutex);

    m_strout.append(str);
}

void Package::appendError(const char* str)
{
    Glib::Mutex::Lock lock(m_mutex);

    m_strerr.append(str);
}

void Package::appendOutput(const std::string& str)
{
    Glib::Mutex::Lock lock(m_mutex);

    m_strout.append(str);
}

void Package::appendError(const std::string& str)
{
    Glib::Mutex::Lock lock(m_mutex);

    m_strerr.append(str);
}

void Package::buildCommandLine(const std::string& cmd,
                               std::list < std::string >& argv)
{
    argv = Glib::shell_parse_argv(cmd);

    if (argv.empty()) {
        throw utils::ArgError(fmt(_(
                    "Package command line: error in command `%1%'")) % cmd);
    }

    std::string exe = argv.front();
    argv.pop_front();
    argv.push_front(Glib::find_program_in_path(exe));
}

}} // namespace vle utils
