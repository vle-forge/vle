/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2013 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2013 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2013 INRA http://www.inra.fr
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


#include <list>
#include <glib.h>
#include <glib/gstdio.h>

#ifdef G_OS_WIN32
#include <io.h>
#endif

#include <vle/utils/Path.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/version.hpp>

#include <glibmm/miscutils.h>
#include <glibmm/fileutils.h>

#include <iostream>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/version.hpp>

namespace vle { namespace utils {

static const char *pkgdirname = "pkgs-" VLE_ABI_VERSION;

namespace fs = boost::filesystem;

Path* Path::mPath = 0;

std::string Path::getLocaleDir() const
{
    return buildDirname(m_prefix, "share", "locale");
}

std::string Path::getHomeFile(const std::string& name) const
{
    return buildFilename(m_home, name);
}

std::string Path::getPixmapDir() const
{
    return buildDirname(m_prefix, VLE_SHARE_DIRS, "pixmaps");
}

std::string Path::getPixmapFile(const std::string& file) const
{
    return buildFilename(m_prefix, VLE_SHARE_DIRS, "pixmaps", file);
}

std::string Path::getGladeDir() const
{
    return buildDirname(m_prefix, VLE_SHARE_DIRS, "glade");
}

std::string Path::getGladeFile(const std::string& file) const
{
    return buildFilename(m_prefix, VLE_SHARE_DIRS, "glade", file);
}

/*
 * packages path
 */

std::string Path::getPackagesDir() const
{
    return buildDirname(m_home, pkgdirname);
}

std::string Path::getTemplateDir() const
{
    return buildFilename(m_prefix, VLE_SHARE_DIRS, "template");
}

std::string Path::getTemplate(const std::string& name) const
{
    return buildFilename(m_prefix, VLE_SHARE_DIRS, "template", name);
}

void Path::copyTemplate(const std::string& name, const std::string& to) const
{
    const std::string& dirname(getTemplate("package"));
    fs::path source(dirname);

    if (not fs::exists(source)) {
        throw utils::InternalError(
            fmt(_("Can not copy '%1%' into '%2%'. '%1%' does not exist in"
                  "'%3%'")) % name % to % dirname);
    }

    if (fs::is_directory(source)) {
        try {
            std::list < std::string > buildeddir;
            for (fs::recursive_directory_iterator it(source), end; it != end;
                 ++it) {
                std::list < std::string > store;
                {
                    fs::path tmp(it->path());
                    for (int i = it.level(); i >= 0; --i) {
#if BOOST_VERSION > 104500
                        store.push_back(tmp.filename().string());
#else
                        store.push_back(tmp.leaf());
#endif
                        tmp = tmp.branch_path();
                    }
                }

                fs::path dst(to);
                {
                    for (std::list < std::string >::const_reverse_iterator jt =
                         store.rbegin(); jt != store.rend(); ++jt) {
                        dst /= *jt;
                    }
                }

                fs::path dir(dst.branch_path());

                if (std::find(buildeddir.rbegin(), buildeddir.rend(),
                              dir.string()) == buildeddir.rend()) {
                    if (not fs::exists(dir)) {
                        fs::create_directories(dir);
                        buildeddir.push_back(dir.string());
                    }
                }
                fs::path src(it->path());
                dir /= src.leaf();

                if (not fs::is_directory(src) and not fs::exists(dir)) {
                    fs::copy_file(src, dir);
                }
            }
        } catch (const std::exception& e) {
            throw utils::InternalError(fmt(
                    _("Error when copy template '%1%' into '%2%: %3%")) % name
                % to % e.what());
        }
    } else {
        try {
            fs::path src(source);
            fs::path dst(to);
            dst /= src.leaf();
            if (fs::exists(src) and not fs::exists(dst)) {
                fs::copy_file(src, dst);
            }
        } catch (const std::exception& e) {
            throw utils::InternalError(fmt(
                    _("Can not copy '%1%' into '%2%': %3%")) % name % to %
                e.what());
        }
    }
}

std::string Path::getPackageSourceDir() const
{
    return m_currentPackageSourcePath;
}

std::string Path::getPackageBinaryDir() const
{
    return m_currentPackagePath;
}

std::string Path::getPackageLibDir() const
{
    return buildDirname(m_currentPackagePath, "lib");
}

std::string Path::getPackageSrcDir() const
{
    return buildDirname(m_currentPackagePath, "src");
}

std::string Path::getPackageDataDir() const
{
    return buildDirname(m_currentPackagePath, "data");
}

std::string Path::getPackageExpDir() const
{
    return buildDirname(m_currentPackagePath, "exp");
}

std::string Path::getPackageOutputDir() const
{
    return buildDirname(m_currentPackagePath, "output");
}

std::string Path::getPackagePluginDir() const
{
    return buildDirname(m_currentPackagePath, "plugins");
}

std::string Path::getPackagePluginSimulatorDir() const
{
    return buildDirname(m_currentPackagePath, "plugins", "simulator");
}

std::string Path::getPackagePluginOutputDir() const
{
    return buildDirname(m_currentPackagePath, "plugins", "output");
}

std::string Path::getPackagePluginGvleModelingDir() const
{
    return buildDirname(m_currentPackagePath, "plugins", "gvle", "modeling");
}

std::string Path::getPackagePluginGvleOutputDir() const
{
    return buildDirname(m_currentPackagePath, "plugins", "gvle", "output");
}

std::string Path::getPackageBuildDir() const
{
    return buildDirname(m_currentPackageSourcePath, "buildvle");
}

std::string Path::getPackageDocDir() const
{
    return buildDirname(m_currentPackagePath, "doc");
}

std::string Path::getPackageFile(const std::string& name) const
{
    return buildFilename(getPackageSourceDir(), name);
}

std::string Path::getPackageLibFile(const std::string& name) const
{
    return buildFilename(getPackageLibDir(), name);
}

std::string Path::getPackageSrcFile(const std::string& name) const
{
    return buildFilename(getPackageSrcDir(), name);
}

std::string Path::getPackageDataFile(const std::string& name) const
{
    return buildFilename(getPackageDataDir(), name);
}

std::string Path::getPackageExpFile(const std::string& name) const
{
    return buildFilename(getPackageExpDir(), name);
}

std::string Path::getPackageOutputFile(const std::string& name) const
{
    return buildFilename(getPackageOutputDir(), name);
}

std::string Path::getPackageDocFile(const std::string& name) const
{
    return buildFilename(getPackageDocDir(), name);
}

std::string Path::getPackagePluginFile(const std::string& name) const
{
    return buildDirname(m_currentPackagePath, "plugins", name);
}

std::string Path::getPackagePluginSimulatorFile(const std::string& name) const
{
    return buildDirname(m_currentPackagePath, "plugins", "simulator", name);
}

std::string Path::getPackagePluginOutputFile(const std::string& name) const
{
    return buildDirname(m_currentPackagePath, "plugins", "output", name);
}

std::string Path::getPackagePluginGvleModelingFile(const std::string& name) const
{
    return buildDirname(m_currentPackagePath, "plugins", "gvle", "modeling", name);
}

std::string Path::getPackagePluginGvleOutputFile(const std::string& name) const
{
    return buildDirname(m_currentPackagePath, "plugins", "gvle", "output", name);
}

std::string Path::getExternalPackageDir(const std::string& name) const
{
    return buildDirname(getPackagesDir(), name);
}

std::string Path::getExternalPackageLibDir(const std::string& name) const
{
    return buildDirname(getPackagesDir(), name, "lib");
}

std::string Path::getExternalPackageSrcDir(const std::string& name) const
{
    return buildDirname(getPackagesDir(), name, "src");
}

std::string Path::getExternalPackageDataDir(const std::string& name) const
{
    return buildDirname(getPackagesDir(), name, "data");
}

std::string Path::getExternalPackageDocDir(const std::string& name) const
{
    return buildDirname(getPackagesDir(), name, "doc");
}

std::string Path::getExternalPackageExpDir(const std::string& name) const
{
    return buildDirname(getPackagesDir(), name, "exp");
}

std::string Path::getExternalPackageBuildDir(const std::string& name) const
{
    return buildDirname(getPackagesDir(), name, "build");
}

std::string Path::getExternalPackageOutputDir(const std::string& name) const
{
    return buildDirname(getPackagesDir(), name, "plugins");
}

std::string Path::getExternalPackagePluginDir(const std::string& name) const
{
    return buildDirname(getPackagesDir(), name, "plugins");
}

std::string Path::getExternalPackagePluginSimulatorDir(
    const std::string& name) const
{
    return buildDirname(getPackagesDir(), name, "plugins", "simulator");
}

std::string Path::getExternalPackagePluginOutputDir(
    const std::string& name) const
{
    return buildDirname(getPackagesDir(), name, "plugins", "output");
}

std::string Path::getExternalPackagePluginGvleModelingDir(
    const std::string& name) const
{
    return buildDirname(getPackagesDir(), name, "plugins", "gvle", "modeling");
}

std::string Path::getExternalPackagePluginGvleOutputDir(
    const std::string& name) const
{
    return buildDirname(getPackagesDir(), name, "plugins", "gvle", "output");
}

std::string Path::getExternalPackageFile(const std::string& name,
                                         const std::string& file) const
{
    return buildFilename(getPackagesDir(), name, file);
}

std::string Path::getExternalPackageLibFile(const std::string& name,
                                            const std::string& file) const
{
    return buildFilename(getPackagesDir(), name, "lib", file);
}

std::string Path::getExternalPackageSrcFile(const std::string& name,
                                            const std::string& file) const
{
    return buildFilename(getPackagesDir(), name, "src", file);
}

std::string Path::getExternalPackageDataFile(const std::string& name,
                                             const std::string& file) const
{
    return buildFilename(getPackagesDir(), name, "data", file);
}

std::string Path::getExternalPackageDocFile(const std::string& name,
                                            const std::string& file) const
{
    return buildFilename(getPackagesDir(), name, "doc", file);
}

std::string Path::getExternalPackageExpFile(const std::string& name,
                                            const std::string& file) const
{
    return buildFilename(getPackagesDir(), name, "exp", file);
}

std::string Path::getExternalPackageOutputFile(
    const std::string& package,
    const std::string& file) const
{
    return buildFilename(getPackagesDir(), package, "output", file);
}

std::string Path::getExternalPackagePluginFile(
    const std::string& package,
    const std::string& file) const
{
    return buildFilename(getPackagesDir(), package, "plugins", file);
}

std::string Path::getExternalPackagePluginSimulatorFile(
    const std::string& package,
    const std::string& file) const
{
    return buildFilename(getPackagesDir(), package, "plugins", "simulator", file);
}

std::string Path::getExternalPackagePluginOutputFile(
    const std::string& package,
    const std::string& file) const
{
    return buildFilename(getPackagesDir(), package, "plugins", "output", file);
}

std::string Path::getExternalPackagePluginGvleModelingFile(
    const std::string& package,
    const std::string& file) const
{
    return buildFilename(
        getPackagesDir(), package, "plugins", "gvle", "modeling", file);
}

std::string Path::getExternalPackagePluginGvleOutputFile(
    const std::string& package,
    const std::string& file) const
{
    return buildFilename(
        getPackagesDir(), package, "plugins", "gvle", "output", file);
}

PathList Path::getInstalledPackages()
{
    fs::path pkgs(Path::path().getPackagesDir());

    if (not fs::exists(pkgs) or not fs::is_directory(pkgs)) {
        throw utils::InternalError(fmt(
                _("Package error: '%1%' is not a directory")) %
            Path::path().getPackagesDir());
    }

    PathList result;
    for (fs::directory_iterator it(pkgs), end; it != end; ++it) {
        if (fs::is_directory(it->status())) {
#if BOOST_VERSION > 104500
            result.push_back(it->path().filename().string());
#elif BOOST_VERSION > 103600
            result.push_back(it->path().filename());
#else
            result.push_back(it->path().leaf());
#endif
        }
    }

    return result;
}

PathList Path::getInstalledExperiments()
{
    fs::path pkgs(Path::path().getPackageExpDir());

    if (not fs::exists(pkgs) or not fs::is_directory(pkgs)) {
        throw utils::InternalError(fmt(
                _("Pkg list error: '%1%' is not an experiments directory")) %
#if BOOST_VERSION > 104500
            pkgs.string());
#else
            pkgs.file_string());
#endif
    }

