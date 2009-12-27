/**
 * @file vle/utils/Pool.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
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


#ifndef VLE_UTILS_POOL_HPP
#define VLE_UTILS_POOL_HPP

#include <vle/version.hpp>
#include <vle/utils/DllDefines.hpp>

#ifdef VLE_HAVE_POOL
#include <glibmm/thread.h>
#include <boost/pool/pool.hpp>
#include <boost/utility.hpp>
#include <vector>
#include <cassert>

namespace vle { namespace utils {

    template < typename T >
    class Pools
    {
    public:
        typedef boost::pool <> Pool;
        typedef std::vector < Pool* > VectorPool;
        typedef VectorPool::size_type size_type;
        typedef VectorPool::iterator iterator;
        typedef VectorPool::const_iterator const_iterator;

        Glib::Mutex poolMutex;

        /**
         * @brief Build a specified number of boost::pool.
         * @param size The number of boost::pool.
         */
        inline Pools(size_type size) :
            m_pools(size, 0)
        {}

        /**
         * @brief Destroy all pools attached.
         */
        inline ~Pools()
        {
            std::for_each(begin(), end(), boost::checked_deleter
                          < Pool > ());
        }

        /**
         * @brief Allocate the specified size into the boost::pool.
         * @param size The size to allocate.
         * @return A pointer to the newly allocated memory.
         */
        inline void* allocate(size_t size)
        {
            Glib::Mutex::Lock lock(poolMutex);
            assert(size < m_pools.size());

            if (not m_pools[size]) {
                m_pools[size] = new Pool (size);
            }
            return m_pools[size]->malloc();
        }

        /**
         * @brief Deallocate the datas for the specified pointer and size.
         * @param deletable The pointer to delete.
         * @param size The size to delete.
         */
        inline void deallocate(void* deletable, size_t size)
        {
            Glib::Mutex::Lock lock(poolMutex);
            assert(size < m_pools.size());
            if (deletable) {
                if (not m_pools[size]) {
                    ::operator delete(deletable);
                } else {
                    if (m_pools[size]->is_from(deletable)) {
                        m_pools[size]->free(deletable);
                    } else {
                        ::operator delete(deletable);
                    }
                }
            }
        }

    private:
        /**
         * @brief A set of boost::pool. Each boost::pool manages a specified
         * size of memory.
         */
        VectorPool m_pools;

        iterator begin() { return m_pools.begin(); }
        iterator end() { return m_pools.end(); }
        const_iterator begin() const { return m_pools.begin(); }
        const_iterator end() const { return m_pools.end(); }
        size_type size() const { return m_pools.size(); }
        Pool* get(size_type index) { return m_pools[index]; }
    };

}} // namespace vle utils

#endif // VLE_HAVE_POOL

#endif
