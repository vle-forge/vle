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

#include <vle/utils/Path.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/version.hpp>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

namespace vle { namespace utils {

Path* Path::mPath = 0;

std::string Path::getSimulatorDir() const
{
    return buildLibrariesPath("simulator");
}

std::string Path::getHomeSimulatorDir() const
{
    return buildHomePath("simulator");
}

std::string Path::getExamplesDir() const
{
    return buildSharePath("examples");
}

std::string Path::getExampleFile(const std::string& file) const
{
    return buildSharePath("examples", file);
}

std::string Path::getStreamDir() const
{
    return buildLibrariesPath("stream");
}

std::string Path::getHomeStreamDir() const
{
    return buildHomePath("stream");
}

std::string Path::getHomeFile(const std::string& name) const
{
    return Glib::build_filename(getHomeDir(), name);
}

std::string Path::getPixmapDir() const
{
    return buildSharePath("pixmaps");
}

std::string Path::getPixmapFile(const std::string& file) const
{
    return buildSharePath("pixmaps", file);
}

std::string Path::getGladeDir() const
{
    return buildSharePath("glade");
}

std::string Path::getGladeFile(const std::string& file) const
{
    return buildSharePath("glade", file);
}

/*
 * outputs
 */

std::string Path::getOutputDir() const
{
    return buildLibrariesPath("outputs");
}

std::string Path::getHomeOutputDir() const
{
    return buildHomePath("outputs");
}

std::string Path::getOutputFile(const std::string& file) const
{
    return buildSharePath("outputs", file);
}

std::string Path::getHomeOutputFile(const std::string& file) const
{
    return buildHomePath("outputs", file);
}

std::string Path::getOutputPixmapDir() const
{
    return buildSharePath("outputs", "pixmaps");
}

std::string Path::getHomeOutputPixmapDir() const
{
    return buildHomePath("outputs", "pixmaps");
}

std::string Path::getOutputPixmapFile(const std::string& file) const
{
    return buildSharePath("outputs", "pixmaps", file);
}

std::string Path::getHomeOutputPixmapFile(const std::string& file) const
{
    return buildHomePath("outputs", "pixmaps", file);
}

std::string Path::getOutputGladeDir() const
{
    return buildSharePath("outputs", "glade");
}

std::string Path::getHomeOutputGladeDir() const
{
    return buildHomePath("outputs", "glade");
}

std::string Path::getOutputGladeFile(const std::string& file) const
{
    return buildSharePath("outputs", "glade", file);
}

std::string Path::getHomeOutputGladeFile(const std::string& file) const
{
    return buildHomePath("outputs", "glade", file);
}

std::string Path::getOutputDocDir() const
{
    return buildSharePath("outputs", "doc");
}

std::string Path::getHomeOutputDocDir() const
{
    return buildHomePath("outputs", "doc");
}

std::string Path::getOutputDocFile(const std::string& file) const
{
    return buildSharePath("outputs", "doc", file);
}

std::string Path::getHomeOutputDocFile(const std::string& file) const
{
    return buildHomePath("outputs", "doc", file);
}

/*
 * conditions
 */

std::string Path::getConditionDir() const
{
    return buildLibrariesPath("conditions");
}

std::string Path::getHomeConditionDir() const
{
    return buildHomePath("conditions");
}

std::string Path::getConditionFile(const std::string& file) const
{
    return buildSharePath("conditions", file);
}

std::string Path::getHomeConditionFile(const std::string& file) const
{
    return buildHomePath("conditions", file);
}

std::string Path::getConditionPixmapDir() const
{
    return buildSharePath("conditions", "pixmaps");
}

std::string Path::getHomeConditionPixmapDir() const
{
    return buildHomePath("conditions", "pixmaps");
}

std::string Path::getConditionPixmapFile(const std::string& file) const
{
    return buildSharePath("conditions", "pixmaps", file);
}

std::string Path::getHomeConditionPixmapFile(const std::string& file) const
{
    return buildHomePath("conditions", "pixmaps", file);
}

std::string Path::getConditionGladeDir() const
{
    return buildSharePath("conditions", "glade");
}

std::string Path::getHomeConditionGladeDir() const
{
    return buildHomePath("conditions", "glade");
}

std::string Path::getConditionGladeFile(const std::string& file) const
{
    return buildSharePath("conditions", "glade", file);
}

std::string Path::getHomeConditionGladeFile(const std::string& file) const
{
    return buildHomePath("conditions", "glade", file);
}

std::string Path::getConditionDocDir() const
{
    return buildSharePath("conditions", "doc");
}

std::string Path::getHomeConditionDocDir() const
{
    return buildHomePath("conditions", "doc");
}

std::string Path::getConditionDocFile(const std::string& file) const
{
    return buildSharePath("conditions", "doc", file);
}

std::string Path::getHomeConditionDocFile(const std::string& file) const
{
    return buildHomePath("conditions", "doc", file);
}

/*
 * packages path
 */

std::string Path::getPackagesDir() const
{
    return buildHomePath("packages");
}

std::string Path::getPackageDir(const std::string& name) const
{
    return buildHomePath("packages", name);
}

std::string Path::getPackageSimulatorsDir(const std::string& name) const
{
    return buildHomePath("packages", name, "lib");
}

std::string Path::getPackageSourcesDir(const std::string& name) const
{
    return buildHomePath("packages", name, "src");
}

std::string Path::getPackageDatasDir(const std::string& name) const
{
    return buildHomePath("packages", name, "data");
}

std::string Path::getPackageExperimentDir(const std::string& name) const
{
    return buildHomePath("packages", name, "exp");
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

std::string Path::buildHomePath(const std::string& name) const
{
    std::list < std::string > lst;
    lst.push_front(m_home);
    lst.push_back(name);
    return Glib::build_path(G_DIR_SEPARATOR_S, lst);
}

std::string Path::buildHomePath(const std::string& directory,
                                const std::string& name) const
{
    std::list < std::string > lst;
    lst.push_front(m_home);
    lst.push_back(directory);
    lst.push_back(name);
    return Glib::build_path(G_DIR_SEPARATOR_S, lst);
}

std::string Path::buildHomePath(const std::string& dir1,
                                const std::string& dir2,
                                const std::string& name) const
{
    std::list < std::string > lst;
    lst.push_front(m_home);
    lst.push_back(dir1);
    lst.push_back(dir2);
    lst.push_back(name);
    return Glib::build_path(G_DIR_SEPARATOR_S, lst);
}

std::string Path::buildLibrariesPath(const std::string& name) const
{
    std::list < std::string > lst;
    lst.push_back(m_prefix);
    lst.push_back(VLE_LIBRARY_DIRS);
    lst.push_back(name);
    return Glib::build_path(G_DIR_SEPARATOR_S, lst);
}

std::string Path::buildLibrariesPath(const std::string& directory,
                                     const std::string& name) const
{
    std::list < std::string > lst;
    lst.push_back(m_prefix);
    lst.push_back(VLE_LIBRARY_DIRS);
    lst.push_back(directory);
    lst.push_back(name);
    return Glib::build_path(G_DIR_SEPARATOR_S, lst);
}

std::string Path::buildLibrariesPath(const std::string& dir1,
                                     const std::string& dir2,
                                     const std::string& name) const
{
    std::list < std::string > lst;
    lst.push_back(m_prefix);
    lst.push_back(VLE_LIBRARY_DIRS);
    lst.push_back(dir1);
    lst.push_back(dir2);
    lst.push_back(name);
    return Glib::build_path(G_DIR_SEPARATOR_S, lst);
}

std::string Path::buildSharePath(const std::string& name) const
{
    std::list < std::string > lst;
    lst.push_front(m_prefix);
    lst.push_back(VLE_SHARE_DIRS);
    lst.push_back(name);
    return Glib::build_path(G_DIR_SEPARATOR_S, lst);
}

std::string Path::buildSharePath(const std::string& directory,
                                 const std::string& name) const
{
    std::list < std::string > lst;
    lst.push_front(m_prefix);
    lst.push_back(VLE_SHARE_DIRS);
    lst.push_back(directory);
    lst.push_back(name);
    return Glib::build_path(G_DIR_SEPARATOR_S, lst);
}

std::string Path::buildSharePath(const std::string& dir1,
                                 const std::string& dir2,
                                 const std::string& name) const
{
    std::list < std::string > lst;
    lst.push_front(m_prefix);
    lst.push_back(VLE_SHARE_DIRS);
    lst.push_back(dir1);
    lst.push_back(dir2);
    lst.push_back(name);
    return Glib::build_path(G_DIR_SEPARATOR_S, lst);
}

bool Path::readEnv(const std::string& variable, PathList& out)
{
    std::string path(Glib::getenv(variable));
    if (not path.empty()) {
        PathList result;
        boost::algorithm::split(result, path, boost::is_any_of(":"),
                                boost::algorithm::token_compress_on);

        PathList::iterator it(std::remove_if(
                result.begin(), result.end(),
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
            throw FileError(boost::format(
                    "Path: VLE_HOME ('%1%') does not exist") % path);
        }
    } else {
        m_home.clear();
    }
}

Path::Path()
{
    if (initPath() == false) {
        throw InternalError("Path initialization failed.");
    }
}

std::ostream& operator<<(std::ostream& out, const Path::PathList& paths)
{
    Path::PathList::const_iterator it = paths.begin();
    while (it != paths.end()) {
        out << "\t" << *it << "\n";
        ++it;
    }
    return out;
}

std::ostream& operator<<(std::ostream& out, const Path& p)
{
    return out
        << "prefix................: " << p.getPrefixDir() << "\n"
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
        << "condition home doc....: " << p.getHomeConditionDocDir() << "\n";
}

}} // namespace vle utils
