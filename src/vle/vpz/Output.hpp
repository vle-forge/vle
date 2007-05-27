/** 
 * @file vpz/Output.hpp
 * @brief 
 * @author The vle Development Team
 * @date mar, 31 jan 2006 17:22:23 +0100
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
         * - TEXT: values are writing into the stream with a text format.
         * - SDML: values are writing into the stream with a SDML format.
         * - EOV: values are writing into the stream with a EOV format.
         * - NET: values are writing into the stream with a NET format.
         */
      enum Format { TEXT, SDML, EOV, NET };

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
         *
         * @throw Exception::Internal if name is empty.
         */
        void setTextStream(const std::string& location = std::string());

        /** 
         * @brief Set the output with the sdml stream information. The name is
         * obligatory, the location defines a filename.
         * 
         * @param location the file name.
         *
         * @throw Exception::Internal if name is empty.
         */
        void setSdmlStream(const std::string& location = std::string());

        /** 
         * @brief Set the output with the Eov stream information. The name and
         * the plugin are obligatory,  the location defines host with syntax:
         * host:port. It location is empty, location is initialised by ":8000".
         * 
         * @param plugin the plugin's name.
         * @param location the host:port definition.
         *
         * @throw Exception::Internal if plugin is empty.
         */
        void setEovStream(const std::string& plugin,
                          const std::string& location = std::string());

        /** 
         * @brief Set the output with the Net stream information. The name and
         * the plugin are obligatory,  the location defines host with syntax:
         * host:port. It location is empty, location is initialised by ":8000".
         * 
         * @param plugin the plugin's name.
         * @param location the host:port definition.
         *
         * @throw Exception::Internal if plugin is empty.
         */
        void setNetStream(const std::string& plugin,
                          const std::string& location = std::string());

        /** 
         * @brief Set the output XML to initialise the plugin. The XML have the
         * OUTPUT tag.
         * 
         * @param xml the string representation of the XML.
         */
        void setXML(const std::string& xml);

        /** 
         * @brief Get the format of this Output.
         * 
         * @return the Format of the plugin.
         */
        Format format() const
        { return m_format; }

        std::string streamformat() const
        { return (m_format == TEXT ? "textstream" : m_format == SDML ?
                  "sdmlstream" : m_format == NET ? "netstream" : "eovstream"); }

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
         * @brief Get the XML of this Output.
         * 
         * @return a string representation of the XML.
         */
        const std::string& xml() const
        { return m_xml; }

        const std::string& name() const
        { return m_name; }

        void setName(const std::string& name)
        { m_name.assign(name); }

    private:
        Format      m_format;
        std::string m_name;
        std::string m_plugin;
        std::string m_location;
        std::string m_xml;
    };

}} // namespace vle vpz

#endif
