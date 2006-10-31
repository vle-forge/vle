/** 
 * @file CSVResult.hpp
 * @brief The result of request in CSV file class.
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

#ifndef VLE_DATA_CSV_RESULT_HPP
#define VLE_DATA_CSV_RESULT_HPP

#include <vle/data/Result.hpp>
#include <vle/data/CSVReader.hpp>
#include <map>
#include <string>
#include <iostream>
#include <list>
#include <vector>

namespace vle { namespace data {

    class CSVResult:public Result
    {
    public:
        virtual ~CSVResult();

        virtual bool next();
        virtual std::string get(const std::string& name) const;
        virtual std::string get(unsigned int index) const;
        virtual std::string getColumnName(unsigned int index) const;
        virtual unsigned int getColumnNumber() const;
        virtual unsigned int getRowNumber() const;

    private:
        std::string request;
        std::ios::pos_type position;
        CSVReader::Csv& csv;
        CSVReader::t_type type;
        std::map < std::string, int > columnNameMap;
        std::vector < std::string > columnNameList;

        // mode Matrix
        std::vector < std::string > valueList;
        std::map < std::string, int > valueMap;

        unsigned int rowNumber;
        unsigned int columnNumber;

        std::vector < int > conditionIndexList;
        std::vector < std::string > conditionValueList;

        // liste des colonnes dans le vecteur résultat
        std::vector < int > selectionList;
        // correspondance entre indice des colonnes et l'indice 
        // dans le vecteur de résultat     
        std::map < int, int > selectionIndexMap; 
        // correspondance entre le nom de la colonne et l'indice de la colonne
        // (sous-ensemble de columnNameMap)
        std::map < std::string, std::pair < int,bool > > selectionNameMap;      
        std::map < std::string, std::list < std::string > > distinctMap;
        std::vector < std::string > result;

        CSVResult(const std::string& request,CSVReader::Csv& csv,
                  CSVReader::t_type type,unsigned int columnNumber);
        int parseColumn(const std::string & col, bool & distinct);

        friend class CSVReader;
    };

}} // namespace vle data

#endif
