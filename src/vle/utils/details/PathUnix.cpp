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
#include <fstream>

#include <vle/utils/Path.hpp>
#include <vle/version.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

namespace vle { namespace utils {


std::string Path::findProgram(const std::string& exe)
{
    char* env_p = std::getenv("PATH");

    std::vector<std::string> splitVec;
    boost::split(splitVec, env_p, boost::is_any_of(":"),
            boost::token_compress_on);

    std::vector<std::string>::const_iterator itb = splitVec.begin();
    std::vector<std::string>::const_iterator ite = splitVec.end();
    for (; itb != ite; itb++) {
        boost::filesystem::path p(*itb);
        p /= exe;
        if (boost::filesystem::exists(p)) {
            return p.native();
        }
    }
    return "";
}

void Path::initHomeDir()
{
    m_home.clear();

    readHomeDir();

    /*
     * If no VLE_HOME directory, we build $HOME/.vle directory.
     */
    if (m_home.empty()) {
        const char* path_str = std::getenv("HOME");
        std::string homepath("");
        if (path_str) {
            homepath.assign(path_str);
        }
        m_home = utils::Path::buildDirname(homepath, ".vle");
    }
}

void Path::initPrefixDir()
{
    m_prefix.assign(VLE_PREFIX_DIR);
}

std::string Path::getTempFile(const std::string& prefix,
                              std::ofstream* file)
{
    std::string result;

    if (file and not file->is_open()) {
        result = prefix + "XXXXXX";
        boost::system::error_code ec;
        result =  utils::Path::buildFilename(
                boost::filesystem::temp_directory_path(ec).string(),
                result);

        char *buffer = new char[result.size()];
        std::strncpy(buffer, result.c_str(), result.size());

        int fd = ::mkstemp(buffer);
        if (fd != -1) {
            file->open(buffer, std::ios_base::trunc | std::ios_base::out |
                       std::ios_base::binary);
            close(fd);

            if (file->is_open()) {
                result.assign(buffer, result.size());
            } else {
                result.clear();
            }
        }

        delete[] buffer;
    }

    return result;
}

}} // namespace vle utils
