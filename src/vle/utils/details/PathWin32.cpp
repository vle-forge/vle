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

#include <windows.h>
#include <winreg.h>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <fstream>
#include <vle/utils/Context.hpp>
#include <vle/utils/ContextPrivate.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/details/UtilsWin.hpp>
#include <vle/vle.hpp>

namespace vle {
namespace utils {

static bool
win32_RegQueryValue(HKEY key, Path* path)
{
    DWORD buffer_size = MAX_PATH;
    auto buffer = std::make_unique<BYTE[]>(buffer_size);

    DWORD ret =
      RegQueryValueExW(key, L"Path", NULL, NULL, buffer.get(), &buffer_size);

    while (ret == ERROR_MORE_DATA) {
        buffer_size += 1;
        buffer = std::make_unique<BYTE[]>(buffer_size);

        ret = RegQueryValueExW(
          key, L"Path", NULL, NULL, buffer.get(), &buffer_size);
    }

    if (ret == ERROR_SUCCESS) {
        LPWSTR wstr = reinterpret_cast<LPWSTR>(buffer.get());
        wstr[buffer_size / sizeof(wchar_t)] = L'\0';
        path->set(wstr);

        return true;
    }

    return false;
}

Path
Context::findInstallPrefix()
{
    {
        /*
         * Try to read prefix from win32 registry (first in
         * HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER otherwise.
         */

        auto version = vle::version_abi();
        std::string key = utils::format("%s %d.%d",
                                        "SOFTWARE\\VLE Development Team\\VLE",
                                        std::get<0>(version),
                                        std::get<1>(version));

        auto widekey = from_utf8_to_wide(key);

        HKEY hkey;
        bool result;
        Path prefix;

        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                          widekey.c_str(),
                          0,
                          KEY_QUERY_VALUE,
                          &hkey) == ERROR_SUCCESS) {
            result = win32_RegQueryValue(hkey, &prefix);
            RegCloseKey(hkey);

            if (result)
                return utils::Path(prefix);
        }

        if (RegOpenKeyExW(
              HKEY_CURRENT_USER, widekey.c_str(), 0, KEY_QUERY_VALUE, &hkey) ==
            ERROR_SUCCESS) {
            result = win32_RegQueryValue(hkey, &prefix);
            RegCloseKey(hkey);

            if (result)
                return utils::Path(prefix);
        }
    }

    show_message_box("Failed to initialized prefix from registry. Try to "
                     "use the current executable to determine the prefix "
                     "but access to cmake, vle, curl or wget will fail.");

    {
        /*
         * Prefix from win32 registry is not found, we try to detect the
         * prefix from the current executable.
         */

        std::wstring filepath(MAX_PATH, '\0');
        DWORD size = ::GetModuleFileNameW(NULL, &filepath[0], filepath.size());

        while (size > filepath.size()) {
            filepath.resize(size, '\0');
            size = ::GetModuleFileNameW(NULL, &filepath[0], filepath.size());
        }

        if (size) {
            Path path(filepath);
            Path result = path.parent_path().parent_path();

            if (result.exists())
                return result;
        }
    }

    show_message_box("Failed to initialized prefix. Please, re-install "
                     "VLE or define a variable into the register "
                     "editor");

    return Path("NOT INITIALIZEDn");
}

Path
Context::findLibrary(const std::string& lib)
{
    Path res = getPrefixDir();
    res /= "bin";
    res /= lib;

    if (res.exists())
        return res.string();

    return {};
}

Path
Context::findProgram(const std::string& exe)
{
    Path res = getPrefixDir();
    res /= "bin";

    if (exe == "cmake" or exe == "cmake.exe")
        res /= "cmake.exe";
    else if (exe == "vle" or exe == "vle.exe")
        res /= "vle.exe";
    else if (exe == "curl" or exe == "curl.exe")
        res /= "curl.exe";
    else if (exe == "wget" or exe == "wget.exe")
        res /= "wget.exe";
    else {
        std::string msg = "Unknown program: ";
        msg += res.string();
        show_message_box(msg);
        res /= "willfail";
    }

    return res;
}

void
Context::initHomeDir()
{
    m_pimpl->m_home.clear();

    readHomeDir();
}

void
Context::initPrefixDir()
{
    m_pimpl->m_prefix = findInstallPrefix();
}
}
} // namespace vle utils
