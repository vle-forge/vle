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
#include <vle/utils/Exception.hpp>
#include <boost/filesystem.hpp>
#include <boost/version.hpp>
#include <glibmm/spawn.h>
#include <glibmm/miscutils.h>
#include <iostream>


namespace vle { namespace utils {

CMakePackage::CMakePackage(const std::string& package)
{
    Path::path().setPackage(package);
}

void CMakePackage::create(std::string& /* out */, std::string& /* err */)
{
    Path& p = utils::Path::path();

    boost::filesystem::create_directory(p.getPackageDir());
    p.copyTemplate("package", p.getPackageDir());
}

void CMakePackage::configure(std::string& out, std::string& err)
{
    Path& p = utils::Path::path();

    boost::filesystem::create_directory(p.getPackageBuildDir());

    std::string prg = Glib::find_program_in_path("cmake");

    std::list < std::string > argv;
    argv.push_back(prg);
    argv.push_back((fmt("-DCMAKE_INSTALL_PREFIX=\'%1%\'") %
                    p.getPackageDir()).str());
    argv.push_back("-DCMAKE_BUILD_TYPE=RelWithDebInfo");
    argv.push_back("..");

    std::copy(argv.begin(), argv.end(),
              std::ostream_iterator < std::string >(std::cout, " "));

    int status;
    try {
        Glib::spawn_sync(p.getPackageBuildDir(), argv, Glib::SpawnFlags(0),
                         sigc::slot < void >(), &out, &err, &status);
    } catch(const Glib::SpawnError& e) {
        throw utils::InternalError(fmt(
                _("Pkg configure error: configure failed %1%")) % e.what());
    }
}

void CMakePackage::build(std::string& out, std::string& err)
{
    Path& p = utils::Path::path();

    std::list < std::string > argv;
    std::string prg = Glib::find_program_in_path("make");
    argv.push_back(prg);
    argv.push_back("all");

    int status;
    try {
        Glib::spawn_sync(p.getPackageBuildDir(), argv, Glib::SpawnFlags(0),
                         sigc::slot < void >(), &out, &err, &status);
    } catch(const Glib::SpawnError& e) {
        throw utils::InternalError(fmt(
                _("Pkg build error: build failed %1%")) % e.what());
    }

    argv.clear();
    argv.push_back(prg);
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
        Glib::spawn_sync(src.file_string(), argv, Glib::SpawnFlags(0),
                         sigc::slot < void >(), &out, &err, &status);
    } catch(const Glib::SpawnError& e) {
        throw utils::InternalError(fmt(
                _("Pkg build error: install lib failed %1%")) % e.what());
    }
}

void CMakePackage::clean(std::string& out, std::string& err)
{
    Path& p = utils::Path::path();

    std::string prg = Glib::find_program_in_path("make");
    std::list < std::string > argv;
    argv.push_back(prg);
    argv.push_back("clean");

    int status;
    try {
        Glib::spawn_sync(p.getPackageBuildDir(), argv, Glib::SpawnFlags(0),
                         sigc::slot < void >(), &out, &err, &status);
    } catch(const Glib::SpawnError& e) {
        throw utils::InternalError(fmt(
                _("Pkg clean error: clean failed %1%")) % e.what());
    }
}

void CMakePackage::package(std::string& out, std::string& err)
{
    Path& p = utils::Path::path();

    std::list < std::string > argv;
    std::string prg = Glib::find_program_in_path("make");
    argv.push_back(prg);
    argv.push_back("package");
    argv.push_back("package_source");

    int status;
    try {
        Glib::spawn_sync(p.getPackageBuildDir(), argv, Glib::SpawnFlags(0),
                         sigc::slot < void >(), &out, &err, &status);
    } catch(const Glib::SpawnError& e) {
        throw utils::InternalError(fmt(
                _("Pkg packaging error: package failed %1%")) % e.what());
    }
}

PathList CMakePackage::getInstalledPackages()
{
    namespace fs = boost::filesystem;

    fs::path pkgs(Path::path().getPackagesDir());

    if (not fs::exists(pkgs) or not fs::is_directory(pkgs)) {
        throw utils::InternalError(fmt(
                _("Package error: '%1%' is not a directory")) %
            Path::path().getPackagesDir());
    }

    PathList result;
    for (fs::directory_iterator it(pkgs), end; it != end; ++it) {
        if (fs::is_directory(it->status())) {
            result.push_back(it->path().file_string());
        }
    }

    return result;
}

PathList CMakePackage::getInstalledExperiments()
{
    namespace fs = boost::filesystem;

    fs::path pkgs(Path::path().getPackageExpDir());

    if (not fs::exists(pkgs) or not fs::is_directory(pkgs)) {
        throw utils::InternalError(fmt(
                _("Pkg list error: '%1%' is not an experiments directory")) %
            pkgs.file_string());
    }

    PathList result;
    for (fs::directory_iterator it(pkgs), end; it != end; ++it) {
#if BOOST_VERSION > 103600
        if (fs::is_regular_file(it->status())) {
            fs::path::string_type ext = it->path().extension();
#else
        if (fs::is_regular(it->status())) {
            fs::path::string_type ext = fs::extension(it->path());
#endif
            if (ext == ".vpz") {
                result.push_back(it->path().file_string());
            }
        }
    }
    return result;
}

PathList CMakePackage::getInstalledLibraries()
{
    namespace fs = boost::filesystem;

    PathList result;
    const PathList& dirs = Path::path().getSimulatorDirs();

    for (PathList::const_iterator it = dirs.begin(); it != dirs.end(); ++it) {
        fs::path dir(*it);

        if (not fs::exists(dir) or not fs::is_directory(dir)) {
            throw utils::InternalError(fmt(
                    _("Pkg list error: '%1%' is not a library directory")) %
                dir.file_string());
        }

        for (fs::directory_iterator jt(dir), end; jt != end; ++jt) {
#if BOOST_VERSION > 103600
            if (fs::is_regular_file(jt->status())) {
                fs::path::string_type ext = jt->path().extension();
#else
            if (fs::is_regular(jt->status())) {
                fs::path::string_type ext = fs::extension(jt->path());
#endif
#ifdef G_OS_WINDOWS
                if (ext == ".dll") {
                    result.push_back(jt->path().file_string());
                }
#elif G_OS_MACOS
                if (ext == ".dylib") {
                    result.push_back(jt->path().file_string());
                }
#else
                if (ext == ".so") {
                    result.push_back(jt->path().file_string());
                }
#endif
            }
        }
    }
    return result;
}

}} // namespace vle utils
