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


#ifndef VLE_VPZ_DYNAMIC_HPP
#define VLE_VPZ_DYNAMIC_HPP

#include <vle/DllDefines.hpp>
#include <vle/vpz/Base.hpp>

namespace vle { namespace vpz {

/**
 * @brief A Dynamic represents the DYNAMIC tag of XML project file. It
 * stores the name of the dynamics, the library and model used by vle, the
 * type local or distant and a string data attached to the dynamics
 * initialisation.
 * Example:
 * @code
 * <dynamic name="xxxx"     <!-- name of the dynamics -->
 *          library="yyyy"  <!-- name of the library -->
 *
 * @endcode
 */
class VLE_API Dynamic : public Base
{
public:
    /**
     * @brief Build a new local Dynamic with a specific name.
     * @param name Name of the Dynamic.
     */
    Dynamic(const std::string& name) :
        m_name(name)
    {}

    /**
     * @brief Nothing to clean.
     */
    virtual ~Dynamic()
    {}

    /**
     * @brief Write the XML representation of this class in the output
     * stream.
     * @param out Output stream where flush this class.
     */
    virtual void write(std::ostream& out) const;

    /**
     * @brief Get the type of this class.
     * @return Return DYNAMIC.
     */
    virtual Base::type getType() const
    { return VLE_VPZ_DYNAMIC; }

    /**
     * @brief Assign a new package name to the dynamics.
     * @param name new name of the package.
     */
    void setPackage(const std::string& name)
    { m_package.assign(name); }

    /**
     * @brief Assign a new library name to the dynamics.
     * @param name new name of the library.
     */
    void setLibrary(const std::string& name)
    { m_library.assign(name); }

    /**
     * @brief Assign a language to the dynamics.
     * @param name new language of the dynamics.
     */
    void setLanguage(const std::string& name)
    { m_language.assign(name); }

    /**
     * @brief Assign a name to the dynamics.
     * @param name of the dynamics.
     */
    void setName(const std::string& name)
    { m_name.assign(name); }

    /**
     * @brief Get the current name of the dynamic.
     * @return the name of the dynamic.
     */
    const std::string& name() const
    { return m_name; }

    /**
     * @brief Return the package name of this dynamics.
     * @return a string represnetion of the package.
     */
    const std::string& package() const
    { return m_package; }

    /**
     * @brief Return the library name of dynamics.
     * @return a string representation of library name.
     */
    const std::string& library() const
    { return m_library; }

    /**
     * @brief Return the language of the dynamics models.
     * @return A reference to the model.
     */
    const std::string& language() const
    { return m_language; }

    /**
     * @brief Return true if this dynamics is a permanent data for the
     * devs::ModelFactory.
     * @return True if this dynamics is a permanent value.
     */
    bool isPermanent() const
    { return m_ispermanent; }

    /**
     * @brief Set the permanent value of this dynamics.
     * @param value True to conserve this dynamics in devs::ModelFactory.
     */
    void permanent(bool value = true)
    { m_ispermanent = value; }

    /**
     * @brief An operator to compare two dynamics by theirs names.
     * @param dynamic The dynamic to search.
     * @return True if current name less that dynamic parameter.
     */
    bool operator<(const Dynamic& dynamic) const
    { return m_name < dynamic.m_name; }

    /**
     * @brief An operator to compare two dynamics.
     * @param dynamic The dynamic to compare.
     * @return True if the dynamics are equals.
     */
    bool operator==(const Dynamic& dynamic) const;

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Predicate functor to check if this dynamic is permanet or not.
     * To use with std::find_if:
     * Example:
     * @code
     * std::vector < Dynamic > v;
     * [...]
     * std::vector < Dynamic >::iterator it =
     *    std::find_if(v.beging(), v.end(), Dynamic::IsPermanent());
     * @endcode
     */
    struct IsPermanent
    {
        /**
         * @brief Check if the Dynamic is a permanent.
         * @param x the Dynamic to test.
         * @return True if the Dynamic is permanent.
         */
        bool operator()(const Dynamic& x) const
        { return x.isPermanent(); }
    };

private:
    std::string     m_name;
    std::string     m_package;
    std::string     m_library;
    std::string     m_language;
    bool            m_ispermanent;
};

}} // namespace vle vpz

#endif
