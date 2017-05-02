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

#ifndef VLE_VPZ_CONDITION_HPP
#define VLE_VPZ_CONDITION_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <vle/DllDefines.hpp>
#include <vle/value/Map.hpp>
#include <vle/vpz/Base.hpp>

namespace vle {
namespace vpz {

/**
 * @brief Define the ConditionValues like a dictionnary, (portname, values).
 */
using ConditionValues =
  std::unordered_map<std::string, std::vector<std::shared_ptr<value::Value>>>;

/**
 * @brief A condition define a couple model name, port name and a Value.
 * This class allow loading and writing a condition.
 */
class VLE_API Condition : public Base
{
public:
    typedef ConditionValues::iterator iterator;
    typedef ConditionValues::const_iterator const_iterator;
    typedef ConditionValues::size_type size_type;
    typedef ConditionValues::value_type value_type;

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
     * @brief Assignment operator. All values are cloned.
     */
    Condition& operator=(const Condition& cnd);

    /**
     * @brief Delete all the values attached to this Conditon.
     */
    virtual ~Condition()
    {
    }

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
    virtual void write(std::ostream& out) const override;

    /**
     * @brief Get the type of this class.
     * @return CONDITION.
     */
    inline virtual Base::type getType() const override
    {
        return VLE_VPZ_CONDITION;
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Manage the ConditionValues
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Build a list of string that contains all port names.
     * @param lst An output string list.
     */
    std::vector<std::string> portnames() const;

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
     * @brief Add a value to a specified port. If port does not exist, it
     * will be create.
     * @param portname name of the port to add value.
     * @param value the value to push.
     */
    void addValueToPort(const std::string& portname,
                        std::shared_ptr<value::Value> value);

    /**
     * @brief Set a value to a specified port. If port contains already
     * value, these values are deleted.
     * @param portname The name of the port to add value.
     * @param value the value to push.
     * @throw utils::ArgError if portname does not exist.
     */
    void setValueToPort(const std::string& portname,
                        std::shared_ptr<value::Value> value);
    /**
     * @brief Clear the specified port.
     * @param portname The name of the port to clear.
     * @throw utils::ArgError if portname does not exist.
     */
    void clearValueOfPort(const std::string& portname);

    /**
     * @brief Fill a MapValue with the first value::Value of each port
     * of this condition. Be careful, values contained in the value::Map
     * are not copies.
     *
     * @param map, the value::MapValue to fill with the first value::Value
     * of each port of the condition.
     */
    std::unordered_map<std::string, std::shared_ptr<value::Value>>
    fillWithFirstValues() const;

    /**
     * @brief Get the value::Set attached to a port.
     * @param portname The name of the port.
     * @return A reference to a value::Set.
     * @throw utils::ArgError if portname not exist.
     */
    const std::vector<std::shared_ptr<value::Value>>& getSetValues(
      const std::string& portname) const;

    /**
     * @brief Get the value::Set attached to a port.
     * @param portname The name of the port.
     * @return A reference to a value::Set.
     * @throw utils::ArgError if portname not exist.
     */
    std::vector<std::shared_ptr<value::Value>>& getSetValues(
      const std::string& portname);

    /**
     * @brief Return a reference to the first value::Value of the specified
     * port.
     * @param portname the name of the port to test.
     * @return A reference to a value::Value.
     * @throw utils::ArgError if portname not exist.
     */
    const std::shared_ptr<value::Value>& firstValue(
      const std::string& portname) const;

    /**
     * @brief Return a reference to the nth value::Value of the specified
     * port.
     * @param portname the name of the specified port.
     * @param i the value of the port.
     * @return A reference to a value::Value.
     * @throw utils::ArgError if portname not exist or if value list
     * have no nth value.
     */
    const std::shared_ptr<value::Value>& nValue(const std::string& portname,
                                                size_t i) const;

    /**
     * @brief Return a reference to the value::Set of the latest added port.
     * This function is principaly used in Sax parser.
     * @return A reference to the value::Set of the port.
     * @throw utils::ArgError if port does not exist.
     */
    std::vector<std::shared_ptr<value::Value>>& lastAddedPort();

    /**
     * @brief This function deletes on each port the values stored
     * in the value::Set
     */
    void deleteValueSet();

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
    {
        return m_list;
    }

    /**
     * @brief Get a reference to the ConditionValues.
     * @return A constant reference to the ConditionValues.
     */
    inline ConditionValues& conditionvalues()
    {
        return m_list;
    }

    /**
     * @brief Get a iterator the begin of the vpz::ConditionValues.
     * @return Get a iterator the begin of the vpz::ConditionValues.
     */
    iterator begin()
    {
        return m_list.begin();
    }

    /**
     * @brief Get a iterator the end of the vpz::ConditionValues.
     * @return Get a iterator the end of the vpz::ConditionValues.
     */
    iterator end()
    {
        return m_list.end();
    }

    /**
     * @brief Get a constant iterator the begin of the vpz::ConditionValues.
     * @return Get a constant iterator the begin of the
     * vpz::ConditionValues.
     */
    const_iterator begin() const
    {
        return m_list.begin();
    }

    /**
     * @brief Get a constant iterator the end of the vpz::ConditionValues.
     * @return Get a constant iterator the end of the vpz::ConditionValues.
     */
    const_iterator end() const
    {
        return m_list.end();
    }

    /**
     * @brief Return the name of the condition.
     * @return
     */
    inline const std::string& name() const
    {
        return m_name;
    }

    /**
     * @brief Set a new name to this condition.
     * @param new_name The new name for this condition.
     * @return
     */
    inline void setName(const std::string& new_name)
    {
        m_name.assign(new_name);
    }

    /**
     * @brief Return true if this condition is a permanent data for the
     * devs::ModelFactory.
     * @return True if this condition is a permanent value.
     */
    inline bool isPermanent() const
    {
        return m_ispermanent;
    }

    /**
     * @brief Set the permanent value of this condition.
     * @param value True to conserve this condition in devs::ModelFactory.
     */
    inline void permanent(bool value = true)
    {
        m_ispermanent = value;
    }

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
        {
            return x.isPermanent();
        }
    };

    /**
     * @brief Unary function to delete the value set.
     * To use with std::for_each
     * or vle::for_each.
     */
    struct DeleteValueSet
    {
        /**
         * @brief Delete and rebuild the Value of the specified Condition.
         * @param x the Condition to delete and rebuild the Value.
         */
        void operator()(Condition& x) const
        {
            x.deleteValueSet();
        }
    };

private:
    Condition() = delete;

    ConditionValues m_list;  /* list of port, values. */
    std::string m_name;      /* name of the condition. */
    std::string m_last_port; /* latest added port. */
    bool m_ispermanent;
};
}
} // namespace vle vpz

#endif
