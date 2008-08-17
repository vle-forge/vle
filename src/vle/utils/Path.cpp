/**
 * @file src/vle/utils/Path.cpp
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
#include <config.h>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

namespace vle { namespace utils {

Path* Path::mPath = 0;

const std::string& Path::getPrefixDir() const
{
    return m_prefix;
}

std::string Path::getPixmapsDir() const
{
    return buildPrefixSharePath(m_prefix, std::string("pixmaps"));
}

std::string Path::getGladeDir() const
{
    return buildPrefixSharePath(m_prefix, "glade");
}

std::string Path::getPixmapsFile(const std::string& file) const
{
    return buildPrefixSharePath(m_prefix, "pixmaps", file);
}

std::string Path::getGladeFile(const std::string& file) const
{
    return buildPrefixSharePath(m_prefix, "glade", file);
}

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

void Path::addModelDir(const std::string& dirname)
{
    if (isDirectory(dirname)) {
        m_model.push_back(dirname);
    }
}

void Path::addPluginDir(const std::string& dirname)
{
    addSimulatorDir(dirname);
    addStreamDir(dirname);
    addModelDir(dirname);
}

std::string Path::buildPrefixPath(const std::string& buf)
{
    std::list < std::string > lst;
    lst.push_front(VLE_PREFIX_DIR);
    lst.push_back(buf);
    return Glib::build_path(G_DIR_SEPARATOR_S, lst);
}

std::string Path::buildPrefixLibrariesPath(const std::string& prefix,
                                           const std::string& name)
{
    std::list < std::string > lst;
    lst.push_back(prefix);
    lst.push_back(VLE_LIBRARY_DIRS);
    lst.push_back(name);
    return Glib::build_path(G_DIR_SEPARATOR_S, lst);
}

std::string Path::buildPrefixSharePath(const std::string& prefix,
                                       const std::string& prg,
                                       const std::string& name)
{
    std::list < std::string > lst;
    lst.push_front(prefix);
    lst.push_back(VLE_SHARE_DIRS);
    lst.push_back(prg);
    if (not name.empty()) {
        lst.push_back(name);
    }
    return Glib::build_path(G_DIR_SEPARATOR_S, lst);
}

std::string Path::buildPath(const std::string& left, const std::string& right)
{
    std::list < std::string > lst;
    lst.push_front(left);
    lst.push_back(right);
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

Path::Path()
{
    if (initPath() == false) {
        throw InternalError("Path initialization failed.");
    }
}

}} // namespace vle utils
