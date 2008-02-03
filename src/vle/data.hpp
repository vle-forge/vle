/**
 * @file vle/data.hpp
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


#ifndef VLE_DATA_DATA_HPP
#define VLE_DATA_DATA_HPP

#include <vle/data/CSVReader.hpp>
#include <vle/data/CSVResult.hpp>
#include <vle/data/DataReader.hpp>
#include <vle/data/DataWriter.hpp>
#include <vle/data/Result.hpp>
#include <vle/data/SQLReader.hpp>
#include <vle/data/SQLResult.hpp>
#include <vle/data/SQLWriter.hpp>

namespace vle {

    /** 
     * @brief This namespace provides access to undefined type. At this
     * time we propose database via an ODBC driver (libsqlxx) and a CSV files
     * access.
     */
    namespace data {

    } // namespace data

} // namespace vle

#endif
