/**
 * @file TextStream.cpp
 * @author The VLE Development Team.
 * @brief Base class of net visualisation plugin.
 */

/*
 * Copyright (C) 2006 - The vle Development Team
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

#include "TextStream.hpp"
#include <vle/devs/sAtomicModel.hpp>
#include <vle/value/Value.hpp>
#include <vle/utils/Debug.hpp>


namespace vle { namespace stream {

TextStream::TextStream() :
    vle::devs::Stream()
{ }

void TextStream::init(const std::string& /* outputPlugin */,
                      const std::string& outputType,
                      const std::string& /* outputLocation */,
                      const std::string& /* parameters */)
{
    m_fileName = outputType + ".dat";
    m_stream.open(m_fileName.c_str());

    Assert(vle::utils::FileError, m_stream.is_open(),
           boost::format("Cannot open file '%1%'\n") % m_fileName);

    m_tmpStream.open(Glib::build_filename(Glib::get_tmp_dir(),
                                          m_fileName).c_str(),
                     std::fstream::in | std::fstream::out |
                     std::fstream::trunc);

    Assert(vle::utils::FileError, m_tmpStream.is_open(),
           boost::format("Cannot open file '%1%'\n") %
            Glib::build_filename(Glib::get_tmp_dir(), m_fileName));

    size = 0;
}

void TextStream::close()
{
    m_stream.close();
}

void TextStream::writeData()
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

void TextStream::writeHead(std::vector < vle::devs::Observable > const &)
{
}

void TextStream::writeTail()
{
}

void TextStream::writeValue(const vle::devs::Time& time,
                            vle::value::Value* value)
{
    size++;
    m_tmpStream << time.getValue() << "\t" << value->toString() << std::endl;
}

void TextStream::writeValues(const vle::devs::Time& time,
                             const vle::devs::StreamModelPortValue& valuelst,
                             const vle::devs::Stream::ObservableVector& obslst)
{
    std::vector < vle::devs::Observable >::const_iterator it = obslst.begin();

    size++;
    m_tmpStream << time.getValue();

    while (it != obslst.end()) {
	devs::StreamModelPortValue::const_iterator val;
	val = valuelst.find(devs::StreamModelPort((*it).model, (*it).portName));

	if (val != valuelst.end()) {
	    m_tmpStream << "\t";
	    m_tmpStream << (*val).second->toFile();
	}
	++it;
    }

    m_tmpStream << std::endl;
}

}} // namespace vle stream

DECLARE_STREAM(vle::stream::TextStream);
