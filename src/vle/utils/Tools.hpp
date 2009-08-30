/**
 * @file vle/utils/Tools.hpp
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


#ifndef UTILS_TOOLS_HPP
#define UTILS_TOOLS_HPP

#include <glibmm/fileutils.h>
#include <glibmm/miscutils.h>
#include <boost/format.hpp>
#include <cstdarg>
#include <string>
#include <sstream>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <vle/utils/DllDefines.hpp>

namespace vle { namespace utils {

    /**
     * test file existence.
     *
     * @param filename file name to test.
     * @return true if exist, false otherwise.
     */
    inline bool exist_file(const std::string& filename)
    {
        return Glib::file_test(filename, Glib::FILE_TEST_IS_REGULAR |
                               Glib::FILE_TEST_EXISTS);
    }

    /**
     * test directory existence.
     *
     * @param dirname directory name to test.
     * @return true if exist, false otherwise.
     */
    inline bool exist_dir(const std::string& dirname)
    {
        return Glib::file_test(dirname, Glib::FILE_TEST_IS_DIR |
                               Glib::FILE_TEST_EXISTS);
    }

    /**
     * build a portable tempory filename like:
     *
     * @code
     * $TMP/<filename> on Unix/Linux
     * $ c:\windows\tmp\<filename> on Windows
     * @endcode
     *
     * @param filename string to add on tempory directory
     * @return a portable tempory filename
     */
    inline std::string build_temp(const std::string& filename)
    {
        return Glib::build_filename(Glib::get_tmp_dir(), filename);
    }

    /**
     * Write specified buffer to tempory file and return the filename. The file
     * is created in the tempory directory.
     *
     * @param prefix if you want to prefix the tempory file.
     * @param buffer buffer to write.
     * @return filename of the new tempory file.
     */
    VLE_UTILS_EXPORT std::string write_to_temp(const std::string& prefix,
                              const std::string& buffer);

    /**
     * Transform a string into an object of class C.
     *
     * example :
     * @code
     * double d = translate<double>("32.435");
     * @endcode
     *
     * @param str std::string to translate into class C type.
     * @return class C.
     */
    template<class C> inline C translate(const std::string& str)
    {
        std::stringstream i;

        C c;
        i << str;
        i >> c;

        return c;
    }

    /**
     * Assert a file existence. Idem exist_file but throw an Exception if
     * file does not exist.
     *
     * @param filename file name to test
     * @throw Exception::File
     */
    inline void assert_exist_file(const std::string& filename)
    {
        if (not exist_file(filename)) {
            throw FileError((fmt(_("File %1% does not exist\n")) %
                            filename).str());
        }
    }

    /**
     * Return true if str can be translate into a double.
     *
     * @param str string to test.
     * @return true if str can be translate, otherwise false.
     */
    inline bool is_double(const std::string& str)
    {
        try {
            boost::lexical_cast < double >(str);
            return true;
        } catch(const boost::bad_lexical_cast& e) {
            return false;
        }
    }


    /**
     * Return true if str can be translate into an integer.
     *
     * @param str string to test.
     * @return true if str can be translate, otherwise false.
     */
    inline bool is_int(const std::string& str)
    {
        try {
            boost::lexical_cast < int >(str);
            return true;
        } catch(const boost::bad_lexical_cast& e) {
            return false;
        }
    }

    /**
     * Return conversion from string into boolean.
     *
     * @param str string to convert.
     * @return true if str == TRUE or true, or a integer !0.
     */
    inline bool is_boolean(const std::string& str)
    {
        if (str == "true") {
            return true;
        } else if (str == "false") {
            return false;
        } else {
            try {
                return boost::lexical_cast < bool >(str);
            } catch(const boost::bad_lexical_cast& e) {
                //TRACE(e.what());
                return false;
            }
        }
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * Transform an object of class C into a string
     *
     * example :
     * @code
     * std::string s = to_string(123.324, 5, 1);
     * @endcode
     *
     * @param c object to transform
     * @param precision
     * @param width
     * @return string representation of templace < class C >
     */
    template <class C > inline
    std::string to_string(const C& c, int precision, int width)
    {
        std::ostringstream o;

        o.precision(precision);
        o.width(width);
        o << c;

        return o.str();
    }

    /**
     * Return conversion from template into string.
     *
     * example :
     * @code
     * std::string v = to_string(23243);
     * std::string l = to_string(141.12341);
     * std::string e = to_string("hello");
     * @endcode
     *
     * @param c object to transform
     * @return string representation of templace < class C >
     */
    template < class C > inline
    std::string to_string(const C& c)
    {
        std::ostringstream o;

        o << c;

        return o.str();
    }

    /**
     * Return conversion from string into double.
     *
     * @param str string to convert.
     * @param def default value to return.
     * @return result of convertion, default if error.
     */
    inline double to_double(const std::string& str,
                                             double def = 0.0)
    {
        try {
            return boost::lexical_cast < double >(str);
        } catch(const boost::bad_lexical_cast& e) {
            //TRACE(e.what());
            return def;
        }
    }

    /**
     * Return conversion from string into long.
     *
     * @param str string to convert.
     * @param def default value to return.
     * @return result of convertion, default if error.
     */
    inline long to_long(const std::string& str, int def = 0l)
    {
        try {
            return boost::lexical_cast < long >(str);
        } catch(const boost::bad_lexical_cast& e) {
            //TRACE(e.what());
            return def;
        }
    }

    /**
     * Return conversion from string into int.
     *
     * @param str string to convert.
     * @param def default value to return.
     * @return result of convertion, default if error.
     */
    inline int to_int(const std::string& str, int def = 0)
    {
        try {
            return boost::lexical_cast < int >(str);
        } catch(const boost::bad_lexical_cast& e) {
            //TRACE(e.what());
            return def;
        }
    }

    /**
     * Return conversion from string into unsigned int.
     *
     * @param str string to convert.
     * @param def default value to return.
     * @return result of convertion, default if error.
     */
    inline unsigned int to_uint(const std::string& str,
                                                 unsigned int def = 0)
    {
        try {
            return boost::lexical_cast < unsigned int >(str);
        } catch(const boost::bad_lexical_cast& e) {
            //TRACE(e.what());
            return def;
        }
    }

    /**
     * Return conversion from string into size_t.
     *
     * @param str string to convert.
     * @param def default value to return.
     * @return result of convertion, default if error.
     */
    inline size_t to_size_t(const std::string& str,
                                             size_t def = 0)
    {
        try {
            return boost::lexical_cast < size_t >(str);
        } catch(const boost::bad_lexical_cast& e) {
            //TRACE(e.what());
            return def;
        }
    }

    /**
     * Return true if str can be translate into a boolean.
     *
     * @param str string to test.
     * @param def default value to return.
     * @return result of convertion, default if error.
     */
    inline bool to_boolean(const std::string& str,
                                            bool def = false)
    {
        if (str == "true") {
            return true;
        } else if (str == "false") {
            return false;
        } else {
            try {
                return boost::lexical_cast < bool >(str);
            } catch(const boost::bad_lexical_cast& e) {
                //TRACE(e.what());
                return def;
            }
        }
    }

    /**
     * Clean string representation of double.
     *
     * @param str string to clean.
     * @throw Exception::BadArg.
     */
    inline void clean_double(std::string& str)
    {
        std::istringstream in(str);
        double d;
        in >> d;

        if (in.fail() == true) {
            throw ArgError(fmt(_("Cannot clean '%1%' into an double\n")) % str);
        }

        std::ostringstream out;
        out << d;

        str.assign(out.str());
    }

    /**
     * Clean string representation of double.
     *
     * @param str string to clean.
     * @throw Exception::BadArg.
     */
    inline void clean_int(std::string& str)
    {
        std::istringstream in(str);
        int i;
        in >> i;

        if (in.fail() == true) {
            throw ArgError(fmt(_("cannot clean '%1%' into an int\n")) % str);
        }

        std::ostringstream out;
        out << i;

        str.assign(out.str());
    }

    /**
     * Clean string representation of double.
     *
     * @param str string to clean.
     * @throw Exception::BadArg.
     */
    inline std::string clean_double(const std::string& str)
    {
        std::istringstream in(str);
        double d;
        in >> d;

        if (in.fail() == true) {
            throw ArgError(fmt(_("cannot clean '%1%' into an double\n")) % str);
        }

        std::ostringstream out;
        out << d;

        return out.str();
    }

    /**
     * Clean string representation of double.
     *
     * @param str string to clean.
     * @throw Exception::BadArg.
     */
    inline std::string clean_int(const std::string& str)
    {
        std::string val(str);
        boost::algorithm::trim(val);

        std::istringstream in(str);
        int i;
        in >> i;

        if (in.fail() == true) {
            throw ArgError(fmt(_("cannot clean '%1%' into an int\n")) % str);
        }

        std::ostringstream out;
        out << i;

        return out.str();
    }

    /**
     * @brief Write the current date and time conform to RFC 822.
     *
     * @return string representation of date.
     */
    VLE_UTILS_EXPORT std::string get_current_date();

    /**
     * @brief Write the current date and time in the format:
     * @code
     * std::cout << get_simple_current_date();
     * // 20080306-1534
     * @endcode
     *
     * @return string representation of the date.
     */
    VLE_UTILS_EXPORT std::string get_simple_current_date();

    /**
     * Return true if unicode string str is an ascii string with only
     * character between A-Z and a-z and size not empty.
     *
     * @param str unicode string to test validity in VLE name
     * @return true if str is correct, otherwise, false.
     */
    VLE_UTILS_EXPORT bool is_vle_string(const Glib::ustring& str);

    /**
     * @Return the stirng representation of error. This string begin with ten
     * minus characters.
     *
     */
    VLE_UTILS_EXPORT Glib::ustring print_trace_report();

    /**
     * Slot of lauched signals.
     *
     * @param signal_number signal description.
     */
    VLE_UTILS_EXPORT void print_trace_signals(int signal_number);

    /** Connection signal to print_trace_signals. */
    VLE_UTILS_EXPORT void install_signal();

    /**
     * @brief Demangle the input type info from C++ compiler.
     * http://gcc.gnu.org/onlinedocs/libstdc++/latest-doxygen/namespaceabi.html
     *
     * @param in the std::type_info to demangle.
     *
     * @return the demangled string or the same if libcwd is not linked.
     */
    VLE_UTILS_EXPORT std::string demangle(const std::type_info& in);

    /**
     * @brief Demangle the input string from C++ compiler.
     * http://gcc.gnu.org/onlinedocs/libstdc++/latest-doxygen/namespaceabi.html
     *
     * @param in the string to demangle.
     *
     * @return the demangled string or the same if libcwd is not linked.
     */
    VLE_UTILS_EXPORT std::string demangle(const std::string& in);

    /**
     * @brief Get the user vle directory, $HOME/.vle and build it if necessary.
     * All error are reported to log file.
     *
     * @return The string $HOME/.vle or empty string if error.
     */
    VLE_UTILS_EXPORT std::string getUserDirectory();

    /**
     * @brief Change the current process to daemon. Current path is reset to
     * '/' on Linux/Unix and 'c://' to Windows, adjust the session and
     * close all opened files.
     */
    VLE_UTILS_EXPORT void buildDaemon();

    /**
     * @brief Initialize the VLE system by:
     * - installling signal (segmentation fault etc.) to standard error
     *   function.
     * - initialize the user directory ($HOME/.vle/ etc.).
     * - initialize the WinSock
     * - initialize the thread system.
     */
    VLE_UTILS_EXPORT void init();

    /**
     * @brief Destroy all singleton and close systems.
     */
    VLE_UTILS_EXPORT void finalize();

    /**
     * @brief Print the VLE version, copyright and an help message.
     *
     * @param out The stream where push the information.
     */
    VLE_UTILS_EXPORT void printHelp(std::ostream& out);

    /**
     * @brief Return the VLE version, copyright and the licences.
     *
     * @param out The stream where push the informations.
     */
    VLE_UTILS_EXPORT void printInformations(std::ostream& out);

    /**
     * @brief Return the VLE version
     *
     * @param out
     */
    VLE_UTILS_EXPORT void printVersion(std::ostream& out);

}} // namespace vle utils

#endif
