/**
 * @file src/vle/data/SQLResult.hpp
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




#ifndef VLE_DATA_SQL_RESULT_HPP
#define VLE_DATA_SQL_RESULT_HPP

#include <vle/data/Result.hpp>
#include <vle/data/SQLReader.hpp>
#include <string>
#include <sqlxx.h>
#include <strutilsxx.h>

namespace vle { namespace data {

    class SQLResult:public Result
    {
    private:
        sqlxx::CSQLResult *value;

        SQLResult(sqlxx::CSQLResult*);

    public:
        virtual ~SQLResult();

        virtual bool next();
        virtual std::string get(const std::string& name) const;
        virtual std::string get(unsigned int index) const;
        virtual std::string getColumnName(unsigned int index) const;
        virtual unsigned int getColumnNumber() const;
        virtual unsigned int getRowNumber() const;

        friend class SQLReader;

    };

}} // namespace vle data

#endif
