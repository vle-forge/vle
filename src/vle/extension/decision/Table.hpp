/*
 * @file vle/extension/decision/Table.hpp
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


#ifndef VLE_EXTENSION_DECISION_TABLE_HPP
#define VLE_EXTENSION_DECISION_TABLE_HPP

#include <vle/extension/DllDefines.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>
#include <string>
#include <map>

namespace vle { namespace extension { namespace decision {

/**
 * @brief Table is a kind of associative containers that stores
 * elements formed by the combination of a std::string and a mapped value.
 */
template < typename T >
class Table
{
public:
    typedef std::map < std::string, T > Dict;
    typedef typename std::map < std::string, T >::value_type value_type;
    typedef typename std::map < std::string, T >::const_iterator const_iterator;
    typedef typename std::map < std::string, T >::iterator iterator;
    typedef typename std::map < std::string, T >::size_type size_type;

    /**
     * @brief Add a new value_type to the Table.
     *
     * @param name The key to add.
     * @param function The value to add.
     *
     * @throw utils::ArgError if the key already exists.
     */
    void add(const std::string& name, const T& function)
    {
        std::pair < iterator, bool > r = mLst.insert(
            std::make_pair(name, function));

        if (not r.second) {
            throw utils::ArgError(
                fmt(_("Decision: id `%1%' already exists")) % name);
        }
    }

    /**
     * @brief Delete the value_type from the Table.
     *
     * @param name The to of the value_type to delete.
     *
     * @throw utils::ArgError if the key does not exist.
     */
    void del(const std::string& name)
    {
        iterator it = get(name);

        mLst.erase(it);
    }

    /**
     * @brief Delete the value_type from the Table.
     *
     * @param it An iterator to the value_type to delete.
     */
    void del(iterator it)
    {
        mLst.erase(it);
    }

    /**
     * @brief Get an iterator from the list.
     *
     * @param name The key to find.
     *
     * @return An iterator.
     */
    iterator find(const std::string& name)
    {
        return mLst.find(name);
    }

    /**
     * @brief Get a constant iterator from the list.
     *
     * @param name The key to find.
     *
     * @return A constant iterator.
     */
    const_iterator find(const std::string& name) const
    {
        return mLst.find(name);
    }

    /**
     * @brief Get an iterator to the value_type referenced by the key.
     *
     * @param name The key to find.
     *
     * @return An iterator.
     *
     * @throw utils::ArgError if the key does not exist.
     */
    iterator get(const std::string& name)
    {
        iterator it = mLst.find(name);

        if (it == mLst.end()) {
            throw utils::ArgError(
                fmt(_("Decision: unknow id `%1%'")) % name);
        }

        return it;
    }

    /**
     * @brief Get a constant iterator to the value_type referenced by the key.
     *
     * @param name The key to find.
     *
     * @return A constant iterator.
     *
     * @throw utils::ArgError if the key does not exist.
     */
    const_iterator get(const std::string& name) const
    {
        const_iterator it = mLst.find(name);

        if (it == mLst.end()) {
            throw utils::ArgError(
                fmt(_("Decision: unknow id `%1%'")) % name);
        }

        return it;
    }

    /**
     * @brief Test if the key exists in the Table.
     *
     * @param name The key to find.
     *
     * @return True if the key exist, false otherwise.
     */
    bool exist(const std::string& name) const
    {
        return mLst.find(name) != mLst.end();
    }

    /**
     * @brief Test if the container is empty.
     *
     * @return True if the container is empty, false otherwise.
     */
    bool empty() const
    {
        return mLst.empty();
    }

    /**
     * @brief Get a reference to the template type T referenced by the key.
     *
     * @param name The key to find.
     *
     * @return A reference to the template type T.
     *
     * @throw utils::ArgError if the key does not exist.
     */
    T& operator[](const std::string& name)
    {
        return get(name)->second;
    }

    /**
     * @brief Get a const reference to the template type T referenced by the
     * key.
     *
     * @param name The key to find.
     *
     * @return A constant reference to the template type T.
     *
     * @throw utils::ArgError if the key does not exist.
     */
    const T& operator[](const std::string& name) const
    {
        return get(name)->second;
    }

    iterator begin() { return mLst.begin(); }
    const_iterator begin() const { return mLst.begin(); }
    iterator end() { return mLst.end(); }
    const_iterator end() const { return mLst.end(); }
    size_type size() const { return mLst.size(); }

private:
    Dict mLst;
};

}}} // namespace vle model decision

#endif
