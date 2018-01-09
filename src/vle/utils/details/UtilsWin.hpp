/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2018 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2018 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2018 INRA http://www.inra.fr
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

#ifndef VLE_UTILS_DETAILS_UTILS_WIN_HPP
#define VLE_UTILS_DETAILS_UTILS_WIN_HPP

#include <direct.h>
#include <tchar.h>
#include <windows.h>

#include <string>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Filesystem.hpp>

namespace vle {
namespace utils {

/**
 * Template structure which uses the type of @c TCHAR (@c char* or @c
 * wchat_t*) and define @c std::string or @c std::wstring.
 *
 * @code
 * using string_type = tchar_type<TCHAR>::value_type;
 * @endcode
 */
template<typename T>
struct tchar_type
{
};

template<>
struct tchar_type<char>
{
    using value_type = std::string;
};

template<>
struct tchar_type<wchar_t>
{
    using value_type = std::wstring;
};

/**
 * Convert utf8 @c std::string into utf16 std::wstring.
 * @param str The utf8 string to convert.
 * @return The wide string.
 *
 * @exception @c std::bad_alloc.
 * @exception @c CastError if an error occurred in low API.
 */
inline std::wstring
from_utf8_to_wide(std::string str)
{
    if (str.empty())
        return {};

    DWORD size = MAX_PATH;
    std::wstring ret;

    do {
        ret.resize(MAX_PATH);
        size = ::MultiByteToWideChar(
          CP_UTF8, 0, str.c_str(), (int)str.size(), &ret[0], (int)ret.size());
    } while (size > ret.size());

    if (size <= 0)
        throw CastError("Fail to convert `%s' to multibyte string",
                        str.c_str());

    ret.resize(size);
    return ret;
}

/**
 * Convert wide @c std::wstring into utf8 std::string.
 * @param wstr The wide string to convert.
 * @return The utf8 string.
 *
 * @exception @c std::bad_alloc.
 * @exception @c CastError if an error occurred in low API.
 */
inline std::string
from_wide_to_utf8(std::wstring wstr)
{
    if (wstr.empty())
        return {};

    DWORD size = MAX_PATH;
    std::string ret;

    do {
        ret.resize(size);
        size = ::WideCharToMultiByte(CP_UTF8,
                                     0,
                                     wstr.c_str(),
                                     (int)wstr.size(),
                                     &ret[0],
                                     (int)ret.size(),
                                     NULL,
                                     NULL);
    } while (size > ret.size());

    if (size <= 0)
        throw CastError("Fail to convert a wide char string");

    ret.resize(size);
    return ret;
}

/**
 * Convert a wide @c TCHAR string using @c wchar_t type into a utf8 string
 * stored in a @c std::string. Conversion is done via the @c from_wide_to_utf8
 * function.
 * @param wstr The wide string to convert.
 *
 * @exception @c std::bad_alloc.
 * @exception @c CastError if an error occurred in low API.
 */
inline std::string
from_tchar(const wchar_t* wstr)
{
    return from_wide_to_utf8(std::wstring(wstr));
}

/**
 * Convert a @c TCHAR string using @c char type into a utf8 string stored in
 * a @c std::string. No conversion is done but this function provides the same
 * API as above.
 * @param str The utf8 string to convert.
 *
 * @exception @c std::bad_alloc.
 */
inline std::string
from_tchar(const char* str)
{
    return std::string(str);
}

/**
 * Convert a file system pathname into a short path name.
 * @param path Path name to convert.
 * @return The converted pathname or the original pathname if an error
 *     occurred.
 *
 * @exception @c std::bad_alloc.
 * @exception @c CastError if an error occurred in low API.
 */
inline Path
convert_to_short_pathname(const Path& path)
{
    DWORD size = MAX_PATH;
    std::wstring buffer;

    do {
        buffer.resize(size);
        size = ::GetShortPathNameW(
          path.wstring().c_str(), &buffer[0], buffer.size());
    } while (size > buffer.size());

    if (size <= 0)
        throw CastError("Fail to convert a Path to short path");

    buffer.resize(size);
    return Path(buffer);
}

/**
 * Get the value of a environment variable.
 * @param name The name of the variable.
 * @return The value of the variable or an empty string.
 *
 * @exception @c std::bad_alloc.
 */
inline std::wstring
get_environment_variable_wide(std::wstring name)
{
    if (name.empty())
        return {};

    std::wstring buffer;
    DWORD size = MAX_PATH;

    do {
        buffer.resize(size);
        size =
          ::GetEnvironmentVariableW(name.c_str(), &buffer[0], buffer.size());
    } while (size > buffer.size());

    if (size <= 0)
        return {};

    buffer.resize(size);

    return buffer;
}

/**
 * Get the value of a environment variable.
 * @param name The name of the variable.
 * @return The value of the variable or an empty string.
 *
 * @exception @c std::bad_alloc.
 */
inline std::string
get_environment_variable(std::string name)
{
    return from_wide_to_utf8(
      get_environment_variable_wide(from_utf8_to_wide(name)));
}

/**
 * Assign the value for a environment variable.
 *
 * @param name The name of the variable.
 * @param value  The value of the variable.
 * @return true if success, false otherwise.
 *
 * @exception @c CastError if an error occurred in low API.
 */
inline bool
set_environment_variable(std::string name, std::string value)
{
    return ::SetEnvironmentVariableW(from_utf8_to_wide(name).c_str(),
                                     from_utf8_to_wide(value).c_str());
}

/**
 * Assign the value for a environment variable.
 *
 * @param name The name of the variable.
 * @param value  The value of the variable.
 * @return true if success, false otherwise.
 *
 * @exception @c CastError if an error occurred in low API.
 */
inline bool
set_environment_variable_wide(std::wstring name, std::wstring value)
{
    return ::SetEnvironmentVariableW(name.c_str(), value.c_str());
}

/**
 * Retrieves the Win32 system message for the error id.
 * @param error_id Produces by the @c GetLastError function.
 * @return An utf8 string.
 *
 * @exception std::bad_alloc.
 * @exception @c CastError if an error occurred in low API.
 */
inline std::string
format_message(DWORD error_id)
{
    LPWSTR buffer = NULL;

    DWORD size = ::FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                    FORMAT_MESSAGE_FROM_SYSTEM |
                                    FORMAT_MESSAGE_IGNORE_INSERTS,
                                  NULL,
                                  error_id,
                                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                  (LPWSTR)&buffer,
                                  BUFSIZ,
                                  NULL);
    if (size <= 0)
        return {};

    buffer[size] = '\0';

    std::string ret;

    try {
        ret = from_wide_to_utf8(buffer);
    } catch (const std::bad_alloc& e) {
        LocalFree(buffer);
        throw e;
    }

    return ret;
}

/**
 * Shows an error message box using the Win32 API on current display with the
 * specified utf8 string.
 * @param description The content to be show in the dialog box.
 *
 * @exception std::bad_alloc.
 * @exception @c CastError if an error occurred in low API.
 */
inline void
show_message_box(std::string description)
{
    ::MessageBoxW(NULL,
                  from_utf8_to_wide(description).c_str(),
                  (LPCWSTR)L"VLE Error",
                  MB_ICONERROR | MB_OK);
}
}
} // namespace vle utils

#endif
