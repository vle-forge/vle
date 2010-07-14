/**
 * @file vle/vpz/Condition.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2010 INRA http://www.inra.fr
 * Copyright (C) 2003-2010 ULCO http://www.univ-littoral.fr
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


#ifndef VLE_VPZ_CONDITION_HPP
#define VLE_VPZ_CONDITION_HPP

#include <vle/vpz/Base.hpp>
#include <vle/vpz/DllDefines.hpp>
#include <vle/value/Map.hpp>
#include <vle/value/Set.hpp>
#include <string>
#include <map>
#include <list>

namespace vle { namespace vpz {

    /**
     * @brief Define the ConditionValues like a dictionnary, (portname, values).
     */
    typedef std::map < std::string, value::Set* > ConditionValues;

    /**
     * @brief A condition define a couple model name, port name and a Value.
     * This class allow loading and writing a condition.
     */
    class VLE_VPZ_EXPORT Condition : public Base
    {
    public:
        /**
         * @brief Define an iterator to the ConditionValues.
         */
        typedef ConditionValues::iterator iterator;

        /**
         * @brief Define a constant iterator to the ConditionValues.
         */
        typedef ConditionValues::const_iterator const_iterator;

        /**
         * @brief Build a new Condition with only a name.
         * @param name The name of the condition.
         */
        Condition(const std::string& name);

        /**
         * @brief Copy constructor. All values are cloned.
         * @param cnd The Condition to copy.
         */
        Condition(const Condition& cnd);

        /**
         * @brief Delete all the values attached to this Conditon.
         */
        virtual ~Condition();

        /**
         * @brief Add Condition informations to the stream.
         * @code
         * <condition name="">
         *  <port name="">
         *   <!-- vle::value XML representation. -->
         *  </port>
         * </condition>
         * @endcode
         *
         * @param out
         */
        virtual void write(std::ostream& out) const;

        /**
         * @brief Get the type of this class.
         * @return CONDITION.
         */
        inline virtual Base::type getType() const
        { return CONDITION; }

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Manage the ConditionValues
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Build a list of string that contains all port names.
         * @param lst An output string list.
         */
        void portnames(std::list < std::string >& lst) const;

        /**
         * @brief Add a port to the value list.
         * @param portname name of the port.
         */
        void add(const std::string& portname);

        /**
         * @brief Remove a port of the value list.
         * @param portname name of the port.
         */
        void del(const std::string& portname);

	/**
	  * @brief Rename a port of the value list.
	  * @param old_name name of the port to rename
	  * @param new_name new name of the specified port
	  */
	void rename(const std::string& oldportname, const std::string& newportname);

        /**
         * @brief Add a value to a specified port. If port does not exist, it
         * will be create.
         * @param portname name of the port to add value.
         * @param value the value to push.
         */
        void addValueToPort(const std::string& portname,
                            value::Value* value);

        /**
         * @brief Add a value to a specified port. If port does not exist, it
         * will be create.
         * @param portname name of the port to add value.
         * @param value the value to push.
         */
        void addValueToPort(const std::string& portname,
                            const value::Value& value);

        /**
         * @brief Set a value to a specified port. If port contains already
         * value, these values are deleted.
         * @param portname The name of the port to add value.
         * @param value the value to push.
         * @throw utils::ArgError if portname does not exist.
         */
        void setValueToPort(const std::string& portname,
                            const value::Value& value);

        /**
         * @brief Clear the specified port.
         * @param portname The name of the port to clear.
         * @throw utils::ArgError if portname does not exist.
         */
        void clearValueOfPort(const std::string& portname);

        /**
         * @brief Build a new Map with the first values of each port of this
         * condition. Be carrefull, dont delete the value::Map directly, you
         * don't delete value::Value from the value::Map.
         *
         * @return A new value::Map, but don't delete the content, just clear
         * the value::Map::MapValue.
         */
        value::Map* firstValues() const;

        /**
         * @brief Get the value::Set attached to a port.
         * @param portname The name of the port.
         * @return A reference to a value::Set.
         * @throw utils::ArgError if portname not exist.
         */
        const value::Set& getSetValues(const std::string& portname) const;

        /**
         * @brief Get the value::Set attached to a port.
         * @param portname The name of the port.
         * @return A reference to a value::Set.
         * @throw utils::ArgError if portname not exist.
         */
        value::Set& getSetValues(const std::string& portname);

        /**
         * @brief Return a reference to the first value::Value of the specified
         * port.
         * @param portname the name of the port to test.
         * @return A reference to a value::Value.
         * @throw utils::ArgError if portname not exist.
         */
        const value::Value& firstValue(const std::string& portname) const;

        /**
         * @brief Return a reference to the nth value::Value of the specified
         * port.
         * @param portname the name of the specified port.
         * @param i the value of the port.
         * @return A reference to a value::Value.
         * @throw utils::ArgError if portname not exist or if value list
         * have no nth value.
         */
        const value::Value& nValue(const std::string& portname, size_t i) const;

        /**
         * @brief Return a reference to the value::Set of the latest added port.
         * This function is principaly used in Sax parser.
         * @return A reference to the value::Set of the port.
         * @throw utils::ArgError if port does not exist.
         */
        value::Set& lastAddedPort();

        /**
         * @brief This function initialise every, to each port, a new
         * value::Set.
         */
        void rebuildValueSet();

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Get/Set functions.
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Get a reference to the ConditionValues.
         * @return A constant reference to the ConditionValues.
         */
        inline const ConditionValues& conditionvalues() const
        { return m_list; }

        /**
         * @brief Return the name of the condition.
         * @return
         */
        inline const std::string& name() const
        { return m_name; }

	/**
	 * @brief Set a new name to this condition.
	 * @param new_name The new name for this condition.
	 * @return
	 */
	inline void setName(const std::string& new_name)
	{ m_name.assign(new_name); }

        /**
         * @brief Return true if this condition is a permanent data for the
         * devs::ModelFactory.
         * @return True if this condition is a permanent value.
         */
        inline bool isPermanent() const
        { return m_ispermanent; }

        /**
         * @brief Set the permanent value of this condition.
         * @param value True to conserve this condition in devs::ModelFactory.
         */
        inline void permanent(bool value = true)
        { m_ispermanent = value; }

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Functors
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Predicate functor to check if this condition is permanet or
         * not. To use with std::find_if:
         * Example:
         * @code
         * std::vector < Condition > v;
         * [...]
         * std::vector < Condition >::iterator it =
         *    std::find_if(v.beging(), v.end(), Condition::IsPermanent());
         * @endcode
         */
        struct IsPermanent
        {
            /**
             * @brief Check if the Condition is permanent.
             * @param x the Condition to check.
             * @return True if the Condition is permanent.
             */
            bool operator()(const Condition& x) const
            { return x.isPermanent(); }
        };

        /**
         * @brief Unary function to rebuild the value set (ie. restore all Set
         * with empty vector). To use with std::for_each or vle::for_each.
         */
        struct RebuildValueSet
        {
            /**
             * @brief Rebuild the Value of the specified Condition.
             * @param x the Condition to rebuild the Value.
             */
            void operator()(Condition& x) const
            { x.rebuildValueSet(); }
        };

    private:
        Condition();

        ConditionValues         m_list;         /* list of port, values. */
        std::string             m_name;         /* name of the condition. */
        std::string             m_last_port;    /* latest added port. */
        bool                    m_ispermanent;
    };

}} // namespace vle vpz

#endif
