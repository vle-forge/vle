/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
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


#include <vle/utils/Package.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Preferences.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Spawn.hpp>
#include <boost/filesystem.hpp>
#include <boost/version.hpp>
#include <glibmm/timer.h>
#include <glibmm/shell.h>
#include <fstream>
#include <ostream>
#include <cstring>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/cast.hpp>
#include <boost/version.hpp>
#include <boost/config.hpp>

namespace fs = boost::filesystem;

namespace vle { namespace utils {

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
    exe = Path::findProgram(argv.front());

    argv.erase(argv.begin());
}

struct Package::Pimpl
{
    Pimpl()
    {
    }

    Pimpl(const std::string& pkgname) :
        m_pkgname(pkgname), m_pkgbinarypath(), m_pkgsourcepath()
    {
        refreshPath();
    }

    ~Pimpl() {}

    std::string  m_pkgname;
    std::string  m_pkgbinarypath;
    std::string  m_pkgsourcepath;

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

    void process(const std::string& exe,
                 const std::string& workingDir,
                 const std::vector < std::string >& argv)
    {
        if (not m_spawn.isfinish()) {
            if (utils::Trace::isInLevel(utils::TRACE_LEVEL_DEVS)) {
                utils::Trace::send(
                    fmt("-[%1%] Need to wait old process before") % exe);
            }

            m_spawn.wait();
            m_spawn.status(&m_message, &m_issuccess);
        }

        bool started = m_spawn.start(exe, workingDir, argv);

        if (not started) {
            throw utils::ArgError(fmt(_("Failed to start `%1%'")) % exe);
        }
    }

    void refreshPath()
    {
        m_pkgbinarypath.clear();
        m_pkgsourcepath.clear();

        if (not m_pkgname.empty()) {
            fs::path  path_binary = Path::path().getBinaryPackagesDir();
            path_binary /= m_pkgname;
            m_pkgbinarypath = path_binary.string();
            fs::path path_source_dir = Path::path().getCurrentDir();
            fs::path path_source_pkg = path_source_dir;
            path_source_pkg /= m_pkgname;
            if (fs::exists(path_source_pkg) ||
                fs::exists(path_source_dir)) {
                m_pkgsourcepath = path_source_pkg.string();
            }
        }
    }

    void select(const std::string& name)
    {
        m_pkgname = name;
        refreshPath();
    }
};


Package::Package(const std::string& pkgname)
    : m_pimpl(new Package::Pimpl(pkgname))
{
}

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
    if (fs::exists(getDir(PKG_SOURCE))) {
        throw utils::FileError( fmt(_("Pkg create error: "
                "the directory %1% already exists")) % getDir(PKG_SOURCE));
    }
    refreshPath();
    fs::create_directory(getDir(PKG_SOURCE));
    Path::path().copyTemplate("package",getDir(PKG_SOURCE));
    m_pimpl->m_strout.append(_("Package creating - done\n"));
}

void Package::configure()
{
    std::string pkg_buildir = getBuildDir(PKG_SOURCE);

    if (m_pimpl->mCommandConfigure.empty()) {
        refreshCommands();
    }
    if (pkg_buildir.empty()) {
        refreshPath();
    }
    if (m_pimpl->mCommandConfigure.empty() ||
            pkg_buildir.empty()) {
        throw utils::FileError(_("Pkg configure error: empty command"));
    }
    if (pkg_buildir.empty()) {
        throw utils::FileError(
                _("Pkg configure error: building directory path is empty"));
    }
    if (not fs::exists(pkg_buildir)) {
        fs::create_directories(pkg_buildir);
    }
    fs::path old_dir = fs::current_path();
    fs::current_path(pkg_buildir);
    std::vector < std::string > argv;
    std::string exe, cmd;
    std::string pkg_binarydir = getDir(PKG_BINARY);
    cmd = (vle::fmt(m_pimpl->mCommandConfigure) % pkg_binarydir).str();
    buildCommandLine(cmd, exe, argv);
    try {
        m_pimpl->process(exe, pkg_buildir, argv);
    } catch(const std::exception& e) {
        fs::current_path(old_dir);
        throw utils::InternalError(fmt(
                _("Pkg configure error: %1%")) % e.what());
    }
    fs::current_path(old_dir);
}

