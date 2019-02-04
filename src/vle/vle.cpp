/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * https://www.vle-project.org
 *
 * Copyright (c) 2003-2018 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2018 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2018 INRA http://www.inra.fr
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

#include <vle/utils/Tools.hpp>
#include <vle/vle.hpp>

#ifdef _WIN32
#include <vle/utils/details/UtilsWin.hpp>
#endif

#define _VLE_STRINGIFY(x) #x
#define VLE_STRINGIFY(x) _VLE_STRINGIFY(x)

namespace vle {

std::tuple<int, int, int, std::string>
version()
{
    return std::make_tuple(VERSION_MAJOR,
                           VERSION_MINOR,
                           VERSION_PATCH,
                           VLE_STRINGIFY(VERSION_EXTRA));
}

std::tuple<int, int>
version_abi()
{
    return std::make_tuple(VERSION_MAJOR, VERSION_MINOR);
}

std::string
string_version()
{
#ifdef VERSION_EXTRA
    return vle::utils::format(
      "%d.%d.%d", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
#else
    return vle::utils::format("%d.%d.%d-%s",
                              VERSION_MAJOR,
                              VERSION_MINOR,
                              VERSION_PATCH,
                              VERSION_EXTRA);
#endif
}

std::string
string_version_abi()
{
    return vle::utils::format("%d.%d", VERSION_MAJOR, VERSION_MINOR);
}

} // namespace vle
