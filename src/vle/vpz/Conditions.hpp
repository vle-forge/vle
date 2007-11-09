/** 
 * @file vpz/Conditions.hpp
 * @brief 
 * @author The vle Development Team
 * @date mar, 31 jan 2006 17:21:28 +0100
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

#ifndef VLE_VPZ_CONDITIONS_HPP
#define VLE_VPZ_CONDITIONS_HPP

#include <string>
#include <list>
#include <vle/vpz/Condition.hpp>
#include <vle/vpz/Base.hpp>

namespace vle { namespace vpz {

    /** 
     * @brief Define a condition list like list of names, conditions.
     */
    typedef std::map < std::string, Condition > ConditionList;

    /** 
     * @brief This class describe a list of condition and allow loading and
     * writing a conditions and condition tags.
     */
    class Conditions : public Base
    {
    public:
        Conditions();

        virtual ~Conditions()
        { }

        /** 
         * @brief Add Conditions informations to the stream.
         * @code
         * <conditions>
         *  <condition name="">
         *  </condition>
         *  <condition name="">
         *  </condition>
         * </conditions>
         * @endcode
         * 
         * @param out 
         */
        virtual void write(std::ostream& out) const;

        virtual Base::type getType() const
        { return CONDITIONS; }

        ////
        //// Manage ConditionList
        ////

        /** 
         * @brief Get a constant reference to the ConditionList.
         * @return A constant reference to the ConditionList.
         */
        inline const ConditionList& conditionlist() const
        { return m_list; }

        /** 
         * @brief Get a reference to the ConditionList.
         * @return A reference to the ConditionList.
         */
        inline ConditionList& conditionlist()
        { return m_list; }

        /** 
         * @brief Add a list of Conditions to the list.
         * @param conditions A Conditions object to add.
         * @throw Exception::Internal if a Condition already exist.
         */
        void add(const Conditions& conditions);

        /** 
         * @brief Add a condition into the conditions list.
         * @param condition the condition to add into the map. Condition is
         * copied.
         * @return the newly created Condition.
         * @throw Exception::Internal if condition with same name and port
         * already exist.
         */
        Condition& add(const Condition& condition);

        /** 
         * @brief Delete the specified condition from the conditions list.
         * @param modelname condition model name.
         * @param portname condition port name.
         */
        void del(const std::string& condition);

        /** 
         * @brief Clear the ConditionList object.
         */
        inline void clear()
        { m_list.clear(); }

        /** 
         * @brief Get the specified condition from conditions list.
         * @param condition 
         * @return 
         */
        const Condition& get(const std::string& condition) const;

        /** 
         * @brief Get the specified condition from conditions list.
         * @param condition 
         * @return 
         */
        Condition& get(const std::string& condition);
        
        /** 
         * @brief Remove all no permanent value of the list. This function is
         * use to clean not usefull data for the devs::ModelFactory. Linear
         * function.
         */
        void cleanNoPermanent();

        /** 
         * @brief This function initialise every, to each port, a new
         * value::Set.
         */
        void rebuildValueSet();
    
    private:
        ConditionList       m_list;

        struct AddCondition
        {
            AddCondition(Conditions& conditions) :
                m_conditions(conditions)
            { }

            void operator()(const ConditionList::value_type& pair)
            { m_conditions.add(pair.second); }

            Conditions& m_conditions;
        };

        struct RebuildValueSet
        {
            void operator()(ConditionList::value_type& pair)
            { pair.second.rebuildValueSet(); }
        };
    };

}} // namespace vle vpz

#endif
