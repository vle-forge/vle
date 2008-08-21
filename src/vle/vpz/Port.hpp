/**
 * @file vle/vpz/Port.hpp
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


#ifndef VLE_VPZ_PORT_HPP
#define VLE_VPZ_PORT_HPP

#include <vle/vpz/Base.hpp>

namespace vle { namespace vpz {

    /**
     * @brief The In port, only used by the SaxParser.
     */
    class In : public Base
    {
    public:
        /**
         * @brief Build a new In.
         */
        In()
        {}

        /**
         * @brief Nothing to delete.
         */
        virtual ~In()
        {}

        /**
         * @brief Nothing to output.
         */
        virtual void write(std::ostream& /* out */) const
        {}

        /**
         * @brief Get the type of this class.
         * @return IN.
         */
        virtual Base::type getType() const
        { return IN; }
    };

    /**
     * @brief The Out port, only used by the SaxParser.
     */
    class Out : public Base
    {
    public:
        /**
         * @brief Build a new Out.
         */
        Out()
        {}

        /**
         * @brief Nothing to delete.
         */
        virtual ~Out()
        {}

        /**
         * @brief Nothing to output.
         */
        virtual void write(std::ostream& /* out */) const
        {}

        /**
         * @brief Get the type of this class.
         * @return OUT.
         */
        virtual Base::type getType() const
        { return OUT; }
    };

    /**
     * @brief The Init port, only used by the SaxParser.
     */
    class Init : public Base
    {
    public:
        /**
         * @brief Build a new Init.
         */
        Init()
        {}

        /**
         * @brief Nothing to delete.
         */
        virtual ~Init()
        {}

        /**
         * @brief Nothing to output.
         */
        virtual void write(std::ostream& /* out */) const
        {}

        /**
         * @brief Get the type of this class.
         * @return INIT.
         */
        virtual Base::type getType() const
        { return INIT; }
    };

    /**
     * @brief The State port, only used by the SaxParser.
     */
    class State : public Base
    {
    public:
        /**
         * @brief Build a new State.
         */
        State()
        {}

        /**
         * @brief Nothing to delete.
         */
        virtual ~State()
        {}

        /**
         * @brief Nothing to output.
         */
        virtual void write(std::ostream& /* out */) const
        {}

        /**
         * @brief Get the type of this class.
         * @return STATE.
         */
        virtual Base::type getType() const
        { return STATE; }
    };

    /**
     * @brief The Port, only used by the SaxParser.
     */
    class Port : public Base
    {
    public:
        /**
         * @brief Build a new Port.
         */
        Port()
        {}

        /**
         * @brief Nothing to delete.
         */
        virtual ~Port()
        {}

        /**
         * @brief Nothing to output.
         */
        virtual void write(std::ostream& /* out */) const
        {}

        /**
         * @brief Get the type of this class.
         * @return PORT.
         */
        virtual Base::type getType() const
        { return PORT; }
    };

}} // namespace vle vlz

#endif
