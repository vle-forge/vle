/**
 * @file vle/value/Pools.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
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


#ifndef VLE_VALUE_POOLS_HPP
#define VLE_VALUE_POOLS_HPP

#include <vle/utils/Pool.hpp>
#include <vle/value/DllDefines.hpp>
#include <vle/version.hpp>

#ifdef VLE_HAVE_POOL
namespace vle { namespace value {

    class Value;

    /**
     * @brief A thread-safe singleton to manage a set of boost::pools defined in
     * the utils::Pools class.
     */
    class VLE_VALUE_EXPORT Pools
    {
    public:
        /**
         * @brief Thread-Safe access to the Pools singleton.
         * @return A reference to the boost::pools.
         */
        static utils::Pools < Value >& pools();

        /**
         * @brief Initialize the singleton.
         */
        static void init()
        { pools(); }

        /**
         * @brief Delete the singleton.
         */
        static void kill()
        { delete m_pool; m_pool = 0; }

    private:
        /**
         * @brief The singleton object.
         */
        static Pools* m_pool;

        /**
         * @brief The set of boost::pool for Value class.
         */
        utils::Pools < Value > m_pools;

        ///
        ////
        ///

        /**
         * @brief Build a new utils::Pools with a default size defined to the
         * max size of the Value (Boolean, Integer, Map, Matrix, Null, Set,
         * String, Table, Tuple or Xml).
         */
        Pools();

        /**
         * @brief Private copy constructor.
         * @param other
         */
        Pools(const Pools& other);

        /**
         * @brief Private assign operator.
         * @param other
         * @return
         */
        Pools& operator=(const Pools& /* other */) { return *this; }
    };

    inline utils::Pools < Value >& Pools::pools()
    {
        if (not m_pool) {
            m_pool = new Pools();
        }
        return m_pool->m_pools;
    }

}} // namespace vle

#endif
#endif

