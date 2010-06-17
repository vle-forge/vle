/**
 * @file vle/utils/Path.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
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

namespace vle { namespace utils {

namespace fs = boost::filesystem;

Path* Path::mPath = 0;

std::string Path::getSimulatorDir() const
{
    return buildDirname(m_prefix, VLE_LIBRARY_DIRS, "simulator");
}

std::string Path::getHomeSimulatorDir() const
{
    return buildDirname(m_home, "simulator");
}

std::string Path::getExamplesDir() const
{
    return buildDirname(m_prefix, "examples");
}

std::string Path::getExampleFile(const std::string& file) const
{
    return buildDirname(m_prefix, VLE_SHARE_DIRS, "examples", file);
}

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
 * outputs
 */

std::string Path::getOutputDir() const
{
    return buildDirname(m_prefix, VLE_LIBRARY_DIRS, "outputs");
}

std::string Path::getHomeOutputDir() const
{
    return buildDirname(m_home, "outputs");
}

std::string Path::getOutputFile(const std::string& file) const
{
    return buildDirname(m_prefix, VLE_SHARE_DIRS, "outputs", file);
}

std::string Path::getHomeOutputFile(const std::string& file) const
{
    return buildDirname(m_home, "outputs", file);
}

std::string Path::getOutputPixmapDir() const
{
    return buildDirname(m_prefix, VLE_SHARE_DIRS, "outputs", "pixmaps");
}

std::string Path::getHomeOutputPixmapDir() const
{
    return buildDirname(m_home, "outputs", "pixmaps");
}

std::string Path::getOutputPixmapFile(const std::string& file) const
{
    return buildFilename(m_prefix, VLE_SHARE_DIRS, "outputs", "pixmaps", file);
}

std::string Path::getHomeOutputPixmapFile(const std::string& file) const
{
    return buildDirname(m_home, "outputs", "pixmaps", file);
}

std::string Path::getOutputGladeDir() const
{
    return buildDirname(m_prefix, VLE_SHARE_DIRS, "outputs", "glade");
}

std::string Path::getHomeOutputGladeDir() const
{
    return buildDirname(m_home, "outputs", "glade");
}

std::string Path::getOutputGladeFile(const std::string& file) const
{
    return buildFilename(m_prefix, VLE_SHARE_DIRS, "outputs", "glade", file);
}

std::string Path::getHomeOutputGladeFile(const std::string& file) const
{
    return buildDirname(m_home, "outputs", "glade", file);
}

std::string Path::getOutputDocDir() const
{
    return buildDirname(m_prefix, VLE_SHARE_DIRS, "outputs", "doc");
}

std::string Path::getHomeOutputDocDir() const
{
    return buildDirname(m_home, "outputs", "doc");
}

std::string Path::getOutputDocFile(const std::string& file) const
{
    return buildFilename(m_prefix, VLE_SHARE_DIRS, "outputs", "doc", file);
}

std::string Path::getHomeOutputDocFile(const std::string& file) const
{
    return buildFilename(m_home, "outputs", "doc", file);
}

/*
 * stream
 */

std::string Path::getStreamDir() const
{
    return buildDirname(m_prefix, VLE_LIBRARY_DIRS, "stream");
}

std::string Path::getHomeStreamDir() const
{
    return buildDirname(m_home, "stream");
}

std::string Path::getStreamFile(const std::string& file) const
{
    return buildDirname(m_prefix, VLE_SHARE_DIRS, "stream", file);
}

std::string Path::getHomeStreamFile(const std::string& file) const
{
    return buildDirname(m_home, "stream", file);
}

/*
 * modeling
 */

std::string Path::getModelingDir() const
{
    return buildDirname(m_prefix, VLE_LIBRARY_DIRS, "modeling");
}

std::string Path::getHomeModelingDir() const
{
    return buildDirname(m_home, "modeling");
}

std::string Path::getModelingFile(const std::string& file) const
{
    return buildDirname(m_prefix, VLE_SHARE_DIRS, "modeling", file);
}

std::string Path::getHomeModelingFile(const std::string& file) const
{
    return buildDirname(m_home, "modeling", file);
}

std::string Path::getModelingPixmapDir() const
{
    return buildDirname(m_prefix, VLE_SHARE_DIRS, "modeling", "pixmaps");
}

std::string Path::getHomeModelingPixmapDir() const
{
    return buildDirname(m_home, "modeling", "pixmaps");
}

std::string Path::getModelingPixmapFile(const std::string& file) const
{
    return buildFilename(m_prefix, VLE_SHARE_DIRS, "modeling", "pixmaps", file);
}

std::string Path::getHomeModelingPixmapFile(const std::string& file) const
{
    return buildFilename(m_home, "modeling", "pixmaps", file);
}

std::string Path::getModelingGladeDir() const
{
    return buildDirname(m_prefix, VLE_SHARE_DIRS, "modeling", "glade");
}

std::string Path::getHomeModelingGladeDir() const
{
    return buildDirname(m_home, "modeling", "glade");
}

std::string Path::getModelingGladeFile(const std::string& file) const
{
    return buildFilename(m_prefix, VLE_SHARE_DIRS, "modeling", "glade", file);
}

std::string Path::getHomeModelingGladeFile(const std::string& file) const
{
    return buildFilename(m_home, "modeling", "glade", file);
}

std::string Path::getModelingDocDir() const
{
    return buildDirname(m_prefix, VLE_SHARE_DIRS, "modeling", "doc");
}

