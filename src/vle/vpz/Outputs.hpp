/** 
 * @file vpz/Outputs.hpp
 * @brief 
 * @author The vle Development Team
 * @date mar, 31 jan 2006 17:23:33 +0100
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

#ifndef VLE_VPZ_OUTPUTS_HPP
#define VLE_VPZ_OUTPUTS_HPP

#include <list>
#include <string>
#include <vle/vpz/Base.hpp>
#include <vle/vpz/Output.hpp>

namespace vle { namespace vpz {


    /** 
     * @brief This class stores a list of Output.
     */
    class Outputs : public Base, public std::map < std::string, Output >
    {
    public:
        Outputs()
        { }

        virtual ~Outputs() { }

        /** 
         * @brief Write under the element XML the tags the XML code. If not
         * output are stored, no tag are build.
         * @code
         * <OUTPUTS>
         *  <OUTPUT NAME="output1" FORMAT="text" LOCATION="" />
         *  ...
         * </OUTPUTS>
         * @endcode
         * 
         * @param elt A node to the parent of OUTPUTS tags. 
         */
        virtual void write(std::ostream& out) const;

        virtual Base::type getType() const
        { return OUTPUTS; }
        
        /** 
         * @brief Add an output with text stream information. The name is
         * obligatory, the location defines a filename.
         * 
         * @param name the output name.
         * @param location the file name.
         *
         * @throw Exception::Internal if name is empty.
         */
        Output& addTextStream(const std::string& name,
                              const std::string& location);

        /** 
         * @brief Add an output with the sdml stream information. The name is
         * obligatory, the location defines a filename.
         * 
         * @param name the output name.
         * @param location the file name.
         *
         * @throw Exception::Internal if name is empty.
         */
        Output& addSdmlStream(const std::string& name,
                              const std::string& location);

        /** 
         * @brief Add an output with the eov stream information. The name and
         * the plugin are obligatory,  the location defines host with syntax:
         * host:port.
         * 
         * @param name the output name.
         * @param plugin the plugin's name.
         * @param location the host:port definition.
         *
         * @throw Exception::Internal if name or plugin are empty.
         */
        Output& addEovStream(const std::string& name,
                             const std::string& plugin,
                             const std::string& location);

        /** 
         * @brief Add an output with the eov stream information. The name and
         * the plugin are obligatory,  the location defines host with syntax:
         * host:port.
         * 
         * @param name the output name.
         * @param plugin the plugin's name.
         * @param location the host:port definition.
         *
         * @throw Exception::Internal if name or plugin are empty.
         */
        Output& addNetStream(const std::string& name,
                             const std::string& plugin,
                             const std::string& location);

        /** 
         * @brief Delete the specified output.
         * @param name the name of the output to delete.
         */
        void del(const std::string& name);

        /** 
         * @brief Add a list of outputs into the current list.
         * @param o the list of outputs to add.
         * @throw Exception::Internal if name or plugin already exist.
         */
        void add(const Outputs& o);

        /** 
         * @brief Find an Output reference from list.
         * @param name Output name.
         * @return A reference to the output.
         * @throw Exception::Internal if Output does not exist.
         */
        Output& get(const std::string& name);

        /** 
         * @brief Find a const Output reference from list.
         * @param name Output name.
         * @return A reference to the output.
         * @throw Exception::Internal if Output does not exist.
         */
        const Output& get(const std::string& name) const;

        /** 
         * @brief Get the list of all output name.
         * @return the list of output name.
         */
        std::list < std::string > outputsname() const;

        /** 
         * @brief Test if an Output exist with the specified name.
         * @param name Output name to test.
         * @return true if Output exist, false otherwise.
         */
        bool exist(const std::string& name) const;
        
    private:
        /** 
         * @brief Add an output into the outputs list. 
         * @param name the name of the output to add.
         * @param o the output to add.
         * @throw Exception::Internal if the output already exist.
         */
        Output& add(const Output& o);
    };

}} // namespace vle vpz

#endif
