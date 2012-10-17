/*
 * @file vle/utils/details/Misc.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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

#include <vle/utils/details/Misc.hpp>
#include <algorithm>
#include <cstring>

namespace vle { namespace utils {

struct strdup_functor
    : std::unary_function < std::string, char* >
{
    char * operator()(const std::string& str) const
    {
        return strdup(str.c_str());
    }
};

void free_str_array(char **args)
{
    char **tmp;

    for (tmp = args; *tmp; ++tmp) {
        free(*tmp);
    }

    delete[] args;
}

char ** convert_string_str_array(const std::vector < std::string >& args)
{
    char **result = 0;

    result = new char*[args.size() + 1];

    std::transform(args.begin(),
                   args.end(),
                   result,
                   strdup_functor());

    result[args.size()] = 0;

    return result;
}

}}
