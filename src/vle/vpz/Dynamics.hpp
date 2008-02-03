/**
 * @file vle/vpz/Dynamics.hpp
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


#ifndef VLE_VPZ_DYNAMICS_HPP
#define VLE_VPZ_DYNAMICS_HPP

#include <set>
#include <vle/vpz/Dynamic.hpp>

namespace vle { namespace vpz {

    /** 
     * @brief Define a list of Dynamcis.
     */
    typedef std::map < std::string, Dynamic > DynamicList;

    class Dynamics : public Base
    {
    public:
        Dynamics()
        { }
       
        virtual ~Dynamics()
        { }

        /** 
         * @brief Write the Dynamics information into stream
         * @code.
         * <dynamics>
         *  [...]
         * </dynamics>
         * @endcode
         * 
         * @param out, the output stream.
         */
        virtual void write(std::ostream&  out) const;

        virtual Base::type getType() const
        { return DYNAMICS; }

        ////
        //// Manage the DynamicList
        ////

        /** 
         * @brief Get a constant reference to the DynamicList.
         * @return Get a constant reference to the DynamicList.
         */
        inline const DynamicList& dynamiclist() const
        { return m_list; }

        /** 
         * @brief Add a list of Dynamics to the list.
         * @param dyns A Dynamics objet to add.
         * @throw Exception::Internal if a Dynamic already exist.
         */
        void add(const Dynamics& dyns);

        /** 
         * @brief Add a Dynamic to the list.
         * @param dyn the Dynamic to add.
         * @return a reference to the newly created dynamics.
         * @throw Exception::Internal if a dynamic with the same model name
         * already exist.
         */
        Dynamic& add(const Dynamic& dyn);

        /** 
         * @brief Delete the dynamic for a specific model name.
         * @param name the name of the model.
         */
        void del(const std::string& name);

        /** 
         * @brief Remove all Dynamic from the DynamicList.
         */
        inline void clear()
        { m_list.clear(); }

        /** 
         * @brief Search a Dynamic with the specified name.
         * @param name Dynamic name to find.
         * @return A constant reference to the Dynamic find.
         * @throw Exception::Internal if no Dynamic find.
         */
        const Dynamic& get(const std::string& name) const;

        /** 
         * @brief Search a Dynamic with the specified name.
         * 
         * @param name Dynamic name to find.
         * 
         * @return A constant reference to the Dynamic find.
         *
         * @throw Exception::Internal if no Dynamic find.
         */
        Dynamic& get(const std::string& name);

        /** 
         * @brief Search a Dynamic with the specified name.
         * 
         * @param name Dynamic name to find.
         * 
         * @return true if founded, false otherwise.
         */
        bool exist(const std::string& name) const;

        /** 
         * @brief Remove all no permanent value of the list. This function is
         * use to clean not usefull data for the devs::ModelFactory. Linear
         * function.
         */
        void cleanNoPermanent();

        ////
        //// Functors
        ////

        /** 
         * @brief Functor to get the name of a Dynamic.
         */
        struct DynamicName
        {
            inline const std::string& operator()(
                const DynamicList::value_type& x) const
            { return x.first; }
        };

        /** 
         * @brief Functor to get the value of a Dynamic.
         */
        struct DynamicValue
        {
            inline const Dynamic& operator()(
                const DynamicList::value_type& x) const
            { return x.second; }
        };

        /** 
         * @brief Functor to add a dynamic to a dynamics.
         */
        struct AddDynamic
        {
            AddDynamic(Dynamics& dynamics) : m_dynamics(dynamics) { }

            void operator()(const DynamicList::value_type& pair)
            { m_dynamics.add(pair.second); }

            Dynamics& m_dynamics;
        };
    private:
        DynamicList     m_list;

    };

}} // namespace vle vpz

#endif
