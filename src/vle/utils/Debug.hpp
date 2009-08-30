/**
 * @file vle/utils/Debug.hpp
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


#ifndef VLE_UTILS_DEBUG_HPP
#define VLE_UTILS_DEBUG_HPP

#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/utils/DllDefines.hpp>
#include <boost/format.hpp>

namespace vle {

    template < typename X >
        inline void Assert(bool test)
        {
            if (not test) {
                throw X();
            }
        }

    template < typename X >
        inline void Assert(bool test, const std::string& error)
        {
            if (not test) {
                throw X(error);
            }
        }

    template < typename X >
        inline void Assert(bool test, const fmt& error)
        {
            if (not test) {
                throw X(error);
            }
        }

    inline void Assert(bool test)
    {
        if (not test) {
            throw utils::InternalError();
        }
    }

    inline void Assert(bool test, const std::string& error)
    {
        if (not test) {
            throw utils::InternalError(error);
        }
    }

    inline void Assert(bool test, const fmt& error)
    {
        if (not test) {
            throw utils::InternalError(error);
        }
    }

} // namespace vle

#endif
