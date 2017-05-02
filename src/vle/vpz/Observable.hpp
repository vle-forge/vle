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

#ifndef VLE_VPZ_OBSERVABLE_HPP
#define VLE_VPZ_OBSERVABLE_HPP

#include <list>
#include <map>
#include <string>
#include <vle/DllDefines.hpp>
#include <vle/vpz/Base.hpp>

namespace vle {
namespace vpz {

/**
 * @brief Define a list of string like a list of view name.
 */
typedef std::list<std::string> ViewNameList;

/**
 * @brief Define a list of string like a list of port name.
 */
typedef std::list<std::string> PortNameList;

/**
 * @brief ObservablePort represents the list of views names that an
 * observale port can containt.
 * @code
 * <port name="y">
 *  <attachedview name="viewA" />
 *  <attachedview name="viewB" />
 * </port>
 * @endcode
 */
class VLE_API ObservablePort : public Base
{
public:
    typedef ViewNameList::iterator iterator;
    typedef ViewNameList::const_iterator const_iterator;
    typedef ViewNameList::size_type size_type;
    typedef ViewNameList::value_type value_type;

    /**
     * @brief Build a new ObservablePort with a specific name.
     * @param portname The name of the port.
     */
    ObservablePort(const std::string& portname)
      : m_name(portname)
    {
    }

    /**
     * @brief Nothing to delete.
     */
    virtual ~ObservablePort()
    {
    }

    /**
     * @brief Write the XML representation of this class.
     * @code
     * <port name="name" />
     * <port name="name2">
     *  <attachedview name="a" />
     * </port>
     * @endcode
     * @param out
     */
    virtual void write(std::ostream& out) const override;

    /**
     * @brief Get the type of this class.
     * @return OBSERVABLEPORT.
     */
    virtual Base::type getType() const override
    {
        return VLE_VPZ_OBSERVABLEPORT;
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Manage ObservablePort.
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Add a new view name to the ObservablePort.
     * @param view the view to insert.
     * @throw utils::ArgError if view name already exist.
     */
    void add(const std::string& view);

    /**
     * @brief Remove a existing views of the ObservablePort.
     * @param view the view to insert.
     */
    void del(const std::string& view);

    /**
     * @brief Return true if views of the ObservablePort.
     * @param view the view name to test.
     * @return true if view exist, false otherwise.
     */
    bool exist(const std::string& view) const;

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Get/Set functions.
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Return the name of the ObservablePort.
     * @return name of the ObservablePort.
     */
    inline const std::string& name() const
    {
        return m_name;
    }

    /**
     * @brief Get a constant reference to the ViewNameList.
     * @return Return a constant reference to the ViewNameList.
     */
    inline const ViewNameList& viewnamelist() const
    {
        return m_list;
    }

    /**
     * @brief Get a constant reference to the ViewNameList.
     * @return Return a constant reference to the ViewNameList.
     */
    inline ViewNameList& viewnamelist()
    {
        return m_list;
    }

    /**
     * @brief Get a iterator the begin of the ViewNameList.
     * @return Get a iterator the begin of the ViewNameList.
     */
    iterator begin()
    {
        return m_list.begin();
    }

    /**
     * @brief Get a iterator the end of the ViewNameList.
     * @return Get a iterator the end of the ViewNameList.
     */
    iterator end()
    {
        return m_list.end();
    }

    /**
     * @brief Get a constant iterator the begin of the ViewNameList.
     * @return Get a constant iterator the begin of the
     * ViewNameList.
     */
    const_iterator begin() const
    {
        return m_list.begin();
    }

    /**
     * @brief Get a constant iterator the end of the ViewNameList.
     * @return Get a constant iterator the end of the ViewNameList.
     */
    const_iterator end() const
    {
        return m_list.end();
    }

private:
    ViewNameList m_list;
    std::string m_name;
};

/**
 * @brief Define a list of ObservablePort.
 */
typedef std::map<std::string, ObservablePort> ObservablePortList;

/**
 * @brief An Observable is a definition of states ports. Each state port is
 * defined by an ObservablePort.
 * @code
 * <observable name="obsB">
 *  <port name="x" />
 *  <port name="y">
 *   <view name="measureA" />
 *  </port>
 *  <port name="z" />
 * </observable>
 * @endcode
 */
class VLE_API Observable : public Base
{
public:
    typedef ObservablePortList::iterator iterator;
    typedef ObservablePortList::const_iterator const_iterator;
    typedef ObservablePortList::size_type size_type;
    typedef ObservablePortList::value_type value_type;

    /**
     * @brief Build a new Observable with specified name.
     * @param name The name of this Observable.
     */
    Observable(const std::string& name)
      : m_name(name)
    {
    }

    /**
     * @brief Nothing to delete.
     */
    virtual ~Observable()
    {
    }

    /**
     * @brief Write the XML representation of this class.
     * @code
     * <observable name="name">
     *  [...]
     * </observable>
     * @endcode
     * @param out The output stream parameter.
     */
    virtual void write(std::ostream& out) const override;

