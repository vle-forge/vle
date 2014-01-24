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

#ifndef VLE_UTILS_DETAILS_PACKAGE_HPP
#define VLE_UTILS_DETAILS_PACKAGE_HPP

#include <vle/utils/RemoteManager.hpp>
#include <boost/functional/hash.hpp>
#include <boost/unordered_set.hpp>
#include <functional>
#include <ostream>
#include <cstring>

namespace vle { namespace utils {

/**
 * A functor to hash @c PackageId.
 *
 */
struct PackageIdHash
    : public std::unary_function < PackageId, std::size_t >
{
    std::size_t operator()(const PackageId& lhs) const
    {
        return boost::hash_value(lhs.name);
    }
};

/**
 * A functor to detect equal packages.
 *
 * A package is equal it tow @c PackageId have the same name.
 */
struct PackageIdEqual
    : public std::binary_function < PackageId, PackageId, bool >
{
    bool operator()(const PackageId& lhs, const PackageId& rhs) const
    {
        return lhs.name == rhs.name;
    }
};

/**
 * A functor to detect packages to update
 *
 * A package can be updated if the two @c PackageId have the same name and if
 * the second @c PackageId has a more recent version
 */
struct PackageIdUpdate
    : public std::binary_function < PackageId, PackageId, bool >
{
    bool operator()(const PackageId& lhs, const PackageId& rhs) const
    {
        if (lhs.name != rhs.name) {
            return false;
        }
        if (rhs.major > lhs.major) {
            return true;
        } else if (rhs.major < lhs.major) {
            return false;
        }
        if (rhs.minor > lhs.minor) {
            return true;
        } else if (rhs.minor < lhs.minor) {
            return false;
        }
        if (rhs.patch > lhs.patch) {
            return true;
        } else if (rhs.patch < lhs.patch) {
            return false;
        }
        return false;
    }
};

inline void cleanup(PackageId& pkg)
{
    pkg.size = 0;
    pkg.name.clear();
    pkg.distribution.clear();
    pkg.maintainer.clear();
    pkg.description.clear();
    pkg.url.clear();
    pkg.md5sum.clear();
    pkg.tags.clear();
    pkg.depends.clear();
    pkg.builddepends.clear();
    pkg.conflicts.clear();
    pkg.major = -1;
    pkg.minor = -1;
    pkg.patch = -1;
}

inline void cleanup(PackageLinkId& pkglinkid)
{
    pkglinkid.name.clear();
    pkglinkid.major = -1;
    pkglinkid.minor = -1;
    pkglinkid.patch = -1;
    pkglinkid.op = PACKAGE_OPERATOR_EQUAL;
}

struct PackageIdClear
    : public std::unary_function < PackageId, void >
{
    void operator()(PackageId& lhs) const
    {
        cleanup(lhs);
    }
};

typedef boost::unordered_multiset < PackageId,
                                    PackageIdHash,
                                    PackageIdEqual > PackagesIdSet;

inline std::ostream& operator<<(std::ostream& os, const PackagesIdSet& b)
{
    std::copy(b.begin(),
              b.end(),
              std::ostream_iterator < PackageId >(os));

    return os;
}

}} // namespace vle utils

#endif /* VLE_UTILS_DETAILS_PACKAGE_HPP */
