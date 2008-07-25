/**
 * @file src/vle/utils/Algo.hpp
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

#ifndef VLE_UTILS_ALGO_HPP
#define VLE_UTILS_ALGO_HPP

#include <algorithm>
#include <functional>

namespace vle { namespace utils {

    /**
     * @brief Copies the values of the elements in the range [first,last) to
     * the range positions beginning at result, except those for which pred is
     * false, which are not copied. The behavior of this function template is
     * equalent to:
     * @code
     * typename InputIterator, typename OutputIterator, typename Predicate >
     * void copy_if(InputIterator first, InputIterator end,
     *                     OutputIterator out, Predicate pred)
     * {
     *     while (first != end) {
     *         if (pred(*first)) {
     *             *out++ = *first;
     *         }
     *         ++first;
     *     }
     * }
     * @encode
     * Example:
     * @code
     * using namespace std;
     * using namespace vle;
     *
     * bool IsOdd (int i) { return ((i%2)==1); }
     *
     * int main ()
     * {
     *   int myints[] = {1,2,3,4,5,6,7,8,9};
     *   vector < int >  myvector out;
     *   vector < int >::iterator it;
     *
     *   copy_if(myints, myints+9, back_inserter(myvector), IsOdd);
     *
     *   cout << "myvector contains:";
     *   for (it=myvector.begin(); it != myvector.end(); ++it)
     *    cout << " " << *it;
     *
     *  cout << endl;
     *
     *  return 0;
     *}
     * @endcode
     *
     * @param first Forward iterator to the initial position in a sequence.
     * @param end Forward iterator to the final position in a sequence.
     * @param out Output iterator to the initial position.
     * @param pred Unary predicate taking an element in the range as argument,
     * and returning a value.
     */
    template <
        typename InputIterator, typename OutputIterator, typename Predicate >
        void copy_if(InputIterator first, InputIterator end,
                     OutputIterator out, Predicate pred)
        {
            for (; first != end; ++first) {
                if (pred(*first)) {
                    *out++ = *first;
                }
            }
        }

    /**
     * @brief Search an element in the range [first,last) which responds to the
     * Predicate.
     * @code
     * template < typename Iterator, typename Predicate >
     * Iterator find_if(Iterator first, Iterator end, Predicate pred)
     * {
     *     for (; first != end; ++first) {
     *         if (pred((*first).second)) {
     *             return first;
     *         }
     *     }
     *     return end;
     * }
     * @endcode
     * @param first Forward iterator to the initial position in a sequence.
     * @param end Forward iterator to the finial position in a sequence.
     * @param pred The predicate to test for each std::pair.
     * @return The iterator which reference to element found or end if not
     * found.
     */
    template <
        typename Iterator, typename Predicate >
        Iterator find_if(Iterator first, Iterator end, Predicate pred)
        {
            for (; first != end; ++first) {
                if (pred((*first).second)) {
                    return first;
                }
            }
            return end;
        }

    /**
     * @brief Apply a function to each element (std::pair) in the range
     * [first,last). To use with a std::map.
     * @code
     * typename InputIterator, typename Function >
     * void for_each(InputIterator first, InputIterator end,
     *               Function f)
     * {
     *     while (first != end) {
     *         f((*first).second);
     *         ++first;
     *     }
     * }
     * @encode
     * Example:
     * @code
     * using namespace std;
     * using namespace vle;
     *
     * struct append_string
     * {
     *     std::string str;
     *
     *     void operator()(const std::string& x)
     *     { str.append(x); }
     * };
     *
     * int main()
     * {
     *     std::map < std::string, std::string > n;
     *     n["tutu"] = "ou";
     *     n["tati"] = "xu";
     *     n["tete"] = "re";
     *     n["tita"] = "nt";
     *     append_string x = vle::for_each(n.begin(), n.end(), append_string());
     *     std::cout << "String: " << x.str << "\n"; // "ouxurent"
     * }
     * @endcode
     *
     * @param first Forward iterator to the initial position in a sequence.
     * @param end Forward iterator to the final position in a sequence.
     * @param f Function to call for each value of the std::pair < key, value >.
     *
     * @return A copy of the Function f.
     */
    template <
        typename InputIterator, typename Function >
        Function for_each(InputIterator first, InputIterator end,
                          Function f)
        {
            for (; first != end; ++first) {
                f((*first).second);
            }
            return f;
        }

    /**
     * @brief A std::unary_function to select the first element of a std::pair.
     */
    template < class __vle_pair >
        struct select1st : public std::unary_function <
                           __vle_pair, typename __vle_pair::first_type >
    {
        /**
         * @brief Get the first element of the std::pair.
         * @param x The pair.
         * @return A constant reference to the first element.
         */
        const typename __vle_pair::first_type&
            operator()(const __vle_pair& x) const
            { return x.first; }

        /**
         * @brief Get the first element of the std::pair.
         * @param x The pair.
         * @return A reference to the first element.
         */
        typename __vle_pair::first_type&
            operator()(__vle_pair& x)
        { return x.first; }
    };

    /**
     * @brief A std::unary_function to select the second element of a std::pair.
     */
    template < class __vle_pair >
        struct select2nd : public std::unary_function <
                           __vle_pair, typename __vle_pair::second_type >
    {
        /**
         * @brief Get the second element of the std::pair.
         * @param x The pair.
         * @return A constant reference to the second element.
         */
        const typename __vle_pair::second_type&
            operator()(const __vle_pair& x) const
            { return x.second; }

        /**
         * @brief Get the second element of the std::pair.
         * @param x The pair.
         * @return A reference to the second element.
         */
        typename __vle_pair::second_type&
            operator()(__vle_pair& x)
        { return x.second; }
    };

}} // namespace vle utils

#endif