    PathList result;
    std::stack < fs::path > stack;
    stack.push(pkgs);

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

PathList Path::getInstalledLibraries()
{
    PathList result;
    const PathList& dirs = Path::path().getSimulatorDirs();

    for (PathList::const_iterator it = dirs.begin(); it != dirs.end(); ++it) {
        fs::path dir(*it);

        if (not fs::exists(dir) or not fs::is_directory(dir)) {
            throw utils::InternalError(fmt(
                    _("Pkg list error: '%1%' is not a library directory")) %
#if BOOST_VERSION > 103600
                dir.string());
#else
                dir.file_string());
#endif
        }

        for (fs::directory_iterator jt(dir), end; jt != end; ++jt) {
#if BOOST_VERSION > 104500
            if (fs::is_regular_file(jt->status())) {
                std::string ext = jt->path().extension().string();
#elif BOOST_VERSION > 103600
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
#if BOOST_VERSION > 104500
                    result.push_back(jt->path().string());
#else
                    result.push_back(jt->path().file_string());
#endif
                }
#endif
            }
        }
    }
    return result;
}

std::string Path::getPackageFromPath(const std::string& path)
{
    fs::path source(path);
    fs::path package(utils::Path::path().getPackagesDir());

    fs::path::iterator it = source.begin();
    fs::path::iterator jt = package.begin();

    while (it != source.end() and jt != package.end()) {
        if ((*it) == (*jt)) {
            ++it;
            ++jt;
        } else {
            break;
        }
    }

    if (jt == package.end() and it != source.end()) {
#if BOOST_VERSION > 104500
        return it->string();
#else
        return *it;
#endif
    } else {
        return std::string();
    }
}

