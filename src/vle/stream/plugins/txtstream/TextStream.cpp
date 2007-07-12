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
#include <vle/value/Value.hpp>
#include <vle/utils/Debug.hpp>


namespace vle { namespace stream {

TextStream::TextStream() :
    devs::Stream()
{ }

void TextStream::init(const std::string& /* outputPlugin */,
                      const std::string& outputType,
                      const std::string& outputLocation,
                      const std::string& /* parameters */)
{
    m_fileName = Glib::build_filename(outputLocation, outputType + ".dat");
    m_stream.open(m_fileName.c_str());

    Assert(utils::FileError, m_stream.is_open(),
           boost::format("Cannot open file '%1%'\n") % m_fileName);
}

void TextStream::close()
{
    m_stream.close();
}

void TextStream::writeData()
{
}

void TextStream::writeHead(const std::vector < devs::Observable >& /* obs */)
{
}

void TextStream::writeTail()
{
}

void TextStream::writeValue(const devs::Time& time, value::Value value)
{
    m_stream << time << "\t" << value->toString() << "\n";
}

void TextStream::writeValues(const devs::Time& time,
                             const devs::StreamModelPortValue& valuelst,
                             const devs::Stream::ObservableVector& obslst)
{
    std::vector < devs::Observable >::const_iterator it = obslst.begin();
    m_stream << time.getValue();

    while (it != obslst.end()) {
	devs::StreamModelPortValue::const_iterator val;
        val = valuelst.find(devs::StreamModelPort(it->simulator(),
                                                  it->portname()));

	if (val != valuelst.end()) {
	    m_stream << "\t";
	    m_stream << val->second->toFile();
	}
	++it;
    }

    m_stream << std::endl;
}

}} // namespace vle stream

DECLARE_STREAM(vle::stream::TextStream);