void Package::test()
{

    std::string pkg_buildir = getBuildDir(PKG_SOURCE);
    if (m_pimpl->mCommandBuild.empty()) {
        refreshCommands();
    }
    if (pkg_buildir.empty()) {
        refreshPath();
    }
    if (m_pimpl->mCommandBuild.empty() ||
            pkg_buildir.empty()) {
        throw utils::FileError(_("Pkg test error: empty command"));
    }
    if (pkg_buildir.empty()) {
        throw utils::FileError(
                _("Pkg test error: building directory path is empty"));
    }
    if (not fs::exists(pkg_buildir)) {
        throw utils::FileError(
                fmt(_("Pkg test error: building directory '%1%' "
                        "does not exist ")) % pkg_buildir.c_str());
    }
    fs::path old_dir = fs::current_path();
    fs::current_path(pkg_buildir);

    std::vector < std::string > argv;
    std::string exe, cmd;

    cmd = (vle::fmt(m_pimpl->mCommandTest) % pkg_buildir).str();
    buildCommandLine(cmd, exe, argv);

    try {
        m_pimpl->process(exe, pkg_buildir, argv);
    } catch (const std::exception& e) {
        fs::current_path(old_dir);
        throw utils::InternalError(
            fmt(_("Pkg error: test launch failed %1%")) % e.what());
    }
    fs::current_path(old_dir);
}

void Package::build()
{
    std::string pkg_buildir = getBuildDir(PKG_SOURCE);
    if (m_pimpl->mCommandBuild.empty()) {
        refreshCommands();
    }
    if (pkg_buildir.empty()) {
        refreshPath();
    }
    if (m_pimpl->mCommandBuild.empty() ||
            pkg_buildir.empty()) {
        throw utils::FileError(_("Pkg build error: empty command"));
    }
    if (pkg_buildir.empty()) {
        throw utils::FileError(
                _("Pkg build error: building directory path is empty"));
    }
    if (not fs::exists(pkg_buildir)) {
        configure();
    }
    fs::path old_dir = fs::current_path();
    fs::current_path(pkg_buildir);

    std::vector < std::string > argv;
    std::string exe, cmd;

    cmd = (vle::fmt(m_pimpl->mCommandBuild) % pkg_buildir).str();
    buildCommandLine(cmd, exe, argv);

    try {
        m_pimpl->process(exe, pkg_buildir, argv);
    } catch(const std::exception& e) {
        fs::current_path(old_dir);
        throw utils::InternalError(fmt(
                _("Pkg build error: build failed %1%")) % e.what());
    }
    fs::current_path(old_dir);
}

void Package::install()
{

    std::string pkg_buildir = getBuildDir(PKG_SOURCE);
    if (m_pimpl->mCommandBuild.empty()) {
        refreshCommands();
    }
    if (pkg_buildir.empty()) {
        refreshPath();
    }
    if (m_pimpl->mCommandBuild.empty() ||
            pkg_buildir.empty()) {
        throw utils::FileError(_("Pkg install error: empty command"));
    }
    if (pkg_buildir.empty()) {
        throw utils::FileError(
                _("Pkg install error: building directory path is empty"));
    }
    if (not fs::exists(pkg_buildir)) {
        throw utils::FileError(
                fmt(_("Pkg install error: building directory '%1%' "
                        "does not exist ")) % pkg_buildir.c_str());
    }
    fs::path old_dir = fs::current_path();
    fs::current_path(pkg_buildir);

    std::vector < std::string > argv;
    std::string exe, cmd;

    cmd = (vle::fmt(m_pimpl->mCommandInstall) % pkg_buildir).str();
    buildCommandLine(cmd, exe, argv);

    fs::path builddir = pkg_buildir;

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

    try {
        m_pimpl->process(exe, pkg_buildir, argv);
    } catch(const std::exception& e) {
        fs::current_path(old_dir);
        throw utils::InternalError(
            fmt(_("Pkg build error: install lib failed %1%")) % e.what());
    }
    fs::current_path(old_dir);
}

void Package::clean()
{
    fs::path pkg_buildir = getBuildDir(PKG_SOURCE);
    if (fs::exists(pkg_buildir)) {
        fs::remove_all(pkg_buildir);
        fs::remove(pkg_buildir);
    }
}

void Package::rclean()
{
    fs::path pkg_buildir = getDir(PKG_BINARY);
    if (fs::exists(pkg_buildir)) {
        fs::remove_all(pkg_buildir);
        fs::remove(pkg_buildir);
    }
}

