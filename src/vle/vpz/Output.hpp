/**
 * @file src/vle/vpz/Output.hpp
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




#ifndef VLE_VPZ_OUTPUT_HPP
#define VLE_VPZ_OUTPUT_HPP

#include <string>
#include <vle/vpz/Base.hpp>

namespace vle { namespace vpz {

    /** 
     * @brief This class defines an output information and store the type of
     * output, text stream, sdml stream or Eov stream. For text and sdml, it
     * store the location filename and for Eov stream, the host.
     */
    class Output : public Base
    {
    public:
        /** 
         * @brief Define the output format of the plugin.
         * - LOCAL: use a local communication for observations.
         * - DISTANT: use a distant communication for observations using trame.
         */
        enum Format { LOCAL, DISTANT };

        Output();

        virtual ~Output() { }

        virtual void write(std::ostream& out) const;

        virtual Base::type getType() const
        { return OUTPUT; }

        /** 
         * @brief Set the output with text stream information. The name is
         * obligatory, the location defines a filename.
         * 
         * @param location the file name.
         * @param plugin to use in output stream.
         *
         * @throw Exception::Internal if name is empty.
         */
        void setLocalStream(const std::string& location,
                            const std::string& plugin);

        /** 
         * @brief Set the output with the sdml stream information. The name is
         * obligatory, the location defines a filename.
         * 
         * @param location the file name.
         * @param plugin to use in output stream.
         *
         * @throw Exception::Internal if name is empty.
         */
        void setDistantStream(const std::string& location,
                              const std::string& plugin);

        /** 
         * @brief Set the output data to initialise the plugin. The data is
         * the children of output tag.
         * 
         * @param data the string representation of the data.
         */
        void setData(const std::string& data);

        /** 
         * @brief Get the format of this Output.
         * 
         * @return the Format of the plugin.
         */
        Format format() const
        { return m_format; }

        std::string streamformat() const
        { return (m_format == LOCAL ? "local" : "distant"); }

        /** 
         * @brief Get the plugin of this Output. If format is not Output::Eov,
         * this string is not cleared.
         * 
         * @return a string representation of plugin.
         */
        const std::string& plugin() const
        { return m_plugin; }

        /** 
         * @brief Get the location of this Output.
         * 
         * @return a string representation of location.
         */
        const std::string& location() const
        { return m_location; }

        /** 
         * @brief Get the data of this Output.
         * 
         * @return a string representation of the data.
         */
        const std::string& data() const
        { return m_data; }

        const std::string& name() const
        { return m_name; }

        void setName(const std::string& name)
        { m_name.assign(name); }

    private:
        Format      m_format;
        std::string m_name;
        std::string m_plugin;
        std::string m_location;
        std::string m_data;
    };

}} // namespace vle vpz

#endif
