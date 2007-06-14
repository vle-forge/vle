/** 
 * @file vpz/Vpz.hpp
 * @brief 
 * @author The vle Development Team
 * @date mar, 14 fév 2006 19:24:50 +0100
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

#ifndef VLE_VPZ_VPZ_HPP
#define VLE_VPZ_VPZ_HPP

#include <vle/vpz/Project.hpp>
#include <vle/vpz/SaxVPZ.hpp>
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
         *
         * @param filename file to read.
         *
         * @throw Exception::VPZ on error.
         */
        void parse_file(const std::string& filename);

        /** 
         * @brief Open a VPZ from a buffer.
         *
         * @param buffer the buffer to parse XML.
         *
         * @throw Exception::VPZ on error.
         */
        void parse_memory(const std::string& buffer);

        /** 
         * @brief Write file into the current VPZ filename open.
         */
        void write();
    
        /** 
         * @brief Write file into the specified filename open.
         * 
         * @param filename file to write.
         */
        void write(const std::string& filename);

        /** 
         * @brief Delete all information from this VPZ.
         */
        void clear();

        std::string writeToString();

        /** 
         * @brief All translator tags are parse, and the plugin was load to
         * attach the new structures, dynamics, graphics and experiments.
         */
        void expandTranslator();
        
        /** 
         * @brief Return true if the file is compressed.
         * 
         * @return true if the file is compressed, false otherwise.
         */
        inline bool isGzip() const
        { return m_isGzip; }

        /** 
         * @brief Return true if the file use a least one translator.
         * 
         * @return true if use translator, false otherwise.
         */
        bool hasNoVLE() const;

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
          * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /** 
         * @brief Parse the buffer to find a value.
         * 
         * @param buffer the buffer to translate.
         *
         * @throw utils::SaxParserError if buffer is not a value.
         * 
         * @return value read from buffer.
         */
        static value::Value parse_value(const std::string& buffer);
        
        /** 
         * @brief Parse the buffer to find a list of values.
         * 
         * @param buffer the buffer to translate.
         *
         * @throw utils::SaxParserError if buffer is not a value.
         * 
         * @return a vector of values read from buffer.
         */
        static std::vector < value::Value > parse_values(
            const std::string& buffer);

        inline void setFilename(const std::string& filename)
        { m_filename.assign(filename); }

        inline const std::string& filename() const
        { return m_filename; }

        inline const Project& project() const
        { return m_project; }

        inline Project& project()
        { return m_project; }

        static std::string get_gzip_content(const std::string& filename);

        static bool is_gzip_file(const std::string& filename);

        static std::string gunzip_file(const std::string& filename);

        /**
         * Add the vpz extension to filename if does not exist, If correct
         * filename is passed, no modification is apply.
         * 
         * @param filename string to change if no extension exist.
         */
        static void fixExtension(std::string& filename);

    private:
        bool                m_isGzip;
        std::string         m_filename;
        vpz::Project        m_project;
    };

}} // namespace vle vpz

#endif
