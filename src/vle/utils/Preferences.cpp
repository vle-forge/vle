/**
 * @file vle/utils/Preferences.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
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


#include <vle/utils/Preferences.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/i18n.hpp>

namespace vle { namespace utils {

Preferences::Preferences(const std::string& file)
    : m_filepath(utils::Path::path().getHomeFile(file))
{
}

Preferences::Preferences(const Preferences& pref)
    : m_filepath(pref.m_filepath)
{
}

void Preferences::load()
{
    std::string line, section, key, value;

    try {
        m_file.open(m_filepath.c_str(), std::ios::in);
    } catch (const std::exception& e) {
        throw utils::InternalError(fmt(_(
                "Preference: cannot open file '%1%' for reading.")) %
            m_filepath);
    }

    while (getline(m_file, line)) {
        if (line[0] != '#') {
            if (line[0] == '[') {
                section = line.substr(1, line.find("]") -1);
            } else {
                key = line.substr(0, line.find("="));
                value = line.substr(line.find("=") + 1, line.length() -1);
                if (!section.empty() and !key.empty() and !value.empty())
                    setAttributes(section, key, value);
            }
        }
    }
    m_file.close();
}

void Preferences::save()
{
    try {
        m_file.open(m_filepath.c_str(), std::ios::out | std::ios::trunc);
    } catch (const std::exception& e) {
        throw utils::InternalError(fmt(_(
                "Preference: cannot open file '%1%' for writing.")) %
            m_filepath);
    }

    m_file << "# VLE config file" << std::endl << std::endl;
    for (Settings::const_iterator it = m_settings.begin();
         it != m_settings.end(); ++it) {
        m_file << "\n[" << it->first << "]" << std::endl;
        for (KeyValue::const_iterator jt = it->second.begin();
             jt != it->second.end(); ++jt) {
            m_file << jt->first << "=" << jt->second << std::endl;
        }
    }
}

void Preferences::assign(const std::string& section, const std::string& key,
                         std::string& value) const
{
    Settings::const_iterator it = m_settings.find(section);
    if (it != m_settings.end()) {
        KeyValue::const_iterator jt = it->second.find(key);
        if (jt != it->second.end()) {
            value = jt->second;
        }
    }
}

const KeyValue& Preferences::getKeyValues(const std::string& section) const
{
    Settings::const_iterator it = m_settings.find(section);

    if (it == m_settings.end()) {
        throw utils::ArgError(fmt(_(
                    "Preferences: section `%1%' is empty")) % section);
    }

    return it->second;
}

std::string Preferences::getAttributes(const std::string& section,
                                       const std::string& key)
{
    return m_settings[section][key];
}

void Preferences::setAttributes(const std::string& section,
                                const std::string& key,
                                const std::string& value)
{
    m_settings[section][key] = value;
}

}} //namespace vle utils
