/** 
 * @file data/Result.hpp
 * @brief The result of request class.
 * @author The vle Development Team
 * @date lun, 23 jan 2006 14:04:28 +0100
 */

/*
 * Copyright (C) 2004, 05, 06 - The vle Development Team
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

#ifndef VLE_DATA_RESULT_HPP
#define VLE_DATA_RESULT_HPP

#include <string>

namespace vle { namespace data {

    class Result
    {
    public:
        Result() { }
        virtual ~Result() { }

        virtual bool next() =0;
        virtual std::string get(const std::string& name) const =0;
        virtual std::string get(unsigned int index) const =0;
        virtual std::string getColumnName(unsigned int index) const =0;
        virtual unsigned int getColumnNumber() const =0;
        virtual unsigned int getRowNumber() const =0;
    };

}} // namespace vle data

#endif
