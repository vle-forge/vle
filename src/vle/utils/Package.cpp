/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2017 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2017 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2017 INRA http://www.inra.fr
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

#include <boost/cast.hpp>
#include <boost/format.hpp>
#include <cstring>
#include <fstream>
#include <ostream>
#include <stack>
#include <thread>
#include <vle/utils/Context.hpp>
#include <vle/utils/ContextPrivate.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Filesystem.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Spawn.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/i18n.hpp>

namespace {

void
remove_all(vle::utils::ContextPtr ctx, const vle::utils::Path& path)
{
    if (not path.exists())
        return;

    if (path.is_file())
        path.remove();

    std::string command;
    ctx->get_setting("vle.command.dir.remove", &command);

    try {
        command = (boost::format(command) % path.string()).str();

        vle::utils::Spawn spawn(ctx);
        auto argv = spawn.splitCommandLine(command);
        auto exe = std::move(argv.front());
        argv.erase(argv.begin());

        if (not spawn.start(
              exe, vle::utils::Path::current_path().string(), argv))
            throw vle::utils::InternalError(_("fail to start cmake command"));

        spawn.wait();

        std::string message;
        bool success;

        spawn.status(&message, &success);

        if (not success && not message.empty())
            vErr(ctx,
                 _("Error during remove directory operation: %s\n"),
                 message.c_str());
    } catch (const std::exception& e) {
        vErr(ctx,
             _("Package remove all: unable to remove `%s' with"
               " the `%s' command\n"),
             path.string().c_str(),
             command.c_str());
    }
}

} // namespace anonymous

namespace vle {
namespace utils {

struct Package::Pimpl
{
    Pimpl(ContextPtr context)
      : m_context(context)
      , m_spawn(context)
    {
    }

    Pimpl(ContextPtr context, std::string pkgname)
      : m_context(context)
      , m_pkgname(std::move(pkgname))
      , m_spawn(context)
    {
        refreshPath();
    }

    ~Pimpl() = default;

    ContextPtr m_context;

    std::string m_pkgname;
    std::string m_pkgbinarypath;
    std::string m_pkgsourcepath;

    Spawn m_spawn;
    std::string m_message;
    std::string m_strout;
    std::string m_strerr;
    std::string mCommandConfigure;
    std::string mCommandTest;
    std::string mCommandBuild;
    std::string mCommandInstall;
    std::string mCommandClean;
    std::string mCommandPack;
    std::string mCommandUnzip;
    std::string mCommandDirCopy;
    bool m_issuccess;

    void process(const std::string& exe,
                 const std::string& workingDir,
                 const std::vector<std::string>& argv)
    {
        if (m_spawn.isstart() and not m_spawn.isfinish()) {
            vDbg(m_context,
                 _("-[%s] Need to wait old process before\n"),
                 exe.c_str());

            m_spawn.wait();
            m_spawn.status(&m_message, &m_issuccess);
        }

        if (not m_spawn.start(exe, workingDir, argv))
            throw utils::ArgError(
              (fmt(_("Failed to start `%1%'")) % exe).str());
    }

