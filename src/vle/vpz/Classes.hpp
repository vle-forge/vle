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

#ifndef VLE_VPZ_CLASSES_HPP
#define VLE_VPZ_CLASSES_HPP

#include <map>
#include <vle/DllDefines.hpp>
#include <vle/vpz/Base.hpp>
#include <vle/vpz/Class.hpp>

namespace vle {
namespace vpz {

/**
 * @brief Define a list of class by a dictionnary class name, class value.
 */
typedef std::map<std::string, Class> ClassList;

/**
 * @brief The class Classes is build on the ClassList typedef to construct a
 * list of class.
 */
class VLE_API Classes : public Base
{
public:
    typedef ClassList::iterator iterator;
    typedef ClassList::const_iterator const_iterator;
    typedef ClassList::size_type size_type;
    typedef ClassList::value_type value_type;

    /**
     * @brief Empty constructor.
     */
    Classes()
    {
    }

    /**
     * @brief Nothing to delete.
     */
    virtual ~Classes()
    {
    }

    /**
     * @brief Write the classes XML representation into the output stream.
     * @code
     * <classes>
     *  <class name="xxxx">
     *   <structure>
     *    ...
     *   </structure>
     *  </class>
     * </classes>
     * @endcode
     * @param out a output stream where write the classes tags.
     */
    virtual void write(std::ostream& out) const override;

    /**
     * @brief Get the type of this class.
     * @return CLASSES.
     */
    virtual Base::type getType() const override
    {
        return VLE_VPZ_CLASSES;
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Manage the ClassList
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Add a new class into the class list.
     * @param name The name of the class.
     * @return A reference to the newly class.
     * @throw utils::ArgError if name already exist.
     */
    Class& add(const std::string& name);

    /**
     * @brief Remove the specified class from the class list.
     * @param name The name of the class.
     */
    void del(const std::string& name);

    /**
     * @brief Get a const reference to the specified class from the class
     * list.
     * @param name The name of the class.
     * @return A constant reference to the class.
     * @throw utils::ArgError if name does not exist.
     */
    const Class& get(const std::string& name) const;

    /**
     * @brief Get a reference to the specified class from the class list.
     * @param name The name of the class.
     * @return A reference to the class.
     * @throw utils::ArgError if name does not exist.
     */
    Class& get(const std::string& name);

    /**
     * @brief Check if a class already exist in the class list.
     * @param name The name of the class.
     * @return true if the class already exist, false otherwise.
     */
    bool exist(const std::string& name) const
    {
        return m_lst.find(name) != m_lst.end();
    }

    /**
     * @brief Rename an existant Class to a newname. Be carefull, the Class
     * is cloned.
     * @param oldname The Class to rename.
     * @param newname The destination name.
     * @throw utils::ArgError if Class oldname does not exist or if newname
     * exists.
     */
    void rename(const std::string& oldname, const std::string& newname);

    /**
     * @brief Remove all class from the class list.
     */
    void clear()
    {
        m_lst.clear();
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Manage the ClassList
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Return a constant iterator to the begin of the ClassList.
     * @return Return a constant iterator to the begin of the ClassList.
     */
    const_iterator begin() const
    {
        return m_lst.begin();
    }

    /**
     * @brief Return a constant reference to the end of the ClassList.
     * @return Return a constant reference to the end of the ClassList.
     */
    const_iterator end() const
    {
        return m_lst.end();
    }

    /**
     * @brief Return a iterator to the begin of the ClassList.
     * @return Return a iterator to the begin of the ClassList.
     */
    iterator begin()
    {
        return m_lst.begin();
    }

    /**
     * @brief Return a reference to the end of the ClassList.
     * @return Return a reference to the end of the ClassList.
     */
    iterator end()
    {
        return m_lst.end();
    }

    /**
     * @brief Get a constant reference to the whole class list.
     * @return Get a constant reference to the whole class list.
     */
    const ClassList& list() const
    {
        return m_lst;
    }

    /**
     * @brief Get a reference to the whole class list.
     * @return Get a reference to the whole class list.
     */
    ClassList& list()
    {
        return m_lst;
    }

    /**
     * @brief Update the dynamics of the AtomicModels where an
     * oldname became newname, for each class graph model.
     * @param oldname the old name of the dynamics.
     * @param newname the new name of the dynamics.
     */
    void updateDynamics(const std::string& oldname,
                        const std::string& newname);

    /**
      * @brief purge the dymamics references of the model where the
      * dynamic is not present in the list for each class
      * @param dynamicslist a list of dynamics name
      */
    void purgeDynamics(const std::set<std::string>& dynamicslist);

    /**
     * @brief Update the Observable of the AtomicModels where an
     * oldname became newname, for the each class graph model.
     * @param oldname the old name of the observable.
     * @param newname the new name of the observable.
     */
    void updateObservable(const std::string& oldname,
                          const std::string& newname);

    /**
     * @brief purge the observables references of the model where the
     * observable is not present in the list for each class
     * @param observablelist a list of observable name
     */
    void purgeObservable(const std::set<std::string>& observablelist);

    /**
     * @brief Update the Conditions of the AtomicModels where an
     * oldname became newname, for each class graph model.
     * @param oldname the old name of the observable.
     * @param newname the new name of the observable.
     */
    void updateConditions(const std::string& oldname,
                          const std::string& newname);

    /**
     * @brief purge the Conditions references for each class model
     * where the Condition is not present in the list.
     * @param conditionlist a list of condition name
     */
    void purgeConditions(const std::set<std::string>& conditionlist);

private:
    ClassList m_lst;
};
}
} // namespace vle vpz

#endif
