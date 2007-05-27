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

#include <string>
#include <vle/vpz/Base.hpp>

namespace vle { namespace vpz {

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

        bool operator==(const Observable& o) const;
        
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
        void setObservable(const std::string& modelname,
                           const std::string& portname,
                           const std::string& group = std::string(),
                           int index = -1);

        /* * * * * * * * * * * * */

        const std::string& modelname() const
        { return m_modelname; }

        const std::string& portname() const
        { return m_portname; }

        const std::string& group() const
        { return m_group; }

        int index() const
        { return m_index; }
        
    private:
        std::string m_modelname;
        std::string m_portname;
        std::string m_group;
        int         m_index;
    };

}} // namespace vle vpz

#endif
