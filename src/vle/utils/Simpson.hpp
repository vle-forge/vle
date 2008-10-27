/**
 * @file vle/utils/Simpson.hpp
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


#ifndef UTILS_SIMPSON_HPP
#define UTILS_SIMPSON_HPP

#include <cstddef>

namespace vle { namespace utils {

    class Simpson
    {
    public:
        Simpson() :
            v_f(NULL),
            v_a(0.0),
            v_b(0.0),
            v_n(1)
        {}

	Simpson(double (*f)(double),double a,double b, int n):
            v_f(f),
            v_a(a),
            v_b(b),
            v_n(n)
        { }

	inline void setInterval(double a,double b)
	{
	    v_a=a;
	    v_b=b;
	}

	inline void setFunction(double (*f)(double))
	{
	    v_f=f;
	}

	inline void setN(int n)
	{
	    v_n=n-(n%2);
	}

	double compute();

    private:
	double (*v_f)(double);
	double v_a,v_b;
	int v_n;
    };

}} // namespace vle utils

#endif
