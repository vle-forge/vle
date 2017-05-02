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

#ifndef VLE_UTILS_DETAILS_UTILS_WIN_HPP
#define VLE_UTILS_DETAILS_UTILS_WIN_HPP

#include <vle/DllDefines.hpp>
#include <vle/utils/Filesystem.hpp>

namespace vle {
namespace utils {

/**
 * Utils functions for Win32
 */
class VLE_API UtilsWin
{
public:
    /**
     * A specific \b Win32 function to convert a path into a 8.3 path.
     * @param path, the path to convert
     * @return the path converted into 8.3 path on windows
     */
    static Path convertPathTo83Path(const Path& path);
};
}
} // namespace vle utils

#endif
