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

#include <windows.h>
#include <vle/utils/details/UtilsWin.hpp>

namespace vle { namespace utils {

std::string UtilsWin::convertPathTo83Path(const std::string& path)
{
    std::string newvalue(path);
    DWORD lenght;

    lenght = GetShortPathName(path.c_str(), NULL, 0);
    if (lenght > 0) {
        TCHAR* buffer = new TCHAR[lenght];
        lenght = GetShortPathName(path.c_str(), buffer, lenght);
        if (lenght > 0) {
            newvalue.assign(static_cast < char* >(buffer));
        }
        delete [] buffer;
    }

    return newvalue;
}

}} // namespace vle utils
