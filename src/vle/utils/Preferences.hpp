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


#ifndef VLE_UTILS_PREFERENCES_HPP
#define VLE_UTILS_PREFERENCES_HPP 1

#include <vle/DllDefines.hpp>
#include <vle/utils/Types.hpp>
#include <string>
#include <vector>

namespace vle { namespace utils {

/**
 * This class permits to read and write the resources files `$VLE_HOME/vle.conf'
 * and stores them into internal structures.
 *
 * The resources file have a classical ini syntax:
 *
 * @code
 * sectionA.key1 = value1
 * sectionA.key2 = value2
 *
 * sectionB.key1 = "string"
 * sectionB.key2 = 123
 * sectionB.key3 = true
 * sectionB.key4 = false
 * sectionB.key5 = 1.123
 * @endcode
 *
 * The @c section and @c key are string without space. The @c value are
 * integer, double, boolean or string. For example:
 *
 * @code
 * package.configure = "cmake %1% .."
 * package.test = "make test"
 * package.install = "make install"
 * package.clean = "make clean"
 * package.package = "make package"
 *
 * gvle.intx = 123
 * gvle.inty = 10
 * gvle.showb = true
 * gvle.boolhide = false
 * gvle.real = 1.12354
 * @endcode
 *
 * The @c Preferences class uses the pimpl idiom and can not be copyable and
 * assignable.
 */
class VLE_API Preferences
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
     * Insert an @c std::string into the specified key.
     *
     * @code
     * Preferences f;
     * f.set("section.key", "vle is wonderful");
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
     * Insert an @c double into the specified key.
     *
     * @code
     * Preferences f;
     * f.set("section.key", 1.123456789);
     * // section.key = 1.123456789
     * @endcode
     *
     * @param key The key.
     * @param value The value.
     *
     * @return true if insertion is successful.
     */
    bool set(const std::string& key, double value);

    /**
     * Insert an @c uint32_t into the specified key.
     *
     * @code
     * Preferences f;
     * f.set("section.key", 23485);
     * // section.key = 23485
     * @endcode
     *
     * @param key The key.
     * @param value The value.
     *
     * @return true if insertion is successful.
     */
    bool set(const std::string& key, uint32_t value);

    /**
     * Insert an @c bool into the specified key.
     *
     * @code
     * Preferences f;
     * f.set("section.key", true);
     * // section.key = true
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
     * Get all the sections name.
     *
     * @code
     * Preferences f;
     * std::vector < std::string > sections;
     * f.get(&sections);
     * assert(sections.size() == 2);
     * assert(sections[0] == "section1");
     * assert(sections[1] == "section2");
     *
     * // section1.key1 = v1
     * // section1.key2 = v2
     * // section2.key3 = v4
     * @endcode
     *
     * @param[out] sections Output parameter to store the name of the sections.
     */
    void get(std::vector < std::string >* sections) const;

    /**
     * Get an @c std::string from specified key.
     *
     * @code
     * Preferences f;
     * std::string value;
     * f.get("section.key", &value);
     * assert(value == "azertyuiop";
     *
     * // section.key = "azertyuiop"
     * @endcode
     *
     * @param[in] key The key.
     * @param[out] value The value to fill.
     *
     * @throw utils::ArgError if section, key do not exist or if the value is
     * not an @c std::string.
     */
    bool get(const std::string& key, std::string* value) const;

    /**
     * Get a @c double from specified key.
     *
     * @code
     * Preferences f;
     * double value;
     * f.get("section.key", &value);
     * assert(value == 1.0);
     *
     * // section1.key = 1.0
     * @endcode
     *
     * @param[in] key The key.
     * @param[out] value The value to fill.
     *
     * @throw utils::ArgError if section, key do not exist or if the value is
     * not a @c real.
     */
    bool get(const std::string& key, double* value) const;

    /**
     * Get a @c uint32_t from specified key.
     *
     * @code
     * Preferences f;
     * uint32_t value;
     * f.get("section.key", &value);
     * assert(value == 12345);
     *
     * // section.key = 12345
     * @endcode
     *
     * @param[in] key The key.
     * @param[out] value The value to fill.
     *
     * @throw utils::ArgError if section, key do not exist or if the value is
     * not an @c integer.
     */
    bool get(const std::string& key, uint32_t* value) const;

    /**
     * Get a @c bool from specified key.
     *
     * @code
     * Preferences f;
     * bool value;
     * f.get("section.key", &value);
     * assert(value);
     *
     * // section.key = true
     * @endcode
     *
     * @param[in] key The key.
     * @param[out] value The value to fill.
     *
     * @throw utils::ArgError if section, key do not exist or if the value is
     * not a @c boolean.
     */
    bool get(const std::string& key, bool* value) const;

private:
    /**
     * Uncopyable class.
     */
    Preferences(const Preferences& other);

    /**
     * Unassignable class.
     */
    Preferences& operator=(const Preferences& other);

    class Pimpl; // @Preferences use the Pimpl idiom. We hide the
    Pimpl* mPimpl; // implementation detail.
};

}} //namespace vle utils

#endif
