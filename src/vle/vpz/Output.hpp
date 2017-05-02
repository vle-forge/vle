/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2017 Gauthier Quesnel <gauthier.quesnel@inra.fr>
 * Copyright (c) 2003-2017 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2017 INRA http://www.inra.fr
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

#include <memory>
#include <string>
#include <vle/DllDefines.hpp>
#include <vle/vpz/Base.hpp>

namespace vle {

namespace value {

class Value;

} // namespace value

namespace vpz {

/**
 * This class defines an output information and stores the type of
 * output, the name of the oov::Plugin, its location and the
 * parametrization of the plug-in).
 */
class VLE_API Output : public Base
{
public:
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
    virtual void write(std::ostream& out) const override;

    /**
     * @brief Get the type of this class.
     * @return OUTPUT.
     */
    virtual Base::type getType() const override
    {
        return VLE_VPZ_OUTPUT;
    }

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
    {
        m_package.assign(name);
    }

    /**
     * @brief Set the output with text stream information. The name is
     * obligatory, the location defines a filename.
     * @param location the file name.
     * @param plugin to use in output stream.
     * @param package the package of the plugin.
     * @throw utils::ArgError if name is empty.
     */
    void setStream(const std::string& location,
                   const std::string& plugin,
                   const std::string& package);

    /**
     * @brief Set the output with text stream information. The name is
     * obligatory, the location defines a filename.
     * @param location the file name.
     * @param plugin to use in output stream.
     * @throw utils::ArgError if name is empty.
     */
    void setStream(const std::string& location, const std::string& plugin);

    /**
     * @brief Set the location of the output plugin.
     * @param location the file name.
     */
    void setStreamLocation(const std::string& location);

    /**
     * @brief Assign data to the Output. If previous data exists, it will be
     * deleted. Be careful, the data parameter is not cloned.
     * @param value The new data to assign.
     */
    void setData(std::shared_ptr<value::Value> value);

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
     * @brief Get the plugin of this Output.
     * @return a string representation of plugin.
     */
    const std::string& plugin() const
    {
        return m_plugin;
    }

    /**
     * @brief Return the package name of this output.
     * @return a string represnetion of the package.
     */
    const std::string& package() const
    {
        return m_package;
    }

    /**
     * @brief Get the location of this Output.
     * @return a string representation of location.
     */
    const std::string& location() const
    {
        return m_location;
    }

    /**
     * @brief Get a reference to the data. The data can be null.
     * @return a string representation of the data.
     */
    const std::shared_ptr<value::Value>& data() const
    {
        return m_data;
    }

    /**
     * @brief Get the name of this output.
     * @return The name of this output.
     */
    const std::string& name() const
    {
        return m_name;
    }

    /**
     * @brief Set the name of this output.
     * @param name The name of this output.
     */
    void setName(const std::string& name)
    {
        m_name.assign(name);
    }

    /**
     * @brief A operator to compare two Views
     * @param view The View to compare
     * @return True if the views are equals
     */
    bool operator==(const Output& output) const;

private:
    std::string m_name;
    std::string m_plugin;
    std::string m_location;
    std::string m_package;
    std::shared_ptr<value::Value> m_data;
};
}
} // namespace vle vpz

#endif
