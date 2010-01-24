/**
 * @file vle/vpz/Replicas.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
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


#ifndef VLE_VPZ_REPLICAS_HPP
#define VLE_VPZ_REPLICAS_HPP

#include <glib/gtypes.h>
#include <vle/vpz/Base.hpp>
#include <vle/vpz/DllDefines.hpp>

namespace vle { namespace vpz {

    /**
     * @brief This class allow load and writing REPLICAS tag into project file
     * VPZ.
     */
    class VLE_VPZ_EXPORT Replicas : public Base
    {
    public:
        /**
         * @brief Build a new Replicas with number an seed equal to zero.
         */
        Replicas() :
            m_number(0),
            m_seed(0)
        {}

        /**
         * @brief Nothing to delete.
         */
        virtual ~Replicas()
        {}

        /**
         * @brief Write the XML representation of this class.
         * @code
         * <replicas seed="123123" number="123" />
         * @endcode
         * @param out Output parameter.
         */
        virtual void write(std::ostream& out) const;

        /**
         * @brief Get the type of this class.
         * @return REPLICAS.
         */
        virtual Base::type getType() const
        { return REPLICAS; }

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Get/Set functions
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Get the current seed system.
         * @return Seed.
         */
        inline guint32 seed() const
        { return m_seed; }

        /**
         * @brief Set he current seed system.
         * @param seed The new seed.
         */
        inline void setSeed(const guint32 seed)
        { m_seed = seed; }

        /**
         * @brief Get the number of replicas.
         * @return Number of replicas.
         */
        inline size_t number() const
        { return m_number; }

        /**
         * @brief Set the number of replicas.
         * @param number of replicas.
         * @throw utils::ArgError if the number of replicas equal 0.
         */
        void setNumber(const size_t number);

    private:
        size_t                  m_number;
        guint32                 m_seed;
    };

}} // namespace vle vpz

#endif
