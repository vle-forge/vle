/**
 * @file vle/utils/Path.cpp
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


#include <glibmm/fileutils.h>
#include <glibmm/miscutils.h>
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

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

namespace vle { namespace utils {

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

std::string Path::getStreamDir() const
{
    return buildDirname(m_prefix, VLE_LIBRARY_DIRS, "stream");
}

std::string Path::getHomeStreamDir() const
{
    return buildDirname(m_home, "stream");
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
 * conditions
 */

std::string Path::getConditionDir() const
{
    return buildDirname(m_prefix, VLE_LIBRARY_DIRS, "conditions");
}

std::string Path::getHomeConditionDir() const
{
    return buildDirname(m_home, "conditions");
}

std::string Path::getConditionFile(const std::string& file) const
{
    return buildDirname(m_prefix, VLE_SHARE_DIRS, "conditions", file);
}

std::string Path::getHomeConditionFile(const std::string& file) const
{
    return buildDirname(m_home, "conditions", file);
}

std::string Path::getConditionPixmapDir() const
{
    return buildDirname(m_prefix, VLE_SHARE_DIRS, "conditions", "pixmaps");
}

std::string Path::getHomeConditionPixmapDir() const
{
    return buildDirname(m_home, "conditions", "pixmaps");
}

std::string Path::getConditionPixmapFile(const std::string& file) const
{
    return buildFilename(m_prefix, VLE_SHARE_DIRS, "conditions", "pixmaps", file);
}

std::string Path::getHomeConditionPixmapFile(const std::string& file) const
{
    return buildFilename(m_home, "conditions", "pixmaps", file);
}

std::string Path::getConditionGladeDir() const
{
    return buildDirname(m_prefix, VLE_SHARE_DIRS, "conditions", "glade");
}

std::string Path::getHomeConditionGladeDir() const
{
    return buildDirname(m_home, "conditions", "glade");
}

std::string Path::getConditionGladeFile(const std::string& file) const
{
    return buildFilename(m_prefix, VLE_SHARE_DIRS, "conditions", "glade", file);
}

std::string Path::getHomeConditionGladeFile(const std::string& file) const
{
    return buildFilename(m_home, "conditions", "glade", file);
}

std::string Path::getConditionDocDir() const
{
    return buildDirname(m_prefix, VLE_SHARE_DIRS, "conditions", "doc");
}

std::string Path::getHomeConditionDocDir() const
{
    return buildDirname(m_home, "conditions", "doc");
}

std::string Path::getConditionDocFile(const std::string& file) const
{
    return buildFilename(m_prefix, VLE_SHARE_DIRS, "conditions", "doc", file);
}

std::string Path::getHomeConditionDocFile(const std::string& file) const
{
    return buildFilename(m_home, "conditions", "doc", file);
}

/*
 * packages path
 */

std::string Path::getPackagesDir() const
{
    return buildDirname(m_home, "pkgs");
}

