/**
 * @file vle/geometry/Vector.hpp
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


#ifndef VLE_GEOMETRY_VECTOR_HPP
#define VLE_GEOMETRY_VECTOR_HPP

#include <vle/geometry/Tuple.hpp>

namespace vle { namespace geometry {

    /**
     * @brief A generic n elements vector.
     * @param n The number of dimension.
     * @param Class The class of the elements.
     *
     * @code
     * #include <iostream>
     * #include <vle/geometry/Vector.hpp>
     *
     * using namespace std;
     * using namespace vle::geometry;
     *
     * int main()
     * {
     *	double v[] = {1.0, 0.0, 0.0};
     *	Vector<3, double> v1(v), v2(0.0);
     *	v2[1] = 1.0;
     *	cout << v1 << endl;
     *	cout << v2 << endl;
     *	cout << v1.cross(v2) << endl;
     *	cout << v1.dot(v2) << endl;
     *
     *	float u[] = {-2.7, 3.1, 1.7, 4.5};
     *	Vector<4, float> u1;
     *	u1 = u;
     *	cout << u1 << endl;
     *	cout << u1.length() << endl;
     *	cout << u1.lengthSquared() << endl;
     *	cout << u1.normalize() << endl;
     *	cout << u1.length() << endl;
     *
     *	return 0;
     * }
     * @endcode
     */
    template <size_t n, class Class>
        class Vector : public vle::geometry::Tuple<n, Class>
    {
    public:
        /**
         * Constructs a vector of length zero.
         */
        inline Vector()
            : vle::geometry::Tuple<n, Class>()
        {}

        /**
         * Constructs a vector of length n, containing n values initialized
         * with value.
         * @param value The element which initializes the elements of the
         * vector.
         */
        inline Vector(const Class& value)
            : vle::geometry::Tuple<n, Class>(value)
        {}

        /**
         * Constructs a vector of length n, containing n values initialized
         * with the first n elements pointed by values. The array pointed by
         * values must contain at least n values.
         * @param values The elements which initializes the elements of the
         * vector.
         */
        inline Vector(const Class* values)
            : vle::geometry::Tuple<n, Class>(values)
        {}

        /**
         * Constructs and initializes a vector from the specified vector.
         * @param t The vector containing the initialization values.
         */
        inline Vector(const Vector<n, Class>& t)
            : vle::geometry::Tuple<n, Class>(t)
        {}

        /**
         * Constructs and initializes a vector from the specified valarray.
         * @param va The valarray containing the initialization values.
         */
        inline Vector(const std::valarray<Class>& va)
            : vle::geometry::Tuple<n, Class>(va)
        {}

        /**
         * Assigns the vector with the elements pointed by values.
         * @param values The values which assigns the elements of the vector.
         * @return The reference of this vector.
         */
        inline Vector<n, Class>& operator=(const Class* values)
        {
            return (Vector<n, Class>&)
                vle::geometry::Tuple<n, Class>::operator=(values);
        }

        /**
         * Assigns the vector with the specified valarray.
         * @param va The valarray containing the values.
         * @return The reference of this vector.
         */
        inline Vector<n, Class>& operator=(const std::valarray<Class>& va)
        {
            return (Vector<n, Class>&) std::valarray<Class>::operator=(va);
        }

        /**
         * Assigns the elements of the vector with the specified value.
         * @param value The value which assigns the elements of the vector.
         * @return The reference of this vector.
         */
        inline Vector<n, Class>& operator=(const Class& value)
        {
            return (Vector<n, Class>&) std::valarray<Class>::operator=(value);
        }

        /**
         * Computes the cross product of this vector and vector v.
         * @param v The other vector.
         * @return The cross product of this vector and vector v.
         */
        inline Vector<n, Class> cross(const Vector<n, Class>& v) const
        {
            Vector<n, Class> vCross;
            if (n == 3) {
                vCross[0] = (*this)[1] * v[2] - (*this)[2] * v[1];
                vCross[1] = (*this)[2] * v[0] - (*this)[0] * v[2];
                vCross[2] = (*this)[0] * v[1] - (*this)[1] * v[0];
            }
            return vCross;
        }

        /**
         * Computes the dot product of this vector and vector v.
         * @param v The other vector.
         * @return The dot product of this vector and vector v.
         */
        inline Class dot(const Vector<n, Class>& v) const
        {
            return (*this * v).sum();
        }

        /**
         * Computes the length of the vector.
         * @return The length of the vector.
         */
        inline Class length() const
        {
            return (Class)std::sqrt(lengthSquared());
        }

        /**
         * Computes the squared length of the vector.
         * @return The squared length of the vector.
         */
        inline Class lengthSquared() const
        {
            return (*this * *this).sum();
        }

        /**
         * Normalizes this vector.
         */
        inline Vector<n, Class>& normalize()
        {
            Class len = length();
            if(len != (Class)0)
                operator/=(len);
            return *this;
        }
    };

}} // namespace geometry vle

#endif
