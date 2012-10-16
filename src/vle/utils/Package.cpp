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
#include <vle/utils/details/Spawn.hpp>
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
#include <boost/cast.hpp>
#include <boost/version.hpp>
#include <boost/config.hpp>

namespace fs = boost::filesystem;

namespace vle { namespace utils {

Package* Package::m_package = 0;

/**
 * Build a vector of string from a command line.
 *
 * @param cmd The command line to parse and extract arguments.
 * @param [out] exe The program name executable.
 * @param [out] argv The list of arguments.
 */
static void buildCommandLine(const std::string& cmd,
                             std::string& exe,
                             std::vector < std::string >& argv)
{
    argv = Glib::shell_parse_argv(cmd);

    if (argv.empty()) {
        throw utils::ArgError(fmt(_(
                    "Package command line: error in command `%1%'")) % cmd);
    }

    exe = Glib::find_program_in_path(argv.front());
    argv.erase(argv.begin());
}

#ifdef _WIN32
/**
 * A specific \b Win32 function to convert a path into a 8.3 path.
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

/**
 * A specific \b Win32 function to build a new environment
 * variable.
 *
 * @param variable
 * @param value
 * @param append
 *
 * @return
 */
static std::string replaceEnvironmentVariable(const std::string& variable,
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
            result += ";";
            result += old;
        }
    }

    return result;
}
#endif

/**
 * Build list of string which represents the 'VARIABLE=VALUE'
 * environment variable.
 *
 * @attention @b Win32: the PKG_CONFIG_FILE is path is update with the
 * vle's pkgconfig directory. The BOOST_ROOT, BOOST_INCLUDEDIR and
 * BOOST_LIBRARYDIR are assigned with the vle's patch to avoid
 * conflict with different boost version.
 *
 * @return A list of string.
 */