    /**
     * @brief Get the type of this class.
     * @return OBSERVABLE.
     */
    virtual Base::type getType() const override
    {
        return VLE_VPZ_OBSERVABLE;
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Manage the ObservablePortList.
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Add a new ObservablePort with only a name.
     * @param portname the name of the ObservablePort.
     * @throw utils::ArgError if an ObservablePort already exist.
     * @return the newly ObservablePort.
     */
    ObservablePort& add(const std::string& portname);

    /**
     * @brief Add a new ObservablePort.
     * @param obs the observable to add.
     * @throw utils::ArgError if an ObservablePort already exist.
     * @return the newly ObservablePort.
     */
    ObservablePort& add(const ObservablePort& obs);

    /**
     * @brief Get a constant ObservablePort.
     * @param portname the name of the ObservablePort to find.
     * @throw utils::ArgError if no ObservablePort exist.
     * @return the ObservablePort finded.
     */
    ObservablePort& get(const std::string& portname);

    /**
     * @brief Get a ObservablePort.
     * @param portname the name of the ObservablePort to find.
     * @throw utils::ArgError if no ObservablePort exist.
     * @return the ObservablePort finded.
     */
    const ObservablePort& get(const std::string& portname) const;

    /**
     * @brief Test if an ObservablePort exist.
     * @param portname the name of the ObservablePort.
     * @return true if ObservablePort exist, false otherwise.
     */
    inline bool exist(const std::string& portname) const
    {
        return m_list.find(portname) != m_list.end();
    }

    /**
    * @brief Delete an ObservablePort.
    * @param portname the name of the ObservablePort to delete.
    * @throw utils::ArgError if the ObservablePort doesn't exist.
    */
    void del(const std::string& portname);

    /**
     * @brief Return true if one ObservablePort have the specified attached
     * view.
     * @param viewname The name of the view to test.
     * @return True if view is found in an ObservablePort, false otherwise.
     */
    bool hasView(const std::string& viewname) const;

    /**
     * @brief Return a list of portname that are attached to the specified
     * view.
     * @param view The name of the view to get port name.
     * @return a list of portname.
     */
    PortNameList getPortname(const std::string& view) const;

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Get/Set function.
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Get a constant reference to the ObservablePortList.
     * @return Return a constant reference to the ObservablePortList.
     */
    inline const ObservablePortList& observableportlist() const
    {
        return m_list;
    }

    /**
     * @brief Get a constant reference to the ObservablePortList.
     * @return Return a constant reference to the ObservablePortList.
     */
    inline ObservablePortList& observableportlist()
    {
        return m_list;
    }

    /**
     * @brief Return true if this observable is a permanent data for the
     * devs::ModelFactory.
     *
     * @return True if this dynamics is a permanent value.
     */
    inline bool isPermanent() const
    {
        return m_ispermanent;
    }

    /**
     * @brief Set the permanent value of this dynamics.
     * @param value True to conserve this dynamics in devs::ModelFactory.
     */
    inline void permanent(bool value = true)
    {
        m_ispermanent = value;
    }

    /**
     * @brief Return the name of the Observable.
     * @return name of the ObservablePort.
     */
    inline const std::string& name() const
    {
        return m_name;
    }

    /**
     * @brief Get a iterator the begin of the ObservablePortList.
     * @return Get a iterator the begin of the ObservablePortList.
     */
    iterator begin()
    {
        return m_list.begin();
    }

    /**
     * @brief Get a iterator the end of the ObservablePortList.
     * @return Get a iterator the end of the ObservablePortList.
     */
    iterator end()
    {
        return m_list.end();
    }

    /**
     * @brief Get a constant iterator the begin of the ObservablePortList.
     * @return Get a constant iterator the begin of the
     * ObservablePortList.
     */
    const_iterator begin() const
    {
        return m_list.begin();
    }

    /**
     * @brief Get a constant iterator the end of the ObservablePortList.
     * @return Get a constant iterator the end of the ObservablePortList.
     */
    const_iterator end() const
    {
        return m_list.end();
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     *
     * Functors
     *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    /**
     * @brief Predicate functor to check if an Observable is permanent.
     */
    struct IsPermanent
    {
        /**
         * @brief Check if the Observable is permanent.
         * @param x The Observable to check.
         * @return true if Observable is permanent.
         */
        inline bool operator()(const Observable& x) const
        {
            return x.isPermanent();
        }
    };

    /**
     * @brief Predicate functor to check if an ObservablePort have an View.
     */
    struct HasView
    {
        /**
         * @brief Constructor.
         * @param name The name of the View.
         */
        HasView(const std::string& name)
          : m_name(name)
        {
        }

        /**
         * @brief Check if the specified ObservablePort have the View
         * m_name.
         * @param x the ObservablePort to check.
         * @return True if the ObservablePort have the View.
         */
        inline bool operator()(const ObservablePort& x) const
        {
            return x.exist(m_name);
        }

        const std::string& m_name; //!< The name of the View to check.
    };

    /**
     * @brief Functor to add view to an ObservablePort.
     */
    struct AddAttachedViewPortname
    {
        /**
         * @brief Constructor to add ObservablePort to the specified
         * PortNameList.
         * @param lst The list where attach the ObservablePort.
         * @param viewname The name of the view.
         */
        AddAttachedViewPortname(PortNameList& lst, const std::string& viewname)
          : m_list(lst)
          , m_viewname(viewname)
        {
        }

        /**
         * @brief Add the ObservablePort to the PortNameList if the
         * ObservablePort is connected to the View m_viewname.
         */
        inline void operator()(const ObservablePortList::value_type& pair)
        {
            if (pair.second.exist(m_viewname)) {
                m_list.push_back(pair.first);
            }
        }

        PortNameList& m_list;          //!< Output parameter of this functor.
        const std::string& m_viewname; //!< The name of the View.
    };

private:
    ObservablePortList m_list;
    std::string m_name;
    bool m_ispermanent;
};
}
} // namespace vle vpz

#endif
