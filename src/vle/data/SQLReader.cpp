/**
 * @file src/vle/data/SQLReader.cpp
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




#include <vle/data/SQLReader.hpp>
#include <vle/data/SQLResult.hpp>

namespace vle { namespace data {

    std::map < std::string , SQLReader* > SQLReader::databases;

    SQLReader::SQLReader(const std::string & databaseName, type type,
			 const std::string & userName, 
			 const std::string & password,
			 const std::string & hostName, 
			 unsigned int port) {
      SQL.setDatabase(databaseName);
      switch (type) {
      case MYSQL: SQL.setType(SQLXX_MYSQL); break;
      case POSTGRES: SQL.setType(SQLXX_POSTGRES); break;
      case ODBC: SQL.setType(SQLXX_ODBC); break;
      }
      if (userName != "") SQL.setUsername(userName);
      if (password != "") SQL.setPassword(password);
      if (hostName != "") SQL.setHostname(hostName);
      if (port != 0) SQL.setPort(port);
      SQL.connect();
    }

    void SQLReader::close() {
      std::map < std::string , SQLReader* >::iterator it = databases.find(SQL.getDatabase());

      SQL.disconnect();
      delete it->second;
      databases.erase(it,it);
    }

    SQLReader* SQLReader::open(const std::string & databaseName, 
			       type type, const std::string & userName,
			       const std::string & password, 
			       const std::string & hostName,
			       unsigned int port) {
      SQLReader* r = NULL;
      std::map < std::string , SQLReader* >::iterator it = databases.find(databaseName);

      if (it == databases.end()) {
	r = new SQLReader(databaseName,type,userName,password,hostName,port);
	databases[databaseName] = r;
      }
      else r = it->second;
      return r;
    }

    Result* SQLReader::exec(const std::string& request) {
      return new SQLResult(SQL.openQuery(request));
    }

}} // namespace vle data
