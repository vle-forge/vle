/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2016 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2016 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2016 INRA http://www.inra.fr
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
#include <vle/utils/Context.hpp>
#include <vle/utils/Filesystem.hpp>
#include <vle/version.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

namespace vle { namespace utils {

FSpath Context::findProgram(const std::string& exe)
{
    char* env_p = std::getenv("PATH");

    std::vector<std::string> splitVec;
    boost::split(splitVec, env_p, boost::is_any_of(":"),
            boost::token_compress_on);

    std::vector<std::string>::const_iterator itb = splitVec.begin();
    std::vector<std::string>::const_iterator ite = splitVec.end();
    for (; itb != ite; itb++) {
        FSpath p(*itb);
        p /= exe;
        if (p.exists())
            return p.string();
    }

    return {};
}

void Context::initHomeDir()
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

        FSpath p(homepath);
        p /= ".vle";

        m_home = p.string();
    }
}

void Context::initPrefixDir()
{
    m_prefix = VLE_PREFIX_DIR;
}

}} // namespace vle utils
