/*
 * @file vle/oov/plugins/File.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
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

#include "File.hpp"
#include "FileType.hpp"
#include <boost/format.hpp>
#include <iomanip>
#include <iostream>
#include <vle/utils/DateTime.hpp>
#include <vle/utils/Exception.hpp>
#include <vle/utils/Filesystem.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/String.hpp>

namespace vle {
namespace oov {
namespace plugin {

File::File(const std::string& location)
  : Plugin(location)
  , m_filetype(0)
  , m_time(-1.0)
  , m_isstart(false)
  , m_havefirstevent(false)
  , m_julian(false)
  , m_type(File::FILE)
  , m_flushbybag(false)
{
}

File::~File()
{
    m_file.close();
    m_buffer.clear();
    delete m_filetype;
}

void
File::onParameter(const std::string& plugin,
                  const std::string& location,
                  const std::string& file,
                  std::unique_ptr<value::Value> parameters,
                  const double& /*time*/)
{
    if (parameters and parameters->isMap()) {
        const value::Map& map = parameters->toMap();
        std::string locale;

        if (map.exist("locale")) {
            locale.assign(map.getString("locale"));
        }
        try {
            if (locale == "user") {
                std::locale selected("");
                m_file.imbue(selected);
            } else {
                std::locale selected(locale.c_str());
                m_file.imbue(selected);
            }
        } catch (...) {
            std::locale selected("C");
            m_file.imbue(selected);
        }

        if (map.exist("type")) {
            std::string type(map.getString("type"));
            if (type == "csv") {
                m_filetype = new CSV();
            } else if (type == "rdata") {
                m_filetype = new Rdata();
            } else if (type == "text") {
                m_filetype = new Text();
            } else {
                throw utils::ArgError(
                  (boost::format("Output plug-in '%1%': unknow type '%2%'") %
                   plugin % type)
                    .str());
            }
        }

        if (map.exist("output")) {
            std::string type(map.getString("output"));
            if (type == "out") {
                m_type = File::STANDARD_OUT;
            } else if (type == "error") {
                m_type = File::STANDARD_ERROR;
            } else if (type == "file") {
                m_type = File::FILE;
            } else {
                throw utils::ArgError(
                  (boost::format("Output plug-in '%1%': unknow type '%2%'") %
                   plugin % type)
                    .str());
            }
        }

        if (map.exist("julian-day")) {
            m_julian = map.getBoolean("julian-day");
        }

        if (map.exist("flush-by-bag")) {
            m_flushbybag = map.getBoolean("flush-by-bag");
        }
    }

    if (not m_filetype) {
        m_filetype = new Text;
    }

    utils::Path p;
    if (location.empty()) {
        p = utils::Path::current_path();
    } else {
        p.set(location);
    }

    p /= file;

    m_filenametmp = p.string();
    m_filename = m_filenametmp;
    m_filename += m_filetype->extension();

    m_file.open(m_filenametmp.c_str());

    if (not m_file.is_open()) {
        throw utils::ArgError(
          (boost::format("Output plug-in '%1%': cannot open file '%2%'\n") %
           plugin % m_filenametmp)
            .str());
    }

    m_file << std::setprecision(std::numeric_limits<double>::digits10);
    parameters.reset();
}

void
File::onNewObservable(const std::string& simulator,
                      const std::string& parent,
                      const std::string& portname,
                      const std::string& /* view */,
                      const double& time)
{
    if (m_isstart) {
        flush();
    } else {
        if (not m_havefirstevent) {
            m_time = time;
            m_havefirstevent = true;
        } else {
            flush();
            m_isstart = true;
        }
    }

    std::string name(buildname(parent, simulator, portname));

    if (m_columns.find(name) != m_columns.end()) {
        throw utils::InternalError(
          (boost::format("Output plug-in: observable '%1%' already exist") %
           name)
            .str());
    }

    m_newbagwatcher[name] = -1.0;
    m_columns[name] = m_buffer.size();
    m_buffer.add(std::unique_ptr<value::Value>());
    m_valid.push_back(false);
}

void
File::onDelObservable(const std::string& /* simulator */,
                      const std::string& /* parent */,
                      const std::string& /* portname */,
                      const std::string& /* view */,
                      const double& /* time */)
{
}

