/*
 * @file vle/oov/NetStreamReader.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2010 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2010 INRA http://www.inra.fr
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


#ifndef VLE_OOV_NETSTREAMREADER_HPP
#define VLE_OOV_NETSTREAMREADER_HPP

#include <vle/oov/StreamReader.hpp>
#include <vle/oov/DllDefines.hpp>
#include <vle/utils/Socket.hpp>

namespace vle { namespace oov {

    /**
     * @brief The NetStreamReader inherits the class StreamReader and provides
     * an TCP/IP access to the observations information produced by models and
     * sends via the devs::NetStreamWriter.
     */
    class VLE_OOV_EXPORT NetStreamReader : public StreamReader
    {
    public:
        /**
         * @brief Build a NetStreamReader based on a new socket server.
         * @param port the port number [0 and 65535].
         * @throw utils::InternalError if the building of the socket server
         * fail.
         */
        NetStreamReader(int port);

        /**
         * @brief Release the resources from the TCP/IP server and delete it.
         */
        virtual ~NetStreamReader();

        /**
         * @brief Build a socket server and wait connection from a
         * devs::NetStreamWriter client.
         */
        void process();

        /**
         * @brief Return the port used by the oov::NetStreamReader socket
         * server.
         * @return the port number [0 and 65535].
         */
        inline int port() const;

        /**
         * @brief Affect a new size to the buffer.
         * @param buffer the new size of the buffer.
         * @throw utils::InternalError if buffer equal 0 or greater than memory
         * possibility.
         */
        void setBufferSize(size_t buffer);

        /**
         * @brief Return the size of the buffer.
         * @return The size of the buffer.
         */
        inline size_t bufferSize() const;

        /**
         * @brief Return a reference to the buffer.
         * @return A reference to the buffer.
         */
        inline const std::string& buffer() const;

        void waitConnection();
        void readConnection();
        void closeConnection();
	void close();

        /**
         * @brief Oov NetStreamReader ask to the CairoPlugin to write a image
         * when receive a value.
         * @param trame
         */
        virtual void onValue(const std::string& simulator,
                             const std::string& parent,
                             const std::string& port,
                             const std::string& view,
                             const double& time,
                             value::Value* value);
    protected:
        /**
         * @brief Dispatch the value::Set receives from the socket into the
         * function of the oov::Plugin.
         * - parameters: This function is called to initialize the plugin.
         *   @param frame The vector of values that define a parameter frame:
         *   the date, the data, the name of the plugin and the location where
         *   write data. For instance:
         * @code
         * int tag = frame.get(0).toInt().value();
         * std::string plugin = frame.get(1).toString().value();
         * std::string location = frame.get(2).toString().value();
         * std::string file = frame.get(3).toString().value();
         * std::string data = frame.get(4).toXml().value();
         * double time = frame.get(5).toDouble().value();
         * @endocde
         * - newObservable: This function is called when a new model is
         *   connected to the StreamWriter. @param frame the vector of value
         *   that define an observable frame: the date, the model's name, the
         *   model's parents, the model's observation port and the name of the
         *   view. For instance:
         * @code
         * int tag = frame.get(0).toInt().value();
         * std::string name = frame.get(1).toString().value();
         * std::string parent = frame.get(2).toString().value();
         * std::string port = frame.get(3).toString().value();
         * std::string view = frame.get(4).toString().value();
         * double time = frame.get(5).toDouble().value();
         * @endcode
         * - delObservable: This function is called when a model is disconnected
         *   from the StreamWriter. The frame the vector of value that define an
         *   observable frame: the date, the model's name, the model's parents,
         *   the model's observation port and the name of the view. For
         *   instance:
         * @code
         * int tag = frame.get(0).toInt().value();
         * std::string name = frame.get(1).toString().value();
         * std::string parent = frame.get(2).toString().value();
         * std::string port = frame.get(3).toString().value();
         * std::string view = frame.get(4).toString().value();
         * double time = frame.get(5).toDouble().value();
         * @endcode
         * - onValue: This function is called when models send value::Value
         *   class from the devs::Dynamics::observation function. Trame the
         *   vector of value that define a value frame: the date and a
         *   value::Set for each models: the model's name, the model's parents,
         *   the model's observation port and the name of the view. For
         *   instance:
         * @code
         * int tag = frame.get(0).toInt().value();
         * std::string name = frame.get(1).toDouble().value();
         * std::string parent = frame.get(2).toDouble().value();
         * std::string port = frame.get(3).toDouble().value();
         * std::string view = frame.get(4).toDouble().value();
         * double time = frame.get(5).toDouble().value();
         * value::Value* = frame.get(6);
         * @endcode
         * - Clone: This function is called when the StreamWriter is closed.
         *   @param frame the vector of value that define an end frame: only the
         *   date. For instance:
         * @code
         * int tag = frame.get(0).toInt().value();
         * double time = frame.get(1).toDouble().value();
         * @endcode
         * @param trame The value receive from the network.
         * @throw utils::ArgError if an error occured into the stream.
         */
        bool dispatch(value::Set& trame);

    private:
        int                 m_port;
        utils::net::Server* m_server;
        std::string         m_buffer;
        unsigned int        m_image;

        void serializePlugin();
    };

    inline int NetStreamReader::port() const
    { return m_port; }

    inline size_t NetStreamReader::bufferSize() const
    { return m_buffer.size(); }

    inline const std::string& NetStreamReader::buffer() const
    { return m_buffer; }

}} // namespace vle oov

#endif
