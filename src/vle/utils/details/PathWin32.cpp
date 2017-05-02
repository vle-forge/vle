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

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <fstream>
#include <vle/utils/Context.hpp>
#include <vle/utils/ContextPrivate.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Tools.hpp>
#include <vle/utils/details/UtilsWin.hpp>
#include <vle/vle.hpp>
#include <windows.h>
#include <winreg.h>

namespace vle {
namespace utils {

static void
ErrorMessage(LPCSTR description)
{
    LPVOID msg;

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL,
                  GetLastError(),
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (LPTSTR)&msg,
                  0,
                  NULL);

    std::string finalmsg =
      (boost::format("%1%: %2%") % description % msg).str();

    MessageBox(NULL, (LPCTSTR)finalmsg.c_str(), TEXT("Error"), MB_ICONERROR);

    LocalFree(msg);
    ExitProcess(1);
}

static bool
win32_RegQueryValue(HKEY hkey, std::string* path)
{
    DWORD sz;
    std::vector<TCHAR> buf(MAX_PATH, '\0');

    if (RegQueryValueEx(
          hkey, (LPCTSTR) "", NULL, NULL, (LPBYTE)&buf[0], &sz) ==
        ERROR_SUCCESS) {
        buf[sz] = '\0';
        path->assign(&buf[0]);

        return true;
    } else {
        sz = MAX_PATH;
        if (RegQueryValueEx(
              hkey, (LPCTSTR) "Path", NULL, NULL, (LPBYTE)&buf[0], &sz) ==
            ERROR_SUCCESS) {
            buf[sz] = '\0';
            path->assign(&buf[0]);

            return true;
        }
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
        std::string key = utils::format("%s %d.%d.0",
                                        "SOFTWARE\\VLE Development Team\\VLE",
                                        std::get<0>(version),
                                        std::get<1>(version));

        HKEY hkey;
        bool result;
        std::string prefix;

        if (RegOpenKeyEx(
              HKEY_LOCAL_MACHINE, key.c_str(), 0, KEY_QUERY_VALUE, &hkey) ==
            ERROR_SUCCESS) {
            result = win32_RegQueryValue(hkey, &prefix);
            RegCloseKey(hkey);

            if (result)
                return utils::Path(prefix);
        }

        if (RegOpenKeyEx(
              HKEY_CURRENT_USER, key.c_str(), 0, KEY_QUERY_VALUE, &hkey) ==
            ERROR_SUCCESS) {
            result = win32_RegQueryValue(hkey, &prefix);
            RegCloseKey(hkey);

            if (result)
                return utils::Path(prefix);
        }
    }

    {
        /*
         * Prefix from win32 registry is not found, we try to detect the
         * prefix from the current executable.
         */

        DWORD size;
        std::vector<TCHAR> filepath(MAX_PATH, '\0');

        if ((size = GetModuleFileName(NULL, &filepath[0], MAX_PATH))) {
            std::vector<TCHAR> buf(MAX_PATH, '\0');

            Path path = &filepath[0];
            Path result = path.parent_path().parent_path();

            if (result.exists())
                return result;
        }
    }

    ErrorMessage("Failed to initialized prefix. Please, re-install VLE or "
                 "define a variable into the the register editor");

    return {};
}

Path
Context::findLibrary(const std::string& lib)
{
    Path res = UtilsWin::convertPathTo83Path(getPrefixDir());
    res /= "bin";
    res /= lib;

    if (res.exists())
        return res.string();

    return {};
}

Path
Context::findProgram(const std::string& exe)
{
    Path res = UtilsWin::convertPathTo83Path(getPrefixDir());
    res /= "bin";

    if (exe == "cmake" or exe == "cmake.exe")
        res /= "cmake.exe";
    else if (exe == "vle" or exe == "vle.exe")
        res /= "vle.exe";
    else if (exe == "curl" or exe == "curl.exe")
        res /= "curl.exe";
    else if (exe == "wget" or exe == "wget.exe")
        res /= "wget.exe";
    else
        res /= "willfail";

    return res;
}

void
Context::initHomeDir()
{
    m_pimpl->m_home.clear();

    readHomeDir();

    /*
     * If no VLE_HOME, we build %HOMEDRIVE%%HOMEPATH%\vle directory.
     */
    if (m_pimpl->m_home.empty()) {
        const char* homedrive_str = std::getenv("HOMEDRIVE");
        std::string homedrive("");
        if (homedrive_str) {
            homedrive.assign(homedrive_str);
        }
        const char* homepath_str = std::getenv("HOMEPATH");
        std::string homepath("");
        if (homepath_str) {
            homepath.assign(homepath_str);
        }

        m_pimpl->m_home = homedrive;
        m_pimpl->m_home /= homepath;
        m_pimpl->m_home /= "vle";
    }
}

void
Context::initPrefixDir()
{
    m_pimpl->m_prefix = findInstallPrefix();
}
}
} // namespace vle utils
