/**
 * @file utils/utils.hpp
 * @author The VLE Development Team.
 * @brief To simplify utils hpp including files.
 */

/*
 * Copyright (c) 2004, 2005 The vle Development Team
 *
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifndef VLE_VALUE_VALUE_HPP
#define VLE_VALUE_VALUE_HPP

#include <vle/value/Boolean.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Null.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Table.hpp>
#include <vle/value/Tuple.hpp>
#include <vle/value/Value.hpp>
#include <value/XML.hpp>



namespace vle {

    /** 
     * @brief The value namespace are used to be share by external event or to
     * develop state model. All class are manager using a reference counter from
     * Boost.org: boost::shared_ptr < type >.
     */
    namespace value {

    } // namespace value

} // namespace vle

#endif
