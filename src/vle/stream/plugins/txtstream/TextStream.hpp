/**
 * @file TextStream.hpp
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

#ifndef OBSERVER_TEXT_STREAM_HPP
#define OBSERVER_TEXT_STREAM_HPP

#include <vle/devs/Stream.hpp>
#include <vle/devs/Observer.hpp>
#include <vle/value/Value.hpp>
#include <fstream>
#include <string>



namespace vle { namespace stream {

    /**
     * Observer to store VLE output into a text stream.
     *
     */
    class TextStream : public vle::devs::Stream
    {
    public:
	TextStream();

	virtual ~TextStream() { }

        virtual void init(const std::string& outputPlugin,
                          const std::string& outputType,
                          const std::string& outputLocation,
                          const std::string& parameters);

	virtual void close();
        
        virtual void writeData();
        
        virtual void writeHead(
                const std::vector < vle::devs::Observable >& variableNameList);
        
        virtual void writeTail();

        virtual void writeValue(const vle::devs::Time& time,
                                vle::value::Value* value);

	virtual void writeValues(const vle::devs::Time& time,
				 const vle::devs::StreamModelPortValue& valuelst,
                                 const Stream::ObservableVector& obslst);
    private:
	std::string     m_fileName;
	std::ofstream   m_stream;
	std::fstream    m_tmpStream;
	unsigned int    size;
  };

}} // namespace vle stream

#endif