void Path::initVleHomeDirectory()
{
    boost::system::error_code ec;

    fs::path pkgs = getPackagesDir();

    if (not fs::exists(pkgs, ec)) {
        if (not fs::create_directories(getPackagesDir(), ec)) {
            throw FileError(fmt(
                    _("Failed to build VLE_HOME directory (%1%):\n%2%")) %
                pkgs.string() % ec.message());
        }
    }
}

/*
 * adding
 */

void Path::addSimulatorDir(const std::string& dirname)
{
    if (fs::is_directory(dirname)) {
        m_simulator.push_back(dirname);
    }
}

void Path::addStreamDir(const std::string& dirname)
{
    if (fs::is_directory(dirname)) {
        m_stream.push_back(dirname);
    }
}

void Path::addOutputDir(const std::string& dirname)
{
    if (fs::is_directory(dirname)) {
        m_output.push_back(dirname);
    }
}

void Path::addModelingDir(const std::string& dirname)
{
    if (fs::is_directory(dirname)) {
        m_modeling.push_back(dirname);
    }
}

bool Path::readEnv(const std::string& variable, PathList& out)
{
    std::string path(Glib::getenv(variable));
    if (not path.empty()) {
        PathList result;
        boost::algorithm::split(result, path, boost::is_any_of(":"),
                                boost::algorithm::token_compress_on);

        PathList::iterator it(std::remove_if(result.begin(), result.end(),
                                             std::not1(IsDirectory())));

        result.erase(it, result.end());

        std::copy(result.begin(), result.end(), std::back_inserter(out));
        return true;
    }
    return false;
}