void Package::pack()
{
    std::string pkg_buildir = getBuildDir(PKG_SOURCE);
    if (m_pimpl->mCommandBuild.empty()) {
        refreshCommands();
    }
    if (pkg_buildir.empty()) {
        refreshPath();
        pkg_buildir = getBuildDir(PKG_SOURCE);
    }
    if (m_pimpl->mCommandBuild.empty()) {
        throw utils::FileError(_("Pkg packaging error: empty command"));
    }
    if (pkg_buildir.empty()) {
        throw utils::FileError(_("Pkg packaging error: "
                "no building directory found"));
    }

    fs::create_directory(pkg_buildir);

    std::vector < std::string > argv;
    std::string exe, cmd;

    cmd = (vle::fmt(m_pimpl->mCommandPack) % pkg_buildir).str();
    buildCommandLine(cmd, exe, argv);

    try {
        m_pimpl->process(exe, pkg_buildir, argv);
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
            error.clear();

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

void Package::select(const std::string& name)
{
    m_pimpl->select(name);
}

void Package::remove(const std::string& toremove, VLE_PACKAGE_TYPE type)
{
    std::string pkg_dir = getDir(type);
    fs::path torm(pkg_dir);
    torm /= toremove;

    if (fs::exists(torm)) {
        fs::remove_all(torm);
        fs::remove(torm);
    }

}

std::string Package::getParentDir(VLE_PACKAGE_TYPE type) const
{
    std::string base_dir = getDir(type);
    if (base_dir.empty()){
        return "";
    } else {
        fs::path base_path = fs::path(base_dir);
        return base_path.branch_path().string();
    }
}

std::string Package::getDir(VLE_PACKAGE_TYPE type) const
{
    switch (type) {
    case PKG_SOURCE:
        return m_pimpl->m_pkgsourcepath;
        break;
    case PKG_BINARY:
        return m_pimpl->m_pkgbinarypath;
        break;
    }
    return "";
}

std::string Package::getLibDir(VLE_PACKAGE_TYPE type) const
{
    return utils::Path::buildDirname(getDir(type), "lib");
}

std::string Package::getSrcDir(VLE_PACKAGE_TYPE type) const
{
    return utils::Path::buildDirname(getDir(type), "src");
}

std::string Package::getDataDir(VLE_PACKAGE_TYPE type) const
{
    return utils::Path::buildDirname(getDir(type), "data");
}

std::string Package::getDocDir(VLE_PACKAGE_TYPE type) const
{
    return utils::Path::buildDirname(getDir(type), "doc");
}

std::string Package::getExpDir(VLE_PACKAGE_TYPE type) const
{
    return utils::Path::buildDirname(getDir(type), "exp");
}

std::string Package::getBuildDir(VLE_PACKAGE_TYPE type) const
{
    return utils::Path::buildDirname(getDir(type), "buildvle");
}

std::string Package::getOutputDir(VLE_PACKAGE_TYPE type) const
{
    return utils::Path::buildDirname(getDir(type), "output");
}

std::string Package::getPluginDir(VLE_PACKAGE_TYPE type) const
{
    return utils::Path::buildDirname(getDir(type), "plugins");
}

std::string Package::getPluginSimulatorDir(VLE_PACKAGE_TYPE type) const
{
    return utils::Path::buildDirname(getDir(type), "plugins", "simulator");
}

std::string Package::getPluginOutputDir(VLE_PACKAGE_TYPE type) const
{
    return utils::Path::buildDirname(getDir(type), "plugins", "output");
}

std::string Package::getPluginGvleGlobalDir(VLE_PACKAGE_TYPE type) const
{
    return utils::Path::buildDirname(getDir(type), "plugins", "gvle", "global");
}

std::string Package::getPluginGvleModelingDir(VLE_PACKAGE_TYPE type) const
{
    return utils::Path::buildDirname(getDir(type), "plugins", "gvle",
            "modeling");
}

std::string Package::getPluginGvleOutputDir(VLE_PACKAGE_TYPE type) const
{
    return utils::Path::buildDirname(getDir(type), "plugins", "gvle", "output");
}

std::string Package::getFile(const std::string& file,
        VLE_PACKAGE_TYPE type) const
{
    return utils::Path::buildDirname(getDir(type), file);
}

std::string Package::getLibFile(const std::string& file,
        VLE_PACKAGE_TYPE type) const
{
    return utils::Path::buildDirname(getDir(type), "lib", file);
}

std::string Package::getSrcFile(const std::string& file,
        VLE_PACKAGE_TYPE type) const
{
    return utils::Path::buildDirname(getDir(type), "src", file);
}

std::string Package::getDataFile(const std::string& file,
        VLE_PACKAGE_TYPE type) const
{
    return utils::Path::buildDirname(getDir(type), "data", file);
}

std::string Package::getDocFile(const std::string& file,
        VLE_PACKAGE_TYPE type) const
{
    return utils::Path::buildDirname(getDir(type), "doc", file);
}

std::string Package::getExpFile(const std::string& file,
        VLE_PACKAGE_TYPE type) const
{
    return utils::Path::buildDirname(getDir(type), "exp", file);
}

std::string Package::getOutputFile(const std::string& file,
        VLE_PACKAGE_TYPE type) const
{
    return utils::Path::buildDirname(getDir(type), "output", file);
}

std::string Package::getPluginFile(const std::string& file,
        VLE_PACKAGE_TYPE type) const
{
    return utils::Path::buildDirname(getDir(type), "plugins", file);
}

std::string Package::getPluginSimulatorFile(const std::string& file,
        VLE_PACKAGE_TYPE type) const
{
    return utils::Path::buildDirname(getDir(type), "plugins", "simulator",
            file);
}

std::string Package::getPluginOutputFile(const std::string& file,
        VLE_PACKAGE_TYPE type) const
{
    return utils::Path::buildDirname(getDir(type), "plugins", "output", file);
}

std::string Package::getPluginGvleModelingFile(const std::string& file,
        VLE_PACKAGE_TYPE type) const
{
    return utils::Path::buildDirname(
        getDir(type), "plugins", "gvle", "modeling", file);
}

std::string Package::getPluginGvleOutputFile(const std::string& file,
        VLE_PACKAGE_TYPE type) const
{
    return utils::Path::buildDirname(
        getDir(type), "plugins", "gvle", "output", file);
}

bool Package::existsBinary() const
{
    fs::path binary_dir = m_pimpl->m_pkgbinarypath;
    return (fs::exists(binary_dir) && fs::is_directory(binary_dir));
}

bool Package::existsSource() const
{
    return (not m_pimpl->m_pkgsourcepath.empty());
}

bool Package::existsFile(const std::string& path, VLE_PACKAGE_TYPE type)
{
    std::string base_dir = getDir(type);
    if (not base_dir.empty()) {
        fs::path tmp(base_dir);
        tmp /= path;
        return fs::exists(tmp) && fs::is_regular_file(tmp);
    }
    return false;
}

void Package::addDirectory(const std::string& path, const std::string& name,
        VLE_PACKAGE_TYPE type)
{
    std::string base_dir = getDir(type);
    if (not base_dir.empty()) {
        fs::path tmp(base_dir);
        tmp /= path;
        tmp /= name;
        if (not fs::exists(tmp)) {
            fs::create_directory(tmp);
        }
    }
}

PathList Package::getExperiments(VLE_PACKAGE_TYPE type) const
{
    fs::path pkg(getExpDir(type));
    if (not fs::exists(pkg) or not fs::is_directory(pkg)) {
        throw utils::InternalError(fmt(
                _("Pkg list error: '%1%' is not an experiments directory")) %
#if BOOST_VERSION > 104500
            pkg.string());
#else
            pkg.file_string());
#endif
    }

    PathList result;
    std::stack < fs::path > stack;
    stack.push(pkg);

    while (not stack.empty()) {
        fs::path dir = stack.top();
        stack.pop();

        for (fs::directory_iterator it(dir), end; it != end; ++it) {
#if BOOST_VERSION > 104500
            if (fs::is_regular_file(it->status())) {
                std::string ext = it->path().extension().string();
#elif BOOST_VERSION > 103600
            if (fs::is_regular_file(it->status())) {
                fs::path::string_type ext = it->path().extension();
#else
            if (fs::is_regular(it->status())) {
                fs::path::string_type ext = fs::extension(it->path());
#endif
                if (ext == ".vpz") {
#if BOOST_VERSION > 104500
                    result.push_back(it->path().string());
#else
                    result.push_back(it->path().file_string());
#endif
                }
            } else if (fs::is_directory(it->status())) {
                stack.push(it->path());
            }
        }
    }
    return result;
}

PathList Package::listLibraries(const std::string& path) const
{
    PathList result;
    fs::path simdir(path);
    if (fs::exists(simdir) and fs::is_directory(simdir)) {
        std::stack < fs::path > stack;
        stack.push(simdir);
        while (not stack.empty()) {
            fs::path dir = stack.top();
            stack.pop();
            for (fs::directory_iterator it(dir), end; it != end; ++it) {
#if BOOST_VERSION > 104500
                if (fs::is_regular_file(it->status())) {
                    std::string ext = it->path().extension().string();
#elif BOOST_VERSION > 103600
                if (fs::is_regular_file(it->status())) {
                    fs::path::string_type ext = it->path().extension();
#else
                if (fs::is_regular(it->status())) {
                    fs::path::string_type ext = fs::extension(it->path());
#endif

#if defined _WIN32
                    if (ext == ".dll") {
#elif defined __APPLE__
                    if (ext == ".dylib") {
#else
                    if (ext == ".so") {
#endif

#if BOOST_VERSION > 104500
                        result.push_back(it->path().string());
#else
                        result.push_back(it->path().file_string());
#endif
                    } else if (fs::is_directory(it->status())) {
                        stack.push(it->path());
                    }
                }
            }
       }
   }
   return result;
}

PathList Package::getPluginsSimulator() const
{
    return listLibraries(getPluginSimulatorDir(PKG_BINARY));
}

PathList Package::getPluginsOutput() const
{
    return listLibraries(getPluginOutputDir(PKG_BINARY));
}

PathList Package::getPluginsGvleGlobal() const
{
    return listLibraries(getPluginGvleGlobalDir(PKG_BINARY));
}

PathList Package::getPluginsGvleModeling() const
{
    return listLibraries(getPluginGvleModelingDir(PKG_BINARY));
}

PathList Package::getPluginsGvleOutput() const
{
    return listLibraries(getPluginGvleOutputDir(PKG_BINARY));
}

std::string Package::rename(const std::string& oldname,
        const std::string& newname,
        VLE_PACKAGE_TYPE type)
{
    fs::path oldfilepath = getDir(type);
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
    return m_pimpl->m_pkgname;
}

void Package::refreshCommands()
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

void Package::refreshPath()
{
    m_pimpl->refreshPath();
}

void Package::fillBinaryContent(std::vector<std::string>& pkgcontent)
{
    std::string header = "Package content from: ";
    header += getDir(vle::utils::PKG_BINARY);

    pkgcontent.clear();
    pkgcontent.push_back(header);

    vle::utils::PathList tmp;

    tmp = getExperiments();
    pkgcontent.push_back("-- experiments : ");
    std::sort(tmp.begin(), tmp.end());
    std::vector<std::string>::const_iterator itb = tmp.begin();
    std::vector<std::string>::const_iterator ite = tmp.end();
    for (; itb!=ite; itb++){
        pkgcontent.push_back(*itb);
    }

    tmp = getPluginsSimulator();
    pkgcontent.push_back("-- simulator plugins : ");
    std::sort(tmp.begin(), tmp.end());
    itb = tmp.begin();
    ite = tmp.end();
    for (; itb!=ite; itb++){
        pkgcontent.push_back(*itb);
    }

    tmp = getPluginsOutput();
    pkgcontent.push_back("-- output plugins : ");
    std::sort(tmp.begin(), tmp.end());
    itb = tmp.begin();
    ite = tmp.end();
    for (; itb!=ite; itb++){
        pkgcontent.push_back(*itb);
    }

    tmp = getPluginsGvleGlobal();
    pkgcontent.push_back("-- gvle global plugins : ");
    std::sort(tmp.begin(), tmp.end());
    itb = tmp.begin();
    ite = tmp.end();
    for (; itb!=ite; itb++){
        pkgcontent.push_back(*itb);
    }

    tmp = getPluginsGvleModeling();
    pkgcontent.push_back("-- gvle modeling plugins : ");
    std::sort(tmp.begin(), tmp.end());
    itb = tmp.begin();
    ite = tmp.end();
    for (; itb!=ite; itb++){
        pkgcontent.push_back(*itb);
    }

    tmp = getPluginsGvleOutput();
    pkgcontent.push_back("-- gvle output plugins : ");
    std::sort(tmp.begin(), tmp.end());
    itb = tmp.begin();
    ite = tmp.end();
    for (; itb!=ite; itb++){
        pkgcontent.push_back(*itb);
    }
    return;
}

VLE_API std::ostream& operator<<(std::ostream& out,
        const VLE_PACKAGE_TYPE& type)
{
    switch (type) {
    case PKG_SOURCE:
        out << "SOURCE";
        break;
    case PKG_BINARY:
        out << "BINARY";
        break;
    }
    return out;
}

std::ostream& operator<<(std::ostream& out, const Package& pkg)
{
    out << "\npackage name.....: " << pkg.m_pimpl->m_pkgname << "\n"
        << "source path........: " << pkg.m_pimpl->m_pkgsourcepath << "\n"
        << "binary path........: " << pkg.m_pimpl->m_pkgbinarypath << "\n"
        << "exists binary......: " << pkg.existsBinary() << "\n"
        << "\n";
    return out;
}

}} // namespace vle utils
