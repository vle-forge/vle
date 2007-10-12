/** 
 * @file vpz/Dynamic.hpp
 * @brief 
 * @author The vle Development Team
 * @date lun, 06 fév 2006 10:10:13 +0100
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

#ifndef VLE_VPZ_DYNAMIC_HPP
#define VLE_VPZ_DYNAMIC_HPP

#include <vle/vpz/Base.hpp>

namespace vle { namespace vpz {


    /** 
     * @brief A Dynamic represents the DYNAMICS tag of XML project file. It
     * stores the name of the dynamics, the library used by vle, the type local
     * or distant and a string data attached to the dynamics initialisation.
     *
     * Example:@n
     * <code>
     * <dynamics name="xxxx"     <!-- name of the dynamics -->
     *           library="yyyy"  <!-- name of the library used by simulator -->
     *           type="distant|local" <!-- distant need attribut location,
     *                                      default is local -->
     *           location="xxx:yyy" /> <!-- the location of the distant library
     *                                      used by simulator -->
     *
     * </code>
     */
    class Dynamic : public Base
    {
    public:
        enum Type { LOCAL, DISTANT };
        
        Dynamic(const std::string& name) :
            m_name(name),
            m_type(LOCAL)
        { }

        virtual ~Dynamic()
        { }

        virtual void write(std::ostream& out) const;

        virtual Base::type getType() const
        { return DYNAMIC; }

        /** 
         * @brief Assign a new library name to the dynamics.
         * 
         * @param name new name of the library.
         */
        void setLibrary(const std::string& name)
        { m_library.assign(name); }

        /** 
         * @brief Assign a model to find into the library.
         * 
         * @param name new name of the model.
         */
        void setModel(const std::string& name)
        { m_model.assign(name); }

        /** 
         * @brief Assign a language to the dynamics.
         * 
         * @param name new language of the dynamics.
         */
        void setLanguage(const std::string& name)
        { m_language.assign(name); }

        /** 
         * @brief Assign a location to the dynamics.
         * 
         * @param host the host where the dynamics library is running.
         * @param port the port of the host.
         */
        void setDistantDynamics(const std::string& host, int port);

        /** 
         * @brief Assign a local position of the dynamics.
         */
        void setLocalDynamics();

        /** 
         * @brief Get the current name of the dynamic.
         * 
         * @return the name of the dynamic.
         */
        inline const std::string& name() const
        { return m_name; }

        /** 
         * @brief The type of dynamic, Wrapping or Mapping.
         * 
         * @return a type of representation.
         */
        inline Dynamic::Type type() const
        { return m_type; }

        /**
         * @brief Return the library name of dynamics.
         * 
         * @return a string representation of library name.
         */
        inline const std::string& library() const
        { return m_library; }
        
        /** 
         * @brief Return the location of the dynamics. Be carefull, check the
         * type() before use this function.
         * 
         * @return A reference to the location.
         */
        inline const std::string& location() const
        { return m_location; }

        /** 
         * @brief Return the model of the library.
         * 
         * @return A reference to the model.
         */
        inline const std::string& model() const
        { return m_model; }

        /** 
         * @brief Return the language of the dynamics models.
         * 
         * @return A reference to the model.
         */
        inline const std::string& language() const
        { return m_language; }

        /** 
         * @brief Return true if this dynamics is a permanent data for the
         * devs::ModelFactory.
         * 
         * @return True if this dynamics is a permanent value.
         */
        inline bool isPermanent() const
        { return m_ispermanent; }

        /** 
         * @brief Set the permanent value of this dynamics.
         * 
         * @param value True to conserve this dynamics in devs::ModelFactory.
         */
        inline void permanent(bool value = true)
        { m_ispermanent = value; }

        /** 
         * @brief An operator to the std::set container.
         * 
         * @param dynamic The dynamic to search.
         * 
         * @return True if current name less that dynamic parameter.
         */
        inline bool operator<(const Dynamic& dynamic) const
        { return m_name < dynamic.m_name; }

    private:
        Dynamic() :
            m_type(LOCAL)
        { }

        std::string     m_name;
        std::string     m_library;
        std::string     m_model;
        std::string     m_language;
        std::string     m_location;
        Dynamic::Type   m_type;
        bool            m_ispermanent;
    };

}} // namespace vle vpz

#endif