    void refreshPath()
    {
        m_pkgbinarypath.clear();
        m_pkgsourcepath.clear();

        if (not m_pkgname.empty()) {
            Path path_binary = m_context->getBinaryPackagesDir()[0];
            path_binary /= m_pkgname;
            m_pkgbinarypath = path_binary.string();
            Path path_source_dir = Path::current_path();
            Path path_source_pkg = path_source_dir;
            path_source_pkg /= m_pkgname;
            if (path_source_pkg.exists() or path_source_dir.exists()) {
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

Package::Package(ContextPtr context)
  : m_pimpl(std::make_unique<Package::Pimpl>(context))
{
}

Package::Package(ContextPtr context, const std::string& pkgname)
  : m_pimpl(std::make_unique<Package::Pimpl>(context, pkgname))
{
}

Package::~Package() = default;

void
Package::create()
{
    if (m_pimpl->mCommandDirCopy.empty())
        refreshCommands();

    if (m_pimpl->mCommandDirCopy.empty())
        throw utils::FileError(_("Pkg configure error: empty command"));

    Path source(m_pimpl->m_context->getTemplate("package"));
    Path destination(getDir(PKG_SOURCE));

    if (destination.exists())
        throw utils::FileError((fmt(_("Pkg create error: "
                                      "the directory %1% already exists")) %
                                destination.string())
                                 .str());

    Path::create_directory(destination);
    std::string command = (vle::fmt(m_pimpl->mCommandDirCopy) %
                           source.string() % destination.string())
                            .str();

    utils::Spawn spawn(m_pimpl->m_context);
    std::vector<std::string> argv = spawn.splitCommandLine(command);
    std::string exe = std::move(argv.front());
    argv.erase(argv.begin());

    try {
        m_pimpl->process(exe, source.string(), argv);
    } catch (const std::exception& e) {
        throw utils::InternalError(
          (fmt(_("Pkg create error: %1%")) % e.what()).str());
    }
}

void
Package::configure()
{
    std::string pkg_buildir = getBuildDir(PKG_SOURCE);

    if (m_pimpl->mCommandConfigure.empty())
        refreshCommands();

    if (pkg_buildir.empty())
        refreshPath();

    if (m_pimpl->mCommandConfigure.empty() || pkg_buildir.empty())
        throw utils::FileError(_("Pkg configure error: empty command"));

    if (pkg_buildir.empty())
        throw utils::FileError(
          _("Pkg configure error: building directory path is empty"));

    Path path{ pkg_buildir };
    if (not path.exists()) {
        if (not Path::create_directories(path))
            throw utils::FileError(
              _("Pkg configure error: fails to build directories"));
    }

    std::string pkg_binarydir = getDir(PKG_BINARY);
    std::string cmd = (fmt(m_pimpl->mCommandConfigure) % pkg_binarydir).str();
    Spawn spawn(m_pimpl->m_context);
    std::vector<std::string> argv = spawn.splitCommandLine(cmd);
    std::string exe = std::move(argv.front());
    argv.erase(argv.begin());
    try {
        m_pimpl->process(exe, pkg_buildir, argv);
    } catch (const std::exception& e) {
        throw utils::InternalError(
          (fmt(_("Pkg configure error: %1%")) % e.what()).str());
    }
}

void
Package::test()
{
    std::string pkg_buildir = getBuildDir(PKG_SOURCE);
    if (m_pimpl->mCommandBuild.empty()) {
        refreshCommands();
    }
    if (pkg_buildir.empty()) {
        refreshPath();
    }
    if (m_pimpl->mCommandBuild.empty() || pkg_buildir.empty()) {
        throw utils::FileError(_("Pkg test error: empty command"));
    }
    if (pkg_buildir.empty()) {
        throw utils::FileError(
          _("Pkg test error: building directory path is empty"));
    }

    Path path{ pkg_buildir };
    if (not path.exists())
        throw utils::FileError(
          (fmt(_("Pkg test error: building directory '%1%' "
                 "does not exist ")) %
           pkg_buildir)
            .str());

    std::string cmd = (fmt(m_pimpl->mCommandTest) % pkg_buildir).str();
    Spawn spawn(m_pimpl->m_context);
    std::vector<std::string> argv = spawn.splitCommandLine(cmd);
    std::string exe = std::move(argv.front());
    argv.erase(argv.begin());

    try {
        m_pimpl->process(exe, pkg_buildir, argv);
    } catch (const std::exception& e) {
        throw utils::InternalError(
          (fmt(_("Pkg error: test launch failed %1%")) % e.what()).str());
    }
}

void
Package::build()
{
    std::string pkg_buildir = getBuildDir(PKG_SOURCE);
    if (m_pimpl->mCommandBuild.empty()) {
        refreshCommands();
    }

    if (pkg_buildir.empty()) {
        refreshPath();
    }
    if (m_pimpl->mCommandBuild.empty() || pkg_buildir.empty()) {
        throw utils::FileError(_("Pkg build error: empty command"));
    }
    if (pkg_buildir.empty()) {
        throw utils::FileError(
          _("Pkg build error: building directory path is empty"));
    }

    Path path{ pkg_buildir };
    if (not path.exists())
        configure();

    std::string cmd = (vle::fmt(m_pimpl->mCommandBuild) % pkg_buildir).str();
    Spawn spawn(m_pimpl->m_context);
    std::vector<std::string> argv = spawn.splitCommandLine(cmd);
    std::string exe = std::move(argv.front());
    argv.erase(argv.begin());

    try {
        m_pimpl->process(exe, pkg_buildir, argv);
    } catch (const std::exception& e) {
        throw utils::InternalError(
          (fmt(_("Pkg build error: build failed %1%")) % e.what()).str());
    }
}

void
Package::install()
{

    std::string pkg_buildir = getBuildDir(PKG_SOURCE);
    if (m_pimpl->mCommandBuild.empty()) {
        refreshCommands();
    }
    if (pkg_buildir.empty()) {
        refreshPath();
    }
    if (m_pimpl->mCommandBuild.empty() || pkg_buildir.empty()) {
        throw utils::FileError(_("Pkg install error: empty command"));
    }
    if (pkg_buildir.empty()) {
        throw utils::FileError(
          _("Pkg install error: building directory path is empty"));
    }

    Path path{ pkg_buildir };
    if (not path.exists())
        throw utils::FileError(
          (fmt(_("Pkg install error: building directory '%1%' "
                 "does not exist ")) %
           pkg_buildir.c_str())
            .str());

    std::string cmd = (vle::fmt(m_pimpl->mCommandInstall) % pkg_buildir).str();
    Spawn spawn(m_pimpl->m_context);
    std::vector<std::string> argv = spawn.splitCommandLine(cmd);
    std::string exe = std::move(argv.front());
    argv.erase(argv.begin());

    Path builddir = pkg_buildir;

    if (not builddir.exists()) {
        throw utils::ArgError(
          (fmt(_("Pkg build error: directory '%1%' does not exist")) %
           builddir.string())
            .str());
    }

    if (not builddir.is_directory()) {
        throw utils::ArgError(
          (fmt(_("Pkg build error: '%1%' is not a directory")) %
           builddir.string())
            .str());
    }

    try {
        m_pimpl->process(exe, pkg_buildir, argv);
    } catch (const std::exception& e) {
        throw utils::InternalError(
          (fmt(_("Pkg build error: install lib failed %1%")) % e.what())
            .str());
    }
}

void
Package::clean()
{
    Path pkg_buildir = getBuildDir(PKG_SOURCE);

    if (pkg_buildir.exists())
        ::remove_all(m_pimpl->m_context, pkg_buildir);
}

void
Package::rclean()
{
    Path pkg_buildir = getDir(PKG_BINARY);

    if (pkg_buildir.exists())
        ::remove_all(m_pimpl->m_context, pkg_buildir);
}

void
Package::pack()
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

    Path::create_directory(pkg_buildir);

    std::string cmd = (fmt(m_pimpl->mCommandPack) % pkg_buildir).str();
    Spawn spawn(m_pimpl->m_context);
    std::vector<std::string> argv = spawn.splitCommandLine(cmd);
    std::string exe = std::move(argv.front());
    argv.erase(argv.begin());

    try {
        m_pimpl->process(exe, pkg_buildir, argv);
    } catch (const std::exception& e) {
        throw utils::InternalError(
          (fmt(_("Pkg packaging error: package failed %1%")) % e.what())
            .str());
    }
}

bool
Package::isFinish() const
{
    return m_pimpl->m_spawn.isfinish();
}

bool
Package::isSuccess() const
{
    if (not m_pimpl->m_spawn.isfinish()) {
        return false;
    }

    m_pimpl->m_spawn.status(&m_pimpl->m_message, &m_pimpl->m_issuccess);

    return m_pimpl->m_issuccess;
}

bool
Package::wait(std::ostream& out, std::ostream& err)
{
    std::string output;
    std::string error;

    output.reserve(Spawn::default_buffer_size);
    error.reserve(Spawn::default_buffer_size);

    while (not m_pimpl->m_spawn.isfinish()) {
        if (m_pimpl->m_spawn.get(&output, &error)) {
            if (not output.empty()) {
                out << output;
                output.clear();
            }

            if (not error.empty()) {
                err << error;
                error.clear();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        } else {
            break;
        }
    }

    m_pimpl->m_spawn.wait();

    //
    // Force to get the latest spawn message. Indeed, spawn process can be
    // finished but the output and error buffers may not clean.
    //

    if (m_pimpl->m_spawn.get(&output, &error)) {
        if (not output.empty()) {
            out << output;
            output.clear();
        }

        if (not error.empty()) {
            err << error;
            error.clear();
        }
    }

    return m_pimpl->m_spawn.status(&m_pimpl->m_message, &m_pimpl->m_issuccess);
}

bool
Package::get(std::string* out, std::string* err)
{
    out->reserve(Spawn::default_buffer_size);
    err->reserve(Spawn::default_buffer_size);

    m_pimpl->m_spawn.get(out, err);
    return true;
}

void
Package::select(const std::string& name)
{
    m_pimpl->select(name);
}

void
Package::remove(const std::string& toremove, VLE_PACKAGE_TYPE type)
{
    std::string pkg_dir = getDir(type);
    Path torm(pkg_dir);
    torm /= toremove;

    if (torm.exists())
        ::remove_all(m_pimpl->m_context, torm);
}

std::string
Package::getParentDir(VLE_PACKAGE_TYPE type) const
{
    std::string base_dir = getDir(type);
    if (base_dir.empty()) {
        return "";
    } else {
        Path base_path = Path(base_dir);
        return base_path.parent_path().string();
    }
}

std::string
Package::getDir(VLE_PACKAGE_TYPE type) const
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

std::string
Package::getLibDir(VLE_PACKAGE_TYPE type) const
{
    Path p(getDir(type));
    p /= "lib";

    return p.string();
}

std::string
Package::getSrcDir(VLE_PACKAGE_TYPE type) const
{
    Path p(getDir(type));
    p /= "src";

    return p.string();
}

std::string
Package::getDataDir(VLE_PACKAGE_TYPE type) const
{
    Path p(getDir(type));
    p /= "data";

    return p.string();
}

std::string
Package::getDocDir(VLE_PACKAGE_TYPE type) const
{
    Path p(getDir(type));
    p /= "doc";

    return p.string();
}

std::string
Package::getExpDir(VLE_PACKAGE_TYPE type) const
{
    Path p(getDir(type));
    p /= "exp";

    return p.string();
}

std::string
Package::getBuildDir(VLE_PACKAGE_TYPE type) const
{
    Path p(getDir(type));
    p /= "buildvle";

    return p.string();
}

std::string
Package::getOutputDir(VLE_PACKAGE_TYPE type) const
{
    Path p(getDir(type));
    p /= "output";

    return p.string();
}

std::string
Package::getPluginDir(VLE_PACKAGE_TYPE type) const
{
    Path p(getDir(type));
    p /= "plugins";

    return p.string();
}

std::string
Package::getPluginSimulatorDir(VLE_PACKAGE_TYPE type) const
{
    Path p(getDir(type));
    p /= "plugins";
    p /= "simulator";

    return p.string();
}

std::string
Package::getPluginOutputDir(VLE_PACKAGE_TYPE type) const
{
    Path p(getDir(type));
    p /= "plugins";
    p /= "output";

    return p.string();
}

std::string
Package::getPluginGvleGlobalDir(VLE_PACKAGE_TYPE type) const
{
    Path p(getDir(type));
    p /= "plugins";
    p /= "gvle";
    p /= "global";

    return p.string();
}

std::string
Package::getPluginGvleModelingDir(VLE_PACKAGE_TYPE type) const
{
    Path p(getDir(type));
    p /= "plugins";
    p /= "gvle";
    p /= "modeling";

    return p.string();
}

std::string
Package::getPluginGvleOutputDir(VLE_PACKAGE_TYPE type) const
{
    Path p(getDir(type));
    p /= "plugins";
    p /= "gvle";
    p /= "output";

    return p.string();
}

std::string
Package::getFile(const std::string& file, VLE_PACKAGE_TYPE type) const
{
    Path p(getDir(type));
    p /= file;

    return p.string();
}

std::string
Package::getLibFile(const std::string& file, VLE_PACKAGE_TYPE type) const
{
    Path p(getDir(type));
    p /= "lib";
    p /= file;

    return p.string();
}

std::string
Package::getSrcFile(const std::string& file, VLE_PACKAGE_TYPE type) const
{
    Path p(getDir(type));
    p /= "src";
    p /= file;

    return p.string();
}

std::string
Package::getDataFile(const std::string& file, VLE_PACKAGE_TYPE type) const
{
    Path p(getDir(type));
    p /= "data";
    p /= file;

    return p.string();
}

std::string
Package::getDocFile(const std::string& file, VLE_PACKAGE_TYPE type) const
{
    Path p(getDir(type));
    p /= "doc";
    p /= file;

    return p.string();
}

std::string
Package::getExpFile(const std::string& file, VLE_PACKAGE_TYPE type) const
{
    Path p(getDir(type));
    p /= "exp";
    p /= file;

    return p.string();
}

std::string
Package::getOutputFile(const std::string& file, VLE_PACKAGE_TYPE type) const
{
    Path p(getDir(type));
    p /= "output";
    p /= file;

    return p.string();
}

std::string
Package::getPluginFile(const std::string& file, VLE_PACKAGE_TYPE type) const
{
    Path p(getDir(type));
    p /= "plugins";
    p /= file;

    return p.string();
}

std::string
Package::getPluginSimulatorFile(const std::string& file,
                                VLE_PACKAGE_TYPE type) const
{
    Path p(getDir(type));
    p /= "plugins";
    p /= "simulator";
    p /= file;

    return p.string();
}

std::string
Package::getPluginOutputFile(const std::string& file,
                             VLE_PACKAGE_TYPE type) const
{
    Path p(getDir(type));
    p /= "plugins";
    p /= "output";
    p /= file;

    return p.string();
}

std::string
Package::getPluginGvleModelingFile(const std::string& file,
                                   VLE_PACKAGE_TYPE type) const
{
    Path p(getDir(type));
    p /= "plugins";
    p /= "gvle";
    p /= "modeling";
    p /= file;

    return p.string();
}

std::string
Package::getPluginGvleOutputFile(const std::string& file,
                                 VLE_PACKAGE_TYPE type) const
{
    Path p(getDir(type));
    p /= "plugins";
    p /= "gvle";
    p /= "output";
    p /= file;

    return p.string();
}

bool
Package::existsBinary() const
{
    Path binary_dir = m_pimpl->m_pkgbinarypath;

    return binary_dir.is_directory();
}

bool
Package::existsSource() const
{
    return (not m_pimpl->m_pkgsourcepath.empty());
}

bool
Package::existsFile(const std::string& path, VLE_PACKAGE_TYPE type)
{
    std::string base_dir = getDir(type);
    if (not base_dir.empty()) {
        Path tmp(base_dir);
        tmp /= path;

        return tmp.is_file();
    }
    return false;
}

void
Package::addDirectory(const std::string& path,
                      const std::string& name,
                      VLE_PACKAGE_TYPE type)
{
    std::string base_dir = getDir(type);
    if (not base_dir.empty()) {
        Path tmp(base_dir);
        tmp /= path;
        tmp /= name;
        if (not tmp.exists()) {
            tmp.create_directory();
        }
    }
}

PathList
Package::getExperiments(VLE_PACKAGE_TYPE type) const
{
    Path pkg(getExpDir(type));

    if (not pkg.is_directory())
        throw utils::InternalError(
          (fmt(_("Pkg list error: '%1%' is not an experiments directory")) %
           pkg.string())
            .str());

    PathList result;
    std::stack<Path> stack;
    stack.push(pkg);

    while (not stack.empty()) {
        Path dir = stack.top();
        stack.pop();

        for (DirectoryIterator it(dir), end; it != end; ++it) {
            if (it->is_file()) {
                std::string ext = it->path().extension();
                if (ext == ".vpz") {
                    result.push_back(it->path().string());
                }
            } else if (it->is_directory()) {
                stack.push(it->path());
            }
        }
    }
    return result;
}

PathList
Package::listLibraries(const std::string& path) const
{
    PathList result;
    Path simdir(path);

    if (simdir.is_directory()) {
        std::stack<Path> stack;
        stack.push(simdir);
        while (not stack.empty()) {
            Path dir = stack.top();
            stack.pop();
            for (DirectoryIterator it(dir), end; it != end; ++it) {
                if (it->is_file()) {
                    std::string ext = it->path().extension();

#if defined _WIN32
                    if (ext == ".dll") {
#elif defined __APPLE__
                    if (ext == ".dylib") {
#else
                    if (ext == ".so") {
#endif

                        result.push_back(it->path().string());
                    } else if (it->is_directory()) {
                        stack.push(it->path());
                    }
                }
            }
        }
    }
    return result;
}

PathList
Package::getPluginsSimulator() const
{
    return listLibraries(getPluginSimulatorDir(PKG_BINARY));
}

PathList
Package::getPluginsOutput() const
{
    return listLibraries(getPluginOutputDir(PKG_BINARY));
}

PathList
Package::getPluginsGvleGlobal() const
{
    return listLibraries(getPluginGvleGlobalDir(PKG_BINARY));
}

PathList
Package::getPluginsGvleModeling() const
{
    return listLibraries(getPluginGvleModelingDir(PKG_BINARY));
}

PathList
Package::getPluginsGvleOutput() const
{
    return listLibraries(getPluginGvleOutputDir(PKG_BINARY));
}

std::string
Package::getMetadataExpDir(VLE_PACKAGE_TYPE type) const
{
    Path f = getDir(type);
    f /= "metadata";
    f /= "exp";
    return f.string();
}

std::string
Package::getMetadataExpFile(const std::string& expName,
                            VLE_PACKAGE_TYPE type) const
{
    Path f = getDir(type);
    f /= "metadata";
    f /= "exp";
    f /= (expName + ".vpm");
    return f.string();
}

std::string
Package::rename(const std::string& oldname,
                const std::string& newname,
                VLE_PACKAGE_TYPE type)
{
    Path oldfilepath = getDir(type);
    oldfilepath /= oldname;

    Path newfilepath = oldfilepath.parent_path();
    newfilepath /= newname;

    if (not oldfilepath.exists() or newfilepath.exists()) {
        throw utils::ArgError(
          (fmt(_("In Package `%1%', can not rename `%2%' in `%3%'")) % name() %
           oldfilepath.string() % newfilepath.string())
            .str());
    }

    Path::rename(oldfilepath, newfilepath);

    return newfilepath.string();
}

void
Package::copy(const std::string& source, std::string& target)
{
    Path::copy_file(source, target);
}

const std::string&
Package::name() const
{
    return m_pimpl->m_pkgname;
}

void
Package::refreshCommands()
{
    m_pimpl->m_context->get_setting("vle.packages.configure",
                                    &m_pimpl->mCommandConfigure);
    m_pimpl->m_context->get_setting("vle.packages.test",
                                    &m_pimpl->mCommandTest);
    m_pimpl->m_context->get_setting("vle.packages.build",
                                    &m_pimpl->mCommandBuild);
    m_pimpl->m_context->get_setting("vle.packages.install",
                                    &m_pimpl->mCommandInstall);
    m_pimpl->m_context->get_setting("vle.packages.clean",
                                    &m_pimpl->mCommandClean);
    m_pimpl->m_context->get_setting("vle.packages.package",
                                    &m_pimpl->mCommandPack);
    m_pimpl->m_context->get_setting("vle.packages.unzip",
                                    &m_pimpl->mCommandUnzip);
    m_pimpl->m_context->get_setting("vle.command.dir.copy",
                                    &m_pimpl->mCommandDirCopy);
}

void
Package::refreshPath()
{
    m_pimpl->refreshPath();
}

void
Package::fillBinaryContent(std::vector<std::string>& pkgcontent)
{
    auto header = vle::utils::format("Package content from: %s",
                                     getDir(vle::utils::PKG_BINARY).c_str());

    pkgcontent.clear();
    pkgcontent.emplace_back(header);

    vle::utils::PathList tmp;

    try {
        tmp = getExperiments();
        pkgcontent.push_back("-- experiments:");
        std::sort(tmp.begin(), tmp.end());

        for (auto& elem : tmp)
            pkgcontent.emplace_back(elem.string());
    } catch (const std::exception& /*e*/) {
    }

    try {
        tmp = getPluginsSimulator();
        pkgcontent.push_back("-- simulator plugins:");
        std::sort(tmp.begin(), tmp.end());

        for (auto& elem : tmp)
            pkgcontent.emplace_back(elem.string());
    } catch (const std::exception& /*e*/) {
    }

    try {
        tmp = getPluginsOutput();
        pkgcontent.push_back("-- output plugins:");
        std::sort(tmp.begin(), tmp.end());

        for (auto& elem : tmp)
            pkgcontent.emplace_back(elem.string());
    } catch (const std::exception& /*e*/) {
    }

    try {
        tmp = getPluginsGvleGlobal();
        pkgcontent.push_back("-- gvle global plugins:");
        std::sort(tmp.begin(), tmp.end());

        for (auto& elem : tmp)
            pkgcontent.emplace_back(elem.string());
    } catch (const std::exception& /*e*/) {
    }

    try {
        tmp = getPluginsGvleModeling();
        pkgcontent.push_back("-- gvle modeling plugins:");
        std::sort(tmp.begin(), tmp.end());

        for (auto& elem : tmp)
            pkgcontent.emplace_back(elem.string());
    } catch (const std::exception& /*e*/) {
    }

    try {
        tmp = getPluginsGvleOutput();
        pkgcontent.push_back("-- gvle output plugins:");
        std::sort(tmp.begin(), tmp.end());

        for (auto& elem : tmp)
            pkgcontent.emplace_back(elem.string());
    } catch (const std::exception& /*e*/) {
    }
}

VLE_API std::ostream&
operator<<(std::ostream& out, const VLE_PACKAGE_TYPE& type)
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

std::ostream&
operator<<(std::ostream& out, const Package& pkg)
{
    out << "\npackage name.....: " << pkg.m_pimpl->m_pkgname << "\n"
        << "source path........: " << pkg.m_pimpl->m_pkgsourcepath << "\n"
        << "binary path........: " << pkg.m_pimpl->m_pkgbinarypath << "\n"
        << "exists binary......: " << pkg.existsBinary() << "\n"
        << "\n";
    return out;
}
}
} // namespace vle utils
