/** 
 * @file Port.hpp
 * @brief 
 * @author The vle Development Team
 * @date Wed, 18 Apr 2007 14:27:10 +0200
 */

/*
 * Copyright (C) 2007 - The vle Development Team
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

#ifndef VLE_VPZ_PORT_HPP
#define VLE_VPZ_PORT_HPP

#include <vle/vpz/Base.hpp>

namespace vle { namespace vpz {

    class In : public Base
    {
    public:
        In() { }

        virtual ~In() { }

        virtual void write(std::ostream& /* out */) const
        { }

        virtual Base::type getType() const
        { return IN; }
    };

    class Out : public Base
    {
    public:
        Out() { }

        virtual ~Out() { }

        virtual void write(std::ostream& /* out */) const
        { }

        virtual Base::type getType() const
        { return OUT; }
    };

    class Init : public Base
    {
    public:
        Init() { }

        virtual ~Init() { }

        virtual void write(std::ostream& /* out */) const
        { }

        virtual Base::type getType() const
        { return INIT; }
    };

    class State : public Base
    {
    public:
        State() { }

        virtual ~State() { }

        virtual void write(std::ostream& /* out */) const
        { }

        virtual Base::type getType() const
        { return STATE; }
    };

    class Port : public Base
    {
    public:
        Port() { }

        virtual ~Port() { }

        virtual void write(std::ostream& /* out */) const
        { }

        virtual Base::type getType() const
        { return PORT; }
    };

}} // namespace vle vlz

#endif