void Path::readHomeDir()
{
    std::string path(Glib::getenv("VLE_HOME"));
    if (not path.empty()) {
        if (fs::is_directory(path)) {
            m_home = path;
        } else {
            throw FileError(fmt(_(
                    "Path: VLE_HOME '%1%' does not exist")) % path);
        }
    } else {
        m_home.clear();
    }
}

Path::Path()
{
    initHomeDir();
    initPrefixDir();

    updateDirs();

    initVleHomeDirectory();
}

void Path::updateDirs()
{
    if (utils::Package::package().name().empty()) {
        initGlobalPluginDirs();
    } else {
        initPackagePluginDirs();
    }
}

void Path::clearPluginDirs()
{
    m_simulator.clear();
    m_stream.clear();
    m_output.clear();
    m_modeling.clear();
}

void Path::initGlobalPluginDirs()
{
    assert(utils::Package::package().name().empty());

    clearPluginDirs();
    m_currentPackagePath.clear();
    m_currentPackageSourcePath.clear();
}

void Path::initPackagePluginDirs()
{
    assert(not utils::Package::package().name().empty());

    clearPluginDirs();

    {
        fs::path binary = m_home;
        binary /= pkgdirname;
        binary /= utils::Package::package().name();

        m_currentPackagePath = binary.string();
    }

    {
        fs::path src = fs::current_path();
        src /= utils::Package::package().name();

        m_currentPackageSourcePath = src.string();
    }

    PathList result;
    for (fs::directory_iterator it(getPackagesDir()), end; it != end; ++it) {
        if (fs::is_directory(it->status())) {
            fs::path package(*it);
            package /= "plugins";
            if (fs::is_directory(package)) {
                {
                    fs::path oov(package);
                    oov = oov /  "output";

                    if (fs::is_directory(oov)) {
                        addStreamDir(oov.string());
                    }
                }
                {
                    fs::path gvleout(package);
                    gvleout = gvleout / "gvle" / "output";

                    if (fs::is_directory(gvleout)) {
                        addOutputDir(gvleout.string());
                    }
                }
                {
                    fs::path gvlemod(package);
                    gvlemod = gvlemod / "gvle" /  "output";

                    if (fs::is_directory(gvlemod)) {
                        addModelingDir(gvlemod.string());
                    }
                }
            }
        }
    }
}

