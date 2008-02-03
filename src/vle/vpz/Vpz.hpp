/**
 * @file vle/vpz/Vpz.hpp
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


#ifndef VLE_VPZ_VPZ_HPP
#define VLE_VPZ_VPZ_HPP

#include <vle/vpz/Project.hpp>
#include <vle/vpz/SaxParser.hpp>
#include <string>

namespace vle { namespace vpz {

    class Vpz : public Base
    {
    public:
        Vpz() :
            m_isGzip(true)
        { }

        Vpz(const std::string& filename);

        Vpz(const Vpz& vpz);

        virtual ~Vpz() { }

        virtual void write(std::ostream& out) const;

        virtual Base::type getType() const
        { return VPZ; } 

        /** 
         * @brief Open a VPZ file project.
         * @param filename file to read.
         * @throw utils::SaxParserError on error.
         */
        void parseFile(const std::string& filename);

        /** 
         * @brief Open a VPZ from a buffer.
         * @param buffer the buffer to parse XML.
         * @throw utils::SaxParserError on error.
         */
        void parseMemory(const std::string& buffer);

        /** 
         * @brief Write file into the current VPZ filename open.
         */
        void write();
    
        /** 
         * @brief Write file into the specified filename open.
         * @param filename file to write.
         */
        void write(const std::string& filename);

        /** 
         * @brief Delete all information from this VPZ.
         */
        void clear();

        std::string writeToString();

        /** 
         * @brief Return true if the file is compressed.
         * @return true if the file is compressed, false otherwise.
         */
        inline bool isGzip() const
        { return m_isGzip; }


        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
          * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /** 
         * @brief Parse the buffer to find a value.
         * @param buffer the buffer to translate.
         * @throw utils::SaxParserError if buffer is not a value.
         * @return value read from buffer.
         */
        static value::Value parseValue(const std::string& buffer);

        /** 
         * @brief Parse the buffer to find a value.
         * @param buffer the buffer to translate
         * @throw utils::SaxParserError if buffer is not a valid trame.
         * @return a reference to the Trame read. You must manage the memory of
         * this trame.
         */
        static TrameList parseTrame(const std::string& buffer);
        
        /** 
         * @brief Parse the buffer to find a list of values.
         * @param buffer the buffer to translate.
         * @throw utils::SaxParserError if buffer is not a value.
         * @return a vector of values read from buffer.
         */
        static std::vector < value::Value > parseValues(
            const std::string& buffer);

        inline void setFilename(const std::string& filename)
        { m_filename.assign(filename); }

        inline const std::string& filename() const
        { return m_filename; }

        inline const Project& project() const
        { return m_project; }

        inline Project& project()
        { return m_project; }

        /**
         * @brief Add the vpz extension to filename if does not exist, If
         * correct filename is passed, no modification is apply.
         * @param filename string to change if no extension exist.
         */
        static void fixExtension(std::string& filename);

        /** 
         * @brief Use the libxml++ DOM parser to check the validation of the VPZ
         * file.
         * @param filename the file name to test.
         * @throw std::exception on error.
         */
        static void validateFile(const std::string& filename);

        /** 
         * @brief Use the libxml++ DOM parser to check the validation of the VPZ
         * buffer.
         * @param buffer the content of the XML.
         * @throw std::exception on error.
         */
        static void validateMemory(const Glib::ustring& buffer);

    private:
        bool                m_isGzip;
        std::string         m_filename;
        vpz::Project        m_project;
    };

}} // namespace vle vpz

#endif
