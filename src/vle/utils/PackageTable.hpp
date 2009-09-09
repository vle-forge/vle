/**
 * @file vle/utils/PackageTable.hpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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

#ifndef VLE_UTILS_PACKAGETABLE_HPP
#define VLE_UTILS_PACKAGETABLE_HPP

#include <vle/utils/DllDefines.hpp>
#include <set>
#include <string>

namespace vle { namespace utils {

    class VLE_UTILS_EXPORT PackageTable
    {
    public:
        typedef std::set < std::string > table_t;
        typedef table_t::const_iterator index;
        typedef table_t::const_iterator const_iterator;
        typedef table_t::iterator iterator;
        typedef table_t::size_type size_type;

        PackageTable();

        /**
         * @brief Assign a new current package.
         * @param package The new package. Can be null if no package is
         * defined.
         */
        void current(const std::string& package);

        /**
         * @brief Get the current selected package.
         * @return A constant reference to the selected package.
         */
        const std::string& current() const { return *m_current; }

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

        const_iterator begin() const { return m_table.begin(); }
        const_iterator end() const { return m_table.end(); }
        size_type size() const { return m_table.size(); }
        bool empty() const { return m_table.empty(); }

    private:
        table_t m_table;
        const_iterator m_current;
    };

}} // namespace vle utils

#endif
