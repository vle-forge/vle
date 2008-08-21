/**
 * @file vle/vpz/Trame.hpp
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


#ifndef VLE_VPZ_TRAME_HPP
#define VLE_VPZ_TRAME_HPP

#include <vle/vpz/Base.hpp>
#include <list>
#include <ostream>
#include <string>

namespace vle { namespace vpz {

    /**
     * @brief Trame inherits from Base class and build a hierarchy of Trame
     * (EndTrame, VLETrame, ValueTrame, DelObservableTrame,
     * NewObservableTrame). Trame is a virtual class.
     */
    class Trame : public Base
    {
    public:
        /**
         * @brief Build a new Trame.
         */
        Trame()
        {}

        /**
         * @brief Nothing to delete.
         */
        virtual ~Trame()
        {}

        /**
         * @brief Get the type of this class.
         * @return Nothing, pure virtual function.
         */
        virtual Base::type getType() const = 0;

        /**
         * @brief Write the XML representation of this class.
         * @param out The output stream.
         */
        virtual void write(std::ostream& out) const = 0;
    };

    /**
     * @brief Define a list of trame.
     */
    typedef std::list < Trame* > TrameList;

    /**
     * @brief VLETrame inherits of Trame and is used to wrap all other Trame
     * into the OOV protocol.
     * @code
     * <vle_trame>
     *  [...] // other trame.
     * </vle_trame>
     * @endcode
     */
    class VLETrame : public Trame
    {
    public:
        /**
         * @brief Bulid an empty VLETrame.
         */
        VLETrame()
        {}

        /**
         * @brief Nothing to delete.
         */
        virtual ~VLETrame()
        {}

        /**
         * @brief Get the type of this class.
         * @return TRAME.
         */
        virtual Base::type getType() const
        { return Base::TRAME; }

        /**
         * @brief Write the XML representation of this class.
         */
        virtual void write(std::ostream& /* out */) const
        {}
    };

}} // namespace vle vpz

#endif
