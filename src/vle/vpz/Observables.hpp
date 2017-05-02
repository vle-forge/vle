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

#ifndef VLE_VPZ_OBSERVABLES_HPP
#define VLE_VPZ_OBSERVABLES_HPP

#include <map>
#include <set>
#include <string>
#include <vle/DllDefines.hpp>
#include <vle/vpz/Base.hpp>
#include <vle/vpz/Observable.hpp>

namespace vle {
namespace vpz {

/**
 * @brief A operator to retrieve the key
 *
 * @param pair the key Observable pair.
 * @return a observable name
 */
inline std::string
observableKey(std::pair<std::string, Observable> pair)
{
    return pair.first;
}

/**
 * @brief A list of Observable.
 */
typedef std::map<std::string, Observable> ObservableList;

/**
 * @brief Observables is a container based on ObservableList to build a list
 * of Observable using the Observable's name as key.
 */
class VLE_API Observables : public Base
{
public:
    typedef ObservableList::iterator iterator;
    typedef ObservableList::const_iterator const_iterator;
    typedef ObservableList::size_type size_type;
    typedef ObservableList::value_type value_type;

    /**
     * @brief Build a empty Observables.
     */
    Observables()
    {
    }

    /**
     * @brief Nothing to delete.
     */
    virtual ~Observables()
    {
    }

    /**
     * @brief A operator to retrieve the list of keys
     *
     * @return vector of Observable names
     */
    std::set<std::string> getKeys();

    /**
     * @brief Write into the std::ostream, the XML reprensentation of
     * Observables.
     * @code
     * <observables>
     *  [...]
     * </observables>
     * @endcode
     * @param out The output stream.
     */
    virtual void write(std::ostream& out) const override;

    /**
     * @brief Get the type of this class.
     * @return OBSERVABLES.
     */
    virtual Base::type getType() const override
    {
        return VLE_VPZ_OBSERVABLES;
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Manage Observables objects
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Add a list of Observable object to the ObservableList.
     * @param obs The lsit of Observable.
     * @throw utils::ArgError if an Observable already exist.
     */
    void add(const Observables& obs);

    /**
     * @brief Add an Observable object to the ObservableList.
     * @param obs The Observable to copy.
     * @return A reference to the newly build Observable.
     * @throw utils::ArgError if observable already exist.
     */
    Observable& add(const Observable& obs);

    /**
     * @brief Return the Observable object by his name.
     * @param name The name of the Observable to remove.
     * @return A reference to the Observable.
     * @throw utils::ArgError if observable not exist.
     */
    Observable& get(const std::string& name);

    /**
     * @brief Return the Observable object by his name.
     * @param name The name of the Observable to remove.
     * @return A constant reference to the Observable.
     * @throw utils::ArgError if observable not exist.
     */
    const Observable& get(const std::string& name) const;

    /**
     * @brief Delete the Observable object by his name.
     * @param name The name of the Observable to remove.
     */
    inline void del(const std::string& name)
    {
        m_list.erase(name);
    }

    /**
      * @brief Rename the specified observable.
      * @param old_name The current observable name
      * @param new_name The new name for the observable
      */
    void rename(const std::string& old_name, const std::string& new_name);

    /**
     * @brief Return true if the name already exist in the ObservableList.
     * @param name The name to check in ObservableList.
     * @return True if the name exist, false otherwise.
     */
    inline bool exist(const std::string& name) const
    {
        return m_list.find(name) != m_list.end();
    }

    /**
     * @brief Return true if the ObservableList is empty.
     * @return Return true if the ObservableList is empty, false otherwise.
     */
    inline bool empty() const
    {
        return m_list.empty();
    }

    /**
     * @brief Remove all Observable from the ObservableList.
     */
    inline void clear()
    {
        m_list.clear();
    }

    /**
     * @brief Remove all Observable from the ObservableList that respond
     * false to the Observable::isPermanent() function.
     */
    void cleanNoPermanent();

    /**
     * @brief Update the name of the attached view of each
     * Observable related to oldname view.
     * @param oldname The old name of the view attached to the port
     * @param newname The new name of the view attached to the port
     */
    void updateView(const std::string& oldname, const std::string& newname);

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Get/Set functions
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Get a constant reference to the ObservableList.
     * @return A constant reference to the ObservableList.
     */
    inline const ObservableList& observablelist() const
    {
        return m_list;
    }

    /**
     * @brief Get a constant reference to the ObservableList.
     * @return A constant reference to the ObservableList.
     */
    inline ObservableList& observablelist()
    {
        return m_list;
    }

    /**
     * @brief Get a iterator the begin of the ObservableList.
     * @return Get a iterator the begin of the ObservableList.
     */
    iterator begin()
    {
        return m_list.begin();
    }

    /**
     * @brief Get a iterator the end of the ObservableList.
     * @return Get a iterator the end of the ObservableList.
     */
    iterator end()
    {
        return m_list.end();
    }

    /**
     * @brief Get a constant iterator the begin of the ObservableList.
     * @return Get a constant iterator the begin of the
     * ObservableList.
     */
    const_iterator begin() const
    {
        return m_list.begin();
    }

    /**
     * @brief Get a constant iterator the end of the ObservableList.
     * @return Get a constant iterator the end of the ObservableList.
     */
    const_iterator end() const
    {
        return m_list.end();
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Functors.
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Functor to add an Observable to a ObservableList.
     */
    struct AddObservable
    {
        /**
         * @brief Add Observable to Observables.
         * @param observables The output parameter to store Observable.
         */
        AddObservable(Observables& observables)
          : m_observables(observables)
        {
        }

        /**
         * @brief Add the Observable to the Observables.
         * @param pair the Observable to add.
         */
        inline void operator()(const ObservableList::value_type& pair)
        {
            m_observables.add(pair.second);
        }

        Observables& m_observables; //!< the output parameters.
    };

private:
    ObservableList m_list;
};
}
} // namespace vle vpz

#endif