static std::vector < std::string > prepareEnvironmentVariable()
{
    std::vector < std::string > envp;
    gchar** allenv = g_listenv();
    gchar** it = allenv;

    while (*it != NULL) {

#ifdef _WIN32
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

#ifdef _WIN32
    /*
     * @attention The Win32 version needs a 8.3 path for the VLE's
     * PKG_CONFIG_PATH and update four others environment variables.
     */
    envp.push_back(replaceEnvironmentVariable(
            "PATH",
            Path::buildFilename(
                convertPathTo83Path(Path::path().getPrefixDir()),
                "bin"),
            true));

    envp.push_back(replaceEnvironmentVariable(
            "PKG_CONFIG_PATH",
            Path::buildFilename(
                convertPathTo83Path(Path::path().getPrefixDir()),
                "lib", "pkgconfig"),
            true));

    envp.push_back(replaceEnvironmentVariable(
            "BOOST_INCLUDEDIR",
            Path::buildFilename(Path::path().getPrefixDir(), "include",
                                "boost"),
            false));

    envp.push_back(replaceEnvironmentVariable(
            "BOOST_LIBRARYDIR",
            Path::buildFilename(Path::path().getPrefixDir(), "lib"),
            false));

    envp.push_back(replaceEnvironmentVariable(
            "BOOST_ROOT",
            Path::path().getPrefixDir(),
            false));
#endif

    return envp;
}



struct Package::Pimpl
{
    Pimpl() {}

    ~Pimpl() {}

    PackageTable m_table;
    Spawn        m_spawn;
    std::string  m_message;
    std::string  m_strout;
    std::string  m_strerr;
    std::string  mCommandConfigure;
    std::string  mCommandTest;
    std::string  mCommandBuild;
    std::string  mCommandInstall;
    std::string  mCommandClean;
    std::string  mCommandPack;
    std::string  mCommandUnzip;
    bool         m_issuccess;
};


Package::Package()
    : m_pimpl(new Package::Pimpl())
{
}

Package::~Package()
{
    delete m_pimpl;
}

void Package::create()
{
    Path& p = utils::Path::path();

    fs::create_directory(p.getPackageDir());
    p.copyTemplate("package", p.getPackageDir());

    m_pimpl->m_strout.append(_("Package creating - done\n"));
}

void Package::configure()
{
    Path& p = utils::Path::path();

    fs::create_directory(p.getPackageBuildDir());

    std::vector < std::string > argv;
    std::string exe, cmd;

    cmd = (vle::fmt(m_pimpl->mCommandConfigure) % p.getPackageDir()).str();

    buildCommandLine(cmd, exe, argv);

    std::vector < std::string > envp = prepareEnvironmentVariable();

    try {
        process(exe, p.getPackageBuildDir(), argv, envp);
    } catch(const std::exception& e) {
        throw utils::InternalError(fmt(
                _("Pkg configure error: configure failed %1%")) % e.what());
    }
}

void Package::test()
{
    Path& p = utils::Path::path();
    fs::create_directory(p.getPackageBuildDir());

    std::vector < std::string > argv;
    std::string exe;

    buildCommandLine(m_pimpl->mCommandTest, exe, argv);

    std::vector < std::string > envp = prepareEnvironmentVariable();

    try {
        process(exe, p.getPackageBuildDir(), argv, envp);
    } catch (const std::exception& e) {
        throw utils::InternalError(
            fmt(_("Pkg error: test launch failed %1%")) % e.what());
    }
}

void Package::build()
{
    Path& p = utils::Path::path();
    fs::create_directory(p.getPackageBuildDir());

    std::vector < std::string > argv;
    std::string exe;

    buildCommandLine(m_pimpl->mCommandBuild, exe, argv);

    std::vector < std::string > envp = prepareEnvironmentVariable();

    try {
        process(exe, p.getPackageBuildDir(), argv, envp);
    } catch(const std::exception& e) {
        throw utils::InternalError(fmt(
                _("Pkg build error: build failed %1%")) % e.what());
    }
}

void Package::install()
{
    Path& p = utils::Path::path();
    fs::create_directory(p.getPackageBuildDir());

    std::vector < std::string > argv;
    std::string exe;

    buildCommandLine(m_pimpl->mCommandInstall, exe, argv);

    fs::path builddir = p.getPackageBuildDir();

    if (not fs::exists(builddir)) {
        throw utils::ArgError(
            fmt(_("Pkg build error: directory '%1%' does not exist")) %
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

    std::vector < std::string > envp = prepareEnvironmentVariable();

    try {
        process(exe, p.getPackageBuildDir(), argv, envp);
    } catch(const std::exception& e) {
        throw utils::InternalError(
            fmt(_("Pkg build error: install lib failed %1%")) % e.what());
    }
}

void Package::clean()
{
    Path& p = utils::Path::path();
    fs::create_directory(p.getPackageBuildDir());

    std::vector < std::string > argv;
    std::string exe;
    buildCommandLine(m_pimpl->mCommandClean, exe, argv);

    std::vector < std::string > envp = prepareEnvironmentVariable();

    try {
        process(exe, p.getPackageBuildDir(), argv, envp);
    } catch(const std::exception& e) {
        throw utils::InternalError(fmt(
                _("Pkg clean error: clean failed %1%")) % e.what());
    }
}

void Package::pack()
{
    Path& p = utils::Path::path();
    fs::create_directory(p.getPackageBuildDir());

    std::vector < std::string > argv;
    std::string exe;
    buildCommandLine(m_pimpl->mCommandPack, exe, argv);

    std::vector < std::string > envp = prepareEnvironmentVariable();

    try {
        process(exe, p.getPackageBuildDir(), argv, envp);
    } catch(const std::exception& e) {
        throw utils::InternalError(fmt(
                _("Pkg packaging error: package failed %1%")) % e.what());
    }
}

bool Package::isFinish() const
{
    return m_pimpl->m_spawn.isfinish();
}

bool Package::isSuccess() const
{
    if (not m_pimpl->m_spawn.isfinish()) {
        return false;
    }

    m_pimpl->m_spawn.status(&m_pimpl->m_message,
                            &m_pimpl->m_issuccess);

    return m_pimpl->m_issuccess;
}

bool Package::wait(std::ostream& out, std::ostream& err)
{
    std::string output;
    std::string error;

    output.reserve(Spawn::default_buffer_size);
    error.reserve(Spawn::default_buffer_size);

    while (not m_pimpl->m_spawn.isfinish()) {
        if (m_pimpl->m_spawn.get(&output, &error)) {
            out << output;
            err << error;

            output.clear();
            err.clear();

            Glib::usleep(1000);
        } else {
            break;
        }
    }

    m_pimpl->m_spawn.wait();

    return m_pimpl->m_spawn.status(&m_pimpl->m_message, &m_pimpl->m_issuccess);
}

bool Package::get(std::string *out, std::string *err)
{
    out->reserve(Spawn::default_buffer_size);
    err->reserve(Spawn::default_buffer_size);

    if (not m_pimpl->m_spawn.isfinish()) {
        if (m_pimpl->m_spawn.get(out, err)) {
            return true;
        }
    }

    return false;
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

void Package::removePackageBinary(const std::string& package)
{
    if (not package.empty()) {
        fs::path tmp = Path::path().getPackagesDir();
        tmp /= package;

        if (fs::exists(tmp)) {
            {
                fs::path build = tmp;
                build /= "build";

                if (fs::exists(build)) {
                    fs::remove_all(build);
                }
            }

            {
                fs::path lib = tmp;
                lib /= "lib";

                if (fs::exists(lib)) {
                    fs::remove_all(lib);
                }
            }

            {
                fs::path plugins = tmp;
                plugins /= "plugins";

                if (fs::exists(plugins)) {
                    fs::remove_all(plugins);
                }
            }

            {
                fs::path doc = tmp;
                doc /= "doc";
                doc /= "html";

                if (fs::exists(doc)) {
                    fs::remove_all(doc);
                }
            }
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

const std::string& Package::name() const
{
    return m_pimpl->m_table.current();
}

bool Package::selected() const
{
    return not m_pimpl->m_table.current().empty();
}

void Package::select(const std::string& name)
{
    m_pimpl->m_table.current(name);

    Path::path().updateDirs();

    if (selected()) {
        changeToOutputDirectory();
    }
}

void Package::refresh()
{
    utils::Preferences prefs;

    prefs.get("vle.packages.configure", &m_pimpl->mCommandConfigure);
    prefs.get("vle.packages.test", &m_pimpl->mCommandTest);
    prefs.get("vle.packages.build", &m_pimpl->mCommandBuild);
    prefs.get("vle.packages.install", &m_pimpl->mCommandInstall);
    prefs.get("vle.packages.clean", &m_pimpl->mCommandClean);
    prefs.get("vle.packages.package", &m_pimpl->mCommandPack);
    prefs.get("vle.packages.unzip", &m_pimpl->mCommandUnzip);
}

PackageTable::index Package::getId(const std::string& package)
{
    return m_pimpl->m_table.get(package);
}

void Package::process(const std::string& exe,
                      const std::string& workingDir,
                      const std::vector < std::string >& argv,
                      const std::vector < std::string >& envp)
{
    if (not m_pimpl->m_spawn.isfinish()) {
        if (utils::Trace::isInLevel(utils::TRACE_LEVEL_DEVS)) {
            utils::Trace::send(
                fmt("-[%1%] Need to wait old process before") % exe);
        }

        m_pimpl->m_spawn.wait();
        m_pimpl->m_spawn.status(&m_pimpl->m_message, &m_pimpl->m_issuccess);
    }

    if (utils::Trace::isInLevel(utils::TRACE_LEVEL_DEVS)) {
        utils::Trace::send(fmt("-[%1%] Try to start") % exe);

        std::vector < std::string >::const_iterator it = envp.begin();
        std::vector < std::string >::const_iterator et = envp.end();

        for (; it != et; ++it) {
            utils::Trace::send((fmt("-[%1%]") % (*it)).str());
        }
    }

    if (not m_pimpl->m_spawn.start(exe, workingDir, argv, envp)) {
        throw utils::ArgError(fmt(_("Failed to start `%1%'")) % exe);
    }
}

}} // namespace vle utils
