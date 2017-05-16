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

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <fstream>
#include <list>
#include <vle/utils/Context.hpp>
#include <vle/utils/ContextPrivate.hpp>
#include <vle/utils/Filesystem.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/vle.hpp>

namespace vle {
namespace utils {

Path
Context::findInstallPrefix()
{
    auto version = vle::version_abi();
    Path path;

    path = findProgram(vle::utils::format(
      "vle-%d.%d", std::get<0>(version), std::get<1>(version)));

    if (not path.empty()) {
        path = path.parent_path(); // remove filename
        path = path.parent_path(); // remove bin
        return path;
    }

#ifdef __APPLE__
    path = findLibrary(vle::utils::format(
      "libvle-%d.%d.dylib", std::get<0>(version), std::get<1>(version)));
#else
    path = findLibrary(vle::utils::format(
      "libvle-%d.%d.so", std::get<0>(version), std::get<1>(version)));
#endif

    if (not path.empty()) {
        path = path.parent_path(); // remove filename
        path = path.parent_path(); // remove lib
        return path;
    }

    path = findProgram("vle");

    if (not path.empty()) {
        path = path.parent_path(); // remove filename
        path = path.parent_path(); // remove bin
        return path;
    }

#ifdef __APPLE__
    path = findLibrary("libvle.dylib");
#else
    path = findLibrary("libvle.so");
#endif

    if (not path.empty()) {
        path = path.parent_path(); // remove filename
        path = path.parent_path(); // remove lib
        return path;
    }

    return path;
}

Path
Context::findLibrary(const std::string& lib)
{
#ifdef __APPLE__
    char* env_p = std::getenv("DYLD_LIBRARY_PATH");
#else
    char* env_p = std::getenv("LD_LIBRARY_PATH");
#endif

    std::vector<std::string> splitVec;
    boost::split(
      splitVec, env_p, boost::is_any_of(":"), boost::token_compress_on);

    splitVec.insert(splitVec.begin(), "/usr/lib");
    splitVec.insert(splitVec.begin(), "/usr/local/lib");
    splitVec.insert(splitVec.begin(), "/opt/lib");

    std::vector<std::string>::const_iterator itb = splitVec.begin();
    std::vector<std::string>::const_iterator ite = splitVec.end();
    for (; itb != ite; itb++) {
        Path p(*itb);
        p /= lib;
        if (p.exists())
            return p.string();
    }

    return {};
}

Path
Context::findProgram(const std::string& exe)
{
    char* env_p = std::getenv("PATH");

    std::vector<std::string> splitVec;
    boost::split(
      splitVec, env_p, boost::is_any_of(":"), boost::token_compress_on);

    std::vector<std::string>::const_iterator itb = splitVec.begin();
    std::vector<std::string>::const_iterator ite = splitVec.end();
    for (; itb != ite; itb++) {
        Path p(*itb);
        p /= exe;
        if (p.exists())
            return p.string();
    }

    return {};
}

void
Context::initHomeDir()
{
    m_pimpl->m_home.clear();

    readHomeDir();

    /*
     * If no VLE_HOME directory, we build $HOME/.vle directory.
     */
    if (m_pimpl->m_home.empty()) {
        const char* path_str = std::getenv("HOME");
        std::string homepath("");
        if (path_str) {
            homepath.assign(path_str);
        }

        Path p(homepath);
        p /= ".vle";

        m_pimpl->m_home = p.string();
    }
}

void
Context::initPrefixDir()
{
    m_pimpl->m_prefix = findInstallPrefix();
}
}
} // namespace vle utils
