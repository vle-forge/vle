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

#include <vle/vpz/Base.hpp>
#include <vle/vpz/Output.hpp>
#include <map>
#include <string>

namespace vle { namespace vpz {

    /** 
     * @brief A list of Output.
     */
    typedef std::map < std::string, Output > OutputList;

    /** 
     * @brief Outputs is a container based on OutputList to build a list of
     * Output using the Output's name as key.
     */
    class Outputs : public Base
    {
    public:
        Outputs()
        { }

        virtual ~Outputs() { }

        /** 
         * @brief Write under the element XML the tags the XML code. If not
         * output are stored, no tag are build.
         * @code
         * <outputs>
         *  <output name="output1" format="local|distant" location="" />
         *  ...
         * </outputs>
         * @endcode
         * @param elt A node to the parent of OUTPUTS tags. 
         */
        virtual void write(std::ostream& out) const;

        virtual Base::type getType() const
        { return OUTPUTS; }

        ////
        //// OutputList managment
        ////

        /** 
         * @brief Return a constant reference to the OutputList.
         * @return A constant reference.
         */
        inline const OutputList& outputlist() const
        { return m_list; }
        
        /** 
         * @brief Add an output with text stream information. The name is
         * obligatory, the location defines a filename.
         * @param name the output name.
         * @param location the file name.
         * @param plugin to use in steam.
         * @throw Exception::Internal if name is empty.
         */
        Output& addLocalStream(const std::string& name,
                               const std::string& location,
                               const std::string& plugin);

        /** 
         * @brief Add an output with the sdml stream information. The name is
         * obligatory, the location defines a filename.
         * @param name the output name.
         * @param location the file name.
         * @param plugin to use in steam.
         * @throw Exception::Internal if name is empty.
         */
        Output& addDistantStream(const std::string& name,
                                 const std::string& location,
                                 const std::string& plugin);

        /** 
         * @brief Add a list of outputs into the current list.
         * @param o the list of outputs to add.
         * @throw Exception::Internal if name or plugin already exist.
         */
        void add(const Outputs& o);

        /** 
         * @brief Add an output into the outputs list. 
         * @param name the name of the output to add.
         * @param o the output to add.
         * @throw Exception::Internal if the output already exist.
         */
        Output& add(const Output& o);

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
         * @brief Delete the specified output.
         * @param name the name of the output to delete.
         */
        void del(const std::string& name);

        /** 
         * @brief Remove all Output from the OutputList.
         */
        inline void clear();

        /** 
         * @brief Test if an Output exist with the specified name.
         * @param name Output name to test.
         * @return true if Output exist, false otherwise.
         */
        inline bool exist(const std::string& name) const;

        /** 
         * @brief Test if the OutputList is empty or not.
         * @return true if OutputList is empty, false otherwise.
         */
        inline bool empty() const;

        ////
        //// Usefull functions.
        ////

        /** 
         * @brief Get the list of all output name.
         * @return the list of output name.
         */
        std::list < std::string > outputsname() const;
        
    private:
        OutputList  m_list;

        struct AddOutput
        {
            inline AddOutput(Outputs& outputs) :
                m_outputs(outputs)
            { }

            inline void operator()(const OutputList::value_type& pair)
            { m_outputs.add(pair.second); }

            Outputs& m_outputs;
        };

        struct AddOutputName
        {
            inline AddOutputName(std::list < std::string >& lst) :
                m_list(lst)
            { }

            inline void operator()(const OutputList::value_type& pair)
            { m_list.push_back(pair.first); }

            std::list < std::string >& m_list;
        };
    };

    ////
    //// Implementation
    ////
    
    void Outputs::clear()
    {
        m_list.clear();
    }

    bool Outputs::exist(const std::string& name) const
    {
        return m_list.find(name) != m_list.end();
    }

    bool Outputs::empty() const
    {
        return m_list.empty();
    }

}} // namespace vle vpz

#endif
