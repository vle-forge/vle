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
        typedef std::list < Condition > ConditionList;

        Conditions();

        virtual ~Conditions() { }

        virtual void write(std::ostream& out) const;

        virtual Base::type getType() const
        { return CONDITIONS; }


        /** 
         * @brief Add a list of Conditions to the list.
         * 
         * @param c A Conditions object to add.
         *
         * @throw Exception::Internal if a Condition already exist.
         */
        void addConditions(const Conditions& c);

        /** 
         * @brief Add a condition into the conditions list.
         * 
         * @param c the condition to add.
         *
         * @throw Exception::Internal if condition with same name and port
         * already exist.
         */
        void addCondition(const Condition& c);

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
        void delCondition(const std::string& modelname,
                          const std::string& portname);

        /** 
         * @brief Get the list of conditions.
         * 
         * @return A reference to the list of conditions.
         */
        ConditionList& conditions()
        { return m_conditions; }

        /** 
         * @brief Get the list of conditions.
         * 
         * @return A reference to the list of conditions.
         */
        const ConditionList& conditions() const
        { return m_conditions; }

        /** 
         * @brief This functor is a helper to find an condition by name in an
         * ConditionList using the standard algorithm std::find_if,
         * std::remove_if etc.
         *
         * Example:@n
         * <code>
         * Glib::ustring name = "a";@n
         * ConditionList::const_iterator it;
         * it = std::find_if(lst.begin(), lst.end(), ConditionHasName("a", "b");
         * </code>
         */
        struct ConditionHasNames
        {
            const Glib::ustring& modelname;
            const Glib::ustring& portname;

            inline ConditionHasNames(const Glib::ustring& modelname,
                                     const Glib::ustring& portname) :
                modelname(modelname),
                portname(portname)
            { }

            inline bool operator()(const Condition& condition) const
            { return condition.modelname() == modelname and
                condition.portname() == portname; }
        };

        Condition& find(const std::string& modelname,
                        const std::string& portname);

        const Condition& find(const std::string& modelname,
                              const std::string& portname) const;

    private:
        ConditionList m_conditions;
    };

}} // namespace vle vpz

#endif
