/**
 * @file vle/data/CSVResult.cpp
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


#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <vle/data/CSVResult.hpp>
#include <vle/utils/Tools.hpp>
#include <iostream>

namespace vle { namespace data {

    /**********************************************************************/
    // Préambule : * type = NO_TITLE
    //                la première ligne peut ne contenir pas de nom d'attribut
    //             * type = TITLE (par défaut), la première est ignorée et sert
    //                de support pour le nom des attributs
    //             * type = MATRIX, la première ligne contient les noms des
    //                attributs sur n colonnes et les valeurs d'un attribut
    //                sur les colonnes suivantes
    // Syntaxe de la requete :
    //  %1%='value1',%2%='value2'|%3%,%4%
    //  'col1'='value1','col2'='value2'|'col3','col4'
    /**********************************************************************/

    CSVResult::CSVResult(const std::string& request,
                         CSVReader::Csv& csv,
                         CSVReader::t_type type,
                         unsigned int columnNumber):
        request(request),
        csv(csv),type(type),
        columnNumber(columnNumber)
    {
        std::string str;
        int colNumber = 0;

        csv.fin.clear();
        csv.fin.seekg(0,std::ios::beg);

        // Chargement des entêtes de colonnes
        if (type == CSVReader::TITLE or type == CSVReader::MATRIX) {
            csv.getline(str);
            if (type == CSVReader::TITLE) colNumber = csv.getnfield();
            else colNumber = columnNumber;
            for (int i = 0; i < colNumber; i++) {
                columnNameList.push_back(csv.getfield(i));
                columnNameMap[csv.getfield(i)] = i+1;
            }
            if (type == CSVReader::MATRIX)
                for (int i = colNumber; i < csv.getnfield(); i++) {
                    valueList.push_back(csv.getfield(i));
                    valueMap[csv.getfield(i)] = i+1;
                }
            csv.fin.seekg(0,std::ios::beg);
        }

        // *********************
        // Analyse de la requête
        // *********************
        std::vector < std::string > list; //= split(request,'|');
        std::vector < std::string > conditionList;// split(list[0],',');
        std::vector < std::string > selectionList; //= split(list[1],',');

        boost::algorithm::split(list, request, boost::is_any_of("|"));
        if (not list[0].empty())
            boost::algorithm::split(conditionList, list[0],
                                    boost::is_any_of(","));
        if (not list[1].empty())
            boost::algorithm::split(selectionList, list[1],
                                    boost::is_any_of(","));

        // Analyse des conditions
        std::vector < std::string >::iterator itc = conditionList.begin();
        int i = 0;

        while (itc != conditionList.end()) {
            std::vector < std::string > expression; // = split(*itc,'=');
            boost::algorithm::split(expression, *itc, boost::is_any_of("="));
            std::string val = expression[1];
            bool distinct;
            int colNum = parseColumn(expression[0],distinct);
            std::string value = val;

            if (val[0] == '\'' and val[val.length()-1] == '\'')
                value = val.substr(1,val.length()-2);
            if (colNum == 0) {
                colNum = valueMap[value];
                this->selectionList.push_back(colNum);
                selectionIndexMap[colNum] = i++;
                selectionNameMap[value] = std::pair <
                    int,bool>(colNum,distinct);
            }
            else {
                conditionIndexList.push_back(colNum);
                conditionValueList.push_back(value);
            }
            ++itc;
        }

        // Analyse des colonnes sélectionnées
        std::vector < std::string >::iterator its = selectionList.begin();

        while (its != selectionList.end()) {
            bool distinct;
            int colNum = parseColumn(*its,distinct);

            if (colNum > 0) {
                this->selectionList.push_back(colNum);
                selectionIndexMap[colNum] = i++;
                if (type != CSVReader::MATRIX) {
                    selectionNameMap[columnNameList[colNum-1]] =
                        std::pair < int,bool>(colNum,distinct);
                    if (distinct)
                        distinctMap[columnNameList[colNum-1]] =
                            std::list<std::string>();
                }
                else {
                    selectionNameMap[valueList[colNum-columnNumber-1]] =
                        std::pair < int,bool>(colNum,distinct);
                    if (distinct)
                        distinctMap[valueList[colNum-columnNumber-1]] =
                            std::list<std::string>();
                }
            }
            ++its;
        }

        // Détermination du nombre d'enregistrements
        rowNumber = 0;
        if (type == CSVReader::NO_TITLE or type == CSVReader::TITLE) {
            if (csv.getline(str) != 0) {
                while (next())
                    if (csv.getnfield() == colNumber) ++rowNumber;
            }
        }
        else {
            if (csv.getline(str) != 0) {
                while (next())
                    if (csv.getnfield() == (int)(colNumber+valueList.size()))
                        ++rowNumber;
            }
        }
        csv.fin.seekg(0,std::ios::beg);

        // Mise à zéro des listes de valeurs distinctes
        std::map < std::string, std::list < std::string > >::iterator it =
            distinctMap.begin();

        while (it != distinctMap.end()) {
            it->second.clear();
            ++it;
        }

        if (type == CSVReader::TITLE or type == CSVReader::MATRIX)
            csv.getline(str);

        position = csv.fin.tellg();
    }

    int CSVResult::parseColumn(const std::string & col, bool & distinct)
    {
        int colNum = -1;
        int first = 0;

        distinct = false;
        if (col[0] == '!') {
            distinct = true;
            first = 1;
        }
        if (col[first] == '%' and col[col.length()-1] == '%')
            colNum = utils::to_int(col.substr(first+1,col.length()-first-2));
        else {
            std::string str = col.substr(first+1,col.length()-first-2);

            if (columnNameMap.find(str) != columnNameMap.end())
                colNum = columnNameMap[str];
        }
        return colNum;
    }

    CSVResult::~CSVResult() {
    }

    bool CSVResult::next() {
        std::string str;
        bool more = false;
        bool ok = false;

        csv.fin.clear();
        csv.fin.seekg(position);

        do {
            if (type == CSVReader::MATRIX)
                more = csv.getline(str) != 0 &&
                    csv.getnfield() ==
                    (int)(columnNameList.size()+valueList.size());
            else
                more = csv.getline(str) != 0 &&
                    csv.getnfield() == (int)columnNameList.size();
            if (more) {
                std::vector < int >::iterator it1 = conditionIndexList.begin();
                std::vector < std::string >::iterator it2 =
                    conditionValueList.begin();

                ok = true;
                if (it1 == conditionIndexList.end()) {
                    std::map < std::string, std::pair < int,bool > >::iterator
                        it = selectionNameMap.begin();

                    while (ok and it != selectionNameMap.end()) {
                        if (it->second.second) {
                            std::string value = csv.getfield(it->second.first-1);
                            std::list < std::string >::iterator itd =
                                find(distinctMap[it->first].begin(),
                                     distinctMap[it->first].end(),value);

                            ok = (itd == distinctMap[it->first].end());
                        }
                        ++it;
                    }
                }
                else
                    while (ok and it1 != conditionIndexList.end()) {
                        ok = csv.getfield(*it1-1) == *it2;

                        // vérifie que la valeur trouvée n'est pas déjà
                        // selectionnée dans le cas de distinct

                        if (ok) {
                            std::map < std::string, std::pair < int,bool >
                                >::iterator it =
                                selectionNameMap.begin();

                            while (ok and it != selectionNameMap.end()) {
                                if (it->second.second) {
                                    std::string value =
                                        csv.getfield(it->second.first-1);
                                    std::list < std::string >::iterator itd =
                                        find(distinctMap[it->first].begin(),
                                             distinctMap[it->first].end(),value);

                                    ok = (itd == distinctMap[it->first].end());
                                }
                                ++it;
                            }
                        }
                        ++it1;
                        ++it2;
                    }
            }
            else ok = false;
        } while (!ok && more);
        if (ok) {
            std::vector < int >::iterator it = selectionList.begin();

            result.clear();
            while (it != selectionList.end()) {
                result.push_back(csv.getfield(*it-1));
                ++it;
            }

            // ajout des valeurs distinctes
            std::map < std::string, std::pair < int,bool > >::iterator it3 =
                selectionNameMap.begin();

            while (it3 != selectionNameMap.end()) {
                if (it3->second.second) {
                    std::string value = csv.getfield(it3->second.first-1);

                    distinctMap[it3->first].push_back(value);
                }
                ++it3;
            }

        }
        if (!more) {
            csv.fin.seekg(0,std::ios::beg);

            // Mise à zéro des listes de valeurs distinctes
            std::map < std::string, std::list < std::string > >::iterator it5 =
                distinctMap.begin();

            while (it5 != distinctMap.end()) {
                it5->second.clear();
                ++it5;
            }
        }

        position = csv.fin.tellg();

        return more;
    }

    std::string CSVResult::get(const std::string& name) const {
        if (selectionNameMap.find(name) != selectionNameMap.end()) {
            int colNum = selectionNameMap.find(name)->second.first;

            return result[selectionIndexMap.find(colNum)->second];
        }
        else return "";
    }

    std::string CSVResult::get(unsigned int i) const {
        if (i <= result.size()) return result[i-1];
        else return "";
    }

    std::string CSVResult::getColumnName(unsigned int i) const {
        if (i < columnNameList.size()) return columnNameList[i-1];
        else return "";
    }

    unsigned int CSVResult::getColumnNumber() const {
        return selectionNameMap.size();
    }

    unsigned int CSVResult::getRowNumber() const {
        return rowNumber;
    }

}} // namespace vle data
