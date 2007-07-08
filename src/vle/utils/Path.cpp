/**
 * @file utils/Path.cpp
 * @author The VLE Development Team.
 * @brief Portable way, i.e. Linux/Unix/Windows to get VLE paths.
 */

/*
 * Copyright (c) 2004, 2005 The vle Development Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include <glibmm/fileutils.h>
#include <glibmm/miscutils.h>
#include <list>
#include <vle/utils/Path.hpp>
#include <vle/utils/Exception.hpp>

#ifdef G_OS_WIN32
#	include <Windows.h>
#	include <winreg.h>
#endif



namespace vle { namespace utils {

Path* Path::mPath = 0;

Path::Path()
{
    if (init_path() == false)
	throw InternalError("Path initialization failed.");
}

std::string Path::build_path(const std::string& left,
			     const std::string& right)
{
    std::list < std::string > lst;
    lst.push_front(left);
    lst.push_back(right);
    return Glib::build_path(G_DIR_SEPARATOR_S, lst);
}

std::string Path::build_user_path(const std::string& dir)
{
    if (dir.empty()) {
        std::list < std::string > arb;
        arb.push_back(Glib::get_home_dir());
        arb.push_back(".vle");
        return Glib::build_path(G_DIR_SEPARATOR_S, arb);
    } else {
        std::list < std::string > arb;
        arb.push_back(Glib::get_home_dir());
        arb.push_back(".vle");
        arb.push_back(dir);
        return Glib::build_path(G_DIR_SEPARATOR_S, arb);
    }
}

#ifdef G_OS_WIN32

bool Path::init_path()
{
    LONG result;
    HKEY hkey;
    std::string system;

    result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\VLE", 0,
			  KEY_QUERY_VALUE, &hkey);

    if (result == ERROR_SUCCESS) {
	char* buf = new char[256];
	DWORD sz = 256;

	if (RegQueryValueEx(hkey, TEXT("system"), NULL, NULL,
			    (LPBYTE)buf, &sz) != ERROR_SUCCESS) {
	    delete[] buf;
	    return false;
	}
	system.assign(buf);
	delete[] buf;

	mTab[0] = build_path(buf, "bin");
	mTab[1] = build_user_path("bin");
	mTab[2] = build_path(buf, "models");
	mTab[3] = build_user_path("models");
	mTab[4] = build_path(buf, "plugins");
	mTab[5] = build_user_path("plugins");
	mTab[6] = build_path(buf, "observers");
	mTab[7] = build_user_path("observers");
	mTab[8] = build_path(buf, "img");
	mTab[9] = build_user_path("img");
	mTab[10] = build_path(buf, "glade");
	mTab[11] = build_user_path("glade");
        mTab[12] = build_user_path("vle");
	mTab[13] = build_path(buf, "stream");
	mTab[14] = build_user_path("stream");
	mTab[15] = build_path(buf, "python");
	mTab[16] = build_user_path("python");
	mTab[17] = build_path(buf, "translator");
	mTab[18] = build_user_path("translator");
	mTab[19] = build_path(buf, "eovplugin");
	mTab[20] = build_user_path("eovplugin");
	return true;
    }
    return false;
}

#else // On Unix, its simple...

std::string Path::build_prefix_path(const char* buf)
{
    std::list < std::string > lst;
    lst.push_front(VLE_PREFIX_DIR);
    lst.push_back(buf);
    return Glib::build_path(G_DIR_SEPARATOR_S, lst);
}

std::string Path::build_prefix_libraries_path(const char* name)
{
    std::list < std::string > lst;
    lst.push_back(VLE_LIBRARY_DIRS);
    lst.push_back(name);
    return Glib::build_path(G_DIR_SEPARATOR_S, lst);
}

std::string Path::build_prefix_share_path(const char* prg, const char* name)
{
    std::list < std::string > lst;
    lst.push_front(VLE_SHARE_DIRS);
    lst.push_back(prg);
    lst.push_back(name);
    return Glib::build_path(G_DIR_SEPARATOR_S, lst);
}

bool Path::init_path()
{
    mTab[0] = build_prefix_path("bin");
    mTab[1] = build_user_path("bin");
    mTab[2] = build_prefix_libraries_path("simulators");
    mTab[3] = build_user_path("simulators");
    mTab[4] = build_prefix_libraries_path("modeling");
    mTab[5] = build_user_path("modeling");
    mTab[6] = build_prefix_libraries_path("observer");
    mTab[7] = build_user_path("observers");
    mTab[8] = build_prefix_share_path("vle", "images");
    mTab[9] = build_user_path("images");
    mTab[10] = VLE_SHARE_DIRS;
    mTab[11] = build_prefix_share_path("gvle", "glade");
    mTab[12] = build_user_path();
    mTab[13] = build_prefix_libraries_path("stream");
    mTab[14] = build_user_path("stream");
    mTab[15] = build_prefix_libraries_path("python");
    mTab[16] = build_user_path("python");
    mTab[17] = build_prefix_libraries_path("translator");
    mTab[18] = build_user_path("translator");
    mTab[19] = build_prefix_libraries_path("stream");
    mTab[20] = build_user_path("eovplugin");
    mTab[21] = build_prefix_libraries_path("analysis");
    mTab[22] = build_user_path("avleplugin");
    return true;
}

#endif // Win32 specific needs.

}} // namespace vle utils
