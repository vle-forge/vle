/**
 * @file vle/vpz/Observables.hpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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


#ifndef VLE_VPZ_OBSERVABLES_HPP
#define VLE_VPZ_OBSERVABLES_HPP

#include <vle/vpz/Base.hpp>
#include <vle/vpz/Observable.hpp>
#include <map>
#include <string>

namespace vle { namespace vpz {

    /**
     * @brief A list of Observable.
     */
    typedef std::map < std::string, Observable > ObservableList;

    /**
     * @brief Observables is a container based on ObservableList to build a list
     * of Observable using the Observable's name as key.
     */
    class Observables : public Base
    {
    public:
        /**
         * @brief Define a iterator for ObservableList.
         */
        typedef ObservableList::iterator iterator;

        /**
         * @brief Define a const_iterator for ObservableList.
         */
        typedef ObservableList::const_iterator const_iterator;

        /**
         * @brief Build a empty Observables.
         */
        Observables()
        {}

        /**
         * @brief Nothing to delete.
         */
        virtual ~Observables()
        {}

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
        virtual void write(std::ostream& out) const;

        /**
         * @brief Get the type of this class.
         * @return OBSERVABLES.
         */
        virtual Base::type getType() const
        { return OBSERVABLES; }

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
        { m_list.erase(name); }

        /**
         * @brief Return true if the name already exist in the ObservableList.
         * @param name The name to check in ObservableList.
         * @return True if the name exist, false otherwise.
         */
        inline bool exist(const std::string& name) const
        { return m_list.find(name) != m_list.end(); }

        /**
         * @brief Return true if the ObservableList is empty.
         * @return Return true if the ObservableList is empty, false otherwise.
         */
        inline bool empty() const
        { return m_list.empty(); }

        /**
         * @brief Remove all Observable from the ObservableList.
         */
        inline void clear()
        { m_list.clear(); }

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
        void updateView(const std::string& oldname,
                        const std::string& newname);

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
        { return m_list; }

        /**
         * @brief Get a constant reference to the ObservableList.
         * @return A constant reference to the ObservableList.
         */
        inline ObservableList& observablelist()
        { return m_list; }

        /**
         * @brief Get a iterator the begin of the ObservableList.
         * @return Get a iterator the begin of the ObservableList.
         */
        iterator begin()
        { return m_list.begin(); }

        /**
         * @brief Get a iterator the end of the ObservableList.
         * @return Get a iterator the end of the ObservableList.
         */
        iterator end()
        { return m_list.end(); }

        /**
         * @brief Get a constant iterator the begin of the ObservableList.
         * @return Get a constant iterator the begin of the
         * ObservableList.
         */
        const_iterator begin() const
        { return m_list.begin(); }

        /**
         * @brief Get a constant iterator the end of the ObservableList.
         * @return Get a constant iterator the end of the ObservableList.
         */
        const_iterator end() const
        { return m_list.end(); }

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
            {}

            /**
             * @brief Add the Observable to the Observables.
             * @param pair the Observable to add.
             */
            inline void operator()(const ObservableList::value_type& pair)
            { m_observables.add(pair.second); }

            Observables& m_observables; //!< the output parameters.
        };


    private:
        ObservableList m_list;
    };

}} // namespace vle vpz

#endif
