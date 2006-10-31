/** 
 * @file Replicas.hpp
 * @brief 
 * @author The vle Development Team
 * @date mar, 31 jan 2006 17:18:23 +0100
 */

/*
 * Copyright (C) 2006 - The vle Development Team
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef VLE_VPZ_REPLICAS_HPP
#define VLE_VPZ_REPLICAS_HPP

#include <vector>
#include <glib/gtypes.h>
#include <vle/vpz/Base.hpp>

namespace vle { namespace vpz {

    /** 
     * @brief This class allow load and writing REPLICAS tag into project file
     * VPZ.
     */
    class Replicas : public Base
    {
    public:
        /** 
         * @brief The different type of replica. RANDOM for randomize seed,
         * ENUMERATE a enumeration of replica and LIST a list of replica with a
         * seed initialise by a value and increase of one.
         */
        enum Seed { RANDOM, ENUMERATE, LIST };

        Replicas();

        virtual ~Replicas();

        virtual void init(xmlpp::Element* elt);

        virtual void write(xmlpp::Element* elt) const;

        /** 
         * @brief Build a vector of seed randomly for a specified size.
         * 
         * @param number the size of the vector to build.
         * @param seed the seed to initialise random generator.
         */
        void setRandom(size_t number, guint32 seed);

        /** 
         * @brief Build a vector using copy of parameter.
         * 
         * @param lst a vector of seed to copy.
         */
        void setEnumerate(const std::vector < guint32 >& lst);

        /** 
         * @brief Build a vector with first case at begin, an all other with
         * begin + i.
         * 
         * @param number the size of the vector to build.
         * @param begin the first value to initialise vector.
         */
        void setList(size_t number, size_t begin);

        /** 
         * @brief Delete all number and restore replica settings to default.
         */
        void clear();

        /** 
         * @brief Build the list of seed using parameter provide by XML or by
         * specialized functions, setRandom, setEnumerate or setList.
         * 
         * @return a vector of seed.
         */
        const std::vector < guint32 >& getList();

        /** 
         * @brief Build th list of seed using parameter provide by XML or by
         * specialized functions, setRandom, setEnumerate or setList.
         * 
         * @return a vector of seed.
         */
        std::vector < guint32 > getNewList() const;

        /** 
         * @brief Get the current seed system.
         * 
         * @return Seed.
         */
        inline Seed seed() const { return m_seed; }

        /** 
         * @brief Get the number of replica.
         * 
         * @return Number of replica.
         */
        inline size_t number() const { return m_number; }

    private:
        size_t                  m_number;
        Seed                    m_seed;
        size_t                  m_begin;
        guint32                 m_seedvalue;
        std::vector < guint32 > m_list;
    };

}} // namespace vle vpz

#endif
