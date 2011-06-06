/*
 * @file vle/utils/Preferences.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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


#ifndef VLE_UTILS_PREFERENCES_HPP
#define VLE_UTILS_PREFERENCES_HPP 1

#include <vle/utils/DllDefines.hpp>
#include <boost/cstdint.hpp>
#include <string>
#include <vector>

namespace vle { namespace utils {

/**
 * This class permit to read and write the resources files `$VLE_HOME/vle.conf'
 * and store them into internal structure. The resources file have an
 * classical ini syntax:
 * @code
 * [section A]
 * key1 = value1
 * key2 = value2
 *
 * [section B]
 * key = "string"
 * key = 123
 * key = true
 * key = false
 * key = 1.123
 * @endcode
 *
 * The @c section and @c key are string without space. The @c value are
 * integer, double, boolean or string. For example:
 *
 * @code
 * [package]
 * configure = "cmake %1% .."
 * test = "make test"
 * install = "make install"
 * clean = "make clean"
 * package = "make package"
 *
 * [gvle]
 * intx = 123
 * inty = 10
 * showb = true
 * boolhide = false
 * real = 1.12354
 * @endcode
 *
 */
class VLE_UTILS_EXPORT Preferences
{
public:
    /**
     * Build a new Preferences object with a specified filename, read the
     * content of the resources file and fill internal structures.
     *
     * @param file The filename to open.
     *
     * @throw utils::ArgError if the opening of the resources file failed
     * or during the reading of the file.
     */
    Preferences(const std::string& filename = std::string("vle.conf"));

    /**
     * Write the resources files if an @c insert function is called.
     */
    ~Preferences();

    //
    // insert data for specified section.
    //

    /**
     * @brief Insert an @c std::string into the specified key.
     * @code
     * Preferences f;
     * f.set("section.key", "vle is wonderful");
     * // [section]
     * // key = "vle is wonderful"
     *
     * // section.key = "vle is wonderful"
     * @endcode
     *
     * @param key The key.
     * @param value The value.
     *
     * @return true if insertion is successful.
     */
    bool set(const std::string& key, const std::string& value);

    /**
     * @brief Insert an @c double into the specified key.
     * @code
     * Preferences f;
     * f.set("section.key", 1.123456789);
     * // [section]
     * // key = 1.123456789
     * // @endcode
     *
     * @param key The key.
     * @param value The value.
     *
     * @return true if insertion is successful.
     */
    bool set(const std::string& key, double value);

    /**
     * @brief Insert an @c boost::uint32_t into the specified key.
     * @code
     * Preferences f;
     * f.set("section.key", 23485);
     * // [section]
     * // key = 23485
     * @endcode
     *
     * @param key The key.
     * @param value The value.
     *
     * @return true if insertion is successful.
     */
    bool set(const std::string& key, boost::uint32_t value);

    /**
     * @brief Insert an @c bool into the specified key.
     * @code
     * Preferences f;
     * f.set("section.key", true);
     * // [section]
     * // key = true
     * @endcode
     *
     * @param key The key.
     * @param value The value.
     *
     * @return true if insertion is successful.
     */
    bool set(const std::string& section, bool value);

    //
    // Retrieve data from resources file.
    //

    /**
     * @brief Get all the sections name.
     * @code
     * Preferences f;
     * std::vector < std::string > sections;
     * f.get(&sections);
     * assert(sections.size() == 2);
     * assert(sections[0] == "section1");
     * assert(sections[1] == "section2");
     *
     * // [section1]
     * // key1 = v1
     * // key2 = v2
     * // [section2]
     * // key3 = v4
     * @endcode
     *
     * @param [out] sections Output parameter to store the name of the sections.
     */
    void get(std::vector < std::string >* sections) const;

    /**
     * @brief Get a @c std::string from specified key.
     * @code
     * Preferences f;
     * std::string value;
     * f.get("section.key", &value);
     * assert(value == "azertyuiop";
     * [section]
     * // key = "azertyuiop"
     * // @endcode
     * @endcode
     *
     * @param key The key.
     * @param [out] value The value to fill.
     *
     * @throw utils::ArgError if section, key do not exist or if the value is
     * not an @c std::string.
     */
    bool get(const std::string& key, std::string* value) const;

    /**
     * @brief Get a @c double from specified key.
     * @code
     * Preferences f;
     * double value;
     * f.get("section.key", &value);
     * assert(value == 1.0);
     * [section]
     * // key = 1.0
     * // @endcode
     * @endcode
     *
     * @param key The key.
     * @param [out] value The value to fill.
     *
     * @throw utils::ArgError if section, key do not exist or if the value is
     * not a @c real.
     */
    bool get(const std::string& key, double* value) const;

    /**
     * @brief Get a @c boost::uint32_t from specified key.
     * @code
     * Preferences f;
     * boost::uint32_t value;
     * f.get("section.key", &value);
     * assert(value == 12345);
     * [section]
     * // key = 12345
     * // @endcode
     * @endcode
     *
     * @param key The key.
     * @param [out] value The value to fill.
     *
     * @throw utils::ArgError if section, key do not exist or if the value is
     * not an @c integer.
     */
    bool get(const std::string& key, boost::uint32_t* value) const;

    /**
     * @brief Get a @c bool from specified key.
     * @code
     * Preferences f;
     * bool value;
     * f.get("section.key", &value);
     * assert(value);
     * [section]
     * // key = true
     * // @endcode
     * @endcode
     *
     * @param key The key.
     * @param [out] value The value to fill.
     *
     * @throw utils::ArgError if section, key do not exist or if the value is
     * not a @c boolean.
     */
    bool get(const std::string& key, bool* value) const;

private:
    Preferences(const Preferences& other);
    Preferences& operator=(const Preferences& other);

    class Pimpl;
    Pimpl* mPimpl;
};

}} //namespace vle utils

#endif
