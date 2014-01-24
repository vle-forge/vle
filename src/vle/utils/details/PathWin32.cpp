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


#include <vle/utils/Path.hpp>
#include <vle/utils/details/UtilsWin.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/version.hpp>
#include <glibmm/miscutils.h>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <windows.h>
#include <winreg.h>
#include <fstream>

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

    std::string finalmsg = (vle::fmt("%1%: %2%") % description % msg).str();

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

std::string Path::findProgram(const std::string& exe)
{
    std::string res("");
    if (exe == "cmake" or exe == "cmake.exe") {
        res =  Path::buildFilename(
                UtilsWin::convertPathTo83Path(Path::path().getPrefixDir()),
                                   "CMake","bin", "cmake.exe");
    } else {
        res = Glib::find_program_in_path(exe);
    }
    return res;
}

void Path::initHomeDir()
{
    m_home.clear();

    readHomeDir();

    /*
     * If no VLE_HOME, we build %HOMEDRIVE%%HOMEPATH%\vle directory.
     */
    if (m_home.empty()) {
        std::string homedrive(Glib::getenv("HOMEDRIVE"));
        std::string homepath(Glib::getenv("HOMEPATH"));
        m_home = utils::Path::buildDirname(homedrive, homepath, "vle");
    }
}

void Path::initPrefixDir()
{
    m_prefix.clear();

    HKEY hkey;
    bool result;
    std::string key(boost::str(fmt("%1% %2%.%3%.0") %
                               "SOFTWARE\\VLE Development Team\\VLE" %
                               VLE_MAJOR_VERSION % VLE_MINOR_VERSION));

    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, key.c_str(),
                     0, KEY_QUERY_VALUE, &hkey) == ERROR_SUCCESS) {
        result = win32_RegQueryValue(hkey, &m_prefix);
        RegCloseKey(hkey);

        if (result) {
            return;
        }
    }

    if (RegOpenKeyEx(HKEY_CURRENT_USER, key.c_str(),
                     0, KEY_QUERY_VALUE, &hkey) == ERROR_SUCCESS) {
        result = win32_RegQueryValue(hkey, &m_prefix);
        RegCloseKey(hkey);

        if (result) {
            return;
        }
    }

    DWORD size;
    std::vector < TCHAR > filepath(MAX_PATH, '\0');

    if ((size = GetModuleFileName(NULL, &filepath[0], MAX_PATH))) {
        std::vector < TCHAR > buf(MAX_PATH, '\0');

        boost::filesystem::path path(&filepath[0]);
        boost::filesystem::path result(path.parent_path().parent_path());

        boost::system::error_code ec;
        if (boost::filesystem::exists(result, ec)) {
            m_prefix.assign(result.string());
            return;
        }
    }

    ErrorMessage("Failed to initialized prefix. Please, re-install VLE or "
            "define a variable into the the register editor");
}

std::string Path::getTempFile(const std::string& prefix,
                              std::ofstream* file)
{
    if (file and not file->is_open()) {
        TCHAR filename[MAX_PATH];
        TCHAR tmp[MAX_PATH];
        DWORD result;

        result = ::GetTempPath(MAX_PATH, tmp);
        if (result and result < MAX_PATH) {
            result = ::GetTempFileName(tmp,
                                       prefix.c_str(),
                                       0,
                                       filename);

            if (result) {
                file->open(static_cast < char* >(filename),
                           std::ios_base::trunc | std::ios_base::out |
                           std::ios_base::binary);

                if (file->is_open()) {
                    return filename;
                }
            }
        }
    }

    return std::string();
}

}} // namespace vle utils
