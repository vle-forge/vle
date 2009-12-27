/**
 * @file vle/geometry/Box.hpp
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


#ifndef VLE_GEOMETRY_BOX_HPP
#define VLE_GEOMETRY_BOX_HPP

#include <algorithm>
#include <ostream>
#include <valarray>
#include <vle/geometry/Point.hpp>

namespace vle { namespace geometry {

    /**
     * @brief A generic axis aligned box.
     * @param n The number of dimensions.
     * @param Class The class of the elements.
     *
     * @code
     * #include <iostream>
     * #include <vle/geometry/Box.hpp>
     * #include <model/crazy/Point.hpp>
     *
     * using namespace std;
     * using namespace vle::geometry;
     *
     * int main()
     * {
     *	double p1[] = {1.23, -1.67, 0.2};
     *	double p2[] = {-2.95, 9.81, 5.34};
     *	Point<3, double> p3(p1), p4(p2);
     *	Box<3, double> b1(p3, p4);
     *	cout << b1 << endl;
     *	cout << b1.lengths() << endl;
     *	cout << b1.hypervolume() << endl;
     *
     *	int p5[] = {1, -2};
     *	int p6[] = {-3, 3};
     *	Point<2, int> p7(p5), p8(p6);
     *	Box<2, int> b2(p7, p8);
     *	cout << b2.getUpper() << endl;
     *	cout << b2.getLower() << endl;
     *	cout << b2.length(0) << endl;
     *	cout << b2.length(1) << endl;
     *
     *	return 0;
     * }
     * @endcode
     */
    template <size_t n, class Class>
        class Box
        {
        public:
            /**
             * Constructs a box.
             */
            inline Box()
            { }

            /**
             * Constructs  and initializes a box given the lower and the upper
             * corner of the box.
             * @param lower The lower corner of the box.
             * @param upper The upper corner of the box.
             */
            inline Box(const Point<n, Class>& lower,
                       const Point<n, Class>& upper) :
                m_lower(lower),
                m_upper(upper)
            {
                normalize();
            }

            /**
             * Constructs and initializes a box from the specified box.
             * @param b The box containing the initialization corners.
             */
            inline Box(const Box<n, Class>& b) :
                m_lower(b.m_lower),
                m_upper(b.m_upper) {}

            /**
             * Assigns the box from the specified box.
             * @param b The box containing the corners.
             * @return The reference of this box.
             */
            inline Box<n, Class>& operator=(const Box<n, Class>& b)
            {
                m_lower = b.m_lower;
                m_upper = b.m_upper;
                return *this;
            }

            /**
             * Gets the lower corner of the box.
             * @return The lower corner of the box.
             */
            inline const Point<n, Class>& getLower() const
            {
                return m_lower;
            }

            /**
             * Gets the upper corner of the box.
             * @return The upper corner of the box.
             */
            inline const Point<n, Class>& getUpper() const
            {
                return m_upper;
            }

            /**
             * Writes in the output-stream a representation of the box. The form
             * is ((x_min,y_min,z_min,...),(x_max,y_max,z_max,...)).
             * @param os The output-stream.
             * @param b The box.
             * @return The reference of the output-stream.
             */
            inline friend std::ostream& operator<<(std::ostream& os,
                                                   const Box<n, Class>& b)
            {
                return os << '(' << b.m_lower << ',' << b.m_upper << ')';
            }

            /**
             * Test for intersection with a point.
             * @param p The point to test the intersection.
             * @return True or false indicating if an intersection occured.
             */
            inline bool intersect(Point<n, Class> p) const
            {
                for (size_t i = 0; i < n; ++i)
                    if (p[i] < m_lower[i] || m_upper[i] < p[i])
                        return false;
                return true;
            }

            /**
             * Computes the length of the specified axis of the box.
             * @param i The specified axis of the box.
             * @return The length of the specified axis of the box.
             */
            inline Class length(size_t i) const
            {
                return m_upper[i] - m_lower[i];
            }

            /**
             * Computes the lengths of the axis of the box.
             * @return The lengths of the axis of the box.
             */
            inline std::valarray<Class> lengths() const
            {
                return m_upper - m_lower;
            }

            /**
             * Sets the lower and the upper corner of the box.
             * @param lower The lower corner of the box.
             * @param upper The upper corner of the box.
             */
            inline void set(const Point<n, Class>& lower,
                            const Point<n, Class>& upper)
            {
                m_lower = lower;
                m_upper = upper;
                normalize();
            }

            /**
             * Sets the lower corner of the box.
             * @param p The lower corner of the box.
             */
            inline void setLower(const Point<n, Class>& p)
            {
                m_lower = p;
                normalize();
            }

            /**
             * Sets the upper corner of the box.
             * @param p The upper corner of the box.
             */
            inline void setUpper(const Point<n, Class>& p)
            {
                m_upper = p;
                normalize();
            }

            /**
             * Computes the hypervolume of the box.
             * @return The hypervolume of the box.
             */
            inline Class hypervolume() const
            {
                return ((Tuple<n, Class>)lengths()).product();
            }

        private:
            Point<n, Class> m_lower, /**< The lower corner of the box. */
                m_upper; /**< The upper corner of the box. */

            /**
             * Sets the min values to the lower corner. Sets the max values to
             * the upper corner.
             */
            inline void normalize()
            {
                for(size_t i = 0; i < n; ++i)
                    if (m_lower[i] > m_upper[i])
                        std::swap(m_lower[i], m_upper[i]);
            }
        };

}} // namespace box vle

#endif
