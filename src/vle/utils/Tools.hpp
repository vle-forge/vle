/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2017 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2017 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2017 INRA http://www.inra.fr
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

#include <limits>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <vector>
#include <vle/DllDefines.hpp>

#if defined(__GNUC__)
#define GCC_ATTRIBUTE_FORMAT(idformat, idarg)                                 \
    __attribute__((format(printf, (idformat), (idarg))))
#else
#define GCC_ATTRIBUTE_FORMAT(idformat, idarg)
#endif

#if defined(__GNUC__) && !defined(__clang__)
#if __GNUC__ >= 5
#define VLE_CXX11_CONSTEXPR constexpr
#define VLE_CXX14_CONSTEXPR constexpr
#else
#define VLE_CXX11_CONSTEXPR constexpr
#define VLE_CXX14_CONSTEXPR
#endif
#elif defined(__clang__)
#if __has_feature(cxx_relaxed_constexpr)
#define VLE_CXX11_CONSTEXPR constexpr
#define VLE_CXX14_CONSTEXPR constexpr
#else
#define VLE_CXX11_CONSTEXPR constexpr
#define VLE_CXX14_CONSTEXPR
#endif
#else
#define VLE_CXX11_CONSTEXPR
#define VLE_CXX14_CONSTEXPR
#endif

namespace vle {
namespace utils {

/**
 * The function \c format() produce a \c std::string according to a C
 * printf format as described in standard \c printf() function.
 *
 * \code
 * std::string A = vle::utils::format("%d is good!", 1);
 * std::string B = vle::utils::format("%s %s!", "hello", "world");
 * \endcode
 *
 * \param format The format to convert.
 *
 * \return a empty string if an error occured otherwise the \e printf()
 * formatted string.
 */
std::string VLE_API format(const char* format,
                           ...) noexcept GCC_ATTRIBUTE_FORMAT(1, 2);

/**
 * The function \c vformat() produces a \c std::string according to a C
 * printf format as described in standard \c vprintf() function.
 *
 * \param format The format to convert.
 * \param ap Arguments.
 *
 * \return a empty string if an error occured otherwise the \e printf()
 * formatted string.
 */
std::string VLE_API vformat(const char* format, va_list ap) noexcept;

/**
 * Return true if the string \c str can be translated the template type \c T.
 * The template type \c T can be one of the following type: bool, float,
 * double, int8_t, uint8_t, int16_t, uint16_t, int32_t or uint32_t.
 *
 * @param str The string to convert.
 *
 * @return true if string can be translated into the type \c T.
 */
template <typename T>
bool is(const std::string& str);

/**
 * Convert the template type \c T into the string representation.
 * The template type \c T can be one of the following type: bool, float,
 * double, int8_t, uint8_t, int16_t, uint16_t, int32_t or uint32_t.
 *
 * @param t An instance of the type \c T to convert.
 *
 * @return A string representation.
 */
template <typename T>
std::string to(const T t);

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
template <typename T>
T to(const std::string& str);

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
template <typename T>
T convert(const std::string& value,
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
VLE_API std::string toScientificString(const double& v, bool locale = false);

/**
 * Tokenize a string with a delimiter
 * @param[in]  str, the string to tokenize
 * @param[out] toks, vector filled with tokens
 * @param[in]  delim, the delimiter for tokenizing
 * @param[in]  trimEmpty, if true remove empty tokens
 * @return the tokens
 */
VLE_API void tokenize(const std::string& str,
                      std::vector<std::string>& toks,
                      const std::string& delim,
                      bool trimEmpty);

/**
 * @brief Return true if @c Source can be casted into @c Target integer
 * type.
 * @details Check if the integer type @c Source is castable into @c Target.
 *
 * @param arg A value.
 * @tparam Source An integer type.
 * @tparam Target An integer type.
 * @return true if @c static_cast<Target>(Source) is valid.
 */
template <typename Target, typename Source>
VLE_CXX14_CONSTEXPR inline bool
is_numeric_castable(Source arg)
{
    static_assert(std::is_integral<Source>::value, "Integer required.");
    static_assert(std::is_integral<Target>::value, "Integer required.");

    using arg_traits = std::numeric_limits<Source>;
    using result_traits = std::numeric_limits<Target>;

    if (result_traits::digits == arg_traits::digits and
        result_traits::is_signed == arg_traits::is_signed)
        return true;

    if (result_traits::digits > arg_traits::digits)
        return result_traits::is_signed or arg >= 0;

    if (arg_traits::is_signed and
        arg < static_cast<Source>(result_traits::min()))
        return false;

    return arg <= static_cast<Source>(result_traits::max());
}

/**
 * @brief An internal exception when an integer cast fail.
 * @details @c numeric_cast_error is used with the
 * @e vle::utils::numerci_cast<TargetT>(SourceT) function that cast
 * integer type to another.
 */
struct VLE_API numeric_cast_error : public std::exception
{
    virtual const char* what() const noexcept
    {
        return "numeric cast error: loss of range in numeric_cast";
    }
};

/**
 * @brief Tries to convert @c Source into @c Target integer.
 * @code
 * std::vector<int> v(1000, 5);
 *
 * // No exception.
 * int i = vle::utils::numeric_cast<int>(v.size());
 *
 * // Throw exception.
 * int8_t j = vle::utils::numeric_cast<int8_t>(v.size());
 * @endcode
 *
 * @param arg A value.
 * @tparam Source An integer type.
 * @tparam Target An integer type.
 * @return @c static_cast<Target>(Source) integer.
 */
template <typename Target, typename Source>
VLE_CXX14_CONSTEXPR inline Target
numeric_cast(Source s)
{
    if (not is_numeric_castable<Target>(s))
        throw vle::utils::numeric_cast_error();

    return static_cast<Target>(s);
}
}
} // namespace vle utils

#endif
