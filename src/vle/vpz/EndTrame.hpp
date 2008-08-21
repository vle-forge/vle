/**
 * @file vle/vpz/EndTrame.hpp
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


#ifndef VLE_VPZ_ENDTRAME_HPP
#define VLE_VPZ_ENDTRAME_HPP

#include <vle/vpz/Trame.hpp>

namespace vle { namespace vpz {

    /**
     * @brief The EndTrame is send to close the communication between client and
     * server oov.
     */
    class EndTrame : public Trame
    {
    public:
        /**
         * @brief Build a new EndTrame with the specific time.
         * @param time The time of this EndTrame.
         */
        EndTrame(const std::string& time)
            : m_time(time)
        {}

        /**
         * @brief Nothing to delete.
         */
        virtual ~EndTrame()
        {}

        /**
         * @brief Get the type of this class.
         * @return TRAME.
         */
        virtual Base::type getType() const
        { return Base::TRAME; }

        /**
         * @brief Write the XML representation of this class.
         * @code
         * <trame type="end" date="654.123" />
         * @endcode
         * @param out output stream.
         */
        virtual void write(std::ostream& out) const;

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Get/Set functions
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Get the time of this trame.
         * @return The time.
         */
        const std::string& time() const
        { return m_time; }

    private:
        std::string     m_time;
    };

}}// namespace vle vpz

#endif
