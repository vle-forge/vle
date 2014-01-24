/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef VLE_UTILS_TOOLS_HPP
#define VLE_UTILS_TOOLS_HPP

#include <vle/DllDefines.hpp>
#include <typeinfo>
#include <string>

namespace vle { namespace utils {

    /**
     * Return true if the string \c str can be translated the template type \c T.
     * The template type \c T can be one of the following type: bool, float,
     * double, int8_t, uint8_t, int16_t, uint16_t, int32_t or uint32_t.
     *
     * @param str The string to convert.
     *
     * @return true if string can be translated into the type \c T.
     */
    template < typename T > bool is(const std::string& str);

    /**
     * Convert the template type \c T into the string representation.
     * The template type \c T can be one of the following type: bool, float,
     * double, int8_t, uint8_t, int16_t, uint16_t, int32_t or uint32_t.
     *
     * @param t An instance of the type \c T to convert.
     *
     * @return A string representation.
     */
    template < typename T > std::string to(const T t);

    /**
     * Convert the string \c str into the template type \c T. The template type
     * \c T can be one of the following type: bool, float, double, int8_t,
     * uint8_t, int16_t, uint16_t, int32_t or uint32_t.
     *
     * @param str The string to convert.
     *
     * @throw utils::ArgError If the string \c str can not be converted.
     *
     * @return A instance of the type \c T.
     */
    template < typename T > T to(const std::string& str);

    /**
     * Convert the string to the output template type with or without
     * the use of locale.
     * \c T can be one of the following type: bool, float, double, int8_t,
     * uint8_t, int16_t, uint16_t, int32_t or uint32_t.
     * @code
     * vle::utils::convert < double >("123.456");
     * // returns 123.456
     *
     * vle::utils::convert < double >("123 456,789", true, "fr_FR");
     * // returns 123456.789
     * @endcode
     *
     * @param value The string to convert
     * @param locale True to use the default locale, false otherwise.
     * @param loc The locale to use.
     *
     * @throw utils::ArgError if a convertion failed.
     *
     * @return A template output type.
     */
    template < typename T > T convert(const std::string& value,
                                      bool locale = false,
                                      const std::string& loc = std::string());

    /**
     * Check if the locale exists on this operating system.
     * @param locale The locale to check.
     * @return True if the locale exists, false otherwise.
     */
    VLE_API bool isLocaleAvailable(const std::string& locale);

    /**
     * Return the a string version of v, in scientific notation
     * (if v is too small or big) or in a standard representaion.
     *
     * @param v double to convert.
     * @param locale  if true, use "," else use "."
     * @return the string representation of the double.
     */
    VLE_API std::string toScientificString(
        const double& v, bool locale = false);

    /**
     * Demangle the input type info from C++ compiler.
     * http://gcc.gnu.org/onlinedocs/libstdc++/latest-doxygen/namespaceabi.html
     *
     * @param in the std::type_info to demangle.
     *
     * @return the demangled string or the same if libcwd is not linked.
     */
    VLE_API std::string demangle(const std::type_info& in);

    /**
     * Demangle the input string from C++ compiler.
     * http://gcc.gnu.org/onlinedocs/libstdc++/latest-doxygen/namespaceabi.html
     *
     * @param in the string to demangle.
     *
     * @return the demangled string or the same if libcwd is not linked.
     */
    VLE_API std::string demangle(const std::string& in);

}} // namespace vle utils

#endif
