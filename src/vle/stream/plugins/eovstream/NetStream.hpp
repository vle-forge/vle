/**
 * @file NetStream.hpp
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

#ifndef OBSERVER_NET_STREAM_HPP
#define OBSERVER_NET_STREAM_HPP

#include <vle/devs/Stream.hpp>
#include <vle/devs/Observer.hpp>
#include <vle/value/Value.hpp>
#include <vle/utils/Socket.hpp>
#include <string>
#include <libxml++/libxml++.h>



namespace vle { namespace stream {

    /**
     * Observer to store VLE output into a net stream. NetStream use TCP/IP
     * connection to store result. This technique allow using of graphical
     * output plugin.
     *
     */
    class NetStream : public vle::devs::Stream
    {
    public:
	NetStream();

        virtual ~NetStream();

        /** 
         * @brief Initialise the connection with the Observer plugin.
         * 
         * @param outputPlugin the name of the plugin, 'plot', 'caview'.
         * @param outputType a type of plugin.
         * @param outputLocation the host in ip:port format.
         * @param parameters the syntax to initialise plugin.
         *
         * @throw Exception::Internal if connection failed.
         */
        virtual void init(const std::string& outputPlugin,
                          const std::string& outputType,
                          const std::string& outputLocation,
                          const std::string& parameters);

        virtual void close();

        virtual void writeData();

	virtual void writeHead(const std::vector < devs::Observable >&
			       variableNameList);

	virtual void writeTail();

        virtual void writeValue(const devs::Time& time,
                                value::Value value);

	virtual void writeValues(const devs::Time& time,
				 const devs::StreamModelPortValue& valuelst,
				 const devs::Stream::ObservableVector& obslst);
    private:
	xmlpp::DomParser            m_parser;
	xmlpp::Element*             m_parameterRoot;
	xmlpp::Document             m_document;
	xmlpp::Element*             m_root;
	std::vector < std::string > m_groupList;
        utils::net::Client*         m_client;
    };

}} // namespace vle stream

#endif
