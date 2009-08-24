/**
 * @file vle/utils/Package.cpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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
#include <vle/utils/Exception.hpp>
#include <boost/filesystem.hpp>
#include <boost/version.hpp>
#include <glibmm/timer.h>
#include <glibmm/miscutils.h>
#include <glibmm/fileutils.h>
#include <glibmm/stringutils.h>
#include <fstream>
#include <ostream>
#include <iostream>
#include <cstring>

#ifdef G_OS_WIN32
#include    <Windows.h>
#include    <io.h>
#else
#   include <sys/wait.h>
#   include <unistd.h>
#   include <errno.h>
#endif


namespace vle { namespace utils {

Package* Package::m_package = 0;

void Package::create()
{
    Path& p = utils::Path::path();

    boost::filesystem::create_directory(p.getPackageDir());
    p.copyTemplate("package", p.getPackageDir());

    appendOutput(_("Package creating - done\n"));
}

void Package::configure()
{
    Path& p = utils::Path::path();

    boost::filesystem::create_directory(p.getPackageBuildDir());

    std::list < std::string > argv;
#ifdef G_OS_WIN32
    argv.push_back(Glib::find_program_in_path("cmake.exe"));;
    argv.push_back("-G");
    argv.push_back("MinGW Makefiles");
#else
    argv.push_back(Glib::find_program_in_path("cmake"));;
#endif
    argv.push_back((fmt("-DCMAKE_INSTALL_PREFIX=%1%") %
                    p.getPackageDir()).str());
    argv.push_back("-DCMAKE_BUILD_TYPE=RelWithDebInfo");
    argv.push_back("..");

    try {
        process(p.getPackageBuildDir(), argv);
    } catch(const std::exception& e) {
        throw utils::InternalError(fmt(
                _("Pkg configure error: configure failed %1%")) % e.what());
    }
}

void Package::build()
{
    Path& p = utils::Path::path();

    std::list < std::string > argv;
#ifdef G_OS_WIN32
    argv.push_back(Glib::find_program_in_path("mingw32-make.exe"));
#else
    argv.push_back(Glib::find_program_in_path("make"));
#endif
    argv.push_back("all");

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

    std::list < std::string > argv;
#ifdef G_OS_WIN32
    argv.push_back(Glib::find_program_in_path("mingw32-make.exe"));
#else
    argv.push_back(Glib::find_program_in_path("make"));
#endif
    argv.push_back("install");

    namespace fs = boost::filesystem;
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

    std::list < std::string > argv;
#ifdef G_OS_WIN32
    argv.push_back(Glib::find_program_in_path("mingw32-make.exe"));
#else
    argv.push_back(Glib::find_program_in_path("make"));
#endif
    argv.push_back("clean");

    try {
        process(p.getPackageBuildDir(), argv);
    } catch(const Glib::SpawnError& e) {
        throw utils::InternalError(fmt(
                _("Pkg clean error: clean failed %1%")) % e.what());
    }
}

void Package::pack()
{
    Path& p = utils::Path::path();

    std::list < std::string > argv;
#ifdef G_OS_WIN32
    argv.push_back(Glib::find_program_in_path("mingw32-make.exe"));
#else
    argv.push_back(Glib::find_program_in_path("make"));
#endif
    argv.push_back("package");
    argv.push_back("package_source");

    try {
        process(p.getPackageBuildDir(), argv);
    } catch(const Glib::SpawnError& e) {
        throw utils::InternalError(fmt(
                _("Pkg packaging error: package failed %1%")) % e.what());
    }
}

void Package::waitProcess()
{
#ifdef __WIN32__
    /* WaitFor*() functions, or examine its exit code with GetExitCodeProcess */
    DWORD status = 0;

    WaitForSingleObject(m_pid, INFINITE);
    GetExitCodeProcess(m_pid, &status);
#else
    int status;
    int result;

    result = waitpid(m_pid, &status, 0);
    if (result == -1) {
        int e = errno;
        appendError((fmt(_("Error waiting process: %1%")) %
                 Glib::strerror(e)).str());
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

void Package::addFile(const std::string& path, const std::string& name)
{
    if (not boost::filesystem::exists(Glib::build_filename(path, name)))
	std::ofstream file(Glib::build_filename(path, name).c_str());
}

void Package::addDirectory(const std::string& path, const std::string& name)
{
    if (not boost::filesystem::exists(Glib::build_filename(path, name)))
	boost::filesystem::create_directory(
	    Glib::build_filename(path, name));
}

void Package::removeFile(const std::string& pathFile)
{
    std::string path = Glib::build_filename(
	Path::path().getPackageDir(), pathFile);
    boost::filesystem::remove_all(path);
    boost::filesystem::remove(path);
}

void Package::renameFile(const std::string& oldFile, std::string& newName)
{
    std::string oldAbsolutePath =
	Glib::build_filename(Path::path().getPackageDir(), oldFile);

    if (boost::filesystem::extension(newName) == "")
	newName += boost::filesystem::extension(oldAbsolutePath);
    std::string newAbsolutePath =
	Glib::build_filename(
	    Path::path().getParentPath(oldAbsolutePath), newName);

    if (not boost::filesystem::exists(newAbsolutePath))
	boost::filesystem::rename(oldAbsolutePath, newAbsolutePath);
}

void Package::select(const std::string& name)
{
    if (name.empty()) {
        m_name.clear();
    } else {
        m_name.assign(name);
    }
    Path::path().updatePackageDirs();
}

                            /*   manage thread   */

void Package::process(const std::string& workingDir,
                      const std::list < std::string >& lst)
{
    if (m_out != 0 or m_err != 0) {
        throw utils::InternalError("Package error: wait an unknow process");
    }

    m_stop = false;

    try {
        int out, err;

        Glib::spawn_async_with_pipes(workingDir,
                                     lst,
				     Glib::SPAWN_SEARCH_PATH,
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

}} // namespace vle utils
