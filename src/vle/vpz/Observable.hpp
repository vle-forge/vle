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
     * @brief ObservablePort represents the list of ports of an Observable.
     * ObservablePort is a std::set of std::string where std::string is the port
     * name.
     */
    class ObservablePort : public std::set < std::string >
    {
    public:
        virtual ~ObservablePort()
        { }
    };


    /** 
     * @brief An observable is a observation on state of a model. It is defines
     * by a couple model name and port name. To allow grouping and sorting data
     * we use two attributes, group and index.
     */
    class Observable : public Base
    {
    public:
        Observable();
        
        virtual ~Observable();

        virtual void write(std::ostream& out) const;

        virtual Base::type getType() const
        { return OBSERVABLE; }

        /** 
         * @brief Set the observable information with model name or port name
         * to observe and the couple group, index if define, to sort multiple
         * observations.
         * 
         * @param modelname model name to observe.
         * @param portname port name to observe.
         * @param group if define, the group where observation is store.
         * @param index if define, the index of the group.
         *
         * @throw Exception::Internal if modelname or portname is empty or if
         * group is define but not the index.
         */
        void setObservable(const std::string& name,
                           const std::string& group = std::string(),
                           int index = -1);


        /** 
         * @brief Insert into the observables ports list a new port.
         * 
         * @param portname the port to push.
         */
        void add(const std::string& portname);

        /* * * * * * * * * * * * */

        inline const std::string& name() const
        { return m_name; }

        inline const std::string& group() const
        { return m_group; }

        inline int index() const
        { return m_index; }

        inline  const ObservablePort& ports() const
        { return m_ports; }

        inline ObservablePort& ports()
        { return m_ports; }
        
    private:
        std::string         m_name;
        std::string         m_group;
        int                 m_index;
        ObservablePort      m_ports;
    };

}} // namespace vle vpz

#endif
