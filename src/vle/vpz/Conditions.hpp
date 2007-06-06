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
     * @brief This class describe a list of condition and allow loading and
     * writing a conditions and condition tags.
     */
    class Conditions : public Base
    {
    public:
        typedef std::map < std::string, Condition > ConditionList;
        typedef ConditionList::const_iterator const_iterator;
        typedef ConditionList::iterator iterator;

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


        /** 
         * @brief Add a list of Conditions to the list.
         * 
         * @param conditions A Conditions object to add.
         *
         * @throw Exception::Internal if a Condition already exist.
         */
        void addConditions(const Conditions& conditions);

        /** 
         * @brief Add a condition into the conditions list.
         * 
         * @param condition the condition to add into the map. Condition is
         * copied.
         *
         * @throw Exception::Internal if condition with same name and port
         * already exist.
         */
        void addCondition(const Condition& condition);

        /** 
         * @brief Just clear the list of vpz::Condition.
         */
        void clear();

        /** 
         * @brief Delete the specified condition from the conditions list.
         * 
         * @param modelname condition model name.
         * @param portname condition port name.
         */
        void delCondition(const std::string& condition);

        /** 
         * @brief Get the specified condition from conditions list.
         * 
         * @param condition 
         * 
         * @return 
         */
        const Condition& find(const std::string& condition) const;

        /** 
         * @brief Get the specified condition from conditions list.
         * 
         * @param condition 
         * 
         * @return 
         */
        Condition& find(const std::string& condition);

        /** 
         * @brief Get the list of conditions.
         * 
         * @return A reference to the list of conditions.
         */
        inline const ConditionList& conditions() const
        { return m_conditions; }

        inline ConditionList& conditions()
        { return m_conditions; }

    private:
        ConditionList m_conditions;
    };

}} // namespace vle vpz

#endif
