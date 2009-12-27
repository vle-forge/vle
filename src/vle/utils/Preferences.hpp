/**
 * @file vle/utils/Preferences.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
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
#define VLE_UTILS_PREFERENCES_HPP

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
    Preferences(const std::string& file = "vle.conf");
    Preferences(Preferences& pref);

    /**
     * @brief Load the content of a config file into settings map
     */
    void load();

    /**
     * @brief Save content of the settings map into a file
     */
    void save();

    /**
     * @brief Get the value of an attribute
     * @param section The section of the attribute to read
     * @param key The key of the attribute to read
     * @return The value of the attribute
     */
    std::string getAttributes(const std::string& section,
                              const std::string& key);

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
