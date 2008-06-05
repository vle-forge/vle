/**
 * @file src/vle/vpz/Observable.hpp
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




#ifndef VLE_VPZ_OBSERVABLE_HPP
#define VLE_VPZ_OBSERVABLE_HPP

#include <map>
#include <list>
#include <string>
#include <vle/vpz/Base.hpp>

namespace vle { namespace vpz {

    /**
     * @brief Define a list of string like a list of view name.
     */
    typedef std::list < std::string > ViewNameList;

    /**
     * @brief Define a list of string like a list of port name.
     */
    typedef std::list < std::string > PortNameList;

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
    class ObservablePort : public Base
    {
    public:
        ObservablePort(const std::string& portname);

        virtual ~ObservablePort()
        { }

        virtual void write(std::ostream& out) const;

        virtual Base::type getType() const
        { return OBSERVABLEPORT; }

        /**
         * @brief Add a new view name to the ObservablePort.
         * @param view the view to insert.
         * @throw utils::SaxParserError if view name already exist.
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

        ////
        //// Get/Set functions
        ////

        /**
         * @brief Return the name of the ObservablePort.
         * @return name of the ObservablePort.
         */
        inline const std::string& name() const
        { return m_name; }

        /**
         * @brief Get a constant reference to the ViewNameList.
         * @return Return a constant reference to the ViewNameList.
         */
        inline const ViewNameList& viewnamelist() const
        { return m_list; }

    private:
        ObservablePort()
        { }

        ViewNameList    m_list;
        std::string     m_name;
    };

    /**
     * @brief Define a list of ObservablePort.
     */
    typedef std::map < std::string, ObservablePort > ObservablePortList;

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
    class Observable : public Base
    {
    public:
        Observable(const std::string& name);

        virtual ~Observable()
        { }

        virtual void write(std::ostream& out) const;

        virtual Base::type getType() const
        { return OBSERVABLE; }

        /**
         * @brief Get a constant reference to the ObservablePortList.
         * @return Return a constant reference to the ObservablePortList.
         */
        inline const ObservablePortList& observableportlist() const
        { return m_list; }

        /**
         * @brief Add a new ObservablePort with only a name.
         * @param portname the name of the ObservablePort.
         * @throw utils::SaxParserError if an ObservablePort already exist.
         * @return the newly ObservablePort.
         */
        ObservablePort& add(const std::string& portname);

        /**
         * @brief Add a new ObservablePort.
         * @param portname the name of the ObservablePort.
         * @throw utils::SaxParserError if an ObservablePort already exist.
         * @return the newly ObservablePort.
         */
        ObservablePort& add(const ObservablePort& obs);

        /**
         * @brief Get a constant ObservablePort.
         * @param portname the name of the ObservablePort to find.
         * @throw utils::SaxParserError if no ObservablePort exist.
         * @return the ObservablePort finded.
         */
        ObservablePort& get(const std::string& portname);

        /**
         * @brief Get a ObservablePort.
         * @param portname the name of the ObservablePort to find.
         * @throw utils::SaxParserError if no ObservablePort exist.
         * @return the ObservablePort finded.
         */
        const ObservablePort& get(const std::string& portname) const;

        /**
         * @brief Test if an ObservablePort exist.
         * @param portname the name of the ObservablePort.
         * @return true if ObservablePort exist, false otherwise.
         */
        inline bool exist(const std::string& portname) const
        { return m_list.find(portname) != m_list.end(); }

         /**
         * @brief Delete an ObservablePort.
         * @param portname the name of the ObservablePort to delete.
         * @throw utils::SaxParserError if the ObservablePort doesn't exist.
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
         * @param viewname The name of the view to get port name.
         * @return a list of portname.
         */
        PortNameList getPortname(const std::string& view) const;

        ////
        //// Get/Set functions.
        ////

        /**
         * @brief Return true if this observable is a permanent data for the
         * devs::ModelFactory.
         *
         * @return True if this dynamics is a permanent value.
         */
        inline bool isPermanent() const
        { return m_ispermanent; }

        /**
         * @brief Set the permanent value of this dynamics.
         *
         * @param value True to conserve this dynamics in devs::ModelFactory.
         */
        inline void permanent(bool value = true)
        { m_ispermanent = value; }

        /**
         * @brief Return the name of the Observable.
         * @return name of the ObservablePort.
         */
        inline const std::string& name() const
        { return m_name; }

    private:
        Observable()
        { }

        ObservablePortList  m_list;
        std::string         m_name;
        bool                m_ispermanent;

        struct HasView
        {
            HasView(const std::string& name) :
                m_name(name)
            { }

            inline bool operator()(const ObservablePortList::value_type& pair)
            { return pair.second.exist(m_name); }

            const std::string& m_name;
        };

        struct AddAttachedViewPortname
        {
            AddAttachedViewPortname(PortNameList& lst,
                                    const std::string& viewname) :
                m_list(lst),
                m_viewname(viewname)
            { }

            inline void operator()(const ObservablePortList::value_type& pair)
            {
                if (pair.second.exist(m_viewname)) {
                    m_list.push_back(pair.first);
                }
            }

            PortNameList&       m_list;
            const std::string&  m_viewname;
        };
    };

}} // namespace vle vpz

#endif
