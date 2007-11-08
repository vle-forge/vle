/** 
 * @file Observables.hpp
 * @brief 
 * @author The vle Development Team
 * @date ven, 15 jun 2007 17:46:11 +0200
 */

/*
 * Copyright (C) 2007 - The vle Development Team
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

#ifndef VLE_VPZ_OBSERVABLES_HPP
#define VLE_VPZ_OBSERVABLES_HPP

#include <vle/vpz/Observable.hpp>
#include <map>
#include <string>
#include <iterator>
#include <algorithm>

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
        Observables()
        { }

        virtual ~Observables()
        { }

        virtual void write(std::ostream& out) const;

        virtual Base::type getType() const
        { return OBSERVABLES; }

        ////
        //// Manage Observables objects.
        ////

        /** 
         * @brief Get a constant reference to the ObservableList.
         * @return A constant reference to the ObservableList.
         */
        inline const ObservableList& observablelist() const
        { return m_list; }

        /** 
         * @brief Add a list of Observable object to the ObservableList.
         * @param obs The lsit of Observable.
         * @throw utils::SaxParserError if an Observable already exist.
         */
        void add(const Observables& obs);

        /** 
         * @brief Add an Observable object to the ObservableList.
         * @param obs The Observable to copy.
         * @return A reference to the newly build Observable.
         * @throw utils::SaxParserError if observable already exist.
         */
        Observable& add(const Observable& obs);

        /** 
         * @brief Return the Observable object by his name.
         * @param name The name of the Observable to remove.
         * @return A reference to the Observable.
         * @throw utils::SaxParserError if observable not exist.
         */
        Observable& get(const std::string& name);

        /** 
         * @brief Return the Observable object by his name.
         * @param name The name of the Observable to remove.
         * @return A constant reference to the Observable.
         * @throw utils::SaxParserError if observable not exist.
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

        ////
        //// Usefull functions.
        ////

        /** 
         * @brief Remove all Observable from the ObservableList that respond
         * false to the Observable::isPermanent() function.
         */
        void cleanNoPermanent();

    private:
        ObservableList m_list;

        struct AddObservable
        {
            AddObservable(Observables& observables) :
                m_observables(observables)
            { }

            inline void operator()(const ObservableList::value_type& pair)
            { m_observables.add(pair.second); }

            Observables& m_observables;
        };
    };

}} // namespace vle vpz

#endif
