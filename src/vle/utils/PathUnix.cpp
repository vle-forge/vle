/*
 * @file vle/utils/PathUnix.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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

#include <vle/utils/Path.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/version.hpp>

#include <glibmm/miscutils.h>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

namespace vle { namespace utils {

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

}} // namespace vle utils
