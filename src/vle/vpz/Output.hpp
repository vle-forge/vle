/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef VLE_VPZ_OUTPUT_HPP
#define VLE_VPZ_OUTPUT_HPP

#include <string>
#include <vle/vpz/Base.hpp>
#include <vle/DllDefines.hpp>

namespace vle {

    namespace value {

        class Value;

    } // namespace value

    namespace vpz {

    /**
     * @brief This class defines an output information and stores the type of
     * output: local or distant, the name of the oov::Plugin, its location
     * (local file system or internet address and the parametrization of the
     * plug-in).
     */
    class VLE_API Output : public Base
    {
    public:
        /**
         * @brief Define the output format of the plugin.
         * - LOCAL: use a local communication for observations.
         * - DISTANT: use a distant communication for observations using trame.
         */
        enum Format { LOCAL, DISTANT };

        /**
         * @brief Build a empty local output.
         */
        Output();

        /**
         * @brief Copy constructor.
         * @param output The Output to copy. The m_data is cloned.
         */
        Output(const Output& output);

        /**
         * @brief Affectation operator.
         * @param output The Output to assign. The m_data is cloned.
         * @return A reference to this.
         */
        Output& operator=(const Output& output);

        /**
         * @brief Nothing to delete.
         */
        virtual ~Output();

        /**
         * @brief Swap all the value between this output and the parameter. The
         * m_data is not cloned.
         * @param output The Output to replace data.
         */
        void swap(Output& output);

        /**
         * @brief Write the XML representation of the Output.
         * @code
         * <output name="name" location="/tmp" format="local" plugin="text" />
         * <output name="name" location="192.168.1.1:12:/tmp" format="distant"
         *         plugin="text" />
         * <output name="name" location="/tmp" plugin="text" />
         *  <![CDATA[ bla bla bla ]]>
         * </output>
         * @endcode
         * @param out The output stream.
         */
        virtual void write(std::ostream& out) const;

        /**
         * @brief Get the type of this class.
         * @return OUTPUT.
         */
        virtual Base::type getType() const
        { return VLE_VPZ_OUTPUT; }

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Manage Output object.
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Assign a new package name to the output.
         * @param name new name of the package.
         */
        void setPackage(const std::string& name)
        { m_package.assign(name); }

        /**
         * @brief Set the output with text stream information. The name is
         * obligatory, the location defines a filename.
         * @param location the file name.
         * @param plugin to use in output stream.
         * @param package the package of the plugin.
         * @throw utils::ArgError if name is empty.
         */
        void setLocalStream(const std::string& location,
                            const std::string& plugin,
                            const std::string& package);

        /**
         * @brief Set the output with text stream information. The name is
         * obligatory, the location defines a filename.
         * @param location the file name.
         * @param plugin to use in output stream.
         * @throw utils::ArgError if name is empty.
         */
        void setLocalStream(const std::string& location,
                            const std::string& plugin);

        /**
         * @brief Set the location of the output plugin.
         * @param location the file name.
         */
        void setLocalStreamLocation(const std::string& location);

        /**
         * @brief Set the output with the sdml stream information. The name is
         * obligatory, the location defines a filename.
         * @param location the file name.
         * @param plugin to use in output stream.
         * @param package the package of the plugin
         * @throw utils::ArgError if name is empty.
         */
        void setDistantStream(const std::string& location,
                              const std::string& plugin,
                              const std::string& package);

        /**
         * @brief Set the output with the sdml stream information. The name is
         * obligatory, the location defines a filename.
         * @param location the file name.
         * @param plugin to use in output stream.
         * @throw utils::ArgError if name is empty.
         */
        void setDistantStream(const std::string& location,
                              const std::string& plugin);

        /**
         * @brief Assign data to the Output. If previous data exists, it will be
         * deleted. Be careful, the data parameter is not cloned.
         * @param value The new data to assign.
         */
        void setData(value::Value* value);

        /**
         * @brief Delete the m_data value and assign null.
         */
        void clearData();

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Get/Set functions.
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Get the format of this Output.
         * @return the Format of the plugin.
         */
        Format format() const
        { return m_format; }

        /**
         * @brief Get the string representation of the format.
         * @return "local" or "distant".
         */
        std::string streamformat() const
        { return (m_format == LOCAL ? "local" : "distant"); }

        /**
         * @brief Get the plugin of this Output.
         * @return a string representation of plugin.
         */
        const std::string& plugin() const
        { return m_plugin; }

        /**
         * @brief Return the package name of this output.
         * @return a string represnetion of the package.
         */
        const std::string& package() const
        { return m_package; }

        /**
         * @brief Get the location of this Output.
         * @return a string representation of location.
         */
        const std::string& location() const
        { return m_location; }

        /**
         * @brief Get a reference to the data. The data can be null.
         * @return a string representation of the data.
         */
        const value::Value* data() const
        { return m_data; }

        /**
         * @brief Get the name of this output.
         * @return The name of this output.
         */
        const std::string& name() const
        { return m_name; }

        /**
         * @brief Set the name of this output.
         * @param name The name of this output.
         */
        void setName(const std::string& name)
        { m_name.assign(name); }

	/**
	 * @brief A operator to compare two Views
	 * @param view The View to compare
	 * @return True if the views are equals
	 */
	bool operator==(const Output& output) const;

    private:
        Format          m_format;
        std::string     m_name;
        std::string     m_plugin;
        std::string     m_location;
        std::string     m_package;
        value::Value*   m_data;
    };

}} // namespace vle vpz

#endif
