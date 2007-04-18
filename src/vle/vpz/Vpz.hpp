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
#include <string>

namespace vle { namespace vpz {

    class Vpz : public Base
    {
    public:
        Vpz() :
            m_isGzip(true)
        { }

        Vpz(const std::string& filename);

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
        void open(const std::string& filename);

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

        inline float version() const
        { return m_version; }

        inline const std::string& author() const
        { return m_author; }

        inline const std::string& date() const
        { return m_date; }

        inline void setVersion(float f)
        { if (f >= 0) m_version = f; }

        inline void setAuthor(const std::string& author)
        { if (not author.empty()) m_author.assign(author); }

        inline void setDate(const std::string& date)
        { if (not date.empty()) m_date.assign(date); }

    private:
        std::string         m_filename;
        Project             m_project;
        bool                m_isGzip;

        float               m_version;
        std::string         m_author;
        std::string         m_date;
    };

}} // namespace vle vpz

#endif
