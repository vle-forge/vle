/**
 * @file vle/data/CSVReader.cpp
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


#include <vle/data/CSVReader.hpp>
#include <vle/data/CSVResult.hpp>
#include <algorithm>

namespace vle { namespace data {

    CSVReader::CSVReaderMap CSVReader::map;

    // endofline: check for and consume \r, \n, \r\n, or EOF
    int CSVReader::Csv::endofline(char c)
    {
	int eol;

	eol = (c=='\r' or c=='\n' or fin.eof());
	if (c == '\r') {
		fin.get(c);
		if (!fin.eof() and c != '\n')
			fin.putback(c);	// read too far
	}
	return eol;
    }

    // getline: get one line, grow as needed
    int CSVReader::Csv::getline(std::string& str)
    {
      char c;
      bool eof;

      line = "";
      do {
	fin.get(c);
	if (!endofline(c)) line += c;
      }
      while (!endofline(c));
      split();
      str = line;
      eof = fin.eof();
      if (eof) fin.clear();
      return !eof;
    }

    // split: split line into fields
    int CSVReader::Csv::split()
    {
      std::string fld;
      unsigned int i, j;

      nfield = 0;
      if (line.length() == 0)
	return 0;
      i = 0;

      do {
	if (i < line.length() && line[i] == '"')
	  j = advquoted(line, fld, ++i);	// skip quote
	else
	  j = advplain(line, fld, i);
	if (nfield >= (int)field.size())
	  field.push_back(fld);
	else
	  field[nfield] = fld;
	nfield++;
	i = j + 1;
      } while (j < line.length());

      return nfield;
    }

    // advquoted: quoted field; return index of next separator
    int CSVReader::Csv::advquoted(const std::string& s, std::string& fld, int i)
    {
      unsigned int j;

      fld = "";
      for (j = i; j < s.length(); j++) {
	if (s[j] == '"' && s[++j] != '"') {
	  unsigned int k = s.find_first_of(fieldsep, j);
	  if (k > s.length())	// no separator found
	    k = s.length();
	  for (k -= j; k-- > 0; )
	    fld += s[j++];
	  break;
	}
	fld += s[j];
      }
      return j;
    }

    // advplain: unquoted field; return index of next separator
    int CSVReader::Csv::advplain(const std::string& s, std::string& fld, int i)
    {
      unsigned int j;

      j = s.find_first_of(fieldsep, i); // look for separator
      if (j > s.length())               // none found
	j = s.length();
      fld = std::string(s, i, j-i);
      return j;
    }

    // getfield: return n-th field
    std::string CSVReader::Csv::getfield(int n)
    {
      if (n < 0 || n >= nfield)
	return "";
      else
	return field[n];
    }

    CSVReader* CSVReader::CSVReaderMap::add(const std::string& name,
					    CSVReader* reader) {
      std::map < std::string , std::pair < unsigned int,
	CSVReader* > >::iterator it = readers.find(name);

      if (it == readers.end()) {
	readers[name] = std::pair < unsigned int, CSVReader*>(0,reader);
	it = readers.find(name);
      }
      else
	it->second.first++;
      return it->second.second;
    }

    bool CSVReader::CSVReaderMap::exist(const std::string& name) {
      return readers.find(name) != readers.end();
    }

    void CSVReader::CSVReaderMap::remove(const std::string& name) {
      std::map < std::string , std::pair < unsigned int,
	CSVReader* > >::iterator it = readers.find(name);

      if (it != readers.end()) {
	if (it->second.first > 1) it->second.first--;
	else {
	  it->second.second->file.close();
	  delete it->second.second;
	  if (readers.size() == 1) readers.clear();
	  else readers.erase(it);
	}
      }
    }

    CSVReader::CSVReaderMap::~CSVReaderMap() {
      std::map < std::string , std::pair < unsigned int,
	CSVReader* > >::iterator it = readers.begin();

      while (it != readers.end()) {
	it->second.second->file.close();
	delete it->second.second;
	++it;
      }
    }

    CSVReader::CSVReader(const std::string & fileName,
			 const std::string & separator,
			 t_type type,unsigned int columnNumber):
      name(fileName),type(type),csv(file,separator),columnNumber(columnNumber) {
      file.open(fileName.c_str());

      // Nombre de lignes et de colonnes du fichier
      std::string str;

      csv.getline(str);
      colNumber = csv.getnfield();
      csv.fin.seekg(0,std::ios::beg);

      rowNumber = 0;
      if (type == CSVReader::TITLE or type == CSVReader::MATRIX)
	csv.getline(str);
      while (csv.getline(str) != 0)
	++rowNumber;
      csv.fin.seekg(0,std::ios::beg);
    }

    CSVReader::~CSVReader() {
    }

    void CSVReader::close() {
      map.remove(name);
    }

    CSVReader& CSVReader::open(const std::string & fileName,
			       const std::string & separator,
			       t_type type, unsigned int columnNumber) {
      CSVReader* r = NULL;

      if (!map.exist(fileName))
	r = new CSVReader(fileName,separator,type,columnNumber);
      r = map.add(fileName,r);
      return *r;
    }

    Result* CSVReader::exec(const std::string& request) {
      return new CSVResult(request,csv,type,columnNumber);
    }

}} // namespace vle data
