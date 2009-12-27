/**
 * @file vle/oov/SimpleFile.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
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


#include <vle/oov/SimpleFile.hpp>
#include <vle/utils/DateTime.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/utils/Path.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Map.hpp>
#include <iostream>

namespace vle { namespace oov { namespace plugin {

SimpleFile::SimpleFile(const std::string& location)
    : Plugin(location), m_time(-1.0), m_isstart(false),
    m_havefirstevent(false), m_julian(false), m_type(SimpleFile::FILE)
{
}

SimpleFile::~SimpleFile()
{
    m_file.close();
}

void SimpleFile::onParameter(const std::string& plugin,
                             const std::string& location,
                             const std::string& file,
                             value::Value* parameters,
                             const double& /* time */)
{
    m_filenametmp = utils::Path::buildFilename(location, file);
    m_filename = m_filenametmp;
    m_filename += extension();

    m_file.open(m_filenametmp.c_str());

    if (not m_file.is_open()) {
        throw utils::ArgError(fmt(
                _("Output plug-in '%1%': cannot open file '%2%'")) % plugin %
            m_filenametmp);
    }

    if (parameters and parameters->isMap()) {
        const value::Map* map = value::toMapValue(parameters);
        if (map->existValue("locale")) {
            std::string locale(map->getString("locale"));
            try {
                if (locale == "user") {
                    std::locale selected("");
                    m_file.imbue(selected);
                } else {
                    std::locale selected(locale.c_str());
                    m_file.imbue(selected);
                }
            } catch (...) {
                throw utils::ArgError(fmt(
                        _("Output plug-in '%1%': unknow locale '%2%'")) %
                    plugin % locale);
            }
        }

        if (map->existValue("output")) {
            std::string type(map->getString("output"));
            if (type == "out") {
                m_type = SimpleFile::STANDARD_OUT;
            } else if (type == "error") {
                m_type = SimpleFile::STANDARD_ERROR;
            } else {
                throw utils::ArgError(fmt(
                        _("Output plug-in '%1%': unknow type '%2%'")) % plugin
                    % type);
            }
        }

        if (map->existValue("julian-day")) {
            m_julian = map->getBoolean("julian-day");
        }
    }

    delete parameters;
}

void SimpleFile::onNewObservable(const std::string& simulator,
                                 const std::string& parent,
                                 const std::string& portname,
                                 const std::string& /* view */,
                                 const double& time)
{
    if (m_isstart) {
        flush(time);
    } else {
        if (not m_havefirstevent) {
            m_time = time;
            m_havefirstevent = true;
        } else {
            flush(time);
            m_isstart = true;
        }
    }

    std::string name(buildname(parent, simulator, portname));

    if (m_columns.find(name) != m_columns.end()) {
        throw utils::InternalError(fmt(
           _("Output plug-in: observable '%1%' already exist")) % name);
    }

    m_columns[name] = m_buffer.size();
    m_buffer.push_back((value::Value*)0);
    m_valid.push_back(false);
}

void SimpleFile::onDelObservable(const std::string& /* simulator */,
                                 const std::string& /* parent */,
                                 const std::string& /* portname */,
                                 const std::string& /* view */,
                                 const double& /* time */)
{
}

void SimpleFile::onValue(const std::string& simulator,
                         const std::string& parent,
                         const std::string& port,
                         const std::string& /* view */,
                         const double& time,
                         value::Value* value)
{
    if (m_isstart) {
        flush(time);
    } else {
        if (not m_havefirstevent) {
            m_time = time;
            m_havefirstevent = true;
        } else {
            flush(time);
            m_isstart = true;
        }
    }

    std::string name(buildname(parent, simulator, port));
    Columns::iterator it = m_columns.find(name);

    if (it == m_columns.end()) {
        throw utils::InternalError(fmt(
                _("Output plugin: columns '%1%' does not exist. "
                  "No observable ?")) % name);
    }

    m_buffer[it->second] = value;
    m_valid[it->second] = true;
}

void SimpleFile::close(const double& time)
{
    finalFlush(time);
    std::vector < std::string > array(m_columns.size());

    Columns::iterator it = m_columns.begin();
    for (it = m_columns.begin(); it != m_columns.end(); ++it) {
        array[it->second] = it->first;
    }
    m_file << "\n";
    m_file.close();

    if (m_type == SimpleFile::FILE) {
        copyToFile(m_filename, array);
    } else {
        copyToStream((m_type == SimpleFile::STANDARD_OUT) ? std::cout :
                     std::cerr, array);
    }
    std::remove(m_filenametmp.c_str());
}

void SimpleFile::flush(double trame_time)
{
    if (trame_time != m_time) {
        if (std::find(m_valid.begin(), m_valid.end(), true) != m_valid.end()) {
            m_file << m_time;
            if (m_julian) {
                writeSeparator(m_file);
                try {
                    m_file << utils::DateTime::toJulianDay(m_time);
                } catch (const std::exception& /*e*/) {
                    throw utils::ModellingError(
                        _("Output plug-in: Year is out of valid range "
                          "in julian day: 1400..10000"));
                }
            }
            writeSeparator(m_file);

            const size_t nb(m_buffer.size());
            for (size_t i = 0; i < nb; ++i) {
                if (m_buffer[i]) {
                    m_buffer[i]->writeFile(m_file);
                } else {
                    m_file << "NA";
                }

                if (i + 1 < nb) {
                    writeSeparator(m_file);
                }
                delete m_buffer[i];
                m_buffer[i] = 0;
                m_valid[i] = false;
            }
            m_file << "\n";
        }
    }
    m_time = trame_time;
}

void SimpleFile::finalFlush(double trame_time)
{
    flush(trame_time);

    if (std::find(m_valid.begin(), m_valid.end(), true) != m_valid.end()) {
        m_file << trame_time;
        writeSeparator(m_file);
        for (Line::iterator it = m_buffer.begin(); it != m_buffer.end(); ++it) {
            if (*it) {
                (*it)->writeFile(m_file);
            } else {
                m_file << "NA";
            }

            if (it + 1 != m_buffer.end()) {
                writeSeparator(m_file);
            }
            delete *it;
            *it = 0;
        }
        m_file << "\n";
    }
}

void SimpleFile::copyToFile(const std::string& filename,
                            const std::vector < std::string >& array)
{
    std::ofstream file(filename.c_str());

    std::vector < std::string > tmp(array);
    if (m_julian) {
        tmp.insert(tmp.begin(), "julian-day");
    }
    tmp.insert(tmp.begin(), "time");
    writeHead(file, tmp);

    std::ifstream tmpfile(m_filenametmp.c_str());
    std::string tmpbuffer;
    while (tmpfile) {
        std::getline(tmpfile, tmpbuffer);
        file << tmpbuffer << '\n';
    }
}

void SimpleFile::copyToStream(std::ostream& stream,
                              const std::vector < std::string >& array)
{
    std::vector < std::string > tmp(array);
    if (m_julian) {
        tmp.insert(tmp.begin(), "julian-day");
    }
    tmp.insert(tmp.begin(), "time");
    writeHead(stream, tmp);

    std::ifstream tmpfile(m_filenametmp.c_str());
    std::string tmpbuffer;
    while (tmpfile) {
        std::getline(tmpfile, tmpbuffer);
        stream << tmpbuffer << '\n';
    }
}

std::string SimpleFile::buildname(const std::string& parent,
                                  const std::string& simulator,
                                  const std::string& port)
{
    std::string r(parent);
    r += ':';
    r += simulator;
    r += '.';
    r += port;
    return r;
}

}}} // namespace vle oov plugin
