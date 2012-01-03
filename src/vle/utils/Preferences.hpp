/*
 * @file vle/utils/Preferences.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2012 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2012 INRA http://www.inra.fr
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

#include <fstream>
#include <map>
#include <string>
#include <vle/utils/DllDefines.hpp>

namespace vle { namespace utils {

typedef std::map < std::string, std::string > KeyValue;
typedef std::map < std::string, KeyValue > Settings;

class VLE_UTILS_EXPORT Preferences
{
public:
    /**
     * @brief Build new Preferences object with a specified filename.
     * @param file The filename to open.
     */
    Preferences(const std::string& file = std::string("vle.conf"));

    /**
     * @brief Copy constructor.
     * @param pref The Preferences object to copy.
     */
    Preferences(const Preferences& pref);

    /**
     * @brief Load the content of a config file into settings map
     */
    void load();

    /**
     * @brief Save content of the settings map into a file
     */
    void save();

    /**
     * @brief Set the variable `value' with the value read in section `section'
     * for the key `key'. If the key or the section do not exist, value is not
     * modified.
     *
     * @param section The section of the attribute to set.
     * @param key The key of the attribute to set.
     * @param value The value to set.
     */
    void assign(const std::string& section, const std::string& key,
                std::string& value) const;

    /**
     * @brief Get the value of an attribute
     * @param section The section of the attribute to read
     * @param key The key of the attribute to read
     * @return The value of the attribute
     */
    std::string getAttributes(const std::string& section,
                              const std::string& key);

    /**
     * @brief Get the values for a specified keys.
     * @param section The section to search values.
     * @return The values.
     */
    const KeyValue& getKeyValues(const std::string& section) const;

    /**
     * @brief Set the value of an attribute
     * @param section The section of the attribute to set
     * @param key The key of the attribute to set
     * @param value The value of the attribute
     */
    void setAttributes(const std::string& section,
                       const std::string& key,
                       const std::string& value);

private:
    std::fstream      m_file;
    std::string       m_filepath;
    Settings          m_settings;
};

}} //namespace vle utils

#endif
