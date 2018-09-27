/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * https://www.vle-project.org
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

#include <vle/utils/Exception.hpp>
#include <vle/utils/details/UtilsWin.hpp>

#include <string>

#include <direct.h>
#include <tchar.h>
#include <windows.h>

namespace vle {
namespace utils {

std::wstring
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

std::string
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

std::string
from_tchar(const wchar_t* wstr)
{
    return from_wide_to_utf8(std::wstring(wstr));
}

std::string
from_tchar(const char* str)
{
    return std::string(str);
}

Path
convert_to_short_pathname(const Path& path)
{
    DWORD size = MAX_PATH;
    std::wstring buffer;

    do {
        buffer.resize(size);
        size = ::GetShortPathNameW(path.wstring().c_str(),
                                   &buffer[0],
                                   static_cast<DWORD>(buffer.size()));
    } while (size > buffer.size());

    if (size <= 0)
        throw CastError("Fail to convert a Path to short path");

    buffer.resize(size);
    return Path(buffer);
}

std::wstring
get_environment_variable_wide(std::wstring name)
{
    if (name.empty())
        return {};

    std::wstring buffer;
    DWORD size = MAX_PATH;

    do {
        buffer.resize(size);
        size = ::GetEnvironmentVariableW(
          name.c_str(), &buffer[0], static_cast<DWORD>(buffer.size()));
    } while (size > buffer.size());

    if (size <= 0)
        return {};

    buffer.resize(size);

    return buffer;
}

std::string
get_environment_variable(std::string name)
{
    return from_wide_to_utf8(
      get_environment_variable_wide(from_utf8_to_wide(name)));
}

bool
set_environment_variable(std::string name, std::string value)
{
    return ::SetEnvironmentVariableW(from_utf8_to_wide(name).c_str(),
                                     from_utf8_to_wide(value).c_str());
}
bool
set_environment_variable_wide(std::wstring name, std::wstring value)
{
    return ::SetEnvironmentVariableW(name.c_str(), value.c_str());
}

std::string
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

void
show_message_box(std::string description)
{
    ::MessageBoxW(NULL,
                  from_utf8_to_wide(description).c_str(),
                  (LPCWSTR)L"VLE Error",
                  MB_ICONERROR | MB_OK);
}
}
} // namespace vle utils
