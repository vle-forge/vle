/**
 * @file utils/Path.hpp
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

#ifndef UTILS_PATH_HPP
#define UTILS_PATH_HPP

#include <string>



namespace vle { namespace utils {

    /**
     * @brief Portable way, i.e. Linux/Unix/Windows to get VLE paths.  Linux
     * and Unix versions use compiled paths introduced by autotools and
     * Makefile, Windows version use Registry to store his paths. This class is
     * a singleton.
     *
     * To use it:
     * @code
     * std::string binpath = utils::Path::path()->getDefaultBinDir();
     * @endcode
     */
    class Path
    {
    public:
        inline const std::string& getDefaultBinDir() const
        { return mTab[0]; }

        inline const std::string& getUserBinDir() const
        { return mTab[1]; }

        inline const std::string& getDefaultModelDir() const
        { return mTab[2]; }

        inline const std::string& getUserModelDir() const
        { return mTab[3]; }

        inline const std::string& getDefaultGUIPluginDir() const
        { return mTab[4]; }

        inline const std::string& getUserGUIPluginDir() const
        { return mTab[5]; }

        inline const std::string& getDefaultObserverPluginDir() const
        { return mTab[6]; }

        inline const std::string& getUserObserverPluginDir() const
        { return mTab[7]; }

        inline const std::string& getDefaultPixmapsDir() const
        { return mTab[8]; }

        inline const std::string& getUserPixmapsDir() const
        { return mTab[9]; }

        inline const std::string& getDefaultGladeDir() const
        { return mTab[10]; }

        inline const std::string& getUserGladeDir() const
        { return mTab[11]; }

        inline const std::string& getUserDir() const
        { return mTab[12]; }

        inline const std::string& getDefaultStreamDir() const
        { return mTab[13]; }

        inline const std::string& getUserStreamDir() const
        { return mTab[14]; }    

        inline const std::string& getDefaultPythonDir() const
        { return mTab[15]; }

        inline const std::string& getUserPythonDir() const
        { return mTab[16]; }

        inline const std::string& getDefaultTranslatorDir() const
        { return mTab[17]; }

        inline const std::string& getUserTranslatorDir() const
        { return mTab[18]; }

        inline const std::string& getDefaultEOVPluginDir() const
        { return mTab[19]; }

        inline const std::string& getUserEOVPluginDir() const
        { return mTab[20]; }

        /** @return a Path object instantiate in singleton method. */
        inline static Path& path()
        { if (mPath == 0) mPath = new Path; return *mPath; }

        /** Delete Path object instantiate from function path(). */
        inline static void kill()
        { delete mPath; mPath = 0; }

        /**
         * Build a path using two string.
         *
         * For instance:
         * @code
         * std::string s = utils::Path::build_path("home", "vle");
         * @endcode
         *
         * This example return "home/vle" on Unix or "home\\vle" on Win32.
         *
         * @param left the string dirname.
         * @param right the string dirname.
         * @return the build path of left and right strings using the good
         * separator.
         */
        static std::string build_path(const std::string& left,
                                      const std::string& right);

        /**
         * Build a path from user home directory to specified dir. The fallowing
         * example return the string "/home/<user>/.vle/model".
         *
         * @code
         * std::string s = utils::Path::build_user_path("model");
         * // s == /home/gauthier/.vle/model on unix with my nickname.
         * // s == c:\Documents and Settings\gauthier\Mes documents on win32.
         * @endcode
         *
         * @param dir the directory to found into the user home dir.
         *
         * @return a concatenation of user home dir and specified directory.
         */
        static std::string build_user_path(const std::string& dir =
                                           std::string());

    private:
        Path();

        ~Path() { }

        bool init_path();

        std::string  mTab[21];
        static Path* mPath; /// The static Path for singleton design pattern.
    };

}} // namespace vle utils

#endif
