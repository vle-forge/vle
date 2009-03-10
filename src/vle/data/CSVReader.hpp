/**
 * @file vle/data/CSVReader.hpp
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


#ifndef VLE_DATA_CSV_READER_HPP
#define VLE_DATA_CSV_READER_HPP

#include <vle/data/DataReader.hpp>
#include <vle/data/Result.hpp>
#include <fstream>
#include <map>
#include <string>
#include <vector>

namespace vle { namespace data {

    class CSVReader:public DataReader
    {
    public:
        enum t_type { NO_TITLE, TITLE, MATRIX };

        static CSVReader& open(const std::string& fileName,
                               const std::string& separator = ";",
                               t_type type = TITLE,
                               unsigned int columnNumber = 0);

        virtual ~CSVReader();

        virtual void close();
        virtual Result* exec(const std::string&);
        virtual unsigned int getColumnNumber() const { return colNumber; }
        virtual unsigned int getRowNumber() const { return rowNumber; }

    private:

        /**
         *  @brief Copyright (C) 1999 Lucent Technologies
         *  Excepted from 'The Practice of Programming'
         *  by Brian W. Kernighan and Rob Pike
         */
        class Csv
        {
        public:
            Csv(std::ifstream& fin, std::string sep):fin(fin),fieldsep(sep) { }

            int getline(std::string&);
            std::string getfield(int n);
            int getnfield() const { return nfield; }

        private:
            std::ifstream& fin;	                // input file pointer
            std::string line;	                // input line
            std::vector < std::string > field;	// field strings
            int nfield;				// number of fields
            std::string fieldsep;		        // separator characters

            int split();
            int endofline(char);
            int advplain(const std::string& line, std::string& fld, int);
            int advquoted(const std::string& line, std::string& fld, int);

            friend class CSVResult;
            friend class CSVReader;
        };

        class CSVReaderMap
        {
        public:
            CSVReaderMap() { }
            virtual ~CSVReaderMap();

            CSVReader* add(const std::string& name,CSVReader* reader);
            bool exist(const std::string& name);
            void remove(const  std::string& name);

            friend class CSVReader;

        private:
            std::map < std::string ,
                std::pair < unsigned int, CSVReader* > > readers;
        };

        static CSVReaderMap map;
        std::string name;
        t_type type;
        std::ifstream file;
        Csv csv;
        unsigned int columnNumber;
        unsigned int colNumber;
        unsigned int rowNumber;

        CSVReader(const std::string & fileName, const std::string & separator,
                  t_type type,unsigned int columnNumber);

        friend class CSVResult;
    };

}} // namespace vle data

#endif
