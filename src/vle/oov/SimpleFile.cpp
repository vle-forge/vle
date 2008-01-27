/** 
 * @file Text.cpp
 * @brief 
 * @author The vle Development Team
 * @date lun, 23 jui 2007 11:48:34 +0200
 */

/*
 * Copyright (C) 2007 - The vle Development Team
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <vle/oov/SimpleFile.hpp>
#include <vle/utils/Debug.hpp>



namespace vle { namespace oov { namespace plugin {

SimpleFile::SimpleFile(const std::string& location) :
    Plugin(location),
    m_time(-1.0),
    m_isstart(false)
{
    m_filenametmp = location;

    m_file.open(m_filenametmp.c_str());

    Assert(utils::InternalError, m_file.is_open(), boost::format(
            "Cannot open file [%1%]") % location);
}

SimpleFile::~SimpleFile()
{
    m_file.close();
}

void SimpleFile::onParameter(const vpz::ParameterTrame& /* trame */)
{
    m_filename = m_filenametmp + extension();
}

void SimpleFile::onNewObservable(const vpz::NewObservableTrame& trame)
{
    std::string name(buildname(trame.name(), trame.port()));

    Assert(utils::InternalError, m_columns.find(name) == m_columns.end(),
           boost::format("Observable %1% already exist") % name);

    m_columns[name] = m_buffer.size();
    m_buffer.push_back(value::Value());
}

void SimpleFile::onDelObservable(const vpz::DelObservableTrame& /* trame */)
{
}

void SimpleFile::onValue(const vpz::ValueTrame& trame)
{
    if (m_isstart) {
        flush(utils::to_double(trame.time()));
    } else {
        if (m_time < .0) {
            m_time = utils::to_double(trame.time());
        } else {
            flush(utils::to_double(trame.time()));
            m_isstart = true;
        }
    }

    for (vpz::ModelTrameList::const_iterator it = trame.trames().begin();
         it != trame.trames().end(); ++it) {

        std::string name(buildname(it->simulator(), it->port()));
        std::map < std::string, int >::iterator jt;
        jt = m_columns.find(name);

        Assert(utils::InternalError, jt != m_columns.end(), boost::format(
                "The columns %1% does not exist. No new Observable ?") % 
            name);

        m_buffer[jt->second] = it->value();
    }
}

void SimpleFile::close(const vpz::EndTrame& trame)
{
    finalFlush(utils::to_double(trame.time()));
    std::vector < std::string > array(m_columns.size());

    std::map < std::string, int >::iterator it = m_columns.begin();
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
        m_file << m_time;
        writeSeparator(m_file);
        std::vector < value::Value >::iterator it;
        for (it = m_buffer.begin(); it != m_buffer.end(); ++it) {
            if ((*it).get()) {
                m_file << (*it)->toString();
            } else {
                m_file << "NA";
            }

            if (it + 1 != m_buffer.end()) {
                writeSeparator(m_file);
            }
            (*it).reset();
        }
        m_file << '\n' << std::flush;
        m_time = trame_time;
    }
}

void SimpleFile::finalFlush(double trame_time)
{
    flush(trame_time);
    bool empty = true;

    for (std::vector < value::Value >::iterator it = m_buffer.begin(); 
	 it != m_buffer.end() and empty; ++it) {
        empty = not (*it).get();
    }

    if (not empty) {
        m_file << trame_time;
        writeSeparator(m_file);
        std::vector < value::Value >::iterator it;
        for (it = m_buffer.begin(); it != m_buffer.end(); ++it) {
            if ((*it).get()) {
                m_file << (*it)->toString();
            } else {
                m_file << "NA";
            }

            if (it + 1 != m_buffer.end()) {
                writeSeparator(m_file);
            }
            (*it).reset();
        }
        m_file << '\n' << std::flush;
    }
}

}}} // namespace vle oov plugin