std::ostream& operator<<(std::ostream& out, const PathList& paths)
{
    PathList::const_iterator it = paths.begin();
    while (it != paths.end()) {
        out << "\t" << *it << "\n";
        ++it;
    }
    return out;
}

std::string Path::buildTemp(const std::string& filename)
{
    return utils::Path::buildFilename(Glib::get_tmp_dir(), filename);
}

std::string Path::writeToTemp(const std::string& prefix,
                              const std::string& buffer)
{
    std::string filename;
    int fd;

    if (prefix.size()) {
        fd = Glib::file_open_tmp(filename, prefix);
    } else {
        fd = Glib::file_open_tmp(filename);
    }

    if (fd == -1) {
        throw utils::InternalError(fmt(
                _("Cannot open file %2% with prefix %1% in temporary "
                  "directory\n")) % prefix % filename);
    }

#ifdef G_OS_WIN32
    ssize_t sz = ::_write(fd, buffer.c_str(), buffer.size());
#else
    ssize_t sz = ::write(fd, buffer.c_str(), buffer.size());
#endif

    if (sz == -1 or sz == 0) {
        throw utils::InternalError(fmt(
            _("Cannot write buffer in file %1% in tempory directory\n")) %
            filename);
    }

#ifdef G_OS_WIN32
    ::_close(fd);
#else
    ::close(fd);
#endif

    return filename;
}


std::string Path::buildFilename(const std::string& dir,
                                const std::string& file)
{
    fs::path f = dir;
    f /= file;

    return f.string();
}

std::string Path::buildFilename(const std::string& dir1,
                                const std::string& dir2,
                                const std::string& file)
{
    fs::path f = dir1;
    f /= dir2;
    f /= file;

    return f.string();
}

std::string Path::buildFilename(const std::string& dir1,
                                const std::string& dir2,
                                const std::string& dir3,
                                const std::string& file)
{
    fs::path f = dir1;
    f /= dir2;
    f /= dir3;
    f /= file;

    return f.string();
}

std::string Path::buildFilename(const std::string& dir1,
                                const std::string& dir2,
                                const std::string& dir3,
                                const std::string& dir4,
                                const std::string& file)
{
    fs::path f = dir1;
    f /= dir2;
    f /= dir3;
    f /= dir4;
    f /= file;

    return f.string();
}

std::string Path::buildFilename(const std::string& dir1,
                                const std::string& dir2,
                                const std::string& dir3,
                                const std::string& dir4,
                                const std::string& dir5,
                                const std::string& file)
{
    fs::path f = dir1;
    f /= dir2;
    f /= dir3;
    f /= dir4;
    f /= dir5;
    f /= file;

    return f.string();
}

std::string Path::buildDirname(const std::string& dir1,
                               const std::string& dir2)
{
    fs::path f = dir1;
    f /= dir2;

    return f.string();
}