std::string Path::getHomeModelingDocDir() const
{
    return buildDirname(m_home, "modeling", "doc");
}

std::string Path::getModelingDocFile(const std::string& file) const
{
    return buildFilename(m_prefix, VLE_SHARE_DIRS, "modeling", "doc", file);
}

std::string Path::getHomeModelingDocFile(const std::string& file) const
{
    return buildFilename(m_home, "modeling", "doc", file);
}


/*
 * packages path
 */

std::string Path::getPackagesDir() const
{
    return buildDirname(m_home, "pkgs");
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
                        store.push_back(tmp.leaf());
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


std::string Path::getPackageDir() const
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
    return buildDirname(m_currentPackagePath, "build");
}

std::string Path::getPackageDocDir() const
{
    return buildDirname(m_currentPackagePath, "doc");
}

std::string Path::getPackageFile(const std::string& name) const
{
    return buildFilename(getPackageDir(), name);
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
#if BOOST_VERSION > 103600
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
            pkgs.file_string());
    }

    PathList result;
    std::stack < fs::path > stack;
    stack.push(pkgs);

    while (not stack.empty()) {
        fs::path dir = stack.top();
        stack.pop();

        for (fs::directory_iterator it(dir), end; it != end; ++it) {
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

void Path::initVleHomeDirectory()
{
    fs::create_directory(getHomeDir());
    fs::create_directory(getHomeStreamDir());
    fs::create_directory(getHomeOutputDir());
    fs::create_directory(getHomeModelingDir());
    fs::create_directory(getHomeSimulatorDir());
    fs::create_directory(getPackagesDir());
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
    m_condition.clear();
    m_modeling.clear();
}

void Path::initGlobalPluginDirs()
{
    assert(utils::Package::package().name().empty());

    clearPluginDirs();
    m_currentPackagePath.clear();

    addStreamDir(getStreamDir());
    addOutputDir(getOutputDir());
    addModelingDir(getModelingDir());
    addSimulatorDir(getSimulatorDir());

    addStreamDir(getHomeStreamDir());
    addOutputDir(getHomeOutputDir());
    addModelingDir(getHomeModelingDir());
    addSimulatorDir(getHomeSimulatorDir());
}

void Path::initPackagePluginDirs()
{
    assert(not utils::Package::package().name().empty());

    clearPluginDirs();

    m_currentPackagePath.assign(buildDirname(
            m_home, "pkgs", utils::Package::package().name()));

    addStreamDir(getPackagePluginOutputDir());
    addOutputDir(getPackagePluginGvleOutputDir());
    addModelingDir(getPackagePluginGvleModelingDir());

    addStreamDir(getStreamDir());
    addOutputDir(getOutputDir());
    addModelingDir(getModelingDir());

    addStreamDir(getHomeStreamDir());
    addOutputDir(getHomeOutputDir());
    addModelingDir(getHomeModelingDir());

    addSimulatorDir(getPackageLibDir());
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

std::string Path::getCurrentPath()
{
    fs::path current = fs::current_path();

    return current.file_string();
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
#if BOOST_VERSION > 103600
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
        << "simulator.............: " << p.getSimulatorDir() << "\n"
        << "output................: " << p.getOutputDir() << "\n"
        << "modeling..............: " << p.getModelingDir() << "\n"
        << "stream................: " << p.getStreamDir() << "\n"
        << "pixmap................: " << p.getPixmapDir() << "\n"
        << "glade.................: " << p.getGladeDir() << "\n"
        << "examples..............: " << p.getExamplesDir() << "\n"
        << "output pixmap.........: " << p.getOutputPixmapDir() << "\n"
        << "outout glade..........: " << p.getOutputGladeDir() << "\n"
        << "output doc............: " << p.getOutputDocDir() << "\n"
        << "modeling pixmap.......: " << p.getModelingPixmapDir() << "\n"
        << "modeling glade........: " << p.getModelingGladeDir() << "\n"
        << "modeling doc..........: " << p.getModelingDocDir() << "\n"
        << "\n"
        << "vle home..............: " << p.getHomeDir() << "\n"
        << "packages..............: " << p.getPackagesDir() << "\n"
        << "simulator home........: " << p.getHomeSimulatorDir() << "\n"
        << "stream home...........: " << p.getHomeStreamDir() << "\n"
        << "output home...........: " << p.getHomeOutputDir() << "\n"
        << "output home pixmap....: " << p.getHomeOutputPixmapDir() << "\n"
        << "output home glade.....: " << p.getHomeOutputGladeDir() << "\n"
        << "output home doc.......: " << p.getHomeOutputDocDir() << "\n"
        << "modeling home.........: " << p.getHomeModelingDir() << "\n"
        << "modeling home pixmap..: " << p.getHomeModelingPixmapDir() << "\n"
        << "modeling home glade...: " << p.getHomeModelingGladeDir() << "\n"
        << "modeling home doc.....: " << p.getHomeModelingDocDir() << "\n"
        << "\n";

    out << "Package dir...........: " << p.getPackageDir() << "\n"
        << "Package lib dir.......: " << p.getPackageLibDir() << "\n"
        << "Package scr dir.......: " << p.getPackageSrcDir() << "\n"
        << "Package data dir......: " << p.getPackageDataDir() << "\n"
        << "Package doc dir.......: " << p.getPackageDocDir() << "\n"
        << "Package exp dir.......: " << p.getPackageExpDir() << "\n"
        << "Package build dir.....: " << p.getPackageBuildDir() << "\n"
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
