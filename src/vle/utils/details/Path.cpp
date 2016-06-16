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


#include <list>
#include <stdlib.h>

#ifdef _WIN32
#include <io.h>
#endif

#include <vle/utils/Path.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/version.hpp>
#include <vle/utils/Filesystem.hpp>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/version.hpp>

namespace vle { namespace utils {

static const char *pkgdirname = "pkgs-" VLE_ABI_VERSION;

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

std::string Path::getBinaryPackagesDir() const
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

std::string Path::getCurrentDir() const
{
    return FSpath::current_path().string();
}

PathList Path::getBinaryPackages()
{
    FSpath pkgs(Path::path().getBinaryPackagesDir());

    if (not pkgs.is_directory())
        throw utils::InternalError(
            (fmt(_("Package error: '%1%' is not a directory")) %
             Path::path().getBinaryPackagesDir()).str());

    PathList result;
    for (FSdirectory_iterator it(pkgs), end; it != end; ++it)
        if (it->is_directory())
            result.push_back(it->path().filename());

    return result;
}

PathList Path::getBinaryLibraries()
{
    PathList result;
    const PathList& dirs = Path::path().getSimulatorDirs();

    for (auto elem : dirs) {
        FSpath dir(elem);

        if (not dir.is_directory())
            throw utils::InternalError(
                (fmt(_("Pkg list error: '%1%' is not a library directory")) %
                 dir.string()).str());

        for (FSdirectory_iterator jt(dir), end; jt != end; ++jt) {
            if (jt->is_file()) {
                std::string ext = jt->path().extension();

#ifdef _WIN32
                if (ext == ".dll")
                    result.push_back(jt->path().string());
#elif __APPLE__
                if (ext == ".dylib")
                    result.push_back(jt->path().string());
#else
                if (ext == ".so")
                    result.push_back(jt->path().string());
#endif              
            }
        }
    }
    return result;
}

// std::string Path::getPackageFromPath(const std::string& path)
// {
//     FSpath source(path);
//     FSpath package(vle::utils::Path::path().getBinaryPackagesDir());

//     fs::path::iterator it = source.begin();
//     fs::path::iterator jt = package.begin();

//     while (it != source.end() and jt != package.end()) {
//         if ((*it) == (*jt)) {
//             ++it;
//             ++jt;
//         } else {
//             break;
//         }
//     }

//     if (jt == package.end() and it != source.end()) {
//         return it->string();
//     } else {
//         return std::string();
//     }
// }

void Path::initVleHomeDirectory()
{
    FSpath pkgs = getBinaryPackagesDir();

    if (not pkgs.exists())
        if (not FSpath::create_directories(getBinaryPackagesDir()))
            throw FileError(
                (fmt(_("Failed to build VLE_HOME directory (%1%)")) %
                 pkgs.string()).str());
}

/*
 * adding
 */

void Path::addSimulatorDir(const std::string& dirname)
{
    if (FSpath::is_directory(dirname))
        m_simulator.push_back(dirname);
}

void Path::addStreamDir(const std::string& dirname)
{
    if (FSpath::is_directory(dirname))
        m_stream.push_back(dirname);
}

void Path::addOutputDir(const std::string& dirname)
{
    if (FSpath::is_directory(dirname))
        m_output.push_back(dirname);
}

void Path::addModelingDir(const std::string& dirname)
{
    if (FSpath::is_directory(dirname))
        m_modeling.push_back(dirname);
}

bool Path::readEnv(const std::string& variable, PathList& out)
{
    const char* path_str = std::getenv(variable.c_str());
    std::string path("");
    if (path_str) {
        path.assign(path_str);
    }
    if (not path.empty()) {
        PathList result;
        boost::algorithm::split(result, path, boost::is_any_of(":"),
                                boost::algorithm::token_compress_on);

        PathList::iterator it(
            std::remove_if(result.begin(), result.end(),
                           [](const std::string& dirname)
                           {
                               return utils::Path::existDirectory(dirname);
                           }));

        result.erase(it, result.end());

        std::copy(result.begin(), result.end(), std::back_inserter(out));
        return true;
    }
    return false;
}

void Path::readHomeDir()
{
    const char* path_str = std::getenv("VLE_HOME");
    std::string path("");
    if (path_str) {
        path.assign(path_str);
    }
    if (not path.empty()) {
        if (FSpath::is_directory(path)) {
            m_home = path;
        } else {
            throw FileError(
                (fmt(_("Path: VLE_HOME '%1%' does not exist")) % path).str());
        }
    } else {
        m_home.clear();
    }
}

Path::Path()
{
    initHomeDir();
    initPrefixDir();
    initVleHomeDirectory();
}

Path::~Path() noexcept = default;

void Path::clearPluginDirs()
{
    m_simulator.clear();
    m_stream.clear();
    m_output.clear();
    m_modeling.clear();
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
    FSpath tmp_path = FSpath::temp_directory_path() / filename;

    return tmp_path.string();
}

std::string Path::buildFilename(const std::string& dir,
                                const std::string& file)
{
    FSpath f = dir;
    f /= file;

    return f.string();
}

std::string Path::buildFilename(const std::string& dir1,
                                const std::string& dir2,
                                const std::string& file)
{
    FSpath f = dir1;
    f /= dir2;
    f /= file;

    return f.string();
}

std::string Path::buildFilename(const std::string& dir1,
                                const std::string& dir2,
                                const std::string& dir3,
                                const std::string& file)
{
    FSpath f = dir1;
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
    FSpath f = dir1;
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
    FSpath f = dir1;
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
    FSpath f = dir1;
    f /= dir2;

    return f.string();
}

std::string Path::buildDirname(const std::string& dir1,
                               const std::string& dir2,
                               const std::string& dir3)
{
    FSpath f = dir1;
    f /= dir2;
    f /= dir3;

    return f.string();
}

std::string Path::buildDirname(const std::string& dir1,
                               const std::string& dir2,
                               const std::string& dir3,
                               const std::string& dir4)
{
    FSpath f = dir1;
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
    FSpath f = dir1;
    f /= dir2;
    f /= dir3;
    f /= dir4;
    f /= dir5;

    return f.string();
}

void Path::fillBinaryPackagesList(std::vector<std::string>& pkglist)
{
    std::string header = "Packages from: ";
    header += path().getBinaryPackagesDir();
    pkglist.clear();
    pkglist.push_back(header);
    PathList pkgs = path().getBinaryPackages();
    std::sort(pkgs.begin(), pkgs.end());
    PathList::const_iterator itb = pkgs.begin();
    PathList::const_iterator ite = pkgs.end();
    for (; itb!=ite; itb++){
        pkglist.push_back(*itb);
    }
    return;
}

std::string Path::getCurrentPath()
{
    FSpath current = FSpath::current_path();

    return current.string();
}

bool Path::exist(const std::string& filename)
{
    FSpath p(filename);

    return p.exists();
}

bool Path::existDirectory(const std::string& filename)
{
    FSpath p(filename);

    return p.is_directory();
}

bool Path::existFile(const std::string& filename)
{
    FSpath p(filename);

    return p.is_file();
}

std::string Path::filename(const std::string& filename)
{
    FSpath path(filename);

    return path.filename();
}

std::string Path::basename(const std::string& filename)
{
    FSpath path(filename);

    return path.basename();
}

std::string Path::dirname(const std::string& filename)
{
    FSpath path(filename);

    return path.parent_path().string();
}

std::string Path::extension(const std::string& filename)
{
    FSpath path(filename);

    return path.extension();
}

std::ostream& operator<<(std::ostream& out, const Path& p)
{
    out << "prefix................: " << p.getPrefixDir() << "\n"
        << "\n"
        << "pixmap................: " << p.getPixmapDir() << "\n"
        << "glade.................: " << p.getGladeDir() << "\n"
        << "\n"
        << "vle home..............: " << p.getHomeDir() << "\n"
        << "packages..............: " << p.getBinaryPackagesDir() << "\n"
        << "\n";
    out << "Real simulators list..:\n" << p.getSimulatorDirs() << "\n"
        << "Real output list......:\n" << p.getOutputDirs() << "\n"
        << "Real modeling list....:\n" << p.getModelingDirs() << "\n"
        << "Real stream list......:\n" << p.getStreamDirs() << "\n"
        << std::endl;

    return out;
}

std::string Path::getParentPath(const std::string& pathfile)
{
    FSpath path(pathfile);

    return path.parent_path().string();
}

}} // namespace vle utils
