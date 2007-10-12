/** 
 * @file vpz/Observable.hpp
 * @brief 
 * @author The vle Development Team
 * @date mar, 31 jan 2006 17:23:57 +0100
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

#ifndef VLE_VPZ_OBSERVABLE_HPP
#define VLE_VPZ_OBSERVABLE_HPP

#include <set>
#include <string>
#include <vle/vpz/Base.hpp>

namespace vle { namespace vpz {

    /** 
     * @brief ObservablePort represents the list of views names that an
     * observale port can containt.
     * @code
     * <port name="y">
     *  <view name="viewA" />
     *  <view name="viewB" />
     * </port>
     * @endcode
     */
    class ObservablePort : public Base, public std::list < std::string >
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

        //
        ///
        //// Get/Set functions
        ///
        //

        /** 
         * @brief Return the name of the ObservablePort.
         * @return name of the ObservablePort.
         */
        inline const std::string& name() const
        { return m_name; }

    private:
        ObservablePort()
        { }

        std::string     m_name;
    };


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
    class Observable : public Base, public std::map < std::string,
                                                      ObservablePort >
    {
    public:
        typedef std::list < std::string > PortnameList;

        Observable(const std::string& name);
        
        virtual ~Observable()
        { }

        virtual void write(std::ostream& out) const;

        virtual Base::type getType() const
        { return OBSERVABLE; }

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
        { return find(portname) != end(); }

        bool hasView(const std::string& name) const;

        PortnameList getPortname(const std::string& view) const;

        //
        ///
        //// Get/Set functions.
        ///
        //
        
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

        std::string         m_name;
        bool                m_ispermanent;
    };

}} // namespace vle vpz

#endif
