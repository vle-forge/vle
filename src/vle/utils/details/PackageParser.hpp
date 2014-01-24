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

#ifndef VLE_UTILS_DETAILS_PACKAGEPARSER_HPP
#define VLE_UTILS_DETAILS_PACKAGEPARSER_HPP

#include <vle/utils/RemoteManager.hpp>
#include <string>

namespace vle { namespace utils {

/**
 * A class to parse and extract packages from package files.
 *
 * @code
 * #include <iostream>
 * #include <iterator>
 * #include <vle/utils/PackageParser.hpp>
 *
 * int main(int argc, char *argv[])
 * {
 *     (void)argc;
 *     (void)argv;
 *
 *     vle::utils::PackageParser parser;
 *
 *     parser.extract("/home/homer/.vle/package.1.1");
 *     parser.extract("/home/homer/.vle/package.1.1.vle");
 *     parser.extract("/home/homer/.vle/package.1.1.record");
 *
 *     std::copy(parser.begin(),
 *               parser.end(),
 *               std::iostream_iterator < vle::utils::Package >(
 *                        std::cout, "\n"));
 * }
 * @endcode
 */
class PackageParser
{
public:
    typedef Packages::value_type value_type;
    typedef Packages::iterator iterator;
    typedef Packages::const_iterator const_iterator;
    typedef Packages::size_type size_type;

    PackageParser();

    ~PackageParser();

    /**
     * Extracts from the specified filepath string, all packages.
     *
     * @param filepath The file to open.
     * @param distribution The distribution to assign for package.
     *
     * @throw std::exception if failure.
     *
     * @return True if success, false otherwise (and log with @c
     * utils::Trace subsytem).
     */
    bool extract(const std::string& filepath, const std::string& distribution);

    iterator begin();

    const_iterator begin() const;

    iterator end();

    const_iterator end() const;

    size_type size() const;

    bool empty() const;

private:
    PackageParser(const PackageParser&);
    PackageParser& operator=(const PackageParser&);

    Packages m_packages; /** The list of extracted packages. */
};

inline PackageParser::PackageParser()
{
}

inline PackageParser::~PackageParser()
{
}

inline Packages::iterator PackageParser::begin()
{
    return m_packages.begin();
}

inline Packages::const_iterator PackageParser::begin() const
{
    return m_packages.begin();
}

inline Packages::iterator PackageParser::end()
{
    return m_packages.end();
}

inline Packages::const_iterator PackageParser::end() const
{
    return m_packages.end();
}

inline Packages::size_type PackageParser::size() const
{
    return m_packages.size();
}

inline bool PackageParser::empty() const
{
    return m_packages.empty();
}

}} // namespace vle utils

#endif /* VLE_UTILS_DETAILS_PACKAGEPARSER_HPP */
