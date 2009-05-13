/**
 * @file vle/utils/PathWin32.cpp
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


#include <glibmm/fileutils.h>
#include <glibmm/miscutils.h>
#include <vle/utils/Path.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/version.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <Windows.h>
#include <winreg.h>

namespace vle { namespace utils {

std::string Path::initHomeDir()
{
    m_home.clear();

    readHomeDir();

    if (m_home.empty()) {
        std::list < std::string > lst;
        lst.push_back(Glib::get_home_dir());
        lst.push_back("vle");
        m_home = Glib::build_path(G_DIR_SEPARATOR_S, lst);
    }
}

bool Path::initPath()
{
    initHomeDir();

    {
        LONG result;
        HKEY hkey;
        std::string key(boost::str(fmt("%1% %2%.%3%.0") %
                    "SOFTWARE\\VLE Development Team\\VLE" %
                    VLE_MAJOR_VERSION % VLE_MINOR_VERSION));

        if ((result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, key.c_str(), 0,
                        KEY_QUERY_VALUE, &hkey)) != ERROR_SUCCESS) {
            result = RegOpenKeyEx(HKEY_CURRENT_USER, key.c_str(),
                    0, KEY_QUERY_VALUE, &hkey);
        }

        if (result == ERROR_SUCCESS) {
            DWORD sz = 256;
            char* buf = new char[sz];

            if (RegQueryValueEx(hkey, (LPCTSTR)"", NULL, NULL,
                        (LPBYTE)buf, &sz) == ERROR_SUCCESS) {
                m_prefix.assign(buf);
            } else {
                sz = 256;
                if (RegQueryValueEx(hkey, (LPCTSTR)"Path", NULL, NULL,
                            (LPBYTE)buf, &sz) == ERROR_SUCCESS) {
                    m_prefix.assign(buf);
                }
            }
            delete[] buf;
        }
    }

    addSimulatorDir(getSimulatorDir());
    addStreamDir(getStreamDir());
    addOutputDir(getOutputDir());
    addConditionDir(getConditionDir());

    addSimulatorDir(getHomeSimulatorDir());
    addStreamDir(getHomeStreamDir());
    addOutputDir(getHomeOutputDir());
    addConditionDir(getHomeConditionDir());

    return true;
}

}} // namespace vle utils
