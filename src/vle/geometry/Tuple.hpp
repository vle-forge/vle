/**
 * @file src/vle/geometry/Tuple.hpp
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




#ifndef VLE_GEOMETRY_TUPLE_HPP
#define VLE_GEOMETRY_TUPLE_HPP

#include <ostream>
#include <valarray>



namespace vle { namespace geometry {

/**
 * @brief A generic n elements tuple.
 * @param n The number of dimension.
 * @param Class The class of the elements.
 *
 * @code
 * #include <iostream>
 * #include <vle/geometry/Tuple.hpp>
 *
 * using namespace std;
 * using namespace vle::geometry;
 *
 * int main()
 * {
 *	double t[] = {0.0, -1.7, 3.9};
 *	Tuple<3, double> t1(t), t2(0.0), t3;
 *	t2[0] = 1.75;
 *	t1[0] = t2[1] = 2.6;
 *	t3 = t1 + t2;
 *	cout << t1 << endl;
 *	cout << t2 << endl;
 *	cout << t3 << endl;

 *	float u[] = {-2.7, 3.1, 1.7, 4.5};
 *	Tuple<4, float> u1, u2(u);
 *	u1 = u;
 *	u2[2] = u2[2] + 0.2;
 *	cout << u1 << endl;
 *	cout << u2 << endl;
 *	cout << u1.equal(u2, 0.3) << endl;
 *	cout << u1.equal(u2, 0.1) << endl;
 *	cout << u1.sum() << endl;
 *	cout << u2.product() << endl;
 *
 *	return 0;
 * }
 * @endcode
 */
template <size_t n, class Class>
class Tuple : public std::valarray<Class>
{

public:

    /**
     * Constructs a tuple of length zero.
     */
    inline Tuple()
	: std::valarray<Class>(n)
	{

	}

    /**
     * Constructs a tuple of length n, containing n values initialized
     * with value.
     * @param value The element which initializes the elements of the tuple.
     */
    inline Tuple(const Class& value)
	: std::valarray<Class>(value, n)
	{

	}

    /**
     * Constructs a tuple of length n, containing n values initialized with
     * the first n elements pointed by values. The array pointed by values
     * must contain at least n values.
     * @param values The elements which initializes the elements of the tuple.
     */
    inline Tuple(const Class* values)
	: std::valarray<Class>(values, n)
	{

	}

    /**
     * Constructs and initializes a tuple from the specified tuple.
     * @param t The tuple containing the initialization values.
     */
    inline Tuple(const Tuple<n, Class>& t)
	: std::valarray<Class>(t)
	{

	}

    /**
     * Constructs and initializes a tuple from the specified valarray.
     * @param va The valarray containing the initialization values.
     */
    inline Tuple(const std::valarray<Class>& va)
	: std::valarray<Class>(va)
	{

	}

    /**
     * Assigns the tuple with the elements pointed by values.
     * @param values The values which assigns the elements of the tuple.
     * @return The reference of this tuple.
     */
    inline Tuple<n, Class>& operator=(const Class* values)
	{
	    return (Tuple<n, Class>&)
		std::valarray<Class>::operator=(std::valarray<Class>(values, n));
	}

    /**
     * Assigns the tuple with the specified valarray.
     * @param va The valarray containing the values.
     * @return The reference of this tuple.
     */
    inline Tuple<n, Class>& operator=(const std::valarray<Class>& va)
	{
	    return (Tuple<n, Class>&) std::valarray<Class>::operator=(va);
	}

    /**
     * Assigns the elements of the tuple with the specified value.
     * @param value The value which assigns the elements of the tuple.
     * @return The reference of this tuple.
     */
    inline Tuple<n, Class>& operator=(const Class& value)
	{
	    return (Tuple<n, Class>&) std::valarray<Class>::operator=(value);
	}

    /**
     * Writes in the output-stream a representation of the tuple.
     * The form is (x,y,z,...).
     * @param os The output-stream.
     * @param t The tuple.
     * @return The reference of the output-stream.
     */
    inline friend std::ostream& operator<<(std::ostream& os,
					   const Tuple<n, Class>& t)
	{
	    os << '(';
	    size_t i = 0;
	    while (i != t.size()) {
		os << t[i++];
		if (i != t.size())
		    os << ',';
	    }
	    return os << ')';
	}


    /**
     * Compute the average of all elements in the tuple.
     * @return The average of all elements in the tuple.
     */
    inline Class average() const
	{
	    return (Class) (this->sum() / (Class)n);
	}

    /**
     * Returns true if the distance between this tuple and tuple t is less
     * than or equal to the epsilon parameter, otherwise returns false.
     * @param t The tuple to be compared to this tuple.
     * @param epsilon The threshold value.
     * @return True or false.
     */
    inline bool equal(const Tuple<n, Class>& t, const Class& epsilon) const
	{
	    std::valarray<Class> va((*this) - t);
	    return (va * va).sum() <= epsilon * epsilon;
	}

    /**
     * Compute the product of all elements in the tuple, product can only be
     * called for a tuple instantiated on a type that supports operator*=()
     * and constructor Class(1).
     * @return The product of all elements in the tuple.
     */
    inline Class product() const
	{
	    Class prod(1);
	    for(size_t i = 0; i < n; ++i)
		prod *= (*this)[i];
	    return prod;
	}

    /**
     * Compute the standart deviation of all elements in the tuple.
     * @return The standart deviation of all elements in the tuple.
     */
    inline Class stdDeviation() const
	{
	    Tuple<n, Class> valAvg(this->average());
	    Tuple<n, Class> diff((*this) - valAvg);
	    return sqrt((diff * diff).sum() / (Class) n);
	}

};

}} // namespace geometry vle

#endif
