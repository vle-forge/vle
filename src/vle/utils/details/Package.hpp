/*
 * @file vle/utils/details/Package.hpp
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
 * A functor to detect updated packages.
 *
 * A package is updated if two @c PackageId have the same name but at least one
 * version number is different.
 */
struct PackageIdUpdate
    : public std::binary_function < PackageId, PackageId, bool >
{
    bool operator()(const PackageId& lhs, const PackageId& rhs) const
    {
        if (lhs.name == rhs.name) {
            return lhs.major != rhs.major or lhs.minor != rhs.minor or
                lhs.patch != rhs.patch;
        }

        return false;
    }
};

struct PackageIdClear
    : public std::unary_function < PackageId, void >
{
    void operator()(PackageId& lhs) const
    {
        lhs.size = 0;
        lhs.name.clear();
        lhs.distribution.clear();
        lhs.maintainer.clear();
        lhs.description.clear();
        lhs.url.clear();
        lhs.md5sum.clear();
        lhs.tags.clear();
        lhs.depends.clear();
        lhs.builddepends.clear();
        lhs.conflicts.clear();
        lhs.major = -1;
        lhs.minor = -1;
        lhs.patch = -1;
    }
};

typedef boost::unordered_multiset < PackageId,
                                    PackageIdHash,
                                    PackageIdEqual > PackagesIdSet;

inline std::ostream& operator<<(std::ostream& os, const PackageLinkId& b)
{
    static const char *sym[5] = { "=", "<", "<=", ">", ">=" };

    os << b.name;

    if (b.major >= 0) {
        os << " (" << sym[static_cast < unsigned >(b.op)] << " " << b.major;

        if (b.minor >= 0) {
            os << "." << b.minor;

            if (b.patch >= 0) {
                os << "." << b.patch;
            }
        }

        os << ")";
    }

    return os;
}

inline std::ostream& operator<<(std::ostream& os, const PackagesLinkId& b)
{
    std::copy(b.begin(),
              b.end(),
              std::ostream_iterator < PackageLinkId >(os, ","));

    return os;
}

inline std::ostream& operator<<(std::ostream& os, const Tags& b)
{
    std::copy(b.begin(),
              b.end(),
              std::ostream_iterator < std::string >(os, ","));

    return os;
}

inline std::ostream& operator<<(std::ostream& os, const PackageId& b)
{
    return os << "Package: " <<  b.name << "\n"
              << "Version: " << b.major
              << "." << b.minor
              << "." << b.patch << "\n"
              << "Depends: " << b.depends << "\n"
              << "Build-Depends: " << b.builddepends << "\n"
              << "Conflicts: " << b.conflicts << "\n"
              << "Maintainer: " << b.maintainer << "\n"
              << "Description: " << b.description << "\n" << " ." << "\n"
              << "Tags: " << b.tags << "\n"
              << "Url: " << b.url << "\n"
              << "Size: " << b.size << "\n"
              << "MD5sum: " << b.md5sum << "\n";
}

inline std::ostream& operator<<(std::ostream& os, const PackagesIdSet& b)
{
    std::copy(b.begin(),
              b.end(),
              std::ostream_iterator < PackageId >(os));

    return os;
}

}} // namespace vle utils

#endif /* VLE_UTILS_DETAILS_PACKAGE_HPP */
