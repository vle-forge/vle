/**
 * @file vle/geometry/Point.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
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


#ifndef VLE_GEOMETRY_POINT_HPP
#define VLE_GEOMETRY_POINT_HPP

#include <vle/geometry/Tuple.hpp>

namespace vle { namespace geometry {

    /**
     * @brief A generic n elements point.
     * @param n The number of dimension.
     * @param Class The class of the elements.
     *
     * @code
     * #include <iostream>
     * #include <model/crazy/Point.hpp>
     *
     * using namespace std;
     * using namespace vle::geometry;
     *
     * int main()
     * {
     *	double p[] = {2.95, -1.67, 3.54};
     *	Point<3, double> p1(p), p2(1.0), p3;
     *	p3 = p1 + p2;
     *	cout << p1 << endl;
     *	cout << p2 << endl;
     *	cout << p3 << endl;
     *	cout << p1.distance(p3) << endl;
     *	cout << p1.distanceSquared(p3) << endl;
     *
     *	int u[] = {1, 3};
     *	Point<2, int> u1(u), u2;
     *	u2 = u;
     *	u2[0] += 2;
     *	cout << u1 << endl;
     *	cout << u2 << endl;
     *	cout << u1.distance(u2) << endl;
     *	cout << u1.distanceSquared(u2) << endl;
     *
     *	return 0;
     * }
     * @endcode
     */
    template <size_t n, class Class>
        class Point : public vle::geometry::Tuple<n, Class>
    {
    public:
        /**
         * Constructs a zero dimension point.
         */
        inline Point()
            : vle::geometry::Tuple<n, Class>()
        {}

        /**
         * Constructs a n dimensions point, containing n values initialized
         * with value.
         * @param value The element which initializes the elements of the point.
         */
        inline Point(const Class& value)
            : vle::geometry::Tuple<n, Class>(value)
        {}

        /**
         * Constructs a n dimensions point, containing n values initialized
         * with the first n elements pointed by values. The array pointed by
         * values must contain at least n values.
         * @param values The elements which initializes the elements of the
         * point.
         */
        inline Point(const Class* values)
            : vle::geometry::Tuple<n, Class>(values)
        {}

        /**
         * Constructs and initializes a point from the specified point.
         * @param t The point containing the initialization values.
         */
        inline Point(const Point<n, Class>& t)
            : vle::geometry::Tuple<n, Class>(t)
        {}

        /**
         * Constructs and initializes a point from the specified valarray.
         * @param va The valarray containing the initialization values.
         */
        inline Point(const std::valarray<Class>& va)
            : vle::geometry::Tuple<n, Class>(va)
        {}

        /**
         * Assigns the point with the elements pointed by values.
         * @param values The values which assigns the elements of the point.
         * @return The reference of this point.
         */
        inline Point<n, Class>& operator=(const Class* values)
        {
            return (Point<n, Class>&)
                vle::geometry::Tuple<n, Class>::operator=(values);
        }

        /**
         * Assigns the point with the specified valarray.
         * @param va The valarray containing the values.
         * @return The reference of this point.
         */
        inline Point<n, Class>& operator=(const std::valarray<Class>& va)
        {
            return (Point<n, Class>&) std::valarray<Class>::operator=(va);
        }

        /**
         * Assigns the elements of the point with the specified value.
         * @param value The value which assigns the elements of the point.
         * @return The reference of this point.
         */
        inline Point<n, Class>& operator=(const Class& value)
        {
            return (Point<n, Class>&) std::valarray<Class>::operator=(value);
        }

        /**
         * Computes the distance between this point and point p.
         * @param p The other point.
         * @return The distance.
         */
        inline Class distance(const Point<n, Class>& p) const
        {
            return (Class)std::sqrt(distanceSquared(p));
        }

        /**
         * Computes the square of the distance between this point and point p.
         * @param p The other point.
         * @return The square of the distance.
         */
        inline Class distanceSquared(const Point<n, Class>& p) const
        {
            std::valarray<Class> va((*this) - p);
            return (va * va).sum();
        }
    };

}} // namespace geometry vle

#endif
