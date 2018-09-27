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

#ifndef VLE_UTILS_PACKAGETABLE_HPP
#define VLE_UTILS_PACKAGETABLE_HPP

#include <set>
#include <string>
#include <vle/DllDefines.hpp>

namespace vle {
namespace utils {

class VLE_API PackageTable
{
public:
    using table_t = std::set<std::string>;
    using index = table_t::const_iterator;
    using const_iterator = table_t::const_iterator;
    using iterator = table_t::iterator;
    using size_type = table_t::size_type;

    PackageTable();

    /**
     * @brief Get an index to the specified package, if package does not
     * exist it is added.
     * @param package The package to get or add.
     * @return An index to the existing or newly package.
     * @throw utils::ArgError if package is empty.
     */
    index get(const std::string& package);

    /**
     * @brief Erase the specificied package from the list.
     * @param id The identifiant of the package to delete.
     * @throw utils::ArgError if package is the current package.
     */
    void remove(index id);

    const_iterator begin() const
    {
        return m_table.begin();
    }
    const_iterator end() const
    {
        return m_table.end();
    }
    size_type size() const
    {
        return m_table.size();
    }
    bool empty() const
    {
        return m_table.empty();
    }

private:
    table_t m_table;
    const_iterator m_current;
};
}
} // namespace vle utils

#endif
