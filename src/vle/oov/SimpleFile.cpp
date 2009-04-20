/**
 * @file vle/oov/SimpleFile.cpp
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


#include <vle/oov/SimpleFile.hpp>
#include <vle/utils/Debug.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Double.hpp>
#include <iostream>

namespace vle { namespace oov { namespace plugin {

SimpleFile::SimpleFile(const std::string& location) :
    Plugin(location),
    m_time(-1.0),
    m_isstart(false)
{
}

SimpleFile::~SimpleFile()
{
    m_file.close();
}

void SimpleFile::onParameter(const std::string& /* plugin */,
                             const std::string& location,
                             const std::string& file,
                             const std::string& /* parameters */,
                             const double& /* time */)
{
    m_filenametmp = Glib::build_filename(location, file);
    m_filename = m_filenametmp;
    m_filename += extension();

    m_file.open(m_filenametmp.c_str());

    Assert <utils::ModellingError >(m_file.is_open(), boost::format(
            "SimpleFile: cannot open file '%1%'") % m_filenametmp);
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
        if (m_time < .0) {
            m_time = time;
        } else {
            flush(time);
            m_isstart = true;
        }
    }

    std::string name(buildname(parent, simulator, portname));

    Assert <utils::InternalError >(m_columns.find(name) == m_columns.end(),
           boost::format("SimpleFile: observable '%1%' already exist") % name);

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
        if (m_time < .0) {
            m_time = time;
        } else {
            flush(time);
            m_isstart = true;
        }
    }

    std::string name(buildname(parent, simulator, port));
    Columns::iterator it = m_columns.find(name);

    Assert < utils::InternalError >(it != m_columns.end(), boost::format(
            "SimpleFile: columns '%1%' does not exist. No new Observable ?") %
            name);

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
    m_file << '\n' << std::flush;
    m_file.close();

    m_file.open(m_filename.c_str());

    {
        std::vector < std::string > tmp(array);
        tmp.insert(tmp.begin(), "time");
        writeHead(m_file, tmp);
    }

    {
        std::ifstream tmpfile(m_filenametmp.c_str());
        std::string tmpbuffer;
        while (tmpfile) {
            std::getline(tmpfile, tmpbuffer);
            m_file << tmpbuffer << '\n';
        }
    }

    m_file << '\n' << std::flush;

    std::remove(m_filenametmp.c_str());
}

void SimpleFile::flush(double trame_time)
{
    if (trame_time != m_time) {
        if (std::find(m_valid.begin(), m_valid.end(), true) != m_valid.end()) {
            m_file << m_time;
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
            m_file << '\n' << std::flush;
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
        m_file << '\n' << std::flush;
    }
}

}}} // namespace vle oov plugin
