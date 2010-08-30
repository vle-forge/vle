/**
 * @file vle/utils/Package.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
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
#include <vle/utils/Exception.hpp>
#include <boost/filesystem.hpp>
#include <boost/version.hpp>
#include <glibmm/timer.h>
#include <glibmm/stringutils.h>
#include <glibmm/miscutils.h>
#include <fstream>
#include <ostream>
#include <iostream>
#include <cstring>
#include <glib/gstdio.h>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/classification.hpp>

#ifdef G_OS_WIN32
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

    try {
        process(p.getPackageBuildDir(), argv);
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

    try {
        process(p.getPackageBuildDir(), argv);
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

    try {
        process(p.getPackageBuildDir(), argv);
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

    fs::path src = p.getPackageBuildDir();
    src /= "src";

    if (not fs::exists(src)) {
        throw utils::ArgError(fmt(
                _("Pkg build error: directory '%1%' does not exist")) %
            src.file_string());
    }

    if (not fs::is_directory(src)) {
        throw utils::ArgError(fmt(
                _("Pkg build error: '%1%' is not a directory")) %
            src.file_string());
    }

    try {
        process(src.file_string(), argv);
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

        try {
            process(p.getPackageBuildDir(), argv);
        } catch(const Glib::SpawnError& e) {
            throw utils::InternalError(fmt(
                    _("Pkg clean error: clean failed %1%")) % e.what());
        }
    }
}

void Package::pack()
{
    Path& p = utils::Path::path();
    fs::create_directory(p.getPackageBuildDir());

    std::list < std::string > argv;
    buildCommandLine(mCommandPack, argv);

    try {
        process(p.getPackageBuildDir(), argv);
    } catch(const Glib::SpawnError& e) {
        throw utils::InternalError(fmt(
                _("Pkg packaging error: package failed %1%")) % e.what());
    }
}

void Package::unzip(const std::string& filename)
{
    Path& p = utils::Path::path();
    fs::create_directory(p.getPackageBuildDir());

    std::list < std::string > argv;
    buildCommandLine(mCommandUnzip, argv);

    try {
        process(p.getPackagesDir(), argv);
    } catch(const Glib::SpawnError& e) {
        throw utils::InternalError(fmt(
                _("Pkg packaging error: unzip `%1%' failed %2%")) % filename %
            e.what());
    }
}

void Package::waitProcess()
{
    m_success = false;
#ifdef G_OS_WIN32
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

void Package::addFile(const std::string& path, const std::string& name)
{
    if (not fs::exists(utils::Path::buildFilename(path, name))) {
	std::ofstream file(utils::Path::buildFilename(path, name).c_str());
    }
}

void Package::addDirectory(const std::string& path, const std::string& name)
{
    if (not fs::exists(utils::Path::buildDirname(path, name))) {
        fs::create_directory(utils::Path::buildDirname(path, name));
    }
}

void Package::removeFile(const std::string& pathFile)
{
    std::string path = utils::Path::buildFilename(
        Path::path().getPackageDir(), pathFile);

    fs::remove_all(path);
    fs::remove(path);
}

void Package::renameFile(const std::string& oldFile, std::string& newName)
{
    std::string oldAbsolutePath = utils::Path::buildFilename(
        Path::path().getPackageDir(), oldFile);

    if (fs::extension(newName) == "")
	newName += fs::extension(oldAbsolutePath);
    std::string newAbsolutePath = utils::Path::buildFilename(
        Path::path().getParentPath(oldAbsolutePath), newName);

    if (not fs::exists(newAbsolutePath))
	fs::rename(oldAbsolutePath, newAbsolutePath);
}

void Package::copyFile(const std::string& sourceFile, std::string& targetFile)
{
    fs::copy_file(sourceFile, targetFile);
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
    prefs.load();

    prefs.assign("package", "configure", mCommandConfigure);
    prefs.assign("package", "test", mCommandTest);
    prefs.assign("package", "build", mCommandBuild);
    prefs.assign("package", "install", mCommandInstall);
    prefs.assign("package", "clean", mCommandClean);
    prefs.assign("package", "pack", mCommandPack);
    prefs.assign("package", "unzip", mCommandUnzip);
}

Package::Package()
    : m_stop(true), m_success(false), m_out(0), m_err(0), m_wait(0), m_pid(0)
{
#ifdef G_OS_WIN32
    mCommandConfigure = "cmake.exe -G MinGWMakefile " \
                       "-DCMAKE_INSTALL_PREFIX=%1% " \
                       "-DCMAKE_BUILD_TYPE=RelWithDebInfo ..";
    mCommandTest = "mingw32-make.exe test";
    mCommandBuild = "mingw32-make.exe all";
    mCommandInstall = "mingw32-make.exe install";
    mCommandClean = "mingw32-make.exe clean";
    mCommandPack = "mingw32-make.exe package package_source";
    mCommandUnzip = "unzip.exe";

#else
    mCommandConfigure = "cmake " \
                       "-DCMAKE_INSTALL_PREFIX=%1% " \
                       "-DCMAKE_BUILD_TYPE=RelWithDebInfo ..";
    mCommandTest = "make test";
    mCommandBuild = "make all";
    mCommandInstall = "make install";
    mCommandClean = "make clean";
    mCommandPack = "make package package_source";
    mCommandUnzip = "unzip";
#endif
}
                            /*   manage thread   */

void Package::process(const std::string& workingDir,
                      const std::list < std::string >& lst)
{
    if (m_out != 0 or m_err != 0) {
        throw utils::InternalError(_("Package error: wait an unknow process"));
    }

    m_stop = false;

    try {
        int out, err;

        Glib::spawn_async_with_pipes(workingDir,
                                     lst,
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
#ifdef G_OS_WIN32
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
#ifdef G_OS_WIN32
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
#ifdef G_OS_WIN32
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
#ifdef G_OS_WIN32
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
    namespace ba = boost::algorithm;

    ba::split(argv, cmd, ba::is_space(), ba::token_compress_on);

    if (argv.empty()) {
        throw utils::ArgError(fmt(_(
                    "Package command line: error in command `%1%'")) % cmd);
    }

    std::string exe = argv.front();
    argv.pop_front();
    argv.push_front(Glib::find_program_in_path(exe));
}

}} // namespace vle utils