std::string Path::buildDirname(const std::string& dir1,
                               const std::string& dir2,
                               const std::string& dir3)
{
    fs::path f = dir1;
    f /= dir2;
    f /= dir3;

    return f.string();
}

std::string Path::buildDirname(const std::string& dir1,
                               const std::string& dir2,
                               const std::string& dir3,
                               const std::string& dir4)
{
    fs::path f = dir1;
    f /= dir2;
    f /= dir3;
    f /= dir4;

    return f.string();
}

std::string Path::buildDirname(const std::string& dir1,
                               const std::string& dir2,
                               const std::string& dir3,
                               const std::string& dir4,
                               const std::string& dir5)
{
    fs::path f = dir1;
    f /= dir2;
    f /= dir3;
    f /= dir4;
    f /= dir5;

    return f.string();
}

std::string Path::getCurrentPath()
{
    fs::path current = fs::current_path();

#if BOOST_VERSION > 104500
    return current.string();
#else
    return current.file_string();
#endif
}

bool Path::exist(const std::string& filename)
{
    fs::path p(filename);
    return fs::exists(p);
}

bool Path::existDirectory(const std::string& filename)
{
    fs::path p(filename);
    return fs::exists(p) and fs::is_directory(p);
}

bool Path::existFile(const std::string& filename)
{
    fs::path p(filename);
#if BOOST_VERSION > 103600
    return fs::exists(p) and fs::is_regular_file(p);
#else
    return fs::exists(p) and fs::is_regular(p);
#endif
}

std::string Path::filename(const std::string& filename)
{
    fs::path path(filename);
#if BOOST_VERSION > 104500
    return path.filename().string();
#elif BOOST_VERSION > 103600
    return path.filename();
#else
    return path.leaf();
#endif
}

std::string Path::basename(const std::string& filename)
{
    return fs::basename(filename);
}

std::string Path::dirname(const std::string& filename)
{
    fs::path path(filename);
#if BOOST_VERSION > 103600
    return path.parent_path().string();
#else
    return path.branch_path().string();
#endif
}

std::string Path::extension(const std::string& filename)
{
    return fs::extension(filename);
}

std::ostream& operator<<(std::ostream& out, const Path& p)
{
    out << "prefix................: " << p.getPrefixDir() << "\n"
        << "\n"
        << "pixmap................: " << p.getPixmapDir() << "\n"
        << "glade.................: " << p.getGladeDir() << "\n"
        << "\n"
        << "vle home..............: " << p.getHomeDir() << "\n"
        << "packages..............: " << p.getPackagesDir() << "\n"
        << "\n";

    out << "Package dir...........: " << p.getPackageSourceDir() << "\n"
        << "Package binary dir....: " << p.getPackageBinaryDir() << "\n"
        << "Package lib dir.......: " << p.getPackageLibDir() << "\n"
        << "Package scr dir.......: " << p.getPackageSrcDir() << "\n"
        << "Package data dir......: " << p.getPackageDataDir() << "\n"
        << "Package doc dir.......: " << p.getPackageDocDir() << "\n"
        << "Package exp dir.......: " << p.getPackageExpDir() << "\n"
        << "Package build dir.....: " << p.getPackageBuildDir() << "\n"
        << "Package simulators....: " << p.getPackagePluginSimulatorDir() << "\n"
        << "Package stream........: " << p.getPackagePluginOutputDir() << "\n"
        << "Package gvle output...: " << p.getPackagePluginGvleOutputDir()
        << "\n"
        << "Package gvle modeling.: " << p.getPackagePluginGvleModelingDir()
        << "\n" << "\n";

    out << "Real simulators list..:\n" << p.getSimulatorDirs() << "\n"
        << "Real output list......:\n" << p.getOutputDirs() << "\n"
        << "Real modeling list....:\n" << p.getModelingDirs() << "\n"
        << "Real stream list......:\n" << p.getStreamDirs() << "\n"
        << std::endl;

    return out;
}

std::string Path::getParentPath(const std::string& pathfile)
{
    fs::path path(pathfile);

#if BOOST_VERSION > 103600
    return path.parent_path().string();
#else
    return path.branch_path().string();
#endif
}

}} // namespace vle utils
