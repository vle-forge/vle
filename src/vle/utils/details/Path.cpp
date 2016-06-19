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

FSpath Path::getLocaleDir() const
{
    FSpath p(m_prefix);
    p /= "share";
    p /= "locale";

    return p;
}

FSpath Path::getHomeFile(const std::string& name) const
{
    FSpath p(m_home);
    p /= name;

    return p;
}

FSpath Path::getPixmapDir() const
{
    FSpath p(m_prefix);
    p /= VLE_SHARE_DIRS;
    p /= "pixmaps";

    return p;
}

FSpath Path::getPixmapFile(const std::string& file) const
{
    FSpath p(m_prefix);
    p /= VLE_SHARE_DIRS;
    p /= "pixmaps";
    p /= file;

    return p;
}

FSpath Path::getGladeDir() const
{
    FSpath p(m_prefix);
    p /= VLE_SHARE_DIRS;
    p /= "glade";

    return p;
}

FSpath Path::getGladeFile(const std::string& file) const
{
    FSpath p(m_prefix);
    p /= VLE_SHARE_DIRS;
    p /= "glade";
    p /= file;

    return p;
}

/*
 * packages path
 */

FSpath Path::getBinaryPackagesDir() const
{
    FSpath p(m_home);
    p /= pkgdirname;

    return p;
}

FSpath Path::getTemplateDir() const
{
    FSpath p(m_prefix);
    p /= VLE_SHARE_DIRS;
    p /= "template";

    return p;
}

FSpath Path::getTemplate(const std::string& name) const
{
    FSpath p(m_prefix);
    p /= VLE_SHARE_DIRS;
    p /= "template";
    p /= name;

    return p;
}

FSpath Path::getCurrentDir() const
{
    return FSpath::current_path();
}

std::vector<std::string> Path::getBinaryPackages()
{
    FSpath pkgs(Path::path().getBinaryPackagesDir());

    if (not pkgs.is_directory())
        throw utils::InternalError(
            (fmt(_("Package error: '%1%' is not a directory")) %
             Path::path().getBinaryPackagesDir().string()).str());

    std::vector <std::string> result;
    for (FSdirectory_iterator it(pkgs), end; it != end; ++it)
        if (it->is_directory())
            result.push_back(it->path().filename());

    return result;
}

//PathList Path::getBinaryLibraries()
//{
//    PathList result;
//    const PathList& dirs = Path::path().getSimulatorDirs();
//
//    for (auto elem : dirs) {
//        FSpath dir(elem);
//
//        if (not dir.is_directory())
//            throw utils::InternalError(
//                (fmt(_("Pkg list error: '%1%' is not a library directory")) %
//                 dir.string()).str());
//
//        for (FSdirectory_iterator jt(dir), end; jt != end; ++jt) {
//            if (jt->is_file()) {
//                std::string ext = jt->path().extension();
//
//#ifdef _WIN32
//                if (ext == ".dll")
//                    result.push_back(jt->path());
//#elif __APPLE__
//                if (ext == ".dylib")
//                    result.push_back(jt->path());
//#else
//                if (ext == ".so")
//                    result.push_back(jt->path());
//#endif
//            }
//        }
//    }
//
//    return result;
//}

void Path::initVleHomeDirectory()
{
    FSpath pkgs = getBinaryPackagesDir();

    if (not pkgs.exists())
        if (not FSpath::create_directories(getBinaryPackagesDir()))
            throw FileError(
                (fmt(_("Failed to build VLE_HOME directory (%1%)")) %
                 pkgs.string()).str());
}

bool Path::readEnv(const std::string& variable, PathList& out)
{
    const char* path_str = std::getenv(variable.c_str());
    std::string path("");
    if (path_str) {
        path.assign(path_str);
    }
    if (not path.empty()) {
        std::vector<std::string> result;
        boost::algorithm::split(result, path, boost::is_any_of(":"),
                                boost::algorithm::token_compress_on);

        auto it = std::remove_if(result.begin(), result.end(),
                                 [](const std::string& dirname)
                                 {
                                     FSpath p(dirname);
                                     return p.is_directory();
                                 });

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
    for (const auto& elem : paths)
        out << '\t' << elem.string() << '\n';

    return out;
}

void Path::fillBinaryPackagesList(std::vector<std::string>& pkglist)
{
    std::string header = "Packages from: ";
    header += path().getBinaryPackagesDir().string();
    pkglist.clear();
    pkglist.push_back(header);
    std::vector<std::string> pkgs = path().getBinaryPackages();
    std::sort(pkgs.begin(), pkgs.end());
    auto itb = pkgs.cbegin(), ite = pkgs.cend();
    for (; itb!=ite; itb++) {
        pkglist.push_back(*itb);
    }
    return;
}

std::ostream& operator<<(std::ostream& out, const Path& p)
{
    out << "prefix................: " << p.getPrefixDir().string() << "\n"
        << "\n"
        << "pixmap................: " << p.getPixmapDir().string() << "\n"
        << "glade.................: " << p.getGladeDir().string() << "\n"
        << "\n"
        << "vle home..............: " << p.getHomeDir().string() << "\n"
        << "packages..............: " << p.getBinaryPackagesDir().string()
        << "\n\n";
    out << "Real simulators list..:\n" << p.getSimulatorDirs() << "\n"
        << "Real output list......:\n" << p.getOutputDirs() << "\n"
        << "Real modeling list....:\n" << p.getModelingDirs() << "\n"
        << "Real stream list......:\n" << p.getStreamDirs() << "\n"
        << std::endl;

    return out;
}

}} // namespace vle utils
