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

#ifndef VLE_UTILS_DETAILS_LOCALPACKAGEMANAGER_HPP
#define VLE_UTILS_DETAILS_LOCALPACKAGEMANAGER_HPP

#include <vle/utils/RemoteManager.hpp>

namespace vle {
namespace utils {

struct LocalPackageManager
{
    /**
     * Try to extract the @e PackageId from the file @e
     * $VLE_HOME/local.pkg;
     *
     * @param [out] out To get the @e PackageId read from the file.
     *
     * @return true if success, false otherwise.
     */
    static bool extract(ContextPtr ctx, Packages* out);

    /**
     * Try to rebuild the @e PackageId from the package directory. This
     * function read for each package the description file.
     *
     * @param [out] out To get the @e PackageId build from package directory.
     *
     * @return true if success, false otherwise.
     */
    static bool rebuild(ContextPtr ctx, Packages* out);
};

struct RemotePackageManager
{
    /**
     * Try to extract the @e PackageId from the $VLE_HOME/[remote].pkg files.
     *
     * @param [out] out To get the @e PackageId read from the files.
     *
     * @return true is success, faled otherwise.
     */
    static bool extract(ContextPtr ctx, Packages* out);
};
}
}

#endif
