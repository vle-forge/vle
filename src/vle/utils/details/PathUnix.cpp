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
#include <glibmm/miscutils.h>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

namespace vle { namespace utils {

std::string Path::findProgram(const std::string& exe)
{
    std::string res("");
    res = Glib::find_program_in_path(exe);
    return res;
}

void Path::initHomeDir()
{
    m_home.clear();

    readHomeDir();

    /*
     * If no VLE_HOME directory, we build $HOME/.vle directory.
     */
    if (m_home.empty()) {
        std::list < std::string > lst;
        lst.push_back(Glib::get_home_dir());
        lst.push_back(".vle");
        m_home = Glib::build_path(G_DIR_SEPARATOR_S, lst);
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

        result = Glib::build_filename(Glib::get_tmp_dir(),
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
