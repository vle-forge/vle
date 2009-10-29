/**
 * @file vle/vpz/Conditions.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.sourceforge.net/projects/vle
 *
 * Copyright (C) 2003 - 2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (C) 2003 - 2009 ULCO http://www.univ-littoral.fr
 * Copyright (C) 2007 - 2009 INRA http://www.inra.fr
 * Copyright (C) 2007 - 2009 Cirad http://www.cirad.fr
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


#ifndef VLE_VPZ_CONDITIONS_HPP
#define VLE_VPZ_CONDITIONS_HPP

#include <vle/vpz/Condition.hpp>
#include <vle/vpz/Base.hpp>
#include <vle/vpz/DllDefines.hpp>
#include <string>
#include <list>

namespace vle { namespace vpz {

    /**
     * @brief Define a condition list like list of names, conditions.
     */
    typedef std::map < std::string, Condition > ConditionList;

    /**
     * @brief This class describe a list of condition and allow loading and
     * writing a conditions and condition tags.
     */
    class VLE_VPZ_EXPORT Conditions : public Base
    {
    public:
        /**
         * @brief Define an iterator to the vpz::ConditionList.
         */
        typedef ConditionList::iterator iterator;

        /**
         * @brief Define a constant iterator to the vpz::ConditionList.
         */
        typedef ConditionList::const_iterator const_iterator;

        /**
         * @brief Build a new vpz::Conditions.
         */
        Conditions()
        {}

        /**
         * @brief Nothing to delete.
         */
        virtual ~Conditions()
        {}

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
         * @param out a output stream where write the classes tags.
         */
        virtual void write(std::ostream& out) const;

        /**
         * @brief Get the type of this class.
         * @return CONDITIONS.
         */
        virtual Base::type getType() const
        { return CONDITIONS; }

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Manage ConditionList
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Build a list of string that contains all condition names.
         * @param lst An output string list.
         */
        void conditionnames(std::list < std::string >& lst) const;

        /**
         * @brief Build a list of string that contains all port names for the
         * specified condition.
         * @param condition The condition to get port name.
         * @param lst An output string list.
         * @throw utils::ArgError if condition not exist.
         */
        void portnames(const std::string& condition,
                       std::list < std::string >& lst) const;

        /**
         * @brief Add a list of Conditions to the list.
         * @param conditions A Conditions object to add.
         * @throw utils::ArgError if a Condition already exist.
         */
        void add(const Conditions& conditions);

        /**
         * @brief Add a condition into the conditions list.
         * @param condition the condition to add into the map. Condition is
         * copied.
         * @return the newly created Condition.
         * @throw utils::ArgError if condition with same name and port already
         * exist.
         */
        Condition& add(const Condition& condition);

        /**
         * @brief Delete the specified condition from the condition list.
         * @param condition The name of the condition.
         */
        void del(const std::string& condition);

	/**
	  * @brief Rename the specified condition.
	  * @param old_name The current condition name
	  * @param new_name The new name for the condition
	  */
	void rename(const std::string& oldconditionname, const std::string& newconditinname);

	/**
	  * @brief Copy the specified condition
	  * @param conditionname The current condition name
	  * @param copyconditionname The copy condition name
	  */
	void copy(const std::string& conditionname,
		  const std::string& copyconditionname);


        /**
         * @brief Clear the ConditionList object.
         */
        inline void clear()
        { m_list.clear(); }

        /**
         * @brief Return true if the name already exist in the ConditionList.
         * @param name The name to check in ConditionList.
         * @return True if the name exist, false otherwise.
         */
        inline bool exist(const std::string& name) const
        { return m_list.find(name) != m_list.end(); }

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

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Get/Set
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

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
         * @brief Get a iterator the begin of the vpz::ConditionList.
         * @return Get a iterator the begin of the vpz::ConditionList.
         */
        iterator begin()
        { return m_list.begin(); }

        /**
         * @brief Get a iterator the end of the vpz::ConditionList.
         * @return Get a iterator the end of the vpz::ConditionList.
         */
        iterator end()
        { return m_list.end(); }

        /**
         * @brief Get a constant iterator the begin of the vpz::ConditionList.
         * @return Get a constant iterator the begin of the
         * vpz::ConditionList.
         */
        const_iterator begin() const
        { return m_list.begin(); }

        /**
         * @brief Get a constant iterator the end of the vpz::ConditionList.
         * @return Get a constant iterator the end of the vpz::ConditionList.
         */
        const_iterator end() const
        { return m_list.end(); }

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Functors
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Functor to add condition to the ConditionList. To use with the
         * std::for_each algorithm.
         */
        struct AddCondition
        {
            /**
             * @brief A constructor to add Condition the the Conditions.
             * @param conditions The Conditions that sotres Condition.
             */
            AddCondition(Conditions& conditions)
                : m_conditions(conditions)
            {}

            /**
             * @brief Add the ConditionList to the Conditions.
             * @param pair the ConditionList to add.
             */
            void operator()(const ConditionList::value_type& pair)
            { m_conditions.add(pair.second); }

            Conditions& m_conditions; //!< Output of this functor.
        };

    private:
        ConditionList       m_list;
    };

}} // namespace vle vpz

#endif