void
File::onValue(const std::string& simulator,
              const std::string& parent,
              const std::string& port,
              const std::string& /*view*/,
              const double& time,
              std::unique_ptr<value::Value> value)
{
    std::string name(buildname(parent, simulator, port));
    Columns::iterator it;

    if (not simulator.empty()) {
        name = buildname(parent, simulator, port);
        it = m_columns.find(name);

        if (it == m_columns.end()) {
            throw utils::InternalError(
              (boost::format("Output plugin: columns '%1%' does not exist. "
                             "No observable ?") %
               name)
                .str());
        }

        if (m_isstart) {
            if (time != m_time ||
                (m_flushbybag && m_newbagwatcher[name] == time)) {
                flush();
            }
        } else {
            if (not m_havefirstevent) {
                m_havefirstevent = true;
            } else {
                flush();
                m_isstart = true;
            }
        }
        m_buffer.set(it->second, std::move(value));
        m_valid[it->second] = true;

        m_newbagwatcher[name] = time;
    }
    m_time = time;
}

std::unique_ptr<value::Matrix>
File::finish(const double& time)
{
    // build the final file
    finalFlush(time);
    std::vector<std::string> array(m_columns.size());

    Columns::iterator it = m_columns.begin();
    for (it = m_columns.begin(); it != m_columns.end(); ++it) {
        array[it->second] = it->first;
    }
    m_file << "\n";
    m_file.close();

    if (m_type == File::FILE) {
        copyToFile(m_filename, array);
    } else {
        copyToStream((m_type == File::STANDARD_OUT) ? std::cout : std::cerr,
                     array);
    }
    std::remove(m_filenametmp.c_str());

    return {};
}

void
File::flush()
{
    if (m_valid.empty() or
        std::find(m_valid.begin(), m_valid.end(), true) != m_valid.end()) {
        m_file << m_time;
        if (m_julian) {
            m_filetype->writeSeparator(m_file);
            try {
                m_file << utils::DateTime::toJulianDay(m_time);
            } catch (const std::exception& /*e*/) {
                throw utils::ModellingError(
                  "Output plug-in: Year is out of valid range "
                  "in julian day: 1400..10000");
            }
        }
        m_filetype->writeSeparator(m_file);

        const size_t nb(m_buffer.size());
        for (size_t i = 0; i < nb; ++i) {
            if (m_buffer[i]) {
                m_buffer[i]->writeFile(m_file);
            } else {
                m_file << "NA";
            }

            if (i + 1 < nb) {
                m_filetype->writeSeparator(m_file);
            }
            m_valid[i] = false;
        }
        m_file << "\n";
    }
}

void
File::finalFlush(double trame_time)
{
    flush();

    if (std::find(m_valid.begin(), m_valid.end(), true) != m_valid.end()) {
        m_file << trame_time;
        if (m_julian) {
            m_filetype->writeSeparator(m_file);
            try {
                m_file << utils::DateTime::toJulianDay(m_time);
            } catch (const std::exception& /*e*/) {
                throw utils::ModellingError(
                  "Output plug-in: Year is out of valid range "
                  "in julian day: 1400..10000");
            }
        }
        m_filetype->writeSeparator(m_file);
        for (value::Set::iterator it = m_buffer.begin(); it != m_buffer.end();
             ++it) {
            if (*it) {
                (*it)->writeFile(m_file);
            } else {
                m_file << "NA";
            }

            if (it + 1 != m_buffer.end()) {
                m_filetype->writeSeparator(m_file);
            }
        }
        m_file << "\n";
        m_buffer.clear();
    }
}

void
File::copyToFile(const std::string& filename,
                 const std::vector<std::string>& array)
{
    std::ofstream file(filename.c_str());

    std::vector<std::string> tmp(array);
    if (m_julian) {
        tmp.insert(tmp.begin(), "julian-day");
    }
    tmp.insert(tmp.begin(), "time");
    m_filetype->writeHead(file, tmp);

    std::ifstream tmpfile(m_filenametmp.c_str());
    std::string tmpbuffer;
    while (tmpfile) {
        std::getline(tmpfile, tmpbuffer);
        file << tmpbuffer << '\n';
    }
}

void
File::copyToStream(std::ostream& stream, const std::vector<std::string>& array)
{
    std::vector<std::string> tmp(array);
    if (m_julian) {
        tmp.insert(tmp.begin(), "julian-day");
    }
    tmp.insert(tmp.begin(), "time");
    m_filetype->writeHead(stream, tmp);

    std::ifstream tmpfile(m_filenametmp.c_str());
    std::string tmpbuffer;
    while (tmpfile) {
        std::getline(tmpfile, tmpbuffer);
        stream << tmpbuffer << '\n';
    }
}

std::string
File::buildname(const std::string& parent,
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
}
}
} // namespace vle oov plugin

DECLARE_OOV_PLUGIN(vle::oov::plugin::File)
