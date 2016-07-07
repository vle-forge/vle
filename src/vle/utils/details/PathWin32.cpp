/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2016 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2016 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2016 INRA http://www.inra.fr
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


#include <vle/utils/Context.hpp>
#include <vle/utils/ContextPrivate.hpp>
#include <vle/utils/details/UtilsWin.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/version.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <windows.h>
#include <winreg.h>

namespace vle { namespace utils {

static void ErrorMessage(LPCSTR description)
{
    LPVOID msg;

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            GetLastError(),
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &msg,
            0, NULL);

    std::string finalmsg = (boost::format("%1%: %2%") % description % msg).str();

    MessageBox(NULL, (LPCTSTR)finalmsg.c_str(), TEXT("Error"), MB_ICONERROR);

    LocalFree(msg);
    ExitProcess(1);
}

static bool win32_RegQueryValue(HKEY hkey, std::string *path)
{
    DWORD sz;
    std::vector < TCHAR > buf(MAX_PATH, '\0');

    if (RegQueryValueEx(hkey, (LPCTSTR)"", NULL, NULL,
            (LPBYTE)&buf[0], &sz) == ERROR_SUCCESS) {
        buf[sz] = '\0';
        path->assign(&buf[0]);

        return true;
    } else {
        sz = MAX_PATH;
        if (RegQueryValueEx(hkey, (LPCTSTR)"Path", NULL, NULL,
                (LPBYTE)&buf[0], &sz) == ERROR_SUCCESS) {
            buf[sz] = '\0';
            path->assign(&buf[0]);

            return true;
        }
    }

    return false;
}

Path Context::findProgram(const std::string& exe)
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

void Context::initHomeDir()
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

void Context::initPrefixDir()
{
    {
        // Try to read prefix from win32 registry (first in
        // HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER otherwise.
        HKEY hkey;
        bool result;
        std::string key = (boost::format("%1% %2%.%3%.0") %
                           "SOFTWARE\\VLE Development Team\\VLE" %
                           VLE_MAJOR_VERSION % VLE_MINOR_VERSION).str();

        std::string prefix;

        if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, key.c_str(),
                         0, KEY_QUERY_VALUE, &hkey) == ERROR_SUCCESS) {
            result = win32_RegQueryValue(hkey, &prefix);
            RegCloseKey(hkey);

            if (result) {
                m_pimpl->m_prefix = prefix;
                return;
            }
        }

        if (RegOpenKeyEx(HKEY_CURRENT_USER, key.c_str(),
                         0, KEY_QUERY_VALUE, &hkey) == ERROR_SUCCESS) {
            result = win32_RegQueryValue(hkey, &prefix);
            RegCloseKey(hkey);

            if (result) {
                m_pimpl->m_prefix = prefix;
                return;
            }
        }
    }

    {
        // Try to read prefix from the Path of vle.exe

        DWORD size;
        std::vector < TCHAR > filepath(MAX_PATH, '\0');

        if ((size = GetModuleFileName(NULL, &filepath[0], MAX_PATH))) {
            std::vector < TCHAR > buf(MAX_PATH, '\0');

            Path path = &filepath[0];
            Path result = path.parent_path().parent_path();

            if (result.exists())
                m_pimpl->m_prefix = result;
        }
    }

    ErrorMessage("Failed to initialized prefix. Please, re-install VLE or "
            "define a variable into the the register editor");
}

}} // namespace vle utils
