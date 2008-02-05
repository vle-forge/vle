/**
 * @file src/vle/data/SQLReader.hpp
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




#ifndef VLE_DATA_SQL_READER_HPP
#define VLE_DATA_SQL_READER_HPP

#include <vle/data/DataReader.hpp>
#include <vle/data/Result.hpp>
#include <sqlxx.h>
#include <strutilsxx.h>
#include <map>

namespace vle { namespace data {

    class SQLReader:public DataReader
    {
    public:
        enum type { MYSQL, POSTGRES, ODBC };

        static SQLReader* open(const std::string & databaseName, 
                               type type = MYSQL,
                               const std::string & userName = "",
                               const std::string & password = "",
                               const std::string & hostName = "",
                               unsigned int port = 0);

        virtual ~SQLReader() { }
        virtual void close();
        virtual Result* exec(const std::string&);

    private:
        static std::map < std::string , SQLReader* > databases;
        sqlxx::CSQL SQL;

        SQLReader(const std::string & databaseName, 
                  type type, const std::string & userName, 
                  const std::string & password, const std::string & hostName,
                  unsigned int port);
    };

}} // namespace vle data

#endif