void Path::updatePackageDirs()
{
    initPackageDirs();
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
    namespace fs = boost::filesystem;

    const std::string& dirname(getTemplate("package"));
    fs::path source(dirname);

    if (not fs::exists(source)) {
        throw utils::InternalError(fmt(
                _("Can not copy '%1%' into '%2%'. '%1%' does not exist in '%3%'"))
            % name % to % dirname);
    }

    if (fs::is_directory(source)) {
        try {
            std::list < std::string > buildeddir;
            for (fs::recursive_directory_iterator it(source), end; it != end; ++it) {
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
                    _("Error when copy template '%1%' into '%2%: %3%")) % name % to %
                e.what());
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

PathList Path::getInstalledLibraries()
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

void Path::initVleHomeDirectory()
{
    namespace fs = boost::filesystem;

    fs::create_directory(getHomeDir());
    fs::create_directory(getHomeStreamDir());
    fs::create_directory(getHomeOutputDir());
    fs::create_directory(getHomeConditionDir());
    fs::create_directory(getHomeSimulatorDir());
    fs::create_directory(getPackagesDir());
}

/*
 * adding
 */

void Path::addSimulatorDir(const std::string& dirname)
{
    if (isDirectory(dirname)) {
        m_simulator.push_back(dirname);
    }
}

void Path::addStreamDir(const std::string& dirname)
{
    if (isDirectory(dirname)) {
        m_stream.push_back(dirname);
    }
}

void Path::addOutputDir(const std::string& dirname)
{
    if (isDirectory(dirname)) {
        m_output.push_back(dirname);
    }
}

void Path::addConditionDir(const std::string& dirname)
{
    if (isDirectory(dirname)) {
        m_condition.push_back(dirname);
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
        if (isDirectory(path)) {
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

    initPluginDirs();
    initPackageDirs();

    initVleHomeDirectory();
}

void Path::initPluginDirs()
{
    addStreamDir(getStreamDir());
    addOutputDir(getOutputDir());
    addConditionDir(getConditionDir());

    addStreamDir(getHomeStreamDir());
    addOutputDir(getHomeOutputDir());
    addConditionDir(getHomeConditionDir());
}

void Path::initPackageDirs()
{
    m_simulator.clear();

    if (utils::Package::package().name().empty()) {
        m_currentPackagePath.clear();
        addSimulatorDir(getSimulatorDir());
        addSimulatorDir(getHomeSimulatorDir());
    } else {
        m_currentPackagePath.assign(buildDirname(
                m_home, "pkgs", utils::Package::package().name()));
        addSimulatorDir(getPackageLibDir());
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

std::string Path::buildFilename(const std::string& dir,
                                const std::string& file)
{
    return Glib::build_filename(dir, file);
}

std::string Path::buildFilename(const std::string& dir1,
                                const std::string& dir2,
                                const std::string& file)
{
    std::list < std::string > lst;
    lst.push_back(dir1);
    lst.push_back(dir2);

    std::string path = Glib::build_path(G_DIR_SEPARATOR_S, lst);

    return Glib::build_filename(path, file);
}

std::string Path::buildFilename(const std::string& dir1,
                                const std::string& dir2,
                                const std::string& dir3,
                                const std::string& file)
{
    std::list < std::string > lst;
    lst.push_back(dir1);
    lst.push_back(dir2);
    lst.push_back(dir3);

    std::string path = Glib::build_path(G_DIR_SEPARATOR_S, lst);

    return Glib::build_filename(path, file);
}

std::string Path::buildFilename(const std::string& dir1,
                                const std::string& dir2,
                                const std::string& dir3,
                                const std::string& dir4,
                                const std::string& file)
{
    std::list < std::string > lst;
    lst.push_back(dir1);
    lst.push_back(dir2);
    lst.push_back(dir3);
    lst.push_back(dir4);

    std::string path = Glib::build_path(G_DIR_SEPARATOR_S, lst);

    return Glib::build_filename(path, file);
}

std::string Path::buildDirname(const std::string& dir1,
                               const std::string& dir2)
{
    std::list < std::string > lst;
    lst.push_back(dir1);
    lst.push_back(dir2);

    return Glib::build_path(G_DIR_SEPARATOR_S, lst);
}

std::string Path::buildDirname(const std::string& dir1,
                               const std::string& dir2,
                               const std::string& dir3)
{
    std::list < std::string > lst;
    lst.push_back(dir1);
    lst.push_back(dir2);
    lst.push_back(dir3);

    return Glib::build_path(G_DIR_SEPARATOR_S, lst);
}

std::string Path::buildDirname(const std::string& dir1,
                               const std::string& dir2,
                               const std::string& dir3,
                               const std::string& dir4)
{
    std::list < std::string > lst;
    lst.push_back(dir1);
    lst.push_back(dir2);
    lst.push_back(dir3);
    lst.push_back(dir4);

    return Glib::build_path(G_DIR_SEPARATOR_S, lst);
}

bool Path::existFile(const std::string& filename)
{
    namespace fs = boost::filesystem;

    fs::path p(filename);
    return fs::exists(p) and fs::is_directory(p);
}

bool Path::existDirectory(const std::string& filename)
{
    namespace fs = boost::filesystem;

    fs::path p(filename);
#if BOOST_VERSION > 103600
    return fs::exists(p) and fs::is_regular_file(p);
#else
    return fs::exists(p) and fs::is_regular(p);
#endif
}

std::ostream& operator<<(std::ostream& out, const Path& p)
{
    out << "prefix................: " << p.getPrefixDir() << "\n"
        << "\n"
        << "simulator.............: " << p.getSimulatorDir() << "\n"
        << "output................: " << p.getOutputDir() << "\n"
        << "condition.............: " << p.getConditionDir() << "\n"
        << "stream................: " << p.getStreamDir() << "\n"
        << "pixmap................: " << p.getPixmapDir() << "\n"
        << "glade.................: " << p.getGladeDir() << "\n"
        << "examples..............: " << p.getExamplesDir() << "\n"
        << "output pixmap.........: " << p.getOutputPixmapDir() << "\n"
        << "outout glade..........: " << p.getOutputGladeDir() << "\n"
        << "output doc............: " << p.getOutputDocDir() << "\n"
        << "condition pixmap......: " << p.getConditionPixmapDir() << "\n"
        << "condition glade.......: " << p.getConditionGladeDir() << "\n"
        << "condition doc.........: " << p.getConditionDocDir() << "\n"
        << "\n"
        << "vle home..............: " << p.getHomeDir() << "\n"
        << "packages..............: " << p.getPackagesDir() << "\n"
        << "simulator home........: " << p.getHomeSimulatorDir() << "\n"
        << "stream home...........: " << p.getHomeStreamDir() << "\n"
        << "output home...........: " << p.getHomeOutputDir() << "\n"
        << "output home pixmap....: " << p.getHomeOutputPixmapDir() << "\n"
        << "output home glade.....: " << p.getHomeOutputGladeDir() << "\n"
        << "output home doc.......: " << p.getHomeOutputDocDir() << "\n"
        << "condition home........: " << p.getHomeConditionDir() << "\n"
        << "condition home pixmap.: " << p.getHomeConditionPixmapDir() << "\n"
        << "condition home glade..: " << p.getHomeConditionGladeDir() << "\n"
        << "condition home doc....: " << p.getHomeConditionDocDir() << "\n"
        << "\n";

    out << "Package dir...........: " << p.getPackageDir() << "\n"
        << "Package lib dir.......: " << p.getPackageLibDir() << "\n"
        << "Package scr dir.......: " << p.getPackageSrcDir() << "\n"
        << "Package data dir......: " << p.getPackageDataDir() << "\n"
        << "Package doc dir.......: " << p.getPackageDocDir() << "\n"
        << "Package exp dir.......: " << p.getPackageExpDir() << "\n"
        << "Package build dir.....: " << p.getPackageBuildDir() << "\n"
        << "\n";

    out << "Real simulators list..:\n" << p.getSimulatorDirs() << "\n"
        << "Real output list......:\n" << p.getOutputDirs() << "\n"
        << "Real condition list...:\n" << p.getConditionDirs() << "\n"
        << "Real stream list......:\n" << p.getStreamDirs() << "\n"
        << std::endl;

    return out;
}

std::string Path::getParentPath(const std::string& pathfile)
{
    boost::filesystem::path path(pathfile);

#if BOOST_VERSION > 103600
    return path.parent_path().string();
#else
    return path.branch_path().string();
#endif
}

}} // namespace vle utils
