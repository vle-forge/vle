/**
 * @file vle/utils/Tools.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
 * Copyright (C) 2003-2010 ULCO http://www.univ-littoral.fr
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


#ifndef VLE_UTILS_TOOLS_HPP
#define VLE_UTILS_TOOLS_HPP

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
     * @brief Return true if str can be translate into a double.
     * @param str string to test.
     * @return true if str can be translate, otherwise false.
     */
    inline bool isDouble(const std::string& str)
    {
        try {
            boost::lexical_cast < double >(str);
            return true;
        } catch(const boost::bad_lexical_cast& e) {
            return false;
        }
    }

    /**
     * @brief Return true if str can be translate into an integer.
     * @param str string to test.
     * @return true if str can be translate, otherwise false.
     */
    inline bool isInt(const std::string& str)
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
    inline bool isBoolean(const std::string& str)
    {
        if (str == "true") {
            return true;
        } else if (str == "false") {
            return false;
        } else {
            try {
                return boost::lexical_cast < bool >(str);
            } catch(const boost::bad_lexical_cast& e) {
                return false;
            }
        }
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * Return conversion from template into string.
     *
     * example :
     * @code
     * std::string v = toString(23243);
     * std::string l = toString(141.12341);
     * std::string e = toString("hello");
     * @endcode
     *
     * @param c object to transform
     * @return string representation of templace < class C >
     */
    template < class C > std::string toString(const C& c)
    {
        std::ostringstream o;
        o << c;
        return o.str();
    }

    /**
     * @brief Check if the locale exists on this operating system.
     * @param locale The locale to check.
     * @return True if the locale exists, false otherwise.
     */
    inline bool isLocaleAvailable(const std::string& locale)
    {
        try {
            std::locale tmp(locale.c_str());
            return true;
        } catch (const std::runtime_error& /*e*/) {
            return false;
        }
    }

    /**
     * @brief Convert the string to the output template type with or without
     * the use of locale.
     * @code
     * vle::utils::convert < double >("123.456");
     * // returns 123.456
     *
     * vle::utils::convert < double >("123 456,789", true, "fr_FR");
     * // returns 123456.789
     * @endcode
     * @param value The string to convert
     * @param locale True to use the default locale, false otherwise.
     * @param loc The locale to use.
     * @throw utils::ArgError if a convertion failed.
     * @return A template output type.
     */
    template < typename output > output convert(
        const std::string& value,
        bool locale = false,
        const std::string& loc = std::string())
    {
        std::stringstream s;

        if (locale or not loc.empty()) {
            if (not isLocaleAvailable(loc)) {
                s.imbue(std::locale::classic());
                locale = false;
            } else {
                s.imbue(std::locale(loc.c_str()));
                locale = true;
            }
        }

        s << value;

        if (s.bad() or s.fail()) {
            throw ArgError("failed to read the value");
        }

        output result;
        s >> result;

        if (s.bad()) {
            throw ArgError("failed to write the value");
        } else {
            std::streambuf* buf = s.rdbuf();

            if (buf->in_avail() > 0 and locale) {
                s.imbue(std::locale::classic());
                s.str(value);
                s >> result;

                if (s.bad()) {
                    throw ArgError("failed to write the value");
                } else {
                    return result;
                }
            } else {
                return result;
            }
        }
    }

    /**
     * Return conversion from string into double.
     *
     * @param str string to convert.
     * @param def default value to return.
     * @return result of convertion, default if error.
     */
    inline double toDouble(const std::string& str, double def = 0.0)
    {
        try {
            return boost::lexical_cast < double >(str);
        } catch(const boost::bad_lexical_cast& e) {
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
    inline long toLong(const std::string& str, int def = 0l)
    {
        try {
            return boost::lexical_cast < long >(str);
        } catch(const boost::bad_lexical_cast& e) {
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
    inline int toInt(const std::string& str, int def = 0)
    {
        try {
            return boost::lexical_cast < int >(str);
        } catch(const boost::bad_lexical_cast& e) {
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
    inline unsigned int toUint(const std::string& str,
                               unsigned int def = 0)
    {
        try {
            return boost::lexical_cast < unsigned int >(str);
        } catch(const boost::bad_lexical_cast& e) {
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
    inline size_t toSizeType(const std::string& str, size_t def = 0)
    {
        try {
            return boost::lexical_cast < size_t >(str);
        } catch(const boost::bad_lexical_cast& e) {
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
    inline bool toBoolean(const std::string& str, bool def = false)
    {
        if (str == "true") {
            return true;
        } else if (str == "false") {
            return false;
        } else {
            try {
                return boost::lexical_cast < bool >(str);
            } catch(const boost::bad_lexical_cast& e) {
                return def;
            }
        }
    }

    /**
     * Return true if unicode string str is an ascii string with only
     * character between A-Z and a-z and size not empty.
     *
     * @param str unicode string to test validity in VLE name
     * @return true if str is correct, otherwise, false.
     */
    VLE_UTILS_EXPORT bool isAlnumString(const std::string& str);

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
