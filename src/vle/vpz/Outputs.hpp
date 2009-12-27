/**
 * @file vle/vpz/Outputs.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
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


#ifndef VLE_VPZ_OUTPUTS_HPP
#define VLE_VPZ_OUTPUTS_HPP

#include <vle/vpz/Base.hpp>
#include <vle/vpz/Output.hpp>
#include <vle/vpz/DllDefines.hpp>
#include <map>
#include <vector>
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
    class VLE_VPZ_EXPORT Outputs : public Base
    {
    public:
        /**
         * @brief Define an iterator to the OutputList.
         */
        typedef OutputList::iterator iterator;

        /**
         * @brief Define a constant iterator to the OutputList.
         */
        typedef OutputList::const_iterator const_iterator;

        /**
         * @brief Build an empty Outputs.
         */
        Outputs()
        {}

        /**
         * @brief Nothing to delete.
         */
        virtual ~Outputs()
        {}

        /**
         * @brief Write the XML representation of this class.
         * @code
         * <outputs>
         *  <output name="output1" format="local|distant" location="" />
         *  ...
         * </outputs>
         * @endcode
         * @param out an output stream.
         */
        virtual void write(std::ostream& out) const;

        /**
         * @brief Return the type of this class.
         * @return OUTPUTS.
         */
        virtual Base::type getType() const
        { return OUTPUTS; }

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Manage the OutputList
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Add an output with text stream information. The name is
         * obligatory, the location defines a filename.
         * @param name the output name.
         * @param location the file name.
         * @param plugin to use in steam.
         * @throw utils::ArgError if name is empty.
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
         * @throw utils::ArgError if name is empty.
         */
        Output& addDistantStream(const std::string& name,
                                 const std::string& location,
                                 const std::string& plugin);

        /**
         * @brief Add a list of outputs into the current list.
         * @param o the list of outputs to add.
         * @throw utils::ArgError if name or plugin already exist.
         */
        void add(const Outputs& o);

        /**
         * @brief Add an output into the outputs list.
         * @param o the output to add.
         * @throw utils::ArgError if the output already exist.
         */
        Output& add(const Output& o);

        /**
         * @brief Find an Output reference from list.
         * @param name Output name.
         * @return A reference to the output.
         * @throw utils::ArgError if Output does not exist.
         */
        Output& get(const std::string& name);

        /**
         * @brief Find a const Output reference from list.
         * @param name Output name.
         * @return A reference to the output.
         * @throw utils::ArgError if Output does not exist.
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
        inline void clear()
        { m_list.clear(); }

        /**
         * @brief Test if an Output exist with the specified name.
         * @param name Output name to test.
         * @return true if Output exist, false otherwise.
         */
        inline bool exist(const std::string& name) const
        { return m_list.find(name) != end(); }

        /**
         * @brief Test if the OutputList is empty or not.
         * @return true if OutputList is empty, false otherwise.
         */
        inline bool empty() const
        { return m_list.empty(); }

	/**
	 * @brief rename an Output with the specified name.
	 * @param oldname the Output to rename
	 * @param newname the new name for the Output
	 */
        void rename(const std::string& oldoutputname,
                    const std::string& newoutputname);

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Get/Set functions.
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Return a constant reference to the OutputList.
         * @return A constant reference.
         */
        inline const OutputList& outputlist() const
        { return m_list; }

        /**
         * @brief Return a constant reference to the OutputList.
         * @return A constant reference.
         */
        inline OutputList& outputlist()
        { return m_list; }

        /**
         * @brief Get a iterator the begin of the OutputList.
         * @return Get a iterator the begin of the OutputList.
         */
        iterator begin()
        { return m_list.begin(); }

        /**
         * @brief Get a iterator the end of the OutputList.
         * @return Get a iterator the end of the OutputList.
         */
        iterator end()
        { return m_list.end(); }

        /**
         * @brief Get a constant iterator the begin of the OutputList.
         * @return Get a constant iterator the begin of the
         * OutputList.
         */
        const_iterator begin() const
        { return m_list.begin(); }

        /**
         * @brief Get a constant iterator the end of the OutputList.
         * @return Get a constant iterator the end of the OutputList.
         */
        const_iterator end() const
        { return m_list.end(); }

        /**
         * @brief Get the list of all output name.
         * @return the list of output name.
         */
        std::vector < std::string > outputsname() const;

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Functors.
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Use to add Output to an OutputList. To use with std::for_each.
         */
        struct AddOutput
        {
            /**
             * @brief Add Output to Outputs.
             * @param outputs The output parameter to store Output.
             */
            inline AddOutput(Outputs& outputs)
                : m_outputs(outputs)
            {}

            /**
             * @brief Add the Output to the Outputs.
             * @param pair the Output to add.
             */
            inline void operator()(const OutputList::value_type& pair)
            { m_outputs.add(pair.second); }

            Outputs& m_outputs; //!< the output parameters.
        };

    private:
        OutputList  m_list;
    };

}} // namespace vle vpz

#endif
