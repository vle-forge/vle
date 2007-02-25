/**
 * @file SDMLStream.cpp
 * @author The VLE Development Team.
 * @brief Base class of net visualisation plugin.
 */

/*
 * Copyright (c) 2005 The VLE Development Team.
 *
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include "SDMLStream.hpp"
#include <vle/value/Value.hpp>
#include <vle/devs/sAtomicModel.hpp>


namespace vle { namespace stream {

SDMLStream::SDMLStream() :
    devs::Stream()
{ }

void SDMLStream::init(const std::string& /*outputPlugin */,
                      const std::string& outputType,
                      const std::string& /* outputLocation */,
                      const std::string& /* parameters */)
{
    m_fileName = outputType + ".sdml";
    m_stream.open(m_fileName.c_str());

    Assert(utils::FileError, m_stream.is_open(),
           boost::format("Cannot open file '%1%'\n") % m_fileName);

    m_tmpStream.open(
        Glib::build_filename(Glib::get_tmp_dir(), m_fileName).c_str(),
        std::fstream::in | std::fstream::out |
        std::fstream::trunc);

    Assert(utils::FileError, m_tmpStream.is_open(),
           boost::format("Cannot open file '%1%'\n") % 
            Glib::build_filename(Glib::get_tmp_dir(), m_fileName));

    size = 0;
}

void SDMLStream::close()
{
    m_stream.close();
}

void SDMLStream::writeData()
{
    m_tmpStream.seekp(0, std::ios_base::beg);
    char* buf = new char[4096];
    buf[4095] = '\0';

    while (not m_tmpStream.eof()) {
	m_tmpStream.getline(buf, 4095);
	m_stream << buf;
	m_stream << std::endl;
    }
    delete[] buf;
    m_tmpStream.close();
}

void SDMLStream::writeHead(std::vector < devs::Observable > const &
                           p_variableNameList)
{
    m_stream << "<?xml version=\"1.0\"?>" << std::endl;
    m_stream << "<StatDataML xmlns=\"htpp://www.omega.org/StatDataML\">"
	     << std::endl << std::endl;
    m_stream << "<dataset>" << std::endl;
    m_stream << "  <array>" << std::endl;
    m_stream << "    <dimension>" << std::endl;
    m_stream << "      <dim size=\"" << size << "\"></dim>" << std::endl;
    m_stream << "      <dim size=\""
	     << p_variableNameList.size()
	     << "\"><e>t</e>";

    std::vector < devs::Observable >::const_iterator it = p_variableNameList.begin();

    while (it != p_variableNameList.end()) {
	m_stream << "<e>" << (*it).model->getName() << ":"
		 << (*it).portName << "</e>";
	++it;
    }

    m_stream << "</dim>" << std::endl;
    m_stream << "    </dimension>" << std::endl;
    m_stream << "    <type><numeric><double /></numeric></type>" << std::endl;
    m_stream << "    <data>" << std::endl;
}

void SDMLStream::writeTail()
{
    m_stream << "    </data>" << std::endl;
    m_stream << "  </array>" << std::endl;
    m_stream << "</dataset>" << std::endl << std::endl;
    m_stream << "</StatDataML>" << std::endl;
}

void SDMLStream::writeValue(const devs::Time& p_time,
                            value::Value value)
{
    size++;
    m_tmpStream << "      <e>" << p_time.getValue()
		<< "</e><e>" << value->toString() << "</e>" << std::endl;
}

void SDMLStream::writeValues(const devs::Time& time,
			     const devs::StreamModelPortValue& valuelst,
			     const devs::Stream::ObservableVector& /*obslst*/)
{
    devs::StreamModelPortValue::const_iterator it = valuelst.begin();

    size++;
    m_tmpStream << "      <e>" << time.getValue() << "</e>";

    while (it != valuelst.end()) {
	m_tmpStream << "<e>" << (*it).second->toString() << "</e>";
	++it;
    }

    m_tmpStream << std::endl;
}

}} // namespace vle stream

DECLARE_STREAM(vle::stream::SDMLStream);
